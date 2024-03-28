#pragma once

#include <chrono>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <string>
#include "ncurses.h"
#include <thread>
#include <iomanip>
#include <exception>


using namespace GeneticAlgorithm;

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
        average_result_ += (sample - average_result_) / (++num_samples_);
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
    }
};

struct PopulationLog: public BaseLog
{
    std::vector<GenerationLog> generations_;
    size_t num_improvements_ = 0;

    void add(GenerationLog generation_log)
    {
        if (max_result_ < generation_log.max_result_)
        {
            ++num_improvements_;
        }
        min_result_ = std::min(min_result_, generation_log.max_result_);
        max_result_ = std::max(max_result_, generation_log.max_result_);
        average_result_ += (generation_log.max_result_ - average_result_) / (++num_samples_);
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
    }
};

struct NodeLog: public TimeLog
{
    enum class NodeStatus
    {
        WAIT = 0,
        IN_WORK = 1,
        DONE = 2
    };

    std::string node_id_;
    PopulationLog population_log_;
    NodeStatus status_;
    std::vector<std::shared_ptr<NodeLog>> ch_node_logs_;

    NodeLog(std::string node_id): node_id_(std::move(node_id)), status_(NodeStatus::WAIT) {}

    virtual void start() override
    {
        TimeLog::start();
    }

    virtual void evolution_start()
    {
        status_ = NodeStatus::IN_WORK;
    }

    virtual void end() override
    {
        TimeLog::end();
        status_ = NodeStatus::DONE;
    }

    std::string nodeToStr()
    {
        std::string res = node_id_ + ": ";
        switch (status_)
        {
        case NodeStatus::WAIT:
        {
            res += "Wait";
            break;
        }
        case NodeStatus::IN_WORK:
        {
            auto cur_work_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start_).count();
            res += std::to_string(cur_work_time) + "s.";
            break;
        }
        case NodeStatus::DONE:
        {
            auto work_time = std::chrono::duration_cast<std::chrono::seconds>(end_ - start_).count();
            res += std::to_string(work_time) + "s.";
            break;
        }
        default:
            break;
        }
        return res;
    }

    std::string setl(std::string str, size_t len)
    {
        if (str.size() > len)
        {
            throw std::logic_error("setw error");
        }
        size_t left_indent = (len - str.size()) / 2;
        size_t right_indent = (len - str.size() + 1) / 2;
        str = std::string(left_indent, ' ') + std::move(str) + std::string(right_indent, ' ');
        return std::move(str);
    }

    std::deque<std::string> toLines()
    {
        std::deque<std::string> res;
        std::string node_info = nodeToStr();
        std::string best_res;
        std::string cur_res;
        size_t len = node_info.size();
        if (population_log_.max_result_ == LLONG_MIN)
        {
            best_res = "None";
            cur_res = "None";
            len = std::max(len, best_res.size());
        }
        else
        {
            best_res = std::to_string((size_t)population_log_.max_result_);
            cur_res = std::to_string((size_t)population_log_.generations_.back().average_result_);
            len = std::max(len, std::max(best_res.size(), cur_res.size()));
        }
        if (ch_node_logs_.size() == 0)
        {
            res.push_front(setl(std::move(cur_res), len));
            res.push_front(setl(std::move(best_res), len));
            res.push_front(setl(std::move(node_info), len));
            return res;
        }
        std::vector<std::deque<std::string>> children_lines(ch_node_logs_.size());
        for (size_t i = 0; i < ch_node_logs_.size(); ++i)
        {
            children_lines[i] = ch_node_logs_[i]->toLines();
        }
        res = children_lines[0];
        std::string sub_links;
        sub_links += std::string((children_lines[0][0].size() - 1) / 2, ' ') + "|" + std::string((children_lines[0][0].size()) / 2, ' ');
        for (size_t i = 1; i < children_lines.size(); ++i)
        {
            sub_links += " " + std::string((children_lines[i][0].size() - 1) / 2, ' ') + "|" + std::string((children_lines[i][0].size()) / 2, ' ');
            size_t cur_line_lenght = res[0].size();
            for (size_t j = 0; j < children_lines[i].size(); ++j)
            {
                if (j == res.size())
                {
                    res.push_back(std::string(cur_line_lenght, ' '));
                }
                res[j] += " " + children_lines[i][j];
            }
            for (size_t j = children_lines[i].size(); j < res.size(); ++j)
            {
                res[j] += std::string(children_lines[i][0].size() + 2, ' ');
            }
        }
        size_t left_indent = (children_lines[0][0].size() - 1) / 2;
        size_t right_indent = (children_lines.back()[0].size()) / 2;
        std::string strip = std::string(left_indent, ' ') + 
                            std::string(res[0].size() - left_indent - right_indent, '-') + 
                            std::string(right_indent, ' ');
        std::string link = std::string((res[0].size() - 1) / 2, ' ') + "|" + std::string(res[0].size() / 2, ' ');

        len = std::max(len, res[0].size());

        res.push_front(std::move(sub_links));
        res.push_front(std::move(strip));
        res.push_front(std::move(link));
        res.push_front(std::move(cur_res));
        res.push_front(std::move(best_res));
        res.push_front(std::move(node_info));

        for (size_t i = 0; i < res.size(); ++i)
        {
            res[i] = setl(std::move(res[i]), len);
        } 
        return res;
    }
};


void runtimeLog(std::shared_ptr<NodeLog> root)
{
    initscr();
    curs_set(0);

    do
    {
        auto lines = root->toLines();
        for (size_t i = 0; i < lines.size(); ++i)
        {
            mvprintw(i, 0, lines[i].c_str());
        }
        refresh();
        std::this_thread::sleep_for(std::chrono::microseconds(200));
    } 
    while (root->status_ != NodeLog::NodeStatus::DONE);

    auto lines = root->toLines();
    clear();
    for (size_t i = 0; i < lines.size(); ++i)
    {
        mvprintw(i, 0, lines[i].c_str());
    }
    mvprintw(lines.size() + 1, 0, "=== DONE ===");
    refresh();
    getch();

    endwin();
}