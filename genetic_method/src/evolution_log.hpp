#pragma once

#include <chrono>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <string>
#include "ncurses.h"
#include <thread>


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

    virtual void end() override
    {
        TimeLog::end();
        status_ = NodeStatus::DONE;
    }

    void check()
    {
        if (status_ != NodeStatus::WAIT) {return;}
        for (size_t i = 0; i < ch_node_logs_.size(); ++i)
        {
            if (ch_node_logs_[i]->status_ != NodeStatus::DONE)
            {
                return;
            }
        }
        status_ = NodeStatus::IN_WORK;
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
            res += "In_Work-" + std::to_string(cur_work_time) + "-s.";
            break;
        }
        case NodeStatus::DONE:
        {
            auto work_time = std::chrono::duration_cast<std::chrono::seconds>(end_ - start_).count();
            res += "Done-" + std::to_string(work_time) + "-s.";
            break;
        }
        default:
            break;
        }
        return res;
    }

    std::deque<std::string> toLines()
    {
        check();
        std::deque<std::string> res;
        if (ch_node_logs_.empty())
        {
            std::string node_info = nodeToStr();
            res.push_front(node_info);
            return res;
        }
        std::vector<std::deque<std::string>> children_lines(ch_node_logs_.size());
        for (size_t i = 0; i < ch_node_logs_.size(); ++i)
        {
            children_lines[i] = ch_node_logs_[i]->toLines();
        }
        std::string node_info = nodeToStr();
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

        res.push_front(std::move(sub_links));
        res.push_front(std::move(strip));
        res.push_front(std::move(link));

        if (res[0].size() >= node_info.size())
        {
            left_indent = (res[0].size() - node_info.size()) / 2;
            right_indent = (res[0].size() - node_info.size() + 1) / 2;
            std::string head = std::string(left_indent, ' ') + node_info + std::string(right_indent, ' ');
            res.push_front(std::move(head));
        }
        else
        {
            left_indent = (node_info.size() - res[0].size()) / 2;
            right_indent = (node_info.size() - res[0].size() + 1) / 2;
            for (size_t i = 0; i < res.size(); ++i)
            {
                res[i] = std::string(left_indent, ' ') + res[i] + std::string(right_indent, ' ');
            }
            res.push_front(std::move(node_info));
        }
        return res;
    }
};


void runtimeLog(std::shared_ptr<NodeLog> root)
{
    initscr();
    scrollok(stdscr, TRUE);
    curs_set(0);

    while(root->status_ != NodeLog::NodeStatus::DONE)
    {
        auto lines = root->toLines();
        clear();
        for (size_t i = 0; i < lines.size(); ++i)
        {
            mvprintw(i, 0, lines[i].c_str());
        }
        refresh();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    getch();
    endwin();
}