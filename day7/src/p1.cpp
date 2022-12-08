#include <algorithm>
#include <fstream>
#include <functional>
#include <numeric>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

class MyFile {
public:
    MyFile(const std::string &name, const size_t size)
    : name_(name)
    , size_(size)
    {

    }
    size_t getSize(void) const { return size_; }
    void print(size_t indent_count) const
    {
        for (size_t i = 0; i < indent_count; i++)
        {
            std::cout << " ";
        }
        std::cout << "- " << name_ << " (file, size=" << size_ << ")" << std::endl;
    }
private:
    std::string name_;
    size_t size_;
};

class MyDir {

public:
    MyDir(const char *name)
      :name_{std::string(name)}
    {
    }
    void creat(const std::string &filename, const size_t filesize)
    {
        files_.push_back(MyFile(filename, filesize));
    }
    MyDir &chdir(const std::string &dirname)
    {
        return subdirs_.at(dirname);
    }
    void mkdir(const std::string &dirname)
    {
        subdirs_.try_emplace(dirname, MyDir(dirname.c_str()));
    }
    size_t setSize(void)
    {
        std::for_each(subdirs_.begin(), subdirs_.end(), [&](auto &s) {size_ += s.second.setSize();});
        std::for_each(files_.cbegin(), files_.cend(), [&](auto &f) {size_ += f.getSize();});
        return size_;
    }
    void getAllSizes(std::vector<size_t> &sizeVec) const
    {
        std::for_each(subdirs_.begin(), subdirs_.end(), [&](auto &s) {s.second.getAllSizes(sizeVec);});
        sizeVec.push_back(size_);
    }
    void print(size_t indent_count = 0) const{
        for (size_t i = 0; i < indent_count; i++)
        {
            std::cout << " ";
        }
        std::cout << "- " << name_ << " (dir)" << std::endl;
        indent_count += 2;
        std::for_each(subdirs_.cbegin(), subdirs_.cend(), [&](auto &s) { s.second.print(indent_count); });
        std::for_each(files_.begin(), files_.cend(), [&](auto &f) { f.print(indent_count); });
    }
        
private:
    std::string name_;
    std::vector<MyFile> files_;
    std::map<std::string, MyDir> subdirs_;
    size_t size_{0};
};

int main(int argc, char **argv)
{
    MyDir root_dir("/");
    std::vector<std::reference_wrapper<MyDir>> path_stack;
    path_stack.push_back(root_dir);

    std::ifstream istream(argv[1], std::ifstream::in);
    std::string str;
    while (getline(istream, str))
    {
        if (str.starts_with("$ cd "))
        {
            std::string dir = str.substr(5, std::string::npos);
            if (dir == "..")
            {
                path_stack.pop_back();
            }
            else if (dir == "/")
            {
                path_stack.clear();
                path_stack.push_back(root_dir);
            }
            else
            {
                path_stack.push_back(path_stack.back().get().chdir(dir));
            }
        }
        else if (str.starts_with("$ ls"))
        {
            // do nothing
        }
        else if (str.starts_with("dir "))
        {
            std::string dir = str.substr(4, std::string::npos);
            path_stack.back().get().mkdir(dir);
        }
        else
        {
            size_t size = atol(str.c_str());

            size_t pos = str.find_first_of(' ') + 1;
            std::string name = str.substr(pos, std::string::npos);
            path_stack.back().get().creat(name, size);
        }
    }
    root_dir.setSize();
    std::vector<size_t> sizes;
    root_dir.getAllSizes(sizes);
    root_dir.print();
    std::copy(sizes.begin(), sizes.end(), std::ostream_iterator<size_t>(std::cout, " "));

    auto sum = std::accumulate(sizes.cbegin(), sizes.cend(), 0, [](size_t a, size_t b) {return (b <= 100000) ? a+b : a;});
    std::cout << "Sum = " << sum << std::endl;

    std::sort(sizes.begin(), sizes.end());
    constexpr size_t disk_size = 70000000;
    constexpr size_t free_needed = 30000000;

    const size_t free_space = disk_size - sizes.back();
    const size_t need_to_free = free_needed - free_space;

    std::cout << "need to free = " << need_to_free << std::endl;

    for (const auto s: sizes)
    {
        if (s > need_to_free)
        {
            std::cout << "Dir size to free = " << s << std::endl;
            break;
        }
    }
}