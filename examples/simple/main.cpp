#include "functions.hpp"
#include <cmath>

HeuristicAlgorithm::GeneticAlgorithm::GeneticAlgorithm<double, double> make_ga(
    std::function<double(double)>& y,
    std::pair<double, double>& best_res,
    double& coef, 
    double step,
    std::chrono::seconds work_time,
    std::chrono::steady_clock::time_point& start_time)
{

    HeuristicAlgorithm::GeneticAlgorithm::GeneticAlgorithm<double, double> ga;
    ga.setAnyFunction(std::make_shared<AnyFunction>(coef, step));
    ga.setConditionsForStopping(std::make_shared<ConditionsForStopping>(work_time, start_time));
    ga.setCrossingover(std::make_shared<Crossingover>());
    ga.setEndEvolutionLog(std::make_shared<EndEvolutionLog>());
    ga.setFitnessFunction(std::make_shared<FitnessFunction>(y, best_res));
    ga.setMutation(std::make_shared<Mutation>(coef));
    ga.setNewGenerationLog(std::make_shared<NewGenerationLog>(best_res));
    ga.setSelection(std::make_shared<Selection>());
    ga.setStartEvolutionLog(std::make_shared<StartEvolutionLog>(start_time));
    return ga;
}

int main()
{
    std::function<double(double)> y = [](double x) {return 1*x*x*x*x - 12*x*x*x + 123*x*x - 1234*x + 12345;};
    std::pair<double, double> best_res = {1000, y(1000)};
    double coef = 100000;
    double step = 2;
    std::chrono::seconds work_time(1);
    std::chrono::steady_clock::time_point start_time;

    auto ga = make_ga(y, best_res, coef, step, work_time, start_time);

    HeuristicAlgorithm::Graph::Node<Population_, Population_, Population_> node("node");
    node.setAlgorithm(std::make_shared<HeuristicAlgorithm::GeneticAlgorithm::GeneticAlgorithm<double, double>>(ga));
    node.setDeserialization(std::make_shared<Deserialization>());
    node.setEndNodeLog(std::make_shared<EndNodeLog>());
    node.setSerialization(std::make_shared<Serialization>());
    node.setStartNodeLog(std::make_shared<StartNodeLog>());

    HeuristicAlgorithm::Graph::Graph<Population_> graph;
    graph.add_node(std::make_shared<HeuristicAlgorithm::Graph::Node<Population_, Population_, Population_>>(node));
    graph.evolution(1);
}