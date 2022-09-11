#include <iostream>
#include <ostream>
#include <ranges>
#include <string>
#include <vector>

#include "Cache.hpp"

// for string delimiter
std::vector<std::string> split(std::string s, std::string delimiter)
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
    {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

int main(int, char **)
{

    sd::Cache cache;

    std::string input = "add asd 123";
    while (std::getline(std::cin, input))
    {
        auto splited = split(input, " ");
        if (splited.size() < 2)
        {
            continue;
        }
        auto &command = splited[0];
        auto &key = splited[1];
        if (command == "add" && splited.size() == 3)
        {
            auto &value = splited[2];
            auto result = cache.Add(key, std::move(value));
            std::cout << result;
        }
        else if (command == "get")
        {
            auto result = cache.Get<std::string>(key);
            std::cout << (result ? *result : "not found");
        }
        else if (command == "set" && splited.size() == 3)
        {
            auto &value = splited[2];
            auto result = cache.Set(key, std::move(value));
            std::cout << result;
        }
        else if (command == "remove")
        {
            auto result = cache.Remove(key);
            std::cout << result;
        }
        else if (command == "count")
        {
            auto result = cache.Count();
            std::cout << result;
        }
        else if (command == "contains")
        {
            auto result = cache.Contains(key);
            std::cout << result;
        }
        else
        {
            std::cout << "Invalid command";
        }
        std::cout << std::endl;
    }
}
