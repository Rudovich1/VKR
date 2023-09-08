#define GM_LOG

#include "../../tools/loger.hpp"
#include "../../knapsack_problem/knapsack_problem.hpp"
#include "functions.hpp"

#include <chrono>
#include <iostream>
#include <fstream>

int main()
{
    size_t generation_size = 10;
    size_t num_marriage_couples = generation_size;
    double mutation_coef = 0.01;
    double cross_coef = 0.8;
    size_t buffer_size = 1;
    size_t chromosome_size = 5;
    KnapsackProblem kp(5, {{1, 1}, {1, 2}, {2, 1}, {2, 2}, {3, 3}}, 3);
    std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
    std::chrono::seconds work_time(5);

    GeneticAlgorithm::Conditions::BaseConditions<bool> conditions(
        FitnessFunction()(kp),
        SelectionFunction()(generation_size),
        MutationFunction()(mutation_coef, num_marriage_couples),
        HomogeneousCrossingoverFunction()(cross_coef, num_marriage_couples),
        ConditionsForStopping()(start_time, work_time),
        AnyFunction()(),
        EndNodeFunction()(),
        buffer_size, generation_size, chromosome_size
    );

    GeneticAlgorithm::EvolutionTree::Node<bool>* node = new GeneticAlgorithm::EvolutionTree::PopulationNode<bool>(
        conditions,
        StartGenerationFunction()(kp, generation_size)
    ); 

    GeneticAlgorithm::General<bool, GeneticAlgorithm::Types::Chromosome<bool>> general(node, ResultFunction()(kp));

    auto result = general.calc();

    std::ofstream out("log.txt");

    _RUN_DUMP_("");
    _GENERAL_DUMP_("");
    _LOG_DUMP_(out, 3, false, "");

    std::cout << kp;
    std::cout << "Result: " << result.getFitness().getVal() - 1 << '\n';
    for (auto i: result.get())
    {
        std::cout << i.get() << " ";
    }
    
    /* Пример вывода:
        Knapsack size: 5
        Knapsack capacity: 3
        (         1 |          2)
        (         1 |          1)
        (         2 |          2)
        (         3 |          3)
        (         2 |          1)
        ------------------------
                9 |          9
        Result: 4
        1 0 1 0 0
    */
}