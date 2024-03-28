#pragma once

#include <unordered_map>
#include <string>
#include <functional>
#include <stdexcept>
#include <thread>
#include <vector>

struct UnitTest
{
    struct Unit
    {
        std::string name;
        std::string verdict;
        bool is_ok;
    };

    static inline std::unordered_map<std::string, std::function<void()>> tests_;

    static void add(const std::string& name, std::function<void()> fun) 
    {
        if (tests_.find(name) != tests_.end()) {std::__throw_logic_error(("test \"" + name + "\" already exists").c_str());}
        tests_[name] = fun;
    }

    static std::vector<Unit> run()
    {
        std::vector<Unit> res(tests_.size());
        std::vector<std::thread> ths;

        std::function fun_calc = [](Unit& unit, const std::string& name, const std::function<void()>& fun)
        {
            unit.name = name;
            try
            {
                fun();
                unit.is_ok = true;
            }
            catch(const std::exception& e)
            {
                unit.verdict = e.what();
                unit.is_ok = false;
            }
        };
        size_t ind = 0;

        for (auto[name, fun]: tests_)
        {
            ths.push_back(std::thread(fun_calc, std::ref(res[ind]), std::ref(name), std::ref(fun)));
            ++ind;
        }
        ind = 0;
        for (; ind < res.size(); ++ind)
        {
            ths[ind].join();
        }
        return res;
    }

    static void clear() {tests_.clear();}
};