#include <algorithm>
#include <cstring>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <set>
#include <vector>

struct Direction
{
    int dx_;
    int dy_;

    Direction(int dx, int dy)
        : dx_(dx), dy_(dy)
    {
    }
};
struct Coord
{
    long x_{};
    long y_{};
    Coord() = default;
    Coord(const Coord&) = default;
    Coord(long x, size_t y)
    : x_ (x)
    , y_ (y)
    {}
    Coord(const char *str)
    {
        x_ = atol(str);
        const char *t = str + 1;
        while (*t != ',')
        {
            t++;
        }
        y_ = atol(t + 1);
    }

    bool operator==(const Coord &other) const {
        return (x_ == other.x_) && (y_ == other.y_);
    }
    bool operator< (const Coord &other) const {
        return (x_ < other.x_) || ((x_ == other.x_) && (y_ < other.y_));
    }
    Coord operator+(const Coord &other) const {
        return Coord(x_ + other.x_, y_ + other.y_);
    }
    Coord operator+(const Direction &other) const {
        return Coord(x_ + other.dx_, y_ + other.dy_);
    }
    friend std::ostream& operator<<(std::ostream &os, const Coord &c);
};
std::ostream& operator<<(std::ostream &os, const Coord &c)
{
    os << "(" << c.x_ << ", " << c.y_ << ")";
    return os;
}

class Map
{
public:
    Map(const char *filename)
    {
        std::ifstream istream(filename, std::ifstream::in);
        std::string line;

        while (getline(istream, line))
        {
            const char *delimiter{" ->"};
            char *token = std::strtok(line.data(), delimiter);
            Coord prev_coord(token);
            update_tl_br(prev_coord);
            token = strtok(nullptr, delimiter);
            while (nullptr != token)
            {
                Coord coord(token);
                add_to_occupied(prev_coord, coord);
                prev_coord = coord;
                token = strtok(nullptr, delimiter);
                update_tl_br(coord);
            }
        }
        update_tl_br(Coord(500,0)); // sand starting position - not sure if needed?
    }
    void clear_sand()
    {
        sand_pos_.clear();
    }
    void add_floor()
    {
        // Can probably be more accurate by adding the height to the current bounding box
        // (or maybe from the center), under the assumption that the sand will look like a 
        // pyramid when the room is full
        constexpr long large = 2500;
        add_to_occupied(Coord(-large, br_.y_ + 2),
                        Coord(large, br_.y_ + 2));
        update_tl_br(Coord(-large, br_.y_ + 3));
        update_tl_br(Coord(large, br_.y_ + 3));
    }

    bool add_sand(Coord coord)
    {
        while (move_sand_one_step(coord))
        {
            if (coord.y_ > br_.y_)
            {
                return false;
            }
        }
        if (coord == Coord(500,0))
        {
            return false;
        }
        sand_pos_.insert(coord);
        return true;
    }
    friend std::ostream& operator<<(std::ostream &os, const Map &m);
private:
    void update_tl_br(const Coord &coord)
    {
        // std::cout << " coord = " << coord << " tl = " << tl_ << " br = " << br_ << std::endl;
        tl_.x_ = std::min(tl_.x_, coord.x_);
        tl_.y_ = std::min(tl_.y_, coord.y_);
        br_.x_ = std::max(br_.x_, coord.x_);
        br_.y_ = std::max(br_.y_, coord.y_);
    }
    void add_to_occupied(const Coord &c1, const Coord &c2)
    {
        Coord start = std::min(c1, c2);
        Coord end = std::max(c1, c2);
        std::cout << "add_to_occupied " << start << " -> " << end << std::endl;
        if (start.x_ == end.x_)
        {
            for (long y = start.y_; y <= end.y_; y++)
            {
                occupied_.insert(Coord(start.x_, y));
            }
        }
        else
        {
            for (long x = start.x_; x <= end.x_; x++)
            {
                occupied_.insert(Coord(x, start.y_));
            }
        }
    }
    bool move_sand_one_step(Coord &coord) const
    {
        const std::array<Direction, 3> directions = {
            Direction{0, 1},
            Direction{-1, 1},
            Direction{1, 1}
        };
        for (const auto &d : directions)
        {
            Coord new_pos = coord + d;
            if (!occupied_.count(new_pos) && !sand_pos_.count(new_pos))
            {
                coord = new_pos;
                return true;
            }
        }
        return false;
    }

    std::set<Coord> occupied_;
    std::set<Coord> sand_pos_;
    Coord tl_{std::numeric_limits<long>::max(), std::numeric_limits<long>::max()};
    Coord br_{0,0};
};

std::ostream& operator<<(std::ostream &os, const Map &m)
{
    for (long y = m.tl_.y_; y <= m.br_.y_; y++)
    {
        for (long x = m.tl_.x_; x <= m.br_.x_; x++)
        {
            if (m.occupied_.count(Coord(x, y)))
            {
                os << '#';
            }
            else if (m.sand_pos_.count(Coord(x, y)))
            {
                os << 'o';
            }
            else
            {
                os << '.';
            }
        }
        os << std::endl;
    }
    return os;
}

int main(int argc, char **argv)
{
    Map map(argv[1]);
    std::cout << map << std::endl;

    size_t count = 0;
    while (map.add_sand(Coord(500,0)))
    {
        //std::cout << map << std::endl;
        count += 1;
    }
    std::cout << "Count = " << count << std::endl;

    map.clear_sand();
    map.add_floor();
    count = 0;
    while (map.add_sand(Coord(500,0)))
    {
        //std::cout << map << std::endl;
        count += 1;
    }
    std::cout << "Count = " << count + 1 << std::endl;

    return 0;
}