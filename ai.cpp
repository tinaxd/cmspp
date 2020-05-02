#include "ai.h"
#include <algorithm>
#include <functional>
#include <iostream>

using namespace minesweeper;

MineAI::MineAI(std::shared_ptr<Board> board)
    : board(std::move(board))
{}

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

bool
MineAI::solve_all(std::shared_ptr<Board> board, bool logging, AICallback cb)
{
    return solve_all(std::move(board), logging, cb, 0);
}

bool
MineAI::solve_all(std::shared_ptr<Board> board,
                  bool logging,
                  AICallback cb,
                  int nest_level)
{
    auto ai = std::make_unique<MineAI>(board);
    ai->assume_nest_level = nest_level;
    if (nest_level > 0) {
        ai->firsttime = false;
    }
    ai->logging() = logging;
    cb.before_start(*board);
    int step = 0;
    while (true) {
        ai->next_step(logging, cb);
        if (!cb.on_step(*board, step++, ai->assume_nest_level)) {
            return false;
        }
        if (board->cleared()) {
            return true;
        } else if (board->failed()) {
            return false;
        }
    }
}

void
MineAI::next_step(bool logging, AICallback cb)
{
    if (firsttime) {
        firsttime = false;
        open_any();
        return;
    }

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
        if (cell.opened()) {
            auto bombs_around = cell.neighbor_bombs();

            std::vector<Cell*> neighbors;
            for (auto dir : ALL_DIRECTIONS) {
                auto index = board->get_cell_index(i, dir);
                if (index.has_value()) {
                    neighbors.push_back(&(*board)[index.value()]);
                }
            }
            int closed_cells_around = 0;
            int flagged_cells_around = 0;
            for (const auto neighbor : neighbors) {
                switch (neighbor->state()) {
                case CellState::Closed:
                    closed_cells_around++;
                    break;
                case CellState::Flagged:
                    flagged_cells_around++;
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
                        if (c->closed()) {
                            c->state() = CellState::Opened;
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
                        if (c->closed()) {
                            c->state() = CellState::Flagged;
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

        bool surrounded_by_open = false;
        for (auto dir : ALL_DIRECTIONS) {
            auto next_index = board->get_cell_index(i, dir);
            if (!next_index.has_value())
                continue;
            if ((*board)[next_index.value()].opened()) {
                surrounded_by_open = true;
                break;
            }
        }
        if (!surrounded_by_open) {
            // surrounded by open only
            continue;
        }

        if (log_enabled) {
            std::cout << "ASSUME closed cell as flagged (entering level "
                      << assume_nest_level + 1 << ") " << i << std::endl;
        }
        auto new_board = std::make_shared<Board>(*board);
        (*new_board)[i].state() = CellState::Flagged;
        try {
            if (solve_all(new_board, logging, cb, assume_nest_level + 1)) {
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
    board->show_game_state(std::cerr, true);
    std::cerr << std::endl;
    throw AIReasoningError("NO LOGIC");
}