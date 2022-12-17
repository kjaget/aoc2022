#include <algorithm>
#include <cstring>
#include <deque>
#include <fstream>
#include <functional>
#include <ios>
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

using Pressure = uint64_t;
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
    const std::vector<std::string> &get_edge_names(void) const { return edge_names_; }
    std::optional<uint32_t> get_valve_index(void) const
    {
        if (flow_rate_ == 0)
        {
            return std::nullopt;
        }
        return valve_index_;
    }
    bool is_valve_opened(uint32_t valve_mask) const
    {
        if (valve_index_ == std::numeric_limits<uint8_t>::max())
        {
            std::cout << "is_valve_opened called on Node with flow == 0"<< std::endl;
            return false;
        }
        uint32_t valve_bit = 1UL << valve_index_;
        return (valve_mask & valve_bit) == valve_bit;
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
    std::vector<std::string> edge_names_;
    std::string name_;
};

using NodeMap = std::map<std::string, Node>;
class Move
{
public:
    Move(const NodeMap &node_map)
    : current_node_{&node_map.at("AA")}
    {
        minute_ = 0;
        prev_node_name_ = "AA";
    }
    Move(const NodeMap &node_map, const uint8_t max_minutes, const Move &prev_move, const std::string &next_node_str)
        : current_node_{&node_map.at(next_node_str)}
    {
        const Node &prev_node = node_map.at(prev_move.current_node_->get_name());
        pressure_ = prev_move.pressure_;
        valves_opened_ = prev_move.valves_opened_;
        minute_ = prev_move.minute_ + 1;
        // staying in same location, opening valve
        if (current_node_->get_name() == prev_node.get_name())
        {
            valves_opened_ |= 1UL << *(current_node_->get_valve_index());
            pressure_ += current_node_->get_flow_rate() * (max_minutes - prev_move.minute_);
            prev_node_was_zero_ = prev_move.prev_node_was_zero_;
            prev_node_name_ = prev_move.prev_node_name_;
        }
        else if (prev_move.current_node_->get_flow_rate() == 0 ||
                prev_move.current_node_->is_valve_opened(valves_opened_))
        {
            prev_node_was_zero_ = true;
            prev_node_name_ = prev_node.get_name();
        }
        else
        {
            prev_node_name_ = prev_node.get_name();
        }
    }
    std::vector<Move> next_moves(const NodeMap &node_map, const uint8_t max_minutes) const
    {
        std::vector<Move> ret;

        const auto &edge_names = current_node_->get_edge_names();
        for (const auto &e : edge_names)
        {
            // Avoid loopoing over nodes with 0 flow rate
            if (prev_node_was_zero_ && (prev_node_name_ == e))
            {
                continue;
            }
            ret.emplace_back(Move(node_map, max_minutes, *this, e));
        }

        // If there's a valve to open that hasn't been, one new move is
        // to open it
        const auto valve_index = current_node_->get_valve_index();
        if (valve_index)
        {
            const uint32_t valve_mask = 1UL << *valve_index;
            if ((valve_mask & valves_opened_) == 0)
            {
                ret.emplace_back(Move(node_map, max_minutes, *this, current_node_->get_name()));
            }
        }

        return ret;
    }
    std::optional<Pressure> final_score(size_t minutes) const
    {
        if (minute_ == minutes)
        {
            return pressure_;
        }
        return std::nullopt;
    }

    friend std::ostream &operator<<(std::ostream &os, const Move &m)
    {
        os << "minute = " << static_cast<int>(m.minute_) << " prev_node_was_zero = " << m.prev_node_was_zero_ << " prev_node_name = " << m.prev_node_name_ << " pressure = " << m.pressure_<< " valves_opened = " << std::hex << m.valves_opened_ << std::dec << " score = " << m.score() << " current_node = " << *m.current_node_;
        return os;
    }
    long score(void) const
    {
        long score = pressure_;
        score -= static_cast<long>(minute_) * 5;
        auto flow_rate = current_node_->get_flow_rate();
        if (flow_rate && !current_node_->is_valve_opened(valves_opened_))
        {
            score += (30 - (minute_ - 1)) * flow_rate;
        }

        return score;
    }

    bool operator<(const Move &other) const
    {
        return score() < other.score();
    }
private:
    uint8_t minute_;
    bool prev_node_was_zero_{false};
    uint16_t valves_opened_{0};
    Pressure pressure_{};
    const Node *current_node_;
    std::string prev_node_name_;
};

int main(int argc, char **argv)
{
    std::ifstream istream(argv[1], std::ifstream::in);
    std::string line;
    constexpr size_t minutes = 30;

    NodeMap nodes;
    uint8_t valve_idx = 0; // static class member?
    while (getline(istream, line))
    {
        Node n = Node(line, valve_idx);
        nodes[n.get_name()] = n;
        std::cout << n << std::endl;
    }

    std::priority_queue<Move> moves;
    moves.push(Move(nodes));

    Pressure best_pressure = 0;
    while(!moves.empty())
    {
        const auto move = moves.top();
        moves.pop();
        //std::cout << "Move = " << move << std::endl;
        const auto this_pressure = move.final_score(minutes);
        if (this_pressure)
        {
            if (*this_pressure > best_pressure)
            {
                best_pressure = std::max(best_pressure, *this_pressure);
                std::cout << "New best pressure = " << best_pressure << std::endl;
            }
            continue;
        }

        const auto next_moves = move.next_moves(nodes, minutes);
        for (const auto &nm : next_moves)
        {
            //std::cout << "    Next move = " << nm << std::endl;
            moves.push(nm);
        }
    }

    return 0;
}