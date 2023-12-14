#pragma once

#include <unordered_map>
#include <vector>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <iomanip>

class Benchmark
{
    #define _BENCHMARK std::chrono

    using TimePoint = _BENCHMARK ::steady_clock::time_point;

    struct BenchInfo
    {
        _BENCHMARK ::milliseconds time;
        long long num_calls = 0;
    };

    static inline std::unordered_map<std::string, BenchInfo> times_;
    std::string name_;
    TimePoint init_time_;

public:

    Benchmark(const std::string& name): name_(name), init_time_(std::chrono::steady_clock::now()){}

    static void logRes(std::ostream& out)
    {
       out << "\nNum functions: " << times_.size() << "\n\n";

        std::vector<std::pair<std::string, BenchInfo>> res;

        for (auto& i: times_)
        {
            res.emplace_back(i.first, i.second);
        }

        auto comp = [](std::pair<std::string, BenchInfo> a, std::pair<std::string, BenchInfo> b)
        {
            if (a.second.time == b.second.time)
            {
                if (a.second.num_calls == b.second.num_calls)
                {
                    return a.first < b.first;
                }
                return a.second.num_calls < b.second.num_calls;
            }
            return a.second.time < b.second.time;
        };

        std::sort(res.begin(), res.end(), comp);

        for (auto& i: res)
        {
            out << std::setw(40) << std::setfill('-') << std::setiosflags(std::ios::left) << i.first << " " 
                << "Time: " << std::setw(10) << std::setiosflags(std::ios::left) << i.second.time.count() << "ms. | "
                << "Num calls: " << i.second.num_calls << '\n'; 
        }
    }

    static void clear()
    {
        times_.clear();
    }

    ~Benchmark()
    {
        if (times_.find(name_) == times_.end())
        {
            times_[name_] = {_BENCHMARK ::duration_cast<_BENCHMARK ::milliseconds>(_BENCHMARK ::steady_clock::now() - init_time_), 1};
        }
        else
        {
            times_[name_].time += _BENCHMARK ::duration_cast<_BENCHMARK ::milliseconds>(_BENCHMARK ::steady_clock::now() - init_time_);
            ++times_[name_].num_calls;
        }
    }
};

// #define _CLASS_NAME_ typeid(*this).name()
#define _FUNCTION_NAME_ __builtin_FUNCTION()
// #define _STR_(c_str) std::string(c_str)

#define _START_BENCHMARK_ Benchmark bench(_FUNCTION_NAME_)

#define _START_TIMER_ std::chrono::steady_clock::time_point _START_ = std::chrono::steady_clock::now()

#define _STOP_TIMER_ std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _START_).count()