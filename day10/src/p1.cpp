#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <set>

struct State
{
    size_t pc_;
    int    x_;
    State(size_t pc, int x)
        :pc_(pc)
        ,x_(x)
    {}
};
std::ostream& operator<<(std::ostream &os, const State &s)
{
    os << "PC=" << s.pc_;
    os << " X=" << s.x_;
    return os;
}

State instr_noop(const std::string &, const State &state)
{
    return State(state.pc_ + 1, state.x_);
}

State instr_addx(const std::string &str, const State &state)
{
    int arg = atoi(str.c_str());
    return State(state.pc_ + 2, state.x_ + arg);
}

char get_crt_pixel(long pc, long x)
{
    const long col = (pc - 1) % 40;
    if ((x >= (col - 1)) && (x <= (col + 1)))
    {
        return '#';
    }
    else
    {
        return '.';
    }
}

int main(int argc, char **argv)
{
    const std::map<std::string, std::function<State(const std::string &, const State &)>> opcodes = {
        {"noop", instr_noop},
        {"addx", instr_addx}
    };
    const std::set<size_t> signal_strength_cycles = {20, 60, 100, 140, 180, 220};

    std::ifstream istream(argv[1], std::ifstream::in);
    std::string str;
    State s(1,1);
    int acc = 0;
    std::string crt;
    while (getline(istream, str))
    {
        std::string args;
        if (str.size() > 4)
        {
            args = str.substr(5, std::string::npos);
        }
        const State prev_s = s;
#if 0
        std::cout << "Sprite position: ";
        for (size_t i = 1; i <= 40; i++)
        {
            std::cout << get_crt_pixel(i, s.x_);
        }
        std::cout << std::endl << std::endl;
        std::cout << "Start cycle   " << s.pc_ << ": begin executing " << str << std::endl;
#endif
        s = opcodes.at(str.substr(0,4))(args, s);
        //std::cout << "Instr = " << str << " prev : " << prev_s <<  " curr : " << s << std::endl;
        for (size_t pc = prev_s.pc_; pc < s.pc_; pc++)
        {
            const auto x = prev_s.x_;
            if (signal_strength_cycles.count(pc))
            {
                //std::cout << "Cycle " << pc << " X = " << x << " strength = " << pc * x << std::endl;
                acc += pc * x;
            }
            const long col = (pc + 1) % 40;

            //std::cout << "During cycle  " << pc << ": CRT draws pixel in position " << (pc - 1) % 40 << std::endl;

            crt += get_crt_pixel(pc, x);
            //std::cout << "Current CRT row: " << crt << std::endl << std::endl;;
        }
    }
    std::cout << "Acc = " << acc << std::endl;
    for (size_t i = 0; i < crt.size(); i+= 40)
    {
        std::cout << crt.substr(i, 40) << std::endl;
    }
    return 0;
}