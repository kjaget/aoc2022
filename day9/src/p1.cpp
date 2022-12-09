#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <set>

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
private:
    long x_;
    long y_;
public:
    Coord(long x, long y) 
    :x_(x)
    ,y_(y)
    {}

    bool operator==(const Coord &other) const
    {
        return (x_ == other.x_) && (y_ == other.y_);
    }

    const Coord& operator+=(const Direction &dir)
    {
        x_ += dir.dx_;
        y_ += dir.dy_;
        return *this;
    }

    bool operator<(const Coord &other) const
    {
        return (x_ < other.x_) || ((x_ == other.x_) && (y_ < other.y_));
    }

    Coord move_tail(const Coord &tail_coords)
    {
        if (tail_coords == *this)
        {
            return tail_coords;
        }
        if (y_ == tail_coords.y_)
        {
            if (tail_coords.x_ == (x_ - 2))
            {
                return Coord(x_ - 1, y_);
            }
            if (tail_coords.x_ == (x_ + 2))
            {
                return Coord(x_ + 1, y_);
            }
            return tail_coords;
        }
        if (x_ == tail_coords.x_)
        {
            if (tail_coords.y_ == (y_ - 2))
            {
                return Coord(x_, y_ - 1);
            }
            if (tail_coords.y_ == (y_ + 2))
            {
                return Coord(x_, y_ + 1);
            }
            return tail_coords;
        }
        if (std::abs(tail_coords.x_ - x_) == 1)
        {
            if (tail_coords.y_ == (y_ - 2))
            {
                return Coord(x_, y_ - 1);
            }
            if (tail_coords.y_ == (y_ + 2))
            {
                return Coord(x_, y_ + 1);
            }
            return tail_coords;
        }
        if (std::abs(tail_coords.y_ - y_ ) == 1)
        {
            if (tail_coords.x_ == (x_ - 2))
            {
                return Coord(x_- 1, y_);
            }
            if (tail_coords.x_ == (x_ + 2))
            {
                return Coord(x_+ 1, y_);
            }
            return tail_coords;
        }
        if ((tail_coords.x_ - x_) == 2)
        {
            if (tail_coords.y_ < y_)
            {
                return Coord(x_ + 1, y_ - 1);
            }
            return Coord(x_ + 1, y_ + 1);
        }
        if ((tail_coords.x_ - x_) == -2)
        {
            if (tail_coords.y_ < y_)
            {
                return Coord(x_ - 1, y_ - 1);
            }
            return Coord(x_ - 1, y_ + 1);
        }
        return tail_coords;
    }
    friend std::ostream& operator<<(std::ostream& os, const Coord &c);
};

std::ostream& operator<<(std::ostream& os, const Coord &c)
{
    os << "(" << c.x_ << ", " << c.y_ << ")";
    return os;
}

int main(int argc, char **argv)
{
    std::map<char, Direction> directions =
    {
        {'R', Direction(1, 0)},
        {'L', Direction(-1, 0)},
        {'U', Direction(0,-1)},
        {'D', Direction(0,1)}
    };


    Coord head_pos(0,0);
    Coord tail_pos(0,0);
    std::set<Coord> tail_visits;
    tail_visits.insert(tail_pos);
    std::array<Coord, 10> snake = {Coord(0,0),Coord(0,0),Coord(0,0),Coord(0,0),Coord(0,0),Coord(0,0),Coord(0,0),Coord(0,0),Coord(0,0),Coord(0,0)};
    std::set<Coord> snake_visits;
    snake_visits.insert(snake.back());

    std::ifstream istream(argv[1], std::ifstream::in);
    std::string str;
    while (getline(istream, str))
    {
        const auto &dir = directions.at(str[0]);
        size_t count = atol(&str[2]);
        for (size_t i = 0; i < count; i++)
        {
            head_pos += dir;
            tail_pos = head_pos.move_tail(tail_pos);
            //std::cout << "Moved " << str[0] << " head_pos = " << head_pos << " tail_pos = " << tail_pos << std::endl;
            tail_visits.insert(tail_pos);
            snake[0] += dir;
            for (size_t i = 1; i < snake.size(); i++)
            {
                snake[i] = snake[i-1].move_tail(snake[i]);
            }
            #if 0
            std::cout << "Moved " << str[0] << " ";
            std::copy(snake.cbegin(), snake.cend(), std::ostream_iterator<Coord>(std::cout, " "));
            std::cout << std::endl;
            #endif
            snake_visits.insert(snake.back());
        }
    }
    std::cout << "Tail count = " << tail_visits.size() << std::endl;
    std::cout << "Snake count = " << snake_visits.size() << std::endl;
    return 0;
}