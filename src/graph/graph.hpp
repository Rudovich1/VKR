#pragma once

#include "node.hpp"

#include <unordered_map>
#include <memory>
#include <vector>
#include <stdexcept>
#include <stack>
#include <string>
#include <unordered_set>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <algorithm>

namespace HeuristicAlgorithm
{
    namespace Graph
    {
        using namespace Interfaces;


        enum class NodeTaskStatus {
            not_in_q,
            in_q,
            completed
        };

        template<class SerializationType>
        struct NodeTask
        {
            using NodeTask_ = NodeTask<SerializationType>;
            using BaseNode_ = BaseNode<SerializationType>;

            NodeTask(std::shared_ptr<BaseNode_> task): task_(task), status_(NodeTaskStatus::not_in_q) {}

            void add_dep(std::shared_ptr<NodeTask_> pref_task)
            {
                if (dependencies_.find(pref_task->task_->id_) != dependencies_.end())
                {
                    std::__throw_logic_error("duplicate dependencies");
                }
                dependencies_[pref_task->task_->id_] = pref_task;
            }

            void add_next(std::shared_ptr<NodeTask_> next_task)
            {
                next_nodes_.push_back(next_task);
            }

            void rem_dep(const std::string& node, SerializationType& ser_data)
            {
                dependencies_.erase(node);
                inputs_.push_back(ser_data);
            }

            SerializationType calc()
            {
                if (!dependencies_.empty())
                {
                    std::__throw_logic_error("unresolved dependencies");
                }
                return task_->evolution(inputs_);
            }

            std::unordered_map<std::string, std::shared_ptr<NodeTask_>> dependencies_;
            std::vector<std::shared_ptr<NodeTask_>> next_nodes_;
            std::vector<SerializationType> inputs_;
            std::shared_ptr<BaseNode_> task_;
            NodeTaskStatus status_;
        };

        template<class SerializationType>
        struct ThreadPool 
        {
            using BaseNode_ = BaseNode<SerializationType>;
            using NodeTask_ = NodeTask<SerializationType>;

            ThreadPool(const size_t num_threads) 
            {
                threads_.reserve(num_threads);
                for (int i = 0; i < num_threads; ++i) 
                {
                    threads_.emplace_back(&ThreadPool::run, this);
                }
            }

            void add_task(std::shared_ptr<NodeTask_> task) 
            {
                last_idx_++;
                std::lock_guard<std::mutex> q_lock(q_mtx_);
                q_.emplace(task);
                q_cv_.notify_one();
            }

            void wait_all() 
            {
                std::unique_lock<std::mutex> lock(tasks_info_mtx_);
                wait_all_cv_.wait(lock, [this]()->bool { return cnt_completed_tasks_ == last_idx_; });
            }

            ~ThreadPool() 
            {
                quite_ = true;
                q_cv_.notify_all();
                for (int i = 0; i < threads_.size(); ++i) {
                    threads_[i].join();
                }
            }

            void run() {
                while (!quite_) {
                    std::unique_lock<std::mutex> lock(q_mtx_);
                    q_cv_.wait(lock, [this]()->bool { return !q_.empty() || quite_; });

                    if (!q_.empty() && !quite_) {
                        std::shared_ptr<NodeTask_> task = std::move(q_.front());
                        q_.pop();
                        task->status_ = NodeTaskStatus::in_q;
                        lock.unlock();

                        auto res = task->calc();

                        ++cnt_completed_tasks_;
                        task->status_ = NodeTaskStatus::completed;

                        for (std::shared_ptr<NodeTask_> next_task: task->next_nodes_)
                        {
                            next_task->rem_dep(task->task_->id_, res);
                            if (next_task->status_ == NodeTaskStatus::not_in_q && next_task->dependencies_.empty())
                            {
                                add_task(next_task);
                            }
                        }
                    }
                    wait_all_cv_.notify_all();
                }
            }

            std::vector<std::thread> threads_;

            std::queue<std::shared_ptr<NodeTask_>> q_;
            std::mutex q_mtx_;
            std::condition_variable q_cv_;
            
            std::mutex tasks_info_mtx_;

            std::condition_variable wait_all_cv_;

            std::atomic<bool> quite_{ false };
            std::atomic<uint64_t> last_idx_{ 0 };
            std::atomic<uint64_t> cnt_completed_tasks_{ 0 };
        };


        template<class SerializationType>
        struct Graph
        {
            using BaseNode_ = BaseNode<SerializationType>;

            void add_node(std::shared_ptr<BaseNode_> node)
            {
                if (nodes_.find(node->id_) != nodes_.end())
                {
                    std::__throw_logic_error(("node \"" + node->id_ + "\" already exists").c_str());
                }
                nodes_[node->id_] = node;
                adj_list_[node->id_] = {};
            }

            void add_edge(std::string& from_node, std::string& to_node)
            {
                if (nodes_.find(from_node) == nodes_.end())
                {
                    std::__throw_logic_error(("node \"" + from_node + "\" not exists").c_str());
                }
                if (nodes_.find(to_node) == nodes_.end())
                {
                    std::__throw_logic_error(("node \"" + to_node + "\" not exists").c_str());
                }
                adj_list_[from_node].push_back(to_node);
            }

            void evolution(size_t num_threads = 1)
            {
                auto sorted_nodes = top_sort();
                ThreadPool<SerializationType> tp(num_threads);
                std::unordered_map<std::string, std::shared_ptr<NodeTask<SerializationType>>> node_tasks;
                for (auto& [node, _]: sorted_nodes)
                {
                    node_tasks[node] = std::make_shared<NodeTask<SerializationType>>(nodes_[node]);
                }
                for (auto& [node, _]: sorted_nodes)
                {
                    for (auto& next_node: adj_list_[node])
                    {
                        node_tasks[node]->add_next(node_tasks[next_node]);
                        node_tasks[next_node]->add_dep(node_tasks[node]);
                    }
                }
                for (auto& [node, _]: sorted_nodes)
                {
                    if (node_tasks[node]->dependencies_.empty() && node_tasks[node]->status_ == NodeTaskStatus::not_in_q)
                    {
                        tp.add_task(node_tasks[node]);
                    }
                }
                tp.wait_all();
            }

        protected:

            std::vector<std::pair<std::string, size_t>> top_sort()
            {
                std::unordered_map<std::string, int> used;
                std::vector<std::pair<std::string, size_t>> res;
                auto& adj_list = adj_list_;

                std::function<void(const std::string&)> dfs = [&](const std::string& node)
                {
                    used[node] = 0;
                    for (const auto& next: adj_list_[node])
                    {
                        if (used.find(next) == used.end())
                        {
                            dfs(next);
                        }
                        if (used[next] == 0)
                        {
                            std::__throw_logic_error("graph has a cycle");
                        }
                    }
                    used[node] = 1;
                    res.push_back({node, 0});
                };

                for (const auto& [node, _]: adj_list_)
                {
                    if (used.find(node) == used.end())
                    {
                        dfs(node);
                    }
                }

                std::reverse(res.begin(), res.end());
                for (auto& [node, idx]: res)
                {
                    idx = used[node];
                    for (const auto& next: adj_list_[node])
                    {
                        used[next] = std::max(used[next], (int)(idx + 1));
                    }
                    --idx;
                }

                return res;
            }
            
            std::unordered_map<std::string, std::shared_ptr<BaseNode_>> nodes_;
            std::unordered_map<std::string, std::vector<std::string>> adj_list_;
        };
    }
}