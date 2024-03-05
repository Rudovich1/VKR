#include "functions.hpp"
#include "../../genetic_method/src/genetic_algorithm.hpp"
#include "../../knapsack-problem/knapsack_problem_generator.hpp"

#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>


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
    ss << to_json_field("work_time", std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(pop_log.end_ - pop_log.start_).count()/1000.) + "s.") << ",\n";
    ss << "\"generations\": [\n";
    for (size_t i = 0; i < pop_log.generations_.size(); ++i)
    {
        ss << "{\n";
        ss << to_json_field("generation_id", i + 1) << ",\n";
        ss << to_json_field("min_result", pop_log.generations_[i].min_result_) << ",\n";
        ss << to_json_field("max_result_", pop_log.generations_[i].max_result_) << ",\n";
        ss << to_json_field("average_result", pop_log.generations_[i].average_result_) << ",\n";
        ss << to_json_field("work_time", std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(pop_log.generations_[i].end_ - pop_log.generations_[i].start_).count()/1000.) + "s.");
        ss << "\n}";
        if (i != pop_log.generations_.size() - 1) {ss << ",\n";}
    }
    ss << "\n]\n}";
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


int main()
{
    auto kp = KnapsackProblemGenerator::loadProblem("task1.txt");
    
    size_t kp_size = kp.size_;
    size_t p_n_size = 100;

    auto pop_node1 = std::make_shared<PopulationNode<bool>>("p_n_1");
    setFuns(*pop_node1, kp, p_n_size, 1./(kp_size)*20, p_n_size, 50);
    auto pop_node2 = std::make_shared<PopulationNode<bool>>("p_n_2");
    setFuns(*pop_node2, kp, p_n_size, 1./(kp_size)*20, p_n_size, 50);
    auto pop_node3 = std::make_shared<PopulationNode<bool>>("p_n_3");
    setFuns(*pop_node3, kp, p_n_size, 1./(kp_size)*20, p_n_size, 50);
    auto pop_node4 = std::make_shared<PopulationNode<bool>>("p_n_4");
    setFuns(*pop_node4, kp, p_n_size, 1./(kp_size)*20, p_n_size, 50);
    auto pop_node5 = std::make_shared<PopulationNode<bool>>("p_n_5");
    setFuns(*pop_node5, kp, p_n_size, 1./(kp_size)*20, p_n_size, 50);
    auto pop_node6 = std::make_shared<PopulationNode<bool>>("p_n_6");
    setFuns(*pop_node6, kp, p_n_size, 1./(kp_size)*20, p_n_size, 50);
    auto pop_node7 = std::make_shared<PopulationNode<bool>>("p_n_7");
    setFuns(*pop_node7, kp, p_n_size, 1./(kp_size)*20, p_n_size, 50);
    auto pop_node8 = std::make_shared<PopulationNode<bool>>("p_n_8");
    setFuns(*pop_node8, kp, p_n_size, 1./(kp_size)*20, p_n_size, 50);
    auto pop_node9 = std::make_shared<PopulationNode<bool>>("p_n_9");
    setFuns(*pop_node9, kp, p_n_size, 1./(kp_size)*20, p_n_size, 50);

    size_t u_n_size = p_n_size * 2;

    auto unary_node1 = std::make_shared<UnaryNode<bool>>("u_n_1", std::move(pop_node1));
    setFuns(*unary_node1, kp, u_n_size, 1./(kp_size)*1, u_n_size * 2, 100);
    auto unary_node2 = std::make_shared<UnaryNode<bool>>("u_n_2", std::move(pop_node2));
    setFuns(*unary_node2, kp, u_n_size, 1./(kp_size)*1, u_n_size * 2, 100);
    auto unary_node3 = std::make_shared<UnaryNode<bool>>("u_n_3", std::move(pop_node3));
    setFuns(*unary_node3, kp, u_n_size, 1./(kp_size)*1, u_n_size * 2, 100);
    auto unary_node4 = std::make_shared<UnaryNode<bool>>("u_n_4", std::move(pop_node4));
    setFuns(*unary_node4, kp, u_n_size, 1./(kp_size)*1, u_n_size * 2, 100);
    auto unary_node5 = std::make_shared<UnaryNode<bool>>("u_n_5", std::move(pop_node5));
    setFuns(*unary_node5, kp, u_n_size, 1./(kp_size)*1, u_n_size * 2, 100);
    auto unary_node6 = std::make_shared<UnaryNode<bool>>("u_n_6", std::move(pop_node6));
    setFuns(*unary_node6, kp, u_n_size, 1./(kp_size)*1, u_n_size * 2, 100);
    auto unary_node7 = std::make_shared<UnaryNode<bool>>("u_n_7", std::move(pop_node7));
    setFuns(*unary_node7, kp, u_n_size, 1./(kp_size)*1, u_n_size * 2, 100);
    auto unary_node8 = std::make_shared<UnaryNode<bool>>("u_n_8", std::move(pop_node8));
    setFuns(*unary_node8, kp, u_n_size, 1./(kp_size)*1, u_n_size * 2, 100);
    auto unary_node9 = std::make_shared<UnaryNode<bool>>("u_n_9", std::move(pop_node9));
    setFuns(*unary_node9, kp, u_n_size, 1./(kp_size)*1, u_n_size * 2, 100);

    std::array<std::shared_ptr<Node<bool>>, 3> ar1 = {std::move(unary_node1), std::move(unary_node2), std::move(unary_node3)};
    std::array<std::shared_ptr<Node<bool>>, 3> ar2 = {std::move(unary_node4), std::move(unary_node5), std::move(unary_node6)};
    std::array<std::shared_ptr<Node<bool>>, 3> ar3 = {std::move(unary_node7), std::move(unary_node8), std::move(unary_node9)};

    size_t k_n_size = p_n_size * 2;

    auto k_node1 = std::make_shared<K_Node<bool, 3>>("k_n_1", std::move(ar1));
    setFuns<3>(*k_node1, kp, k_n_size, 1./(kp_size)*10, k_n_size, 50);
    auto k_node2 = std::make_shared<K_Node<bool, 3>>("k_n_2", std::move(ar2));
    setFuns<3>(*k_node2, kp, k_n_size, 1./(kp_size)*10, k_n_size, 50);
    auto k_node3 = std::make_shared<K_Node<bool, 3>>("k_n_3", std::move(ar3));
    setFuns<3>(*k_node3, kp, k_n_size, 1./(kp_size)*10, k_n_size, 50);

    auto unary_node11 = std::make_shared<UnaryNode<bool>>("u_n_11", std::move(k_node1));
    setFuns(*unary_node11, kp, u_n_size * 2, 1./(kp_size)*0.1, u_n_size * 4, 100);
    auto unary_node12 = std::make_shared<UnaryNode<bool>>("u_n_12", std::move(k_node2));
    setFuns(*unary_node12, kp, u_n_size * 2, 1./(kp_size)*0.1, u_n_size * 4 * 2, 100);
    auto unary_node13 = std::make_shared<UnaryNode<bool>>("u_n_13", std::move(k_node3));
    setFuns(*unary_node13, kp, u_n_size * 2, 1./(kp_size)*0.1, u_n_size * 4 * 2, 100);

    std::array<std::shared_ptr<Node<bool>>, 3> ar11 = {std::move(unary_node11), std::move(unary_node12), std::move(unary_node13)};

    auto k_node11 = std::make_shared<K_Node<bool, 3>>("k_n_11", std::move(ar11));
    setFuns<3>(*k_node11, kp, k_n_size * 2, 1./(kp_size)*5, k_n_size * 2, 50);

    auto unary_node111 = std::make_shared<UnaryNode<bool>>("u_n_111", std::move(k_node11));
    setFuns(*unary_node111, kp, u_n_size * 3, 1./(kp_size)*0.01, u_n_size * 6, 100);

    General<bool> general(std::move(unary_node111));
    auto [res, log] = general.calc();
    std::ofstream logs("log.json");
    std::stringstream ss;
    node_log_to_json(ss, *log);
    logs << ss.str();
}