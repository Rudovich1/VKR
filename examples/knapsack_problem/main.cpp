#include "functions.hpp"
#include "../../genetic_method/src/genetic_algorithm.hpp"
#include "../../knapsack-problem/knapsack_problem_generator.hpp"

#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <iostream>


std::string to_json_field(std::string name, std::string data)
{
    return "\"" + name + "\": \"" + data + "\"";
}

std::string to_json_field(std::string name, size_t data)
{
    return "\"" + name + "\": \"" + std::to_string(data) + "\"";
}

std::string to_json_field(std::string name, double data)
{
    return "\"" + name + "\": \"" + std::to_string(data) + "\"";
}

std::stringstream& population_log_to_json(std::stringstream& ss, const PopulationLog& pop_log)
{
    ss << "{\n";
    ss << to_json_field("min_result", pop_log.min_result_) << ",\n";
    ss << to_json_field("max_result", pop_log.max_result_) << ",\n";
    ss << to_json_field("average_result", pop_log.average_result_) << ",\n";
    ss << to_json_field("num_improvements", pop_log.num_improvements_) << ",\n";
    ss << to_json_field("num_iters", pop_log.generations_.size()) << ",\n";
    ss << to_json_field("work_time", std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(pop_log.end_ - pop_log.start_).count()/1000.) + "s.") << "\n";
    ss << "}";
    return ss;
}

std::stringstream& node_log_to_json(std::stringstream& ss, const NodeLog& node_log)
{
    ss << "{\n";
    ss << to_json_field("node_id", node_log.node_id_) << ",\n";
    ss << to_json_field("work_time", std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(node_log.end_ - node_log.start_).count()/1000.) + "s.") << ",\n";
    ss << "\"" << "population_log" << "\": ";
    population_log_to_json(ss, node_log.population_log_) << ",\n";
    ss << "\"" << "sub_nodes" << "\": [\n";
    for (size_t i = 0; i < node_log.ch_node_logs_.size(); ++i)
    {
        node_log_to_json(ss, *node_log.ch_node_logs_[i]);
        if (i != node_log.ch_node_logs_.size() - 1) {ss << ",\n";}
    }
    ss << "\n]\n}";
    return ss;
}

KnapsackProblem kp = KnapsackProblemGenerator::loadProblem("task1.txt");


std::array<std::shared_ptr<Node<bool>>, 1> make_layer(
    std::array<std::shared_ptr<Node<bool>>, 1> pref,
    size_t generation_size,
    size_t num_not_imp,
    double num_mutations)
{
    return pref;
}

template<size_t num_nodes>
std::array<std::shared_ptr<Node<bool>>, 1> make_layer(
    std::array<std::shared_ptr<Node<bool>>, num_nodes> pref,
    size_t generation_size,
    size_t num_not_imp,
    double num_mutations)
{
    std::array<std::shared_ptr<Node<bool>>, num_nodes / 2> res;

    for (size_t i = 0; i < num_nodes / 2; ++i)
    {
        std::string k_name = std::to_string(num_nodes / 2) + "_" + std::to_string(i + 1);
        std::array<std::shared_ptr<Node<bool>>, 2> arr = {std::move(pref[i*2]), std::move(pref[i*2 + 1])};
        auto k_node = std::make_shared<K_Node<bool, 2>>(std::move(k_name), std::move(arr));
        setFuns<2>(*k_node, kp, generation_size, 1./(kp.size_)*num_mutations, generation_size, num_not_imp);
        res[i] = std::move(k_node);
    }
    return make_layer(std::move(res), generation_size, num_not_imp, num_mutations / 2);
}

template<size_t num_nodes>
std::array<std::shared_ptr<Node<bool>>, 1> make_layer(
    size_t generation_size,
    size_t num_not_imp,
    double num_mutations)
{
    std::array<std::shared_ptr<Node<bool>>, num_nodes> res;
    for (size_t i = 0; i < num_nodes; ++i)
    {
        std::string p_name = std::to_string(num_nodes) + "_" + std::to_string(i + 1);
        auto p_node = std::make_shared<PopulationNode<bool>>(std::move(p_name));
        setFuns(*p_node, kp, generation_size, 1./(kp.size_)*num_mutations, generation_size, num_not_imp);
        res[i] = std::move(p_node);
    }
    return make_layer(std::move(res), generation_size, num_not_imp, num_mutations / 2);
}


template<size_t num_nodes>
std::shared_ptr<Node<bool>> tmp(size_t generation_size, size_t num_pairs, size_t num_not_imp, double num_mutations)
{
    std::array<std::shared_ptr<Node<bool>>, num_nodes> p_nodes;
    for (size_t i = 0; i < num_nodes; ++i)
    {
        std::string p_name = "p" + std::to_string(i + 1);
        auto p_node = std::make_shared<PopulationNode<bool>>(std::move(p_name));
        setFuns(*p_node, kp, generation_size, 1./(kp.size_)*num_mutations, num_pairs, num_not_imp);
        p_nodes[i] = std::move(p_node);
    }
    std::string k_name = "k";
    auto k_node = std::make_shared<K_Node<bool, num_nodes>>(std::move(k_name), std::move(p_nodes));
    setFuns<num_nodes>(*k_node, kp, generation_size, 1./(kp.size_)*num_mutations, num_pairs, num_not_imp);
    return k_node;
}

int main()
{
    auto tree = make_layer<4>(200, 200, 4);

    General<bool> general(std::move(tree[0]), true, true);
    auto [res, log] = general.calc();
    std::ofstream logs("log_bintree4_one.json");
    std::stringstream ss;
    node_log_to_json(ss, *log);
    logs << ss.str();
}