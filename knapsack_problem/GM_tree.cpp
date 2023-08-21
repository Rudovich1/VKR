#define PRINT

#include "GM_functions.hpp"
#include "knapsack_problem.hpp"
#include "knapsack_problem_generator.hpp"
#include "knapsack_problem_solver.hpp"

#include <chrono>

int main()
{
    size_t num_items = 1000;
    size_t min_item_weight = 1;
    size_t max_item_weight = 10;
    size_t min_item_value = 1;
    size_t max_item_value = 10;
    size_t min_knapsack_size = 10000;
    size_t max_knapsack_size = 20000;
    size_t generation_size = 500;
    size_t num_marriage_couples = 250;
    double mutation_coef = 0.01;
    double cross_coef = 0.7;
    size_t num_no_improvements = 100;
    size_t buffer_size = 1;
    // std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
    // std::chrono::seconds work_time(20);

    size_t max_counter = 20;
    size_t counter = 0;
    double best_result = 0.;
    GeneticAlgorithm::Types::Chromosome<bool> res(generation_size);
    GeneticAlgorithm::Types::Chromosome<bool> tmp_res(generation_size);
    tmp_res.getFitness().setVal(0.);
    res.getFitness().setVal(0.);

    KnapsackProblem kp = KnapsackProblemGenerator::generateProblem(
        num_items, min_item_weight, max_item_weight, min_item_value, max_item_value, min_knapsack_size, max_knapsack_size);

    GeneticAlgorithm::Conditions::BaseConditions<bool> base_conditions(
        FitnessFunctions::StablePenalty_FitnessFunction()(kp),
        ProximityFunctions::Base_ProximityFunction()(),
        SelectionFunctions::Base_SelectionFunction()(generation_size),
        MutationFunctions::OnlyNewChromosome_MutationFunction()(mutation_coef, num_marriage_couples),
        CrossingoverFunctions::Homogeneous_CrossingoverFunction()(cross_coef, num_marriage_couples),
        ConditionsForStopping::NoImprovements_ConditionsForStopping()(max_counter, counter, best_result),
        AnyFunctions::SaveBestResult_Function()(tmp_res),
        EndNode::BaseEndNodeFunction()(counter, best_result, tmp_res),
        buffer_size, generation_size, num_items);

    GeneticAlgorithm::EvolutionTree::PopulationNode<bool> pop_node_1(base_conditions, StartGenerationFunctions::Mutate_StartGenerationFunction()(kp, generation_size, mutation_coef));
    GeneticAlgorithm::EvolutionTree::PopulationNode<bool> pop_node_2(base_conditions, StartGenerationFunctions::Mutate_StartGenerationFunction()(kp, generation_size, mutation_coef));
    GeneticAlgorithm::EvolutionTree::PopulationNode<bool> pop_node_3(base_conditions, StartGenerationFunctions::Mutate_StartGenerationFunction()(kp, generation_size, mutation_coef));
    GeneticAlgorithm::EvolutionTree::PopulationNode<bool> pop_node_4(base_conditions, StartGenerationFunctions::Mutate_StartGenerationFunction()(kp, generation_size, mutation_coef));
    
    GeneticAlgorithm::EvolutionTree::BinaryNode<bool> binary_node_1(base_conditions, PoolingPopulations::BasePoolingPopulations()(), &pop_node_1, &pop_node_2);
    GeneticAlgorithm::EvolutionTree::BinaryNode<bool> binary_node_2(base_conditions, PoolingPopulations::BasePoolingPopulations()(), &pop_node_3, &pop_node_4);

    GeneticAlgorithm::EvolutionTree::Node<bool> *node = new GeneticAlgorithm::EvolutionTree::BinaryNode<bool>(base_conditions, PoolingPopulations::BasePoolingPopulations()(), &binary_node_1, &binary_node_2);

    GeneticAlgorithm::General<bool, GeneticAlgorithm::Types::Chromosome<bool>> general(node, ResultFunctions::BestChromosomeInLastGeneration_ResultFunction()(kp));

    res = general.calc();

    std::cout << kp << '\n' << "Gen: " << res.getFitness().getVal() - 1 << '\n' << "Dp: " << KnapsackProblemSolver::dynamicProgrammingMethod(kp) << '\n';
    // for (size_t i = 0; i < res.get().size(); ++i)
    // {
    //     std::cout << res.get()[i].get() << " ";
    // }
}