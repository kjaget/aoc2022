#include <algorithm>
#include <fstream>
#include <functional>
#include <numeric>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>


class ForestMap
{
public:
    ForestMap(const char *filename)
    {
        std::ifstream istream(filename, std::ifstream::in);

        std::string str;
        map_.push_back(ForestMapRow{});
        while (getline(istream, str))
        {
            map_.push_back(ForestMapRow{});
            auto &row = map_.back();
            row.push_back(0);
            for (const auto s: str)
            {
                if (!isdigit(s))
                {
                    break;
                }
                row.push_back(s - '0');
            }
            row.push_back(0);
        }
        map_.push_back(ForestMapRow(map_[map_.size() - 2].size(), 0));
        map_[0] = map_.back();

        generate_height_maps();
        generate_vis_map();
    }

    size_t count(void) const
    {
        return std::accumulate(vis_map_.cbegin(), vis_map_.cend(), 0,
                [&](int init, const std::vector<bool>& boolvec){ // binaryOp that sums a single vector<bool>
                    return std::accumulate(
                        boolvec.begin(), boolvec.end(), // iterators for the inner vector
                        init);
                    }
        );

    }

    void print(void) const
    {
        std::cout  << "Map:" << std::endl;
        print_vec_vec<uint8_t, uint16_t>(map_);
        for (size_t i = 0; i < height_maps_.size(); i++)
        {
            std::cout << "HM[" << i << "]: dr = " << directions_[i].dr_ << " dc = " << directions_[i].dc_ << std::endl;
            print_vec_vec<uint8_t, uint16_t>(height_maps_[i]);
        }
        std::cout << "Vis" << std::endl;
        print_vec_vec<bool, uint16_t>(vis_map_);
    }

private:
    template <typename VecT, typename PrintT>
    void print_vec_vec(const std::vector<std::vector<VecT>> &vec) const
    {
        for (const auto &r : vec)
        {
            std::copy(r.begin(), r.end(), std::ostream_iterator<PrintT>(std::cout, ""));
            std::cout << std::endl;
        }
    }
    using ForestMapRow = std::vector<uint8_t>;
    using ForestMapType =std::vector<ForestMapRow>;
    ForestMapType map_;
    std::array<ForestMapType, 4> height_maps_;
    
    std::vector<std::vector<bool>> vis_map_;

    struct Direction
    {
        int dr_;
        int dc_;

        Direction(int dr, int dc)
            : dr_(dr)
            , dc_(dc)
        {}
    };
    std::array<Direction, 4> directions_ = {
        Direction{-1, 0},
        Direction{1, 0},
        Direction{0, -1},
        Direction{0, 1}
    };

    void generate_height_maps(void)
    {
        for (size_t d = 0; d < directions_.size(); d++)
        {
            const auto &dir = directions_[d];
            generate_height_maps_worker(height_maps_[d], dir.dr_, dir.dc_, (dir.dr_ < 0) || (dir.dc_ < 0));
        }
    }

    void generate_height_maps_worker(ForestMapType &hm, const int dr, const int dc, const bool forward)
    {
        hm.clear();
        const size_t rows = map_.size() - 1;
        const size_t cols = map_[0].size() - 1;
        for (size_t r = 0; r < map_.size(); r++)
        {
            hm.push_back(map_[0]);
        }

        if (forward)
        {
            for (size_t r = 1; r < rows; r++)
            {
                auto &row = hm[r];
                for (size_t c = 1; c < cols; c++)
                {
                    row[c] = std::max(map_[r + dr][c + dc], hm[r + dr][c + dc]);
                }
            }
        }
        else
        {
            for (size_t r = rows-1; r > 0; r--)
            {
                auto &row = hm[r];
                for (size_t c = cols-1; c > 0; c--)
                {
                    row[c] = std::max(map_[r + dr][c + dc], hm[r + dr][c + dc]);
                }
            }

        }
    }

    void generate_vis_map(void)
    {
        const size_t rows = map_.size() - 1;
        const size_t cols = map_[0].size() - 1;
        vis_map_.push_back(std::vector<bool>(map_[0].size(), false));
        vis_map_.push_back(std::vector<bool>(map_[0].size(), true));
        vis_map_.back().front() = false;
        vis_map_.back().back() = false;
        for (size_t r = 2; r < map_.size() - 2; r++)
        {
            vis_map_.push_back(std::vector<bool>(map_[0].size(), false));
        }
        vis_map_.push_back(std::vector<bool>(map_[0].size(), true));
        vis_map_.back().front() = false;
        vis_map_.back().back() = false;
        vis_map_.push_back(std::vector<bool>(map_[0].size(), false));
        for (size_t r = 1; r < rows - 1; r++)
        {
            vis_map_[r][1] = true;
            vis_map_[r][vis_map_[0].size() - 2] = true;
        }

        for (size_t r = 2; r < rows - 1; r++)
        {
            auto &row = vis_map_[r];
            auto &m_row = map_[r];
            for (size_t c = 2; c < cols - 1; c++)
            {
                for (const auto &hm : height_maps_)
                {
                    row[c] = row[c] || (m_row[c] > hm[r][c]);
                }
            }
        }
    }
};

int main(int argc, char **argv)
{
    ForestMap forest_map(argv[1]);
    forest_map.print();
    std::cout << "Count = " << forest_map.count() << std::endl;
}