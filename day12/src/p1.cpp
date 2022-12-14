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
    uint8_t x_{};
    uint8_t y_{};
    Coord() = default;
    Coord(const Coord&) = default;
    Coord(uint8_t x, uint8_t y)
    : x_ (x)
    , y_ (y)
    {}

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
    os << "(" << static_cast<int>(c.x_) << ", " << static_cast<int>(c.y_) << ")";
    return os;
}

class Map
{
public:
    using MapRow = std::vector<uint8_t>;
    Map(const char *filename)
    {
        map_.push_back(MapRow());
        std::ifstream istream(filename, std::ifstream::in);
        std::string line;
        while (getline(istream, line))
        {
            std::cout << "line = " << line << std::endl;
            map_.push_back(MapRow());
            auto &r = map_.back();
            r.push_back(std::numeric_limits<uint8_t>::max());
            for (const auto c : line)
            {
                if (islower(c))
                {
                    r.push_back(c - 'a');
                }
                else if (c == 'S')
                {
                    r.push_back(0);
                    
                }
                else if (c == 'E')
                {
                    end_coord_.x_ = r.size();
                    end_coord_.y_ = map_.size() - 1;
                    r.push_back('z' - 'a');
                }
                if (r.back() == 0)
                {
                    Coord start_coord (r.size() - 1, map_.size() - 1);
                    start_coords_.push_back(start_coord);
                }
            }
            r.push_back(std::numeric_limits<uint8_t>::max());
        }
        map_.push_back(MapRow(map_.back().size(), std::numeric_limits<uint8_t>::max()));
        map_[0] = map_.back();
        reset_min_path();

    }
    void reset_min_path(void)
    {
        min_path_length_to_coord_.clear();
        for (const auto &sc: start_coords_)
        {
            min_path_length_to_coord_[sc] = 0;
        }
    }
    std::vector<Coord> valid_moves(const Coord &origin, const size_t path_len) const
    {
        const std::array<Direction, 4> directions = {
            Direction{-1, 0},
            Direction{1, 0},
            Direction{0, -1},
            Direction{0, 1}
        };
        std::vector<Coord> ret;
        std::for_each(directions.cbegin(), directions.cend(), [&](const auto &d) {
            const Coord map_coord = origin + d;
            std::cout << "From " << origin << " = " << static_cast<int>(map_[origin.y_][origin.x_]) <<
            " to " << map_coord << " = " << static_cast<int>(map_[map_coord.y_][map_coord.x_]) << std::endl;

            if (map_[map_coord.y_][map_coord.x_] <= (map_[origin.y_][origin.x_] + 1))
            {
                auto it = min_path_length_to_coord_.find(map_coord);
                if (it != min_path_length_to_coord_.end())
                {
                    size_t prev_best = it->second;
                    std::cout << map_coord << " seen before, previous best len = " << prev_best << " this len = " << path_len << std::endl;
                    if (prev_best <= path_len)
                    {
                        return;
                    }
                    it->second = path_len + 1;
                }
                else
                {
                    std::cout << "Adding " << map_coord << " with length " << path_len << std::endl;
                    min_path_length_to_coord_[map_coord] = path_len;
                }
                std::cout << "From " << origin << " to " << map_coord << " is a valid move" << std::endl;
                ret.push_back(map_coord);
            }
        });
        return ret;
    }

    const std::vector<Coord>& start_coords() const
    {
        return start_coords_;
    }
    const Coord& end_coord() const
    {
        return end_coord_;
    }
    friend std::ostream& operator<<(std::ostream &os, const Map &m);
private:
    std::vector<MapRow> map_;
    std::vector<Coord> start_coords_;
    Coord end_coord_;
    mutable std::map<Coord, size_t> min_path_length_to_coord_;
};
std::ostream& operator<<(std::ostream &os, const Map &m)
{
    for (const auto &r : m.map_)
    {
        for (const auto c : r)
        {
            os << static_cast<int>(c) << " ";
        }
        os << std::endl;
    }
    os << "Start coords : ";
    for (const auto &sc : m.start_coords_)
    {
        os << sc << " ";
    }
    os << std::endl;
    os << "End coord = " << m.end_coord_ << std::endl;
    return os;
}

class Path
{
public:
    Path(const Coord &start){
        path_coords_.push_back(start);
    }
    Path(const Path &prev_moves, const Coord &new_move) 
    : path_coords_(prev_moves.path_coords_)
    {
        path_coords_.push_back(new_move);
    }
    bool operator<(const Path &other) const {
        return path_coords_.size() >  other.path_coords_.size();
    }
    const Coord& back() const{
        return path_coords_.back();
    }
    size_t length() const{
        return path_coords_.size();
    }
    void append(const Coord &coord) {
        path_coords_.push_back(coord);
    }
    void set_valid(const size_t len)
    {
        if (len <= path_coords_.size())
        {
            std::cout << "Set valid (1arg) : clearing path " << path_coords_.back() << " with len " << path_coords_.size() << std::endl;
            valid_ = false;
        }
    }
    void set_valid(const Coord &coord, const size_t len)
    {
        if (coord == path_coords_.back())
        {
            if (len <= path_coords_.size())
            {
                std::cout << "Set valid (2arg) : clearing path " << path_coords_.back() << " with len " << path_coords_.size() << std::endl;
                valid_ = false;
            }
        }
    }
    bool valid() const
    {
        return valid_;
    }
private:
    std::vector<Coord> path_coords_;
    bool valid_{true};
};

int main(int argc, char **argv)
{
    Map map(argv[1]);
    std::cout << "Map " << std::endl << map;
    size_t best_path_length = std::numeric_limits<size_t>::max();

    const auto &start_coords = map.start_coords();
    for (const auto &sc : start_coords)
    {
        std::deque<Path> paths;
        paths.push_back(Path(sc));

        while (paths.size())
        {
            const auto path = paths.front();
            paths.pop_front();
            if (!path.valid())
            {
                continue;
            }
            std::cout << "Next move from " << path.back() << std::endl;
            const auto next_moves = map.valid_moves(path.back(), path.length());
            for (const auto &m : next_moves)
            {
                std::cout << "    Next move = " << m << std::endl;
                if (m == map.end_coord())
                {
                    best_path_length = std::min(best_path_length, path.length());
                    for (auto &p : paths)
                    {
                        p.set_valid(best_path_length);
                    }
                    std::cout << "New path found, length = " << path.length() << std::endl;
                }
                else
                {
                    if (path.length() <= best_path_length)
                    {
                        for (auto &p : paths)
                        {
                            p.set_valid(m, path.length());
                        }
                        paths.push_back(Path(path, m));
                    }
                }
            }
        }
    }

    return 0;
}