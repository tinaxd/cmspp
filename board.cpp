#include "board.h"
#include <array>
#include <random>
#include <stdexcept>
#include <string>
#include <tuple>

using namespace minesweeper;

Board::Board(int width, int height, int n_bombs, bool initialize)
    : width_(width)
    , height_(height)
    , init_bombs_(n_bombs)
{
    if (width <= 0 || height <= 0 || n_bombs < 0 || n_bombs >= width * height) {
        throw std::runtime_error("illegal arguments");
    }
    if (initialize) {
        setup_cells(width, height, n_bombs);
        build_neighbor_map();
    } else {
        auto cells = width * height;
        for (auto i = 0; i < cells; i++) {
            cells_.append(Cell(false));
        }
    }
}

Board::Board(int width, int height, int n_bombs, const QVector<int>& excludes)
    : Board(width, height, n_bombs, false)
{
    if (n_bombs >= width * height - excludes.size()) {
        throw std::runtime_error("illegal arguments");
    }
    setup_cells(width, height, n_bombs, excludes);
    build_neighbor_map();
}

Board::Board(int width, int height, int n_bombs, const QVector<Board::Point>& excludes)
    : Board(width, height, n_bombs, false)
{
    if (n_bombs >= width * height - excludes.size()) {
        throw std::runtime_error("illegal arguments");
    }
    QVector<int> excludes_index;
    for (const auto& ex : excludes) {
        excludes_index.append(from_point(ex));
    }
    setup_cells(width, height, n_bombs, excludes_index);
    build_neighbor_map();
}

Board::Board(const Board& board)
    : width_(board.width_)
    , height_(board.height_)
    , init_bombs_(board.init_bombs_)
    , failed_(board.failed_)
    , cells_(board.cells_)
{
}

Board::Board(Board&& board)
    : width_(board.width_)
    , height_(board.height_)
    , init_bombs_(board.init_bombs_)
    , failed_(board.failed_)
    , cells_(std::move(board.cells_))
{
}

Board::~Board()
{
}

Board&
Board::operator=(const Board& board)
{
    width_ = board.width_;
    height_ = board.height_;
    init_bombs_ = board.init_bombs_;
    failed_ = board.failed_;
    cells_ = board.cells_;
    return *this;
}

Board&
Board::operator=(Board&& board)
{
    width_ = board.width_;
    height_ = board.height_;
    init_bombs_ = board.init_bombs_;
    failed_ = board.failed_;
    cells_ = std::move(board.cells_);
    return *this;
}

void Board::setup_cells(int width, int height, int n_bombs)
{
    setup_cells(width, height, n_bombs, QVector<int>());
}

void Board::setup_cells(int width, int height, int n_bombs, const QVector<int>& excludes)
{
    std::random_device seeder;
    std::mt19937 random;
    random.seed(seeder());

    auto cells = width * height;
    for (auto i = 0; i < cells; i++) {
        cells_.append(Cell(false));
    }

    QVector<size_t> bomb_indices;
    for (auto i = 0; i < cells; i++) {
        if (!excludes.contains(i)) {
            bomb_indices.append(i);
        }
    }

    for (auto i = 0; i < n_bombs; i++) {
        auto rand = random() % bomb_indices.size();
        auto index = bomb_indices.at(rand);
        bomb_indices.erase(bomb_indices.begin() + rand);
        cells_[index].has_bomb() = true;
    }
}

void Board::build_neighbor_map()
{
    std::array<Direction, 8> dirs = { Direction::LeftUp, Direction::Up,
        Direction::RightUp, Direction::Left,
        Direction::Right, Direction::LeftDown,
        Direction::Down, Direction::RightDown };
    for (auto i = 0; i < get_total_cells(); i++) {
        int bombs = 0;
        for (auto dir : dirs) {
            auto index = get_cell_index(i, dir);
            if (index.has_value() && cells_.at(index.value()).has_bomb()) {
                bombs++;
            }
        }
        cells_[i].set_neighbor_bombs(bombs);
    }
}

