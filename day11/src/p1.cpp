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
#include <set>
#include <vector>

class OperationBase
{
    public:
    OperationBase()
    {
    }
    virtual unsigned long operator()(unsigned long old) const = 0;
    virtual std::string string(void) const = 0;
    virtual unsigned long val(void) const = 0;
};

class OperationMultiply : public OperationBase
{
    public: 
    OperationMultiply(const unsigned long val)
    : val_(val)
    { }

    unsigned long operator()(unsigned long old) const override{
        return val_ * old;
    }
    std::string string(void) const override {
        return std::string("is multiplied");
    }
    unsigned long val(void) const override {
        return val_;
    }

    private:
    unsigned long val_;
};

class OperationAdd : public OperationBase
{
    public: 
    OperationAdd(const unsigned long val)
    : val_(val)
    { }

    unsigned long operator()(unsigned long old) const override{
        return val_ + old;
    }
    std::string string(void) const override {
        return std::string("increases");
    }
    unsigned long val(void) const override {
        return val_;
    }
    private:
    unsigned long val_;
};

class OperationSquared : public OperationBase
{
    public: 
    OperationSquared() 
    { }

    unsigned long operator()(unsigned long old) const override{
        return old * old;
    }
    std::string string(void) const override {
        return std::string("squared");
    }
    unsigned long val(void) const override {
        return 0;
    }
    private:
};



class TestBase
{
    public:
    TestBase()
    {
    }
    virtual bool operator()(unsigned long input) const = 0;
    virtual unsigned long val(void) const = 0;
};

class TestDivisibleBy: public TestBase
{
    public:
    TestDivisibleBy(unsigned long val)
    : val_(val)
    {}
    
    bool operator()(unsigned long input) const override
    {
        return (input % val_) == 0;
    }
    unsigned long val(void) const override {
        return val_;
    }

    private:
    unsigned long val_;
};

class Monkey
{

public:
Monkey(std::ifstream &istream)
{
    std::string line;
    getline(istream, line);
    if (line.size() < 2)
    {
        getline(istream, line);
    }
    getline(istream, line); // skip "Monkey N:"
    std::string itemstr = line.substr(line.find(":") + 2); 
    char const * const delimiter{" ,"};
    char *token{::std::strtok(itemstr.data(), delimiter)};
    while(nullptr != token)
    {
        items_.push_back(atol(token));
        token = std::strtok(nullptr, delimiter);
    }
    getline(istream, line);
    auto pos = line.find("*");
    if (pos != line.npos)
    {
        if (std::string(&line[pos + 2]) == "old")
        {
        operation_ = std::make_unique<OperationSquared>();
        }
        else
        {
        operation_ = std::make_unique<OperationMultiply>(atol(&line[pos+2]));
        }
    }
    else
    {
        auto pos = line.find("+");
        operation_ = std::make_unique<OperationAdd>(atol(&line[pos+2]));
    }
    getline(istream, line);
    test_ = std::make_unique<TestDivisibleBy>(atol(&line[21]));
    getline(istream, line);
    next_monkey_idx_[1] = atol(&line[29]);
    getline(istream, line);
    next_monkey_idx_[0] = atol(&line[30]);
}

void add_item(unsigned long val)
{
    items_.push_back(val);
}

unsigned long get_divisor(void) const
{
    return test_->val();
}

struct ProcessOutput
{
    unsigned long worry_;
    size_t monkey_idx_;
    ProcessOutput(const unsigned long worry, const size_t monkey_idx)
    : worry_(worry)
    , monkey_idx_(monkey_idx)
    {
    }
};
std::optional<ProcessOutput> process_one_item(unsigned long divide_by)
{
    if (items_.empty())
    {
        return std::nullopt;
    }
    process_counter_ += 1;
    const auto itemval = items_.front();
    items_.pop_front();
    const auto worry = (*operation_)(itemval) / divide_by;
    #if 0
    std::cout << "  Monkey inspects an item with a worry level of " << itemval << "." << std::endl;
    std::cout << "    Worry level " << operation_->string() << " by " << operation_->val() << " to "  << (*operation_)(itemval) << "." << std::endl;
    std::cout << "    Monkey gets bored with item. Worry level is divided by 3 to " << worry << "." << std::endl;
    std::cout << "    Current worry level is not divisible by " << test_->val() << "." << std::endl;
    std::cout << "    Item with worry level " << worry << " is thrown to monkey " << next_monkey_idx_[(*test_)(worry)] << std::endl;
    #endif
    return ProcessOutput(worry, next_monkey_idx_[(*test_)(worry)]);
}

size_t get_process_counter(void) const
{
    return process_counter_;
}

private:
std::deque<unsigned long> items_;
std::unique_ptr<OperationBase> operation_;
std::unique_ptr<TestBase> test_;
std::array<size_t, 2> next_monkey_idx_;
size_t process_counter_{0};
};

int main(int argc, char **argv)
{
    std::vector<Monkey> monkeys;
    std::ifstream istream(argv[1], std::ifstream::in);
    std::string str;
    do
    {
        monkeys.emplace_back(istream);
    } while (getline(istream, str));

    constexpr size_t rounds = 10000;
    std::set<size_t> print_round = {1, 20, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000};
    unsigned long modulo = 1UL;
    std::for_each(monkeys.cbegin(), monkeys.cend(), [&](const auto &m)
                    { modulo *= m.get_divisor(); });
    for (size_t round = 0; round < rounds; round++)
    {
        for (size_t monkey = 0; monkey < monkeys.size(); monkey++)
        {
            //std::cout << "Monkey " << monkey << ":" << std::endl;
            for (auto ret = monkeys[monkey].process_one_item(1); ret; ret = monkeys[monkey].process_one_item(1))
            {
                monkeys[ret->monkey_idx_].add_item(ret->worry_ % modulo);
            }
        }
        if (print_round.count(round + 1))
        {
            std::cout << "Round " << round + 1 << std::endl;
            std::vector<size_t> process_counts;
            std::for_each(monkeys.cbegin(), monkeys.cend(), [&](const auto &m)
                          { process_counts.push_back(m.get_process_counter()); });

            std::copy(process_counts.cbegin(), process_counts.cend(), std::ostream_iterator<size_t>(std::cout, " "));
            std::cout << std::endl;
        }
    }

    std::vector<size_t> process_counts;
    std::for_each(monkeys.cbegin(), monkeys.cend(), [&](const auto &v)
                  { process_counts.push_back(v.get_process_counter()); });
    std::sort(process_counts.begin(), process_counts.end());
    std::copy(process_counts.cbegin(), process_counts.cend(), std::ostream_iterator<size_t>(std::cout, " "));
    std::cout << std::endl;
    std::cout << process_counts.back() * process_counts[process_counts.size()- 2] << std::endl;

    return 0;
}