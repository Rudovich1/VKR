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

    virtual void start() {start_ = std::chrono::steady_clock::now();}
    virtual void end() {end_ = std::chrono::steady_clock::now();}
};

struct BaseLog: public DataLog, public TimeLog {};

struct GenerationLog: public BaseLog
{
    void add(double sample)
    {
        min_result_ = std::min(min_result_, sample);
        max_result_ = std::max(max_result_, sample);
        average_result_ += sample;
        ++num_samples_;
    }

    virtual void start()
    {
        TimeLog::start();
        min_result_ = LLONG_MAX;
        max_result_ = LLONG_MIN;
        average_result_ = 0.;
        num_samples_ = 0;
    }

    virtual void end()
    {
        TimeLog::end();
        average_result_ /= num_samples_;
    }
};

struct PopulationLog: public BaseLog
{
    std::vector<GenerationLog> generations_;
    size_t num_improvements_ = 0;

    // PopulationLog() {}

    // PopulationLog(PopulationLog&& population_log):
    //     generations_(std::move(population_log.generations_)),
    //     num_improvements_(population_log.num_improvements_) {}

    // PopulationLog& operator=(PopulationLog&& population_log)
    // {
    //     generations_ = std::move(population_log.generations_);
    //     num_improvements_ = population_log.num_improvements_;
    //     return *this;
    // }

    void add(GenerationLog generation_log)
    {
        if (max_result_ < generation_log.max_result_)
        {
            ++num_improvements_;
        }
        min_result_ = std::min(min_result_, generation_log.max_result_);
        max_result_ = std::max(max_result_, generation_log.max_result_);
        average_result_ += generation_log.max_result_;
        ++num_samples_;
        generations_.push_back(std::move(generation_log));
    }

    virtual void start()
    {
        TimeLog::start();
        min_result_ = LLONG_MAX;
        max_result_ = LLONG_MIN;
        average_result_ = 0.;
        num_samples_ = 0;
        generations_.clear();
    }

    virtual void end()
    {
        TimeLog::end();
        average_result_ /= num_samples_;
    }
};

struct NodeLog: public TimeLog
{
    std::string node_id_;
    PopulationLog population_log_;
    std::vector<NodeLog> ch_node_logs_;

    NodeLog(std::string node_id): node_id_(std::move(node_id)) {}

    // NodeLog(NodeLog&& node_log):
    //     node_id_(std::move(node_log.node_id_)),
    //     population_log_(std::move(node_log.population_log_)),
    //     ch_node_logs_(std::move(node_log.ch_node_logs_)) {}
};
