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

class NodeBase;
using SymTab = std::map<std::string, std::shared_ptr<NodeBase>>;
class NodeBase
{
public:
    NodeBase() = default;

    virtual long long process(void) const = 0;
    virtual void build_tree(const SymTab &symtab) = 0;
    virtual void set_subvalue(const long long input_value) const = 0;
    virtual bool is_humn(void) const = 0;
private:
};

class LeafNode : public NodeBase
{
public:
    LeafNode(const long long value)
    : value_(value)
    {

    }
    void build_tree(const SymTab &symtab) override
    {
    }
    long long process(void) const override
    {
        return value_;
    }
    virtual void set_subvalue(const long long input_value) const override
    {
    }
    virtual bool is_humn(void) const override { return false;}
private:
    long long value_;
};

class HumnNode : public LeafNode
{
public:
    HumnNode()
    : LeafNode(-1)
    {
    }
    void set_subvalue(const long long input_value) const override
    {
        std::cout << "humn = " << input_value << std::endl;
    }
    bool is_humn(void) const override { return true; }
};

class ArithmeticNode: public NodeBase
{
public:
    ArithmeticNode(const std::string &left_label, const std::string &right_label)
        : left_label_(left_label), right_label_(right_label)
    {
    }
    void build_tree(const SymTab &symtab) override
    {
        left_node_ = symtab.at(left_label_);
        right_node_ = symtab.at(right_label_);
    }

    virtual long long process(void) const = 0;
    virtual void set_subvalue(const long long input_value) const = 0;
    bool is_humn(void) const override
    {
        return left_node_->is_humn() || right_node_->is_humn();
    }

private:
    std::string left_label_;
    std::string right_label_;
protected:
    bool left_is_humn(void) const
    {
        return left_node_->is_humn();
    }
    std::shared_ptr<NodeBase> left_node_;
    std::shared_ptr<NodeBase> right_node_;
};

class AddNode : public ArithmeticNode
{
public: 
    AddNode(const std::string &left_label, const std::string &right_label)
    : ArithmeticNode(left_label, right_label)
    {
    }
    long long process(void) const override
    {
        return left_node_->process() + right_node_->process();
    }
    void set_subvalue(const long long input_value) const override
    {
        if (left_is_humn())
        {
            left_node_->set_subvalue(input_value - right_node_->process());
        }
        else
        {
            right_node_->set_subvalue(input_value - left_node_->process());
        }
    }
};
class SubtractNode : public ArithmeticNode
{
public: 
    SubtractNode(const std::string &left_label, const std::string &right_label)
    : ArithmeticNode(left_label, right_label)
    {
    }
    long long process(void) const override
    {
        return left_node_->process() - right_node_->process();
    }
    void set_subvalue(const long long input_value) const override
    {
        if (left_is_humn())
        {
            left_node_->set_subvalue(input_value + right_node_->process());
        }
        else
        {
            right_node_->set_subvalue(left_node_->process() - input_value);
        }
    }
};
class MultiplyNode : public ArithmeticNode
{
public: 
    MultiplyNode(const std::string &left_label, const std::string &right_label)
    : ArithmeticNode(left_label, right_label)
    {
    }
    long long process(void) const
    {
        return left_node_->process() * right_node_->process();
    }
    void set_subvalue(const long long input_value) const override
    {
        if (left_is_humn())
        {
            left_node_->set_subvalue(input_value / right_node_->process());
        }
        else
        {
            right_node_->set_subvalue(input_value / left_node_->process());
        }
    }
};
class DivideNode : public ArithmeticNode
{
public: 
    DivideNode(const std::string &left_label, const std::string &right_label)
    : ArithmeticNode(left_label, right_label)
    {
    }
    long long process(void) const
    {
        return left_node_->process() / right_node_->process();
    }
    void set_subvalue(const long long input_value) const override
    {
        if (left_is_humn())
        {
            left_node_->set_subvalue(input_value * right_node_->process());
        }
        else
        {
            right_node_->set_subvalue(left_node_->process() / input_value);
        }
    }
};
class RootNode : public ArithmeticNode
{
public: 
    RootNode(const std::string &left_label, const std::string &right_label)
    : ArithmeticNode(left_label, right_label)
    {
    }
    long long process(void) const
    {
        std::cout << left_node_->process() << " " << right_node_->process() << std::endl;
        return 0;
    }
    void set_subvalue(const long long input_value) const override
    {
        if (left_is_humn())
        {
            left_node_->set_subvalue(right_node_->process());
        }
        else
        {
            right_node_->set_subvalue(left_node_->process());
        }
    }
};

int main(int argc, char **argv)
{
    std::ifstream istream(argv[1], std::ifstream::in);
    std::string line;

    SymTab sym_tab;
    while (getline(istream, line))
    {
        if (line.substr(0,4) == "humn")
        {
            sym_tab[line.substr(0,4)] = std::make_shared<HumnNode>();
        }
        else if (isdigit(line[6]))
        {
            sym_tab[line.substr(0,4)] = std::make_shared<LeafNode>(atoi(&line[6]));
        }
        else if (line.substr(0,4) == "root")
        {
            sym_tab[line.substr(0,4)] = std::make_shared<RootNode>(line.substr(6,4), line.substr(13,4));
        }
        else if (line[11] == '+')
        {
            sym_tab[line.substr(0,4)] = std::make_shared<AddNode>(line.substr(6,4), line.substr(13,4));
        }
        else if (line[11] == '-')
        {
            sym_tab[line.substr(0,4)] = std::make_shared<SubtractNode>(line.substr(6,4), line.substr(13,4));
        }
        else if (line[11] == '*')
        {
            sym_tab[line.substr(0,4)] = std::make_shared<MultiplyNode>(line.substr(6,4), line.substr(13,4));
        }
        else if (line[11] == '/')
        {
            sym_tab[line.substr(0,4)] = std::make_shared<DivideNode>(line.substr(6,4), line.substr(13,4));
        }
    }
    for (auto &s : sym_tab)
    {
        s.second->build_tree(sym_tab);
    }
 //   std::cout << sym_tab.at("root")->process() << std::endl;
    sym_tab.at("root")->set_subvalue(0);
    return 0;
}