#pragma once

#include "board.h"
#include <QSharedPointer>
#include <array>
#include <functional>
#include <optional>
#include <stdexcept>

using namespace minesweeper;

class AIReasoningError : public std::runtime_error {
public:
    AIReasoningError(const std::string& what)
        : std::runtime_error(what)
    {
    }
};

constexpr std::array<Direction, 8> ALL_DIRECTIONS = {
    Direction::LeftUp, Direction::Up, Direction::RightUp,
    Direction::Left, Direction::Right, Direction::LeftDown,
    Direction::Down, Direction::RightDown
};

struct AICallback {
    virtual void before_start(const QSharedPointer<Board>& board) = 0;
    virtual bool on_step(const QSharedPointer<Board>& board, int current_step, int nest_level) = 0;
};

class MineAI {
    QSharedPointer<Board> board;
    bool log_enabled;
    bool firsttime = true;
    int assume_nest_level = 0;

public:
    MineAI(QSharedPointer<Board> board);

    bool& logging() { return log_enabled; }
    const bool& logging() const { return log_enabled; }

    void next_step(bool logging, AICallback& cb);

    bool static solve_all(QSharedPointer<Board> board,
        bool logging,
        AICallback& cb);
    bool static solve_all(QSharedPointer<Board> board,
        bool logging,
        AICallback& cb,
        int nest_level);

    std::optional<int> open_any();
};
