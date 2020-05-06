#include "ai.h"
#include <QEventLoop>
#include <QPair>
#include <QThread>
#include <QTimer>
#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>

using namespace minesweeper;

MineAI::MineAI(QSharedPointer<Board> board)
    : board(std::move(board))
{
}

std::optional<int>
MineAI::open_any()
{
    auto cells = board->get_total_cells();
    for (auto i = 0; i < cells; i++) {
        if ((*board)[i].closed()) {
            if (log_enabled) {
                std::cout << "RANDOM Open cell " << i << std::endl;
            }
            board->open_cell(i);
            return i;
        }
    }
    return std::optional<int>();
}

bool MineAI::solve_all(QSharedPointer<Board> board, bool logging, AICallback& cb)
{
    return solve_all(std::move(board), logging, cb, 0);
}

bool MineAI::solve_all(QSharedPointer<Board> board,
    bool logging,
    AICallback& cb,
    int nest_level)
{
    auto ai = std::make_unique<MineAI>(board);
    ai->assume_nest_level = nest_level;
    if (nest_level > 0) {
        ai->firsttime = false;
    }
    ai->logging() = logging;
    cb.before_start(board);
    int step = 0;
    while (true) {
        if (board->cleared()) {
            return true;
        } else if (board->failed()) {
            return false;
        }
        ai->next_step(logging, cb);
        if (!cb.on_step(board, step++, ai->assume_nest_level)) {
            return false;
        }
    }
}

void erase_assumption(const Board& base, Board& erased)
{
    assert(base.width() == erased.width());
    assert(base.height() == erased.height());
    const int cells = base.get_total_cells();

    for (int i = 0; i < cells; i++) {
        if (!base[i].is_assumption() && erased[i].is_assumption()) {
            erased[i].is_assumption() = false;
        }
    }
}

void MineAI::next_step(bool logging, AICallback& cb)
{
    const auto in_assumption = assume_nest_level > 0;

    // 最初からマスが開いてる場合にバグる
    //    if (firsttime) {
    //        firsttime = false;
    //        open_any();
    //        return;
    //    }

    auto cells = board->get_total_cells();
    bool ok = false;
    for (auto i = 0; i < cells; i++) {
        const auto& cell = (*board)[i];

        // skip flagged cells.
        if (cell.flagged()) {
            continue;
        }

        // we cannot get any information from closed cells.
        if (cell.closed()) {
            continue;
        }

        // we can solve puzzle by utilizing neighbor bomb cell hints.
        if (cell.opened() && !cell.is_assumption()) {
            auto bombs_around = cell.neighbor_bombs();

            std::vector<QPair<Cell*, int>> neighbors;
            for (auto dir : ALL_DIRECTIONS) {
                auto index = board->get_cell_index(i, dir);
                if (index.has_value()) {
                    neighbors.push_back(qMakePair(&(*board)[index.value()], index.value()));
                }
            }
            int closed_cells_around = 0;
            int flagged_cells_around = 0;
            for (const auto neighbor : neighbors) {
                switch (neighbor.first->state()) {
                case CellState::Closed:
                    closed_cells_around++;
                    break;
                case CellState::Flagged:
                    flagged_cells_around++;
                    break;
                default:
                    break;
                }
            }

            if (bombs_around == flagged_cells_around) {
                if (closed_cells_around > 0) {
                    // open all remaining cells.
                    if (log_enabled) {
                        std::cout << "Open cells around " << i << " by logic." << std::endl;
                    }
                    for (const auto c : neighbors) {
                        if (c.first->closed()) {
                            if (!in_assumption) {
                                board->open_cell(c.second);
                            } else {
                                c.first->state() = CellState::Opened;
                                c.first->is_assumption() = true;
                            }
                        }
                    }
                    ok = true;
                    break;
                } else {
                    continue;
                }
            } else if (bombs_around > flagged_cells_around) {
                if (closed_cells_around == bombs_around - flagged_cells_around) {
                    // flag all remaining cells.
                    if (log_enabled) {
                        std::cout << "Flag cells around " << i << " by logic." << std::endl;
                    }
                    for (const auto c : neighbors) {
                        if (c.first->closed()) {
                            c.first->state() = CellState::Flagged;
                            c.first->is_assumption() = in_assumption;
                        }
                    }
                    ok = true;
                    break;
                } else {
                    continue;
                }
            } else {
                throw AIReasoningError("bombs_around < flagged_cells_around");
            }
        }
    }

    if (ok) {
        return;
    }

    // start assume!
    auto cells1 = board->get_total_cells();
    for (auto i = 0; i < cells1; i++) {
        const auto& cell = (*board)[i];
        if (!cell.closed()) {
            // closed only
            continue;
        }

        bool surrounded_by_nonassumption_open = false;
        for (auto dir : ALL_DIRECTIONS) {
            auto next_index = board->get_cell_index(i, dir);
            if (!next_index.has_value())
                continue;
            const auto& next_cell = (*board)[next_index.value()];
            if (next_cell.opened() && !next_cell.is_assumption()) {
                surrounded_by_nonassumption_open = true;
                break;
            }
        }
        if (!surrounded_by_nonassumption_open) {
            // surrounded by non-assumption open only
            continue;
        }

        if (log_enabled) {
            std::cout << "ASSUME closed cell as flagged (entering level "
                      << assume_nest_level + 1 << ") " << i << std::endl;
        }
        auto new_board = QSharedPointer<Board>(new Board(*board));
        (*new_board)[i].state() = CellState::Flagged;
        (*new_board)[i].is_assumption() = true;
        try {
            if (solve_all(new_board, logging, cb, assume_nest_level + 1)) {
                erase_assumption(*board, *new_board);
                *board = *new_board;
                return;
            }
        } catch (const AIReasoningError& e) {
            if (log_enabled) {
                std::cout << e.what() << std::endl;
                std::cout << "ASSUME " << i << " failed (back to level "
                          << assume_nest_level << ")" << std::endl;
            }
        }
    }

    // random strategy...
    if (log_enabled) {
        board->show_game_state(std::cerr, true);
        std::cerr << std::endl;
    }
    throw AIReasoningError("NO LOGIC");
}

namespace minesweeper {
struct DoNothing : public AICallback {
    void before_start(const QSharedPointer<Board>&) override {};
    bool on_step(const QSharedPointer<Board>&, int, int) override { return true; };
};
}

BoardBuilder::BoardBuilder(QObject* parent)
    : QObject(parent)
{
}

Board* BoardBuilder::generateLogicalBoard(std::function<Board*()> generator, std::optional<int> maxAttempts)
{
    while (true) {
        attempts++;
        if (maxAttempts.has_value() && maxAttempts.value() < attempts) {
            return nullptr;
        }

        emit nextAttempt(attempts);

        Board* board = generator();
        if (board == nullptr) {
            qDebug("generator fail");
            continue;
        }

        if (aiCheck(*board)) {
            qDebug("AI check success");
            return board;
        } else {
            qDebug("AI check failed");
            delete board;
        }
    }
}

bool BoardBuilder::aiCheck(const Board& board)
{
    board.show_game_state(std::cout, false);
    std::cout << std::endl;
    try {
        DoNothing cb;
        std::cout << std::endl;
        return MineAI::solve_all(QSharedPointer<Board>(new Board(board)),
            false, cb);
    } catch (const AIReasoningError& e) {
        qDebug(e.what());
        return false;
    }
}
