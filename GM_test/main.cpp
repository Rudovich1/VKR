#define PRINT

#include "functions.hpp"

#include "../knapsack_problem/knapsack_problem_generator.hpp"
#include "../knapsack_problem/knapsack_problem_solver.hpp"
#include <iostream>

int main()
{
    size_t num_items = 100;
    size_t min_item_weight = 1;
    size_t max_item_weight = 5;
    size_t min_item_value = 1;
    size_t max_item_value = 5;
    size_t min_knapsack_size = 10;
    size_t max_knapsack_size = 100;
    size_t generation_size = 100;
    double mutation_coef = 0.01;
    size_t buffer_size = 1;
    std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
    std::chrono::seconds work_time(20);

    KnapsackProblem kp = KnapsackProblemGenerator::generateProblem(
        num_items, min_item_weight, max_item_weight, min_item_value, max_item_value, min_knapsack_size, max_knapsack_size);

    MyFitnessFunctionWrapper fitness_function;
    MyProximityFunctionWrapper proximity_function;
    MyConditionsForStoppingWrapper conditions_for_stoping;
    MyCrossingoverFunctionWrapper crossingover_function;
    MyAnyFunctionWrapper any_function;
    MyMutationFunctionWrapper mutation_function;
    MyResultFunctionWrapper result_function;
    MySelectionFunctionWrapper selection_function;
    MyStartGenerationFunctionWrapper start_generation_function;


    GeneticAlgorithm::Types::Chromosome<bool> result(num_items);
    result.getFitness().setVal(fitness_function(kp)(result));

    GeneticAlgorithm::Conditions::BaseConditions<bool> BC(
        fitness_function(kp),
        proximity_function(),
        selection_function(generation_size),
        mutation_function(mutation_coef),
        crossingover_function(),
        conditions_for_stoping(start_time, work_time),
        any_function(result),
        buffer_size, generation_size, num_items
    );

    GeneticAlgorithm::EvolutionTree::Node<bool>* node = new GeneticAlgorithm::EvolutionTree::PopulationNode<bool>(BC, start_generation_function(kp, generation_size));

    GeneticAlgorithm::General<bool, GeneticAlgorithm::Types::Chromosome<bool>> gen_alg(node, result_function(result));

    gen_alg.calc();

    std::cout << kp << '\n' << "Gen: " << result.getFitness().getVal() - 1 << '\n' << "Dp: " << KnapsackProblemSolver::dynamicProgrammingMethod(kp) << '\n';
    for (size_t i = 0; i < result.get().size(); ++i)
    {
        std::cout << result.get()[i].get() << " ";
    }
};