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

class Node
{
public:
    Node() = default;
    Node(const std::string &line, uint8_t &valve_index)
    {
        name_ = line.substr(6,2);
        flow_rate_ = atoi(&line[line.find_first_of('=') + 1]);
        if (flow_rate_ > 0)
        {
            valve_index_ = valve_index++;
        }

        size_t idx = line.find_first_of(',') - 2;
        if (idx == (std::string::npos - 2))
        {
            idx = line.size() - 2;
        }
        while (idx < line.size())
        {
            edge_names_.push_back(line.substr(idx, 2));
            idx += 4;
        }
    }
    Node(const Node &other) = default;
    const std::string &get_name(void) const { return name_; }
    uint8_t get_flow_rate(void) const { return flow_rate_; }
    std::optional<uint32_t> get_valve_index(void) const
    {
        if (flow_rate_ == 0)
        {
            return std::nullopt;
        }
        return valve_index_;
    }
    friend std::ostream &operator<<(std::ostream &os, const Node &n)
    {
        os << " Node " << n.name_ << " valve_idx = " << static_cast<int>(n.valve_index_) << " flow_rate = " << static_cast<int>(n.flow_rate_) << " connections = ";
        for (const auto &en : n.edge_names_)
        {
            os << en << " ";
        }
        return os;

    }
private:
    uint8_t valve_index_{std::numeric_limits<uint8_t>::max()};
    uint8_t flow_rate_{0};
    //std::vector<const Node &> edges_;
    std::vector<std::string> edge_names_;
    std::string name_;
};

using NodeMap = std::map<std::string, Node>;
class Move
{
public:
    Move(const NodeMap &node_map)
    : current_node_{node_map.at("AA")}, prev_node_{node_map.at("AA")}
    {
        minute_ = 0;
    }
    Move(const NodeMap &node_map, const Move &prev_move, const std::string &next_node_str)
        : current_node_{node_map.at(next_node_str)}, prev_node_{node_map.at(prev_move.current_node_.get_name())}
    {
        minute_ += 1;
        // staying in same location, opening valve
        if (current_node_.get_name() == prev_node_.get_name())
        {
            if (prev_node_.get_name() != "AA")
            {
                valves_opened_ = prev_move.valves_opened_ | 1UL << *(current_node_.get_valve_index());
                if (prev_move.pressure_ == 0)
                {
                    pressure_ = current_node_.get_flow_rate();
                }
                else
                {
                    pressure_ = prev_move.pressure_ * current_node_.get_flow_rate();
                }
            }
        }
        if (prev_move.current_node_.get_flow_rate() == 0 ||
            ((*(prev_move.current_node_.get_valve_index())) & prev_move.valves_opened_))
        {
            prev_node_was_zero_ = true;
            prev_node_name_ = prev_node_.get_name();
        }
    }

private:
    uint8_t minute_;
    bool prev_node_was_zero_{false};
    uint16_t valves_opened_{0};
    uint32_t pressure_{0};
    const Node &prev_node_;
    const Node &current_node_;
    std::string prev_node_name_;
};


int main(int argc, char **argv)
{
    std::ifstream istream(argv[1], std::ifstream::in);
    std::string line;

    NodeMap nodes;
    uint8_t valve_idx = 0; // static class member?
    while (getline(istream, line))
    {
        Node n = Node(line, valve_idx);
        nodes[n.get_name()] = n;
        std::cout << n << std::endl;
    }
    return 0;
}