std::optional<int>
Board::get_cell_index(int base, Direction direction)
{
    auto tmp = from_index(base);
    int column = tmp.first;
    int row = tmp.second;
    switch (direction) {
    case Direction::LeftUp:
        if (column <= 0 || row <= 0)
            return std::optional<int>();
        return from_point(column - 1, row - 1);
    case Direction::Up:
        if (row <= 0)
            return std::optional<int>();
        return from_point(column, row - 1);
    case Direction::RightUp:
        if (column >= width_ - 1 || row <= 0)
            return std::optional<int>();
        return from_point(column + 1, row - 1);
    case Direction::Left:
        if (column <= 0)
            return std::optional<int>();
        return from_point(column - 1, row);
    case Direction::Right:
        if (column >= width_ - 1)
            return std::optional<int>();
        return from_point(column + 1, row);
    case Direction::LeftDown:
        if (column <= 0 || row >= height_ - 1)
            return std::optional<int>();
        return from_point(column - 1, row + 1);
    case Direction::Down:
        if (row >= height_ - 1)
            return std::optional<int>();
        return from_point(column, row + 1);
    case Direction::RightDown:
        if (column >= width_ - 1 || row >= height_ - 1)
            return std::optional<int>();
        return from_point(column + 1, row + 1);
    }
    throw std::logic_error("unreachable");
}

Board::Point
Board::from_index(int index) const
{
    return qMakePair<int, int>(index % width_, index / height_);
}

int Board::from_point(const Board::Point& point) const
{
    return from_point(point.first, point.second);
}

int Board::from_point(int column, int row) const
{
    return column + row * width_;
}

std::ostream&
Board::operator<<(std::ostream& os) const
{
    return show_game_state(os, true);
}

std::ostream&
Board::show_game_state(std::ostream& os, bool disclose_bombs) const
{
    for (auto i = 0; i < width_ * height_; i++) {
        if (i != 0 && i % width_ == 0) {
            os << '\n';
        }
        os << char_of_cell(cells_.at(i), disclose_bombs);
        os << ' ';
    }
    return os;
}

char Board::char_of_cell(const Cell& c, bool disclose_bomb)
{
    if (c.flagged()) {
        return disclose_bomb && !c.has_bomb() ? 'f' : 'F';
    }

    if (disclose_bomb && c.has_bomb()) {
        return '+';
    }

    if (c.opened()) {
        if (c.neighbor_bombs() == 0) {
            return ' ';
        } else {
            return std::to_string(c.neighbor_bombs()).at(0);
        }
    }

    return 'O';
}

void Board::open_cell(const Point& point)
{
    open_cell(from_point(point));
}

void Board::open_cell(int index)
{
    if (cells_.at(index).has_bomb()) {
        failed_ = true;
        return;
    }
    open_cell4(index);
}

void Board::open_cell(int column, int row)
{
    open_cell(from_point(column, row));
}

void Board::open_cell4(int index)
{
    auto& cell = cells_[index];
    if (cell.has_bomb() || cell.opened() || cell.flagged()) {
        // do not disclose.
        return;
    }

    cell.state() = CellState::Opened;

    if (cell.neighbor_bombs() > 0) {
        // disclose this cell, but no neighbor cells.
        return;
    }

    std::array<Direction, 4> dirs = {
        Direction::Up, Direction::Left, Direction::Right, Direction::Down
    };
    for (auto dir : dirs) {
        auto next_index = get_cell_index(index, dir);
        if (next_index.has_value()) {
            open_cell4(next_index.value());
        }
    }
}

bool Board::cleared() const
{
    for (const auto& cell : cells_) {
        if ((!cell.has_bomb() && !cell.opened()) || (cell.has_bomb() && cell.opened())) {
            return false;
        }
    }
    return true;
}

void Board::toggle_flag(int index)
{
    auto& cell = cells_[index];
    switch (cell.state()) {
    case CellState::Opened:
        return;
    case CellState::Closed:
        cell.state() = CellState::Flagged;
        break;
    case CellState::Flagged:
        cell.state() = CellState::Closed;
        break;
    }
}

void Board::toggle_flag(const Point& point)
{
    toggle_flag(from_point(point));
}

void Board::toggle_flag(int column, int row)
{
    toggle_flag(from_point(column, row));
}

void LazyInitBoard::generateActualBoard(int excludeCellIndex)
{
    setup_cells(width_, height_, init_bombs_, QVector<int> { excludeCellIndex });
    build_neighbor_map();
}

LazyInitBoard::LazyInitBoard(int width, int height, int n_bombs)
    : Board(width, height, n_bombs, false)
{
}

LazyInitBoard::~LazyInitBoard()
{
}

void LazyInitBoard::open_cell(const Board::Point& point)
{
    open_cell(from_point(point));
}

void LazyInitBoard::open_cell(int index)
{
    if (beforeInit) {
        generateActualBoard(index);
        beforeInit = false;
    }
    Board::open_cell(index);
}

void LazyInitBoard::open_cell(int column, int row)
{
    open_cell(from_point(column, row));
}
