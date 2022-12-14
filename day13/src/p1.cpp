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

enum ORDER_STATUS
{
    IN_RIGHT_ORDER,
    IN_WRONG_ORDER,
    KEEP_LOOKING
};

class MessageItemInt;
class MessageItemList;

class MessageItem
{
public:
    virtual std::string parse_list(std::string str) {return str;}
    virtual std::string toString(void) const
    {
        return std::string();
    }
    virtual ORDER_STATUS compare(const MessageItemInt &rhs) const = 0;
    virtual ORDER_STATUS compare(const MessageItemList &rhs) const = 0;
    friend std::ostream &operator<<(std::ostream &os, const MessageItem &m);

};
std::ostream &operator<<(std::ostream &os, const MessageItem &m)
{
    os << m.toString();
    return os;
}
class MessageItemInt : public MessageItem
{
public:
        MessageItemInt(const std::string &string)
            : item_(atoi(string.c_str()))
        {
        }
        MessageItemInt(const int v)
        : item_(v)
        {
        }
        std::string toString(void) const override
        {
            return std::to_string(item_);
        }

        ORDER_STATUS compare(const MessageItemInt &rhs) const
        {
            //std::cout << "Compare " << *this << " vs " << rhs << std::endl;
            if (item_ < rhs.item_)
            {
                //std::cout << " Left side is smaller, so inputs are in ther right order" << std::endl;
                return IN_RIGHT_ORDER;
            }
            if (item_ > rhs.item_)
            {
                //std::cout << " Right side is smaller, so inputs are not in the right order" << std::endl;
                return IN_WRONG_ORDER;
            }
            return KEEP_LOOKING;
        }

        ORDER_STATUS compare(const MessageItemList &rhs) const;
        int value() const {return item_;}
private:
    int item_;
};

class MessageItemList : public MessageItem
{
public:
    MessageItemList()
    {
    }
    MessageItemList(const MessageItemInt &other)
    {
        items_.push_back(std::make_unique<MessageItemInt>(other.value()));
    }
    std::string parse_int(const std::string &str)
    {
        items_.push_back(std::make_unique<MessageItemInt>(str));
        size_t idx = 0;
        while (isdigit(str[idx]))
        {
            idx += 1;
        }
        return str.substr(idx);
    }
    std::string parse_list(std::string str)
    {
        while (str.size())
        {
            // At end of list, return since this full list is parsed
            if (str[0] == ']')
            {
                return str.substr(1);
            }
            if (isdigit(str[0]))
            {
                //std::cout << "Parsing int = " << str << std::endl;
                str = parse_int(str);
                //std::cout << "After parsing int = " << str << std::endl;
                if (str[0] == ']')
                {
                    return str.substr(1);
                }
            }
            else // start of a new list
            {
                //std::cout << "Parsing list = " << str << std::endl;
                items_.push_back(std::make_unique<MessageItemList>());
                str = items_.back()->parse_list(str.substr(1));
                //std::cout << "After parsing list = " << str << std::endl;
            }
            // Skip over comma delimiting list entries
            if (str[0] == ',')
            {
                str = str.substr(1);
            }
        }
        return str;
    }
    std::string toString(void) const override
    {
        std::string ret;
        ret += '[';

        for (const auto &i : items_)
        {
            ret += i->toString();
            ret += ' ';
        }
        ret += ']';
        return ret;
    }

    ORDER_STATUS compare(const MessageItemList &rhs) const
    {
        //std::cout << "Compare " << *this << " vs " << rhs << std::endl;
        for (size_t i = 0; i < items_.size(); i++)
        {
            if (i >= rhs.items_.size())
            {
                //std::cout << "Right side ran out of items, so inputs are not in the right order" << std::endl;
                return IN_WRONG_ORDER;
            }
            ORDER_STATUS rc;
            MessageItemInt *mii = dynamic_cast<MessageItemInt *>(rhs.items_[i].get());
            if (mii)
            {
                rc = items_[i]->compare(*mii);
            }
            else
            {
                MessageItemList *mil = dynamic_cast<MessageItemList *>(rhs.items_[i].get());
                rc = items_[i]->compare(*mil);
            }
            if (rc != KEEP_LOOKING)
            {
                return rc;
            }
        }
        if (items_.size() < rhs.items_.size())
        {
            //std::cout << "Left side ran out of items, so inputs are in the right order" << std::endl;
            return IN_RIGHT_ORDER;
        }
        return KEEP_LOOKING;
    }

    ORDER_STATUS compare(const MessageItemInt &rhs) const
    {
        //std::cout << "Compare " << *this << " vs " << rhs << std::endl;
        return compare(MessageItemList(rhs));
    }
private:
    std::vector<std::unique_ptr<MessageItem>> items_;
};

ORDER_STATUS MessageItemInt::compare(const MessageItemList &rhs) const
{
    //std::cout << "Compare " << *this << " vs " << rhs << std::endl;
    return MessageItemList(*this).compare(rhs);
}

class Message
{
public:
    Message(const std::string &str)
    {
        message_.parse_list(str.substr(1));
    }
    ORDER_STATUS compare(const Message &rhs) const
    {
        return message_.compare(rhs.message_);
    }
    bool operator<(const Message &rhs) const
    {
        return message_.compare(rhs.message_) == IN_RIGHT_ORDER;
    }
    bool operator==(const Message &rhs) const
    {
        return message_.compare(rhs.message_) == KEEP_LOOKING;
    }

private:
    MessageItemList message_;
    friend std::ostream &operator<<(std::ostream &os, const Message &m);
};
std::ostream &operator<<(std::ostream &os, const Message &m)
{
    os << m.message_;
    return os;
}

int main(int argc, char **argv)
{
    std::ifstream istream(argv[1], std::ifstream::in);
    std::string line;
    size_t index = 1;
    size_t sum = 0;
    std::vector<Message> messages;

    while (getline(istream, line))
    {
        messages.emplace_back(Message(line));
        getline(istream, line);
        messages.emplace_back(Message(line));

        getline(istream, line);
        //std::cout << " == Pair " << index << " ==" << std::endl;
        if (messages[messages.size() - 2].compare(messages[messages.size() -1]) == IN_RIGHT_ORDER)
        {
            sum += index;
        }
        index += 1;
    }
    std::cout << "Index sum = " << sum << std::endl;
    messages.emplace_back("[[2]]");
    messages.emplace_back("[[6]]");
    std::sort(messages.begin(), messages.end());
    std::copy(messages.cbegin(), messages.cend(), std::ostream_iterator<Message>(std::cout, "\n"));
    const auto it1 = std::find(messages.cbegin(), messages.cend(), Message("[[2]]"));
    const auto it2 = std::find(messages.cbegin(), messages.cend(), Message("[[6]]"));
    const size_t product = (1 + std::distance(messages.cbegin(), it1)) * (1 + std::distance(messages.cbegin(), it2));
    
    std::cout << "Index product = " << product << std::endl;
    return 0;
}