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


std::string log_to_json(const std::unordered_map<std::string, PopulationLog>& log)
{
    std::stringstream ss;
    ss << "[\n";
    size_t ind = 0;
    for (auto[node_id, data]: log)
    {
        ss << "{\n";
        ss << to_json_field("node_id", node_id) << ",\n";
        ss << to_json_field("min_result", data.min_result_) << ",\n";
        ss << to_json_field("max_result", data.max_result_) << ",\n";
        ss << to_json_field("average_result", data.average_result_) << ",\n";
        ss << to_json_field("num_improvements", data.num_improvements) << ",\n";
        ss << to_json_field("work_time", std::chrono::duration_cast<std::chrono::milliseconds>(data.end_ - data.start_).count()/1000.) << ",\n";
        ss << "\"generations\": [\n";
        for (size_t i = 0; i < data.generations.size(); ++i)
        {
            ss << "{\n";
            ss << to_json_field("generation_id", i + 1) << ",\n";
            ss << to_json_field("min_result", data.generations[i].min_result_) << ",\n";
            ss << to_json_field("max_result_", data.generations[i].max_result_) << ",\n";
            ss << to_json_field("average_result", data.generations[i].average_result_) << ",\n";
            ss << to_json_field("work_time", std::chrono::duration_cast<std::chrono::milliseconds>(data.generations[i].end_ - data.generations[i].start_).count()/1000.);
            ss << "\n}";
            if (i != data.generations.size() - 1) {ss << ",\n";}
        }
        ss << "\n]\n}";
        if (ind != log.size() - 1) {ss << ",\n";}
        ++ind;
    }
    ss << "\n]";
    return ss.str();
}

int main()
{
    auto kp = KnapsackProblemGenerator::loadProblem("task1.txt");
    
    auto pop_node1 = std::make_unique<PopulationNode<bool>>("population_node_1");
    setFuns(*pop_node1, kp, 100, 0.1, 500, 10);
    // auto pop_node2 = std::make_unique<PopulationNode<bool>>("population_node_2");
    // setFuns(*pop_node2, kp, 1000, 0.1, 5000, 200);
    // auto pop_node3 = std::make_unique<PopulationNode<bool>>("population_node_3");
    // setFuns(*pop_node3, kp, 1000, 0.1, 5000, 200);
    // auto pop_node4 = std::make_unique<PopulationNode<bool>>("population_node_4");
    // setFuns(*pop_node4, kp, 1000, 0.1, 5000, 200);
    // auto pop_node5 = std::make_unique<PopulationNode<bool>>("population_node_5");
    // setFuns(*pop_node5, kp, 1000, 0.1, 5000, 200);
    // auto pop_node6 = std::make_unique<PopulationNode<bool>>("population_node_6");
    // setFuns(*pop_node6, kp, 1000, 0.1, 5000, 200);
    // auto pop_node7 = std::make_unique<PopulationNode<bool>>("population_node_7");
    // setFuns(*pop_node7, kp, 1000, 0.1, 5000, 200);
    // auto pop_node8 = std::make_unique<PopulationNode<bool>>("population_node_8");
    // setFuns(*pop_node8, kp, 1000, 0.1, 5000, 200);
    // auto pop_node9 = std::make_unique<PopulationNode<bool>>("population_node_9");
    // setFuns(*pop_node9, kp, 1000, 0.1, 5000, 200);

    General<bool> general(std::move(pop_node1));
    auto res = general.calc();
    auto log = Node<bool>::logs_;
    std::ofstream logs("log.json");
    logs << log_to_json(log);
}