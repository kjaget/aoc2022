#include <algorithm>
#include <cstring>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <numeric>
#include <limits>
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
public:
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
    long l1_distance(const Coord &other) const
    {
        return std::abs(x_ - other.x_) +
               std::abs(y_ - other.y_);
    }
    friend std::ostream& operator<<(std::ostream &os, const Coord &c);
};
std::ostream& operator<<(std::ostream &os, const Coord &c)
{
    os << "(" << c.x_ << ", " << c.y_ << ")";
    return os;
}

template <typename T>
class RangeEl
{
public:
    RangeEl(const T start, const T end)
    : start_(start)
    , end_(end)
    {
    }
    bool operator<(const RangeEl<T> &other)
    {
        return (start_ < other.start_) || ((start_ == other.start_) && (end_ < other.end_));
    }
    size_t size(void) const
    {
        return end_ - start_ + 1;
    }

    bool adjacent_or_in(const T val) const
    {
        return (val >= (start_ - 1)) && (val <= (end_ + 1));
    }
    
    long start_;
    long end_;
};

template <typename T>
class Range
{
public:
    Range() = default;
    void insert(const T val)
    {
        insert(val, val);
    }
    void insert(const T start, const T end)
    {
        for (size_t i = 0; i < range_.size(); i++)
        {
            if (range_[i].adjacent_or_in(start) || range_[i].adjacent_or_in(end))
            {
                range_[i].start_ = std::min(start, range_[i].start_);
                range_[i].end_ = std::max(end, range_[i].end_);
                while ((i > 0) && (range_[i-1].end_ >= range_[i].start_))
                {
                    range_[i].start_ = std::min(range_[i].start_, range_[i-1].start_);
                    range_.erase(range_.begin() + i - 1);
                }
                while ((i < (range_.size() - 1)) && (range_[i].end_ >= range_[i+1].start_))
                {
                    range_[i].end_ = std::max(range_[i].end_, range_[i+1].end_);
                    range_.erase(range_.begin() + i + 1);
                }
                return;
            }
        }
        range_.push_back(RangeEl(start, end));
        std::sort(range_.begin(), range_.end());
    }
    void insert(const std::vector<RangeEl<T>>::const_iterator begin,
                const std::vector<RangeEl<T>>::const_iterator end)
    {
        for (auto it = begin; it != end; ++it)
        {
            insert(it->start_, it->end_);
        }
    }
    void erase(const T val)
    {
        for (size_t i = 0; i < range_.size(); i++)
        {
            if (val == range_[i].start_)
            {
                range_[i].start_ += 1;
                if (range_[i].start_ > range_[i].end_)
                {
                    range_.erase(range_.begin() + i);
                }
                return;
            }
            else if (val == range_[i].end_)
            {
                range_[i].end_ -= 1;
                if (range_[i].start_ > range_[i].end_)
                {
                    range_.erase(range_.begin() + i);
                }
                return;
            }
            else if ((val > range_[i].start_) && (val < range_[i].end_))
            {
                const auto prev_end = range_[i].end_;
                range_[i].end_ = val - 1;
                range_.push_back(RangeEl(val + 1, prev_end));
                std::sort(range_.begin(), range_.end());
                return;
            }
        }
    }
    std::vector<RangeEl<T>>::const_iterator cbegin(void) const
    {
        return range_.cbegin();
    }
    std::vector<RangeEl<T>>::const_iterator cend(void) const
    {
        return range_.cend();
    }
    size_t size(void) const
    {
        return std::accumulate(range_.cbegin(), range_.cend(), 0UL, [](size_t a, const RangeEl<T> &t)
                               { return a + t.size(); }
        );
    }
private:
    std::vector<RangeEl<T>> range_;
};

class SensorAndBeacon
{
public:
    SensorAndBeacon(const std::string &str)
    {
        const auto sensor_x_pos = str.find_first_of('=') + 1;
        const auto sensor_y_pos = str.find_first_of('=', sensor_x_pos) + 1;
        sensor_ = Coord(atol(&str[sensor_x_pos]), atol(&str[sensor_y_pos]));

        const auto beacon_x_pos = str.find_first_of('=', sensor_y_pos) + 1;
        const auto beacon_y_pos = str.find_first_of('=', beacon_x_pos) + 1;

        beacon_ = Coord(atol(&str[beacon_x_pos]), atol(&str[beacon_y_pos]));
    }
    const Coord &beacon() const { return beacon_;}
    const Coord &sensor() const { return sensor_;}
    long l1_distance() const 
    {
        return beacon_.l1_distance(sensor_);
    }
    Range<long> not_on_row(const long row) const
    {
        Range<long> ret;
        const auto l1_d = l1_distance();
        for (long r = (sensor_.y_ - l1_d); r <= (sensor_.y_ + l1_d); r++)
        {
            if (r == row)
            {
                //std::cout << "Sensor at " << sensor_ << " hits row " << row << std::endl;
                for (long c = (sensor_.x_ - l1_d); c <= (sensor_.x_ + l1_d); c++)
                {
                    if (sensor_.l1_distance(Coord(c, r)) <= l1_d)
                    {
                        //std::cout << "  adding " << c << " to set" << std::endl;
                        ret.insert(c);
                    }
                }
            }
        }
        return ret;
    }
    void remove_sensor_and_beacon_from_set(Range<long> &s, const long row) const
    {
        if (sensor_.y_ == row)
        {
            s.erase(sensor_.x_);
        }
        if (beacon_.y_ == row)
        {
            s.erase(beacon_.x_);
        }
        //std::cout << "Removing " << sensor_.x_ << " and " << beacon_.x_ << std::endl;
    }
    friend std::ostream &operator<<(std::ostream &os, const SensorAndBeacon &sb);

private:
    Coord beacon_;
    Coord sensor_;
};
std::ostream& operator<<(std::ostream &os, const SensorAndBeacon &sb)
{
    os << "Sensor at " << sb.sensor_ << ", beacon at " << sb.beacon_ << " distance = " << sb.l1_distance();
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
            sensor_and_beacons_.emplace_back(line);
        }
    }
    Range<long> not_on_row(long row) const
    {
        Range<long> ret;
        for (const auto &sb : sensor_and_beacons_)
        {
            const auto r = sb.not_on_row(row);
            
            ret.insert(r.cbegin(), r.cend());
            #if 0
            std::copy(r.begin(), r.end(), std::ostream_iterator<long>(std::cout, " "));
            std::cout << std::endl;
            std::copy(ret.begin(), ret.end(), std::ostream_iterator<long>(std::cout, " "));
            std::cout << std::endl;
            #endif
        }
        std::cout <<" before " << ret.size() << std::endl;
        for (const auto &sb : sensor_and_beacons_)
        {
            sb.remove_sensor_and_beacon_from_set(ret, row);
            #if 0
            std::copy(ret.begin(), ret.end(), std::ostream_iterator<long>(std::cout, " "));
            std::cout << std::endl;
            #endif
        }
        std::cout <<" after " << ret.size() << std::endl;
        return ret;
    }

friend std::ostream& operator<<(std::ostream &os, const Map &m);
private:
    std::vector<SensorAndBeacon> sensor_and_beacons_;
};

std::ostream& operator<<(std::ostream &os, const Map &m)
{
    for (const auto &sb : m.sensor_and_beacons_)
    {
        std::cout << sb << std::endl;
    }
    return os;
}

int main(int argc, char **argv)
{
    Map map(argv[1]);
    std::cout << map << std::endl;
    std::cout << "Num invalid spaces in row " << map.not_on_row(atoi(argv[2])).size() << std::endl;
    return 0;
}