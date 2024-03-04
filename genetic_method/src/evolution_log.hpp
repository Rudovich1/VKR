#pragma once

#include <chrono>
#include <algorithm>


struct DataLog
{
    double min_result_ = LLONG_MAX;
    double max_result_ = LLONG_MIN;
    double average_result_ = 0.;
    size_t num_samples_ = 0;
};

struct TimeLog
{
    std::chrono::steady_clock::time_point start_;
    std::chrono::steady_clock::time_point end_;

    TimeLog(): start_(std::chrono::steady_clock::now()), end_(std::chrono::steady_clock::now()) {}

    void start() {start_ = std::chrono::steady_clock::now();}
    void end() {end_ = std::chrono::steady_clock::now();}
};

struct BaseStatistics: public DataLog, public TimeLog {};

struct GenerationLog: public BaseStatistics
{
    void add(double sample)
    {
        min_result_ = std::min(min_result_, sample);
        max_result_ = std::max(max_result_, sample);
        average_result_ += sample;
        ++num_samples_;
    }

    void clear()
    {
        min_result_ = LLONG_MAX;
        max_result_ = LLONG_MIN;
        average_result_ = 0.;
        num_samples_ = 0;
        start_ = end_ = std::chrono::steady_clock::now();
    }
};

struct PopulationLog: public BaseStatistics
{
    std::vector<GenerationLog> generations;
    size_t num_improvements = 0;

    void add(GenerationLog generation_log)
    {
        if (max_result_ < generation_log.max_result_)
        {
            ++num_improvements;
        }
        min_result_ = std::min(min_result_, generation_log.max_result_);
        max_result_ = std::max(max_result_, generation_log.max_result_);
        average_result_ += generation_log.max_result_;
        ++num_samples_;
        generations.push_back(std::move(generation_log));
    }
};
