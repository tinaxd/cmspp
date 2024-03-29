#pragma once

#include <utility>
#include <vector>
#include <iostream>
#include <optional>

namespace minesweeper {

enum class CellState {
    Opened,
    Closed,
    Flagged
};

enum class Direction {
    LeftUp,
    Up,
    RightUp,
    Left,
    Right,
    LeftDown,
    Down,
    RightDown
};

class Cell;
class Board;

class Cell {
    bool has_bomb_;
    CellState state_;
    int neighbor_bombs_ = 0;
    bool assumption = false;

public:
    Cell()
        : Cell(false)
    {
    }

    Cell(bool has_bomb, CellState state = CellState::Closed)
        : has_bomb_(has_bomb)
        , state_(state)
    {
    }

    Cell(const Cell& cell)
        : has_bomb_(cell.has_bomb_)
        , state_(cell.state_)
        , neighbor_bombs_(cell.neighbor_bombs_)
        , assumption(cell.assumption)
    {
    }

    Cell& operator=(const Cell& cell)
    {
        has_bomb_ = cell.has_bomb_;
        state_ = cell.state_;
        neighbor_bombs_ = cell.neighbor_bombs_;
        assumption = cell.assumption;
        return *this;
    }

    bool& has_bomb() { return has_bomb_; }
    const bool& has_bomb() const { return has_bomb_; }

    CellState& state() { return state_; }
    const CellState& state() const { return state_; }

    void set_neighbor_bombs(int bombs) { neighbor_bombs_ = bombs; }
    int neighbor_bombs() const { return neighbor_bombs_; }

    bool opened() const { return state_ == CellState::Opened; }
    bool closed() const { return state_ == CellState::Closed; }
    bool flagged() const { return state_ == CellState::Flagged; }

    bool& is_assumption() { return assumption; }
    const bool& is_assumption() const { return assumption; }
};

class Board {
protected:
    int width_;
    int height_;
    int init_bombs_;
    std::vector<Cell> cells_;
    bool failed_ = false;

    void setup_cells(int width, int height, int n_bombs);
    void setup_cells(int width, int height, int n_bombs, const std::vector<int>& excludes);
    void build_neighbor_map();
    virtual void initAll();

    static char char_of_cell(const Cell& c, bool disclose_bomb);

    void open_cell4(int index);

public:
    using Point = std::pair<int, int>;

    Board(int width, int height, int n_bombs, bool initialize = true);
    Board(int width, int height, int n_bombs, const std::vector<int>& excludes, bool ai_check = false);
    Board(int width, int height, int n_bombs, const std::vector<Point>& excludes, bool ai_check = false);
    Board(const Board& board);
    Board(Board&& board);
    virtual ~Board();

    Board& operator=(const Board& board);
    Board& operator=(Board&& board);

    std::optional<int> get_cell_index(int base, Direction direction);

    Point from_index(int index) const;
    int from_point(const Point& point) const;
    int from_point(int column, int row) const;

    int width() const { return width_; }
    int height() const { return height_; }
    int init_bombs() const { return init_bombs_; }

    std::ostream& operator<<(std::ostream& os) const;
    std::ostream& show_game_state(std::ostream& os, bool disclose_bombs) const;

    virtual void open_cell(const Point& point);
    virtual void open_cell(int index);
    virtual void open_cell(int column, int row);

    bool failed() const { return failed_; }
    bool cleared() const;

    Cell& operator[](int index) { return cells_[index]; }
    const Cell& operator[](int index) const { return cells_.at(index); }
    Cell& operator[](const Point& point) { return cells_[from_point(point)]; }
    const Cell& operator[](const Point& point) const
    {
        return cells_.at(from_point(point));
    }

    void toggle_flag(int index);
    void toggle_flag(const Point& point);
    void toggle_flag(int xIndex, int yIndex);

    int get_total_cells() const { return height_ * width_; }
};

class LazyInitBoard : public Board {
protected:
    bool beforeInit = true;
    void generateActualBoard(int excludeCellIndex, bool openCell);
    bool ai_check;

    void initAll() override;

public:
    LazyInitBoard(int width, int height, int n_bombs, bool ai_check = false);
    LazyInitBoard(const LazyInitBoard& lb);
    LazyInitBoard(LazyInitBoard&& lb);
    virtual ~LazyInitBoard();

    LazyInitBoard& operator=(const LazyInitBoard& lb);
    LazyInitBoard& operator=(LazyInitBoard&& lb);

    virtual void open_cell(const Point& point) override;
    virtual void open_cell(int index) override;
    virtual void open_cell(int column, int row) override;
};
}
