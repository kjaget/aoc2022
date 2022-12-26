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
#include <thread>
#include <vector>

//#define SAVE_MOVES
using Pressure = uint64_t;
class Node
{
public:
    Node() = default;
    Node(const std::string &line)
    {
        name_ = line.substr(6,2);
        flow_rate_ = atoi(&line[line.find_first_of('=') + 1]);
        if (flow_rate_ > 0)
        {
            valve_index_ = last_valve_index_++;
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
    static bool all_valves_opened(uint32_t valve_mask)
    {
        uint32_t valve_bits = (1UL << last_valve_index_) - 1;
        return (valve_mask & valve_bits) == valve_bits;
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
    static uint8_t get_last_valve_index(void) { return last_valve_index_;}
private:
    uint8_t valve_index_{std::numeric_limits<uint8_t>::max()};
    uint8_t flow_rate_{0};
    std::vector<std::string> edge_names_;
    std::string name_;
    static uint8_t last_valve_index_;
};

uint8_t Node::last_valve_index_ = 0;

struct MoveState
{
    Pressure pressure_;
    uint32_t key_;
};
using NodeMap = std::map<std::string, Node>;
class Move
{
public:
    Move(const NodeMap &node_map)
    : current_node_{&node_map.at("AA")}
    {
        minute_ = 0;
    }
    Move(const NodeMap &node_map, const uint8_t max_minutes, const Move &prev_move, const std::string &next_node_str, const uint32_t other_valves_opened = 0)
    {
        current_node_ = &node_map.at(next_node_str);
        const Node &prev_node = node_map.at(prev_move.current_node_->get_name());
        pressure_ = prev_move.pressure_;
        valves_opened_ = prev_move.valves_opened_;
        other_valves_opened_ = other_valves_opened;
        minute_ = prev_move.minute_ + 1;
#ifdef SAVE_MOVES
        moves_ = prev_move.moves_;
        moves_.push_back(next_node_str);
#endif
        // staying in same location, opening valve
        if ((current_node_->get_name() == prev_node.get_name()) && !current_node_->is_valve_opened(valves_opened_ | other_valves_opened_))
        {
            valves_opened_ |= 1UL << *(current_node_->get_valve_index());
            pressure_ += current_node_->get_flow_rate() * (max_minutes - prev_move.minute_ - 1);
        }
    }

    uint32_t get_valves() const {return valves_opened_;}
    std::vector<Move> next_moves(const NodeMap &node_map, const uint8_t max_minutes, uint32_t other_valves = 0U) const
    {
        std::vector<Move> ret;

        const auto &edge_names = current_node_->get_edge_names();
        for (const auto &e : edge_names)
        {
            ret.emplace_back(Move(node_map, max_minutes, *this, e, other_valves));
        }

        // If there's a valve to open that hasn't been, one new move is
        // to open it
        const auto valve_index = current_node_->get_valve_index();
        if (valve_index)
        {
            const uint32_t valve_mask = 1UL << *valve_index;
            if ((valve_mask & (valves_opened_ | other_valves)) == 0)
            {
                ret.emplace_back(Move(node_map, max_minutes, *this, current_node_->get_name(), other_valves));
            }
        }

        return ret;
    }
    std::optional<Pressure> final_score(size_t minutes, uint32_t extra_valves_opened = 0U) const
    {
        if ((minute_ == minutes) || Node::all_valves_opened(valves_opened_ | extra_valves_opened))
        {
            return pressure_;
        }
        return std::nullopt;
    }

    friend std::ostream &operator<<(std::ostream &os, const Move &m)
    {
        os << "minute = " << static_cast<int>(m.minute_) << " pressure = " << m.pressure_<< " valves = " << std::hex << m.valves_opened_ << " other valves = " << m.other_valves_opened_ << std::dec << " score = " << m.score() << " current_node = " << *m.current_node_;
#ifdef SAVE_MOVES
        os << std::endl << "    ";
        for (const auto &ms : m.moves_)
        {
            os << ms << " ";
        }
#endif
        return os;
    }
    long score(void) const
    {
        long score = pressure_;
        score -= static_cast<long>(minute_) * 50;
        auto flow_rate = current_node_->get_flow_rate();
        if (flow_rate && !current_node_->is_valve_opened(valves_opened_ | other_valves_opened_))
        {
            score += (30 - minute_ - 1) * flow_rate;
        }
        #if 0
        for (size_t i = 0; (i + 2) < moves_.size(); i++)
        {
            if ((moves_[i] == moves_[i + 2]))
            {
                score = std::numeric_limits<long>::min();
            }
        }
        #endif
#ifdef SAVE_MOVES
        if (moves_.size() < 3)
        {
            score += (3 - moves_.size()) * 1000;
        }
#endif

        return score;
    }

    bool operator<(const Move &other) const
    {
        return score() < other.score();
    }
    MoveState state(void) const
    {
        MoveState state;

        state.pressure_ = pressure_;
        const std::string location = current_node_->get_name();
        uint32_t location_num = (location[0] - 'A') * 26UL + location[1] - 'A';

        state.key_  =  static_cast<uint32_t>(valves_opened_)        & 0x0000FFFFUL;
        state.key_ |= (static_cast<uint32_t>(minute_) << 16)        & 0x001F0000UL; // only really need 5 bits
        state.key_ |= (static_cast<uint32_t>(location_num) << 21)   & 0x7FE00000UL; // really only need 10 bits
        return state;
    }
    bool can_not_improve_on(Pressure best_pressure, const NodeMap &node_map, uint32_t minutes) const
    {
        return false;
        const uint32_t valves_open = get_valves();

        Pressure pressure = pressure_;
        std::vector<Pressure> flow_rates_left;
        for (const auto &n: node_map)
        {
            const auto f = n.second.get_flow_rate();
            if (f)
            {
                flow_rates_left.push_back(f);
            }
        }
        std::sort(flow_rates_left.begin(), flow_rates_left.end(), std::greater<Pressure>());
        for (size_t i = 0; i < flow_rates_left.size(); i++)
        {
            pressure += flow_rates_left[i] * (minutes - minute_);
        }
        return pressure < best_pressure;

    }
private:
    uint16_t valves_opened_{0};
    uint16_t other_valves_opened_{0};
    uint8_t minute_;
    Pressure pressure_{};
    const Node *current_node_;
#ifdef SAVE_MOVES
    std::vector<std::string> moves_;
#endif
};

struct DualMoveState
{
    Pressure pressure_;
    uint64_t key_;
};
class DualMove
{
public:
    DualMove(const NodeMap &node_map)
    : my_move_(node_map)
    , elephant_move_(node_map)
    {

    }
    DualMove(const Move &my_move, const Move &elephant_move)
        : my_move_(my_move)
        , elephant_move_(elephant_move)
    {

    }
    DualMove(const NodeMap &node_map, const uint8_t max_minutes, const DualMove &prev_move, const std::array<std::string,2> &next_node_str)
        : my_move_(node_map, max_minutes, prev_move.my_move_, next_node_str[0])
        , elephant_move_(node_map, max_minutes, prev_move.elephant_move_, next_node_str[1])
    {
    }
    std::vector<DualMove> next_moves(const NodeMap &node_map, const uint8_t max_minutes) const
    {
        std::vector<Move> my_next_moves = my_move_.next_moves(node_map, max_minutes, elephant_move_.get_valves());

//std::cout << "next_moves : " << std::endl << *this;
        std::vector<DualMove> ret;
        for (const auto &mm: my_next_moves)
        {
            std::vector<Move> elephant_next_moves = elephant_move_.next_moves(node_map, max_minutes, mm.get_valves());
            for (const auto &em: elephant_next_moves)
            {
                // Check to make sure both moves don't turn on valves at the same time
                ret.emplace_back(DualMove(mm, em));
//std::cout << std::endl << ret.back();
            }
        }
        return ret;
    }
    long score(void) const
    {
        const auto my_score = my_move_.score();
        if (my_score == std::numeric_limits<long>::min())
        {
            return std::numeric_limits<long>::min();
        }
        const auto el_score = elephant_move_.score();
        if (el_score == std::numeric_limits<long>::min())
        {
            return std::numeric_limits<long>::min();
        }
        return my_score + el_score;
    }

    bool operator<(const DualMove &other) const
    {
        return score() < other.score();
    }
    DualMoveState state(void) const
    {
        DualMoveState ret;
        const MoveState my_state = my_move_.state();
        const MoveState el_state = elephant_move_.state();

        ret.pressure_ = my_state.pressure_ + el_state.pressure_;
        ret.key_  = (my_state.key_ | el_state.key_) & 0x0000FFFFUL;
        ret.key_ |= my_state.key_ & 0x001F0000UL; // minutes, same for both states

        const uint16_t my_pos = (my_state.key_ >> 21) & 0x03ffU;
        const uint16_t el_pos = (el_state.key_ >> 21) & 0x03ffU;
        const uint64_t p1 = std::min(my_pos, el_pos);
        const uint64_t p2 = std::max(my_pos, el_pos);
        ret.key_ |= p1 << 32;
        ret.key_ |= p2 << 48;

        return ret;
    }
    std::optional<Pressure> final_score(size_t minutes, uint32_t extra_valves_opened = 0U) const
    {
        auto my_final_score = my_move_.final_score(minutes, elephant_move_.get_valves());
        auto el_final_score = elephant_move_.final_score(minutes, my_move_.get_valves());
        if (my_final_score && el_final_score)
        {
            return *my_final_score + *el_final_score;
        }
        return std::nullopt;

    }
    bool can_not_improve_on(Pressure best_pressure, const NodeMap &node_map, uint32_t minutes) const
    {
        const uint32_t valves_open = my_move_.get_valves() | elephant_move_.get_valves();
        const MoveState my_state = my_move_.state();
        const MoveState el_state = elephant_move_.state();

        Pressure pressure = my_state.pressure_ + el_state.pressure_;
        uint32_t minute = (my_state.key_ >> 16) & 0x001f;
        std::vector<Pressure> flow_rates_left;
        for (const auto &n: node_map)
        {
            const auto f = n.second.get_flow_rate();
            if (f)
            {
                flow_rates_left.push_back(f);
            }
        }
        std::sort(flow_rates_left.begin(), flow_rates_left.end(), std::greater<Pressure>());
        for (size_t i = 0; i < flow_rates_left.size(); i++)
        {
            pressure += flow_rates_left[i] * (minutes - minute - i / 2);
        }
        return pressure < best_pressure;

    }
    friend std::ostream &operator<<(std::ostream &os, const DualMove &dm)
    {
        os << "me : " << dm.my_move_ << std::endl << "el : " << dm.elephant_move_;
        return os;
    }
private:
    Move my_move_;
    Move elephant_move_;
};

template <class MoveT, class KeyT>
Pressure solve(const NodeMap &nodes, const size_t minutes, uint32_t other_valves = 0U)
{
    std::priority_queue<MoveT> moves;
    moves.push(MoveT(nodes));

    Pressure best_pressure = 0;
    std::map<KeyT, uint16_t> prev_moves;
    while(!moves.empty())
    {
        const auto move = moves.top();
        moves.pop();

        // Track the best pressure seen for a given board state (minutes left + pressure + valves opened)
        auto move_state = move.state();
        auto it = prev_moves.find(move_state.key_);
        if (it != prev_moves.end())
        {
            auto prev_pressure = it->second;
            if (move_state.pressure_ <= it->second)
            {
                continue;
            }
            it->second = move_state.pressure_;
        }
        else
        {
            prev_moves.insert(std::make_pair(move_state.key_, move_state.pressure_));
        }

        //std::cout << "Move = " << move << std::endl;
        const auto this_pressure = move.final_score(minutes);
        if (this_pressure)
        {
            if (*this_pressure > best_pressure)
            {
                best_pressure = std::max(best_pressure, *this_pressure);
                //std::cout << "New best pressure = " << best_pressure << " " << move << std::endl;
            }
            continue;
        }

        const auto next_moves = move.next_moves(nodes, minutes, other_valves);
        for (const auto &nm : next_moves)
        {
            //std::cout << "    Next move = " << nm << std::endl;
            #if 0
            if (nm.score() == std::numeric_limits<long>::min())
            {
                continue;
            }
            if (nm.can_not_improve_on(best_pressure, nodes, minutes))
            {
                continue;
            }
            #endif
            moves.push(nm);
        }
    }
    return best_pressure;
}


void solve_thread(const NodeMap &nodes, const size_t minutes, const uint32_t start_valve_mask, const uint32_t end_valve_mask, Pressure &result)
{
    Pressure best_sum = 0;
    const uint32_t valve_mask_count = 1U << Node::get_last_valve_index(); 
    for (uint32_t valve_mask = start_valve_mask; valve_mask < end_valve_mask; valve_mask += 1)
    {
        const auto invert_valve_mask =(valve_mask_count - 1) ^ valve_mask;
        auto p1 = solve<Move, uint32_t>(nodes, 26, valve_mask);
        auto p2 = solve<Move, uint32_t>(nodes, 26, invert_valve_mask);
        best_sum = std::max(best_sum, p1 + p2);
        std::cout << "valve_mask = " << std::hex << valve_mask << " invert valve mask = " << invert_valve_mask << std::dec << " p1 = " << p1 << " p2 = " << p2 << " sum = " << p1 + p2 << " best_sum = " << best_sum << std::endl;
    }
    result = best_sum;
}
int main(int argc, char **argv)
{
    std::ifstream istream(argv[1], std::ifstream::in);
    std::string line;
    constexpr size_t minutes = 30;

    NodeMap nodes;
    while (getline(istream, line))
    {
        Node n = Node(line);
        nodes[n.get_name()] = n;
        std::cout << n << std::endl;
    }
    solve<Move, uint32_t>(nodes, 30);
    //solve<DualMove, uint64_t>(nodes, 26);

    Pressure best_sum = 0;
    uint32_t valve_mask_count = 1U << Node::get_last_valve_index(); 
    constexpr uint32_t NUM_THREADS = 16;
    std::vector<Pressure> results(NUM_THREADS);
    std::vector<std::thread> threads;
    size_t i = 0;
    for (uint32_t valve_mask = 0U; valve_mask < valve_mask_count; valve_mask += (valve_mask_count / NUM_THREADS))
    {
        threads.emplace_back(std::thread(solve_thread, std::ref(nodes), minutes, valve_mask, std::min(valve_mask + valve_mask_count / NUM_THREADS, valve_mask_count - 1), std::ref(results[i++])));
    }
    for (auto &t: threads)
    {
        t.join();
    }

    std::copy(results.cbegin(), results.cend(), std::ostream_iterator<Pressure>(std::cout, " "));
    std::cout << std::endl;
    return 0;
}