#include "functions.hpp"
#include "../../knapsack-problem/knapsack_problem_generator.hpp"

#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <iostream>
#include <thread>


int main()
{
    KnapsackProblem kp = KnapsackProblemGenerator::loadProblem("task1.txt");
    Data data(std::move(kp));
    data.max_v = LLONG_MIN;
    data.min_w = LLONG_MAX;
    for (const auto& item: data.kp.items_)
    {
        data.max_v = std::max(data.max_v, item.value_);
        data.min_w = std::min(data.min_w, item.weight_);
    }
    data.generation_size = 200;
    data.max_times = 100;
    data.mutation_coef = 0.001;
    data.num_new_pairs = 400;

    auto node1 = std::make_shared<PopulationNode<bool, long long>>();
    auto node2 = std::make_shared<UnaryNode<bool, long long>>(node1);
    auto node_log1 = std::make_shared<NodeLog>("node1");
    auto node_log2 = std::make_shared<NodeLog>("node2");

    node_log2->ch_node_logs_.push_back(node_log1);

    setFuns(*node1.get(), data);
    setFuns(*node2.get(), data);
    setLogFuns(*node1.get(), *node_log1.get());
    setLogFuns(*node2.get(), *node_log2.get());

    std::thread runtime_log(runtimeLog, std::ref(node_log2));
    auto res_population = node2->evolution();
    runtime_log.join();
}