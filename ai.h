#pragma once

#include "board.h"
#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>

using namespace minesweeper;

class AIReasoningError : public std::runtime_error
{
public:
    AIReasoningError(const std::string& what)
        : std::runtime_error(what)
    {}
};

constexpr std::array<Direction, 8> ALL_DIRECTIONS = {
    Direction::LeftUp, Direction::Up,       Direction::RightUp,
    Direction::Left,   Direction::Right,    Direction::LeftDown,
    Direction::Down,   Direction::RightDown
};

struct AICallback
{
    std::function<void(Board& board)> before_start;
    std::function<bool(Board& board, int current_step, int nest_level)> on_step;
};

class MineAI
{
    std::shared_ptr<Board> board;
    bool log_enabled;
    bool firsttime = true;
    int assume_nest_level = 0;

public:
    MineAI(std::shared_ptr<Board> board);

    bool& logging() { return log_enabled; }
    const bool& logging() const { return log_enabled; }

    void next_step(bool logging, AICallback cb);

    bool static solve_all(std::shared_ptr<Board> board,
                          bool logging,
                          AICallback cb);
    bool static solve_all(std::shared_ptr<Board> board,
                          bool logging,
                          AICallback cb,
                          int nest_level);

    std::optional<int> open_any();
};