// #define BENCHMARK
// #define DEBUG
#define TEST
#define KNAPSACK_TEST
#define COUT_LOG

#include "tools/knapsack_problem_test.hpp"
#include "knapsack_problem/knapsack_problem_solver.hpp"
#include <string>
#include <fstream>
#include <sstream>

double knapsackGeneticTest(
    KnapsackProblem knapsack_problem,
    int population_size,
    double mutation_rate,
    double new_rundom_chromosome_coef,
    KnapsackProblemSolver::GeneticMethod::StartPopulationType start_population_type,
    KnapsackProblemSolver::GeneticMethod::PopulationMutateType population_mutate_type,
    KnapsackProblemSolver::GeneticMethod::TrainType train_type,
    size_t max_generations,
    size_t max_work_time,
    size_t num_runs
)
{

    double greedy_method = KnapsackProblemSolver::geneticMethod(
        knapsack_problem,
        population_size,
        mutation_rate,
        new_rundom_chromosome_coef,
        start_population_type,
        population_mutate_type,
        train_type,
        max_generations,
        max_work_time,
        num_runs);

    #ifdef KNAPSACK_TEST
        std::stringstream meta;
        meta  << "Population size: " << population_size << " | "
            << "Mutation rate: " << mutation_rate * 100 << '%' << " | "
            << "New rundom chromosome coef: " << new_rundom_chromosome_coef * 100 << '%' << " | "
            << "Start population type: " << start_population_type << " | "
            << "Population mutate type: " << population_mutate_type << " | "
            << "Train type: " << train_type << " | "
            << "Max generations: " << max_generations << " | "
            << "Max work time: " << max_work_time;

        _GENERAL_DUMP_(meta.str());
    #endif

    return greedy_method;
}

void customKnapsackTest(
    std::vector<std::pair<std::string, std::string>>& tests, 
    std::vector<int>& population_sizes,
    std::vector<double>& mutation_rates,
    std::vector<double>& new_rundom_chromosome_coefs,
    std::vector<KnapsackProblemSolver::GeneticMethod::StartPopulationType>& start_population_types,
    std::vector<KnapsackProblemSolver::GeneticMethod::PopulationMutateType>& population_mutate_types,
    std::vector<KnapsackProblemSolver::GeneticMethod::TrainType>& train_types,
    std::vector<size_t>& maxes_generations,
    std::vector<size_t>& max_work_times,
    size_t num_runs,
    size_t depth)
{

    size_t max_num = tests.size() * population_sizes.size() * mutation_rates.size() * new_rundom_chromosome_coefs.size() * start_population_types.size()
        * population_mutate_types.size() * train_types.size() * maxes_generations.size() * max_work_times.size();

    size_t num = 1;

    for (auto& test: tests)
    {
        KnapsackProblem knapsack_problem = KnapsackProblemGenerator::loadProblem(test.first); 
        double dynamic_method = KnapsackProblemSolver::dynamicProgrammingMethod(knapsack_problem);
        double greedy_method = KnapsackProblemSolver::greedyMethod(knapsack_problem);

        _START_TIMER_;

        for (auto& population_size: population_sizes)
        {
            for (auto& mutation_rate: mutation_rates)
            {
                for (auto& new_rundom_chromosome_coef: new_rundom_chromosome_coefs)
                {
                    for (auto& start_population_type: start_population_types)
                    {
                        for (auto& population_mutate_type: population_mutate_types)
                        {
                            for (auto& train_type: train_types)
                            {
                                for (auto& max_generations: maxes_generations)
                                {
                                    for (auto& max_work_time: max_work_times)
                                    {
                                        knapsackGeneticTest(
                                            knapsack_problem,
                                            population_size,
                                            mutation_rate,
                                            new_rundom_chromosome_coef,
                                            start_population_type,
                                            population_mutate_type,
                                            train_type,
                                            max_generations,
                                            max_work_time,
                                            num_runs);

                                        std::cout << num << "\\" << max_num << " --- " << (double) _STOP_TIMER_ / 1000. << "s." << '\n';
                                        ++num;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        std::stringstream ss;
        ss << "Test path: " << test.first << " | " 
            << "Dynamic res: " << dynamic_method << " | " 
            << "Greedy res: " << greedy_method << " | " 
            << "Time: " << (double) _STOP_TIMER_ / 60000. << "m.";

        std::ofstream out(test.second);
        _LOG_DUMP_(out, depth, true, ss.str());
        out.close();
    }
}

void genTest(std::string file_name)
{
    size_t size = 50;
    int min_weight = 10;
    int max_weight = 30;
    double min_value = 1;
    double max_value = 3;
    int min_knapsack_size = 500;
    int max_knapsack_size = 1000;
    
    KnapsackProblem knapsack_problem = KnapsackProblemGenerator::generateProblem(size, min_weight, max_weight, min_value, max_value, min_knapsack_size, max_knapsack_size);
    KnapsackProblemGenerator::saveProblem(knapsack_problem, file_name);
}

int main(int argc, char* argv[])
{
    try
    {
        #ifdef TEST
            #define GEN_MET KnapsackProblemSolver::GeneticMethod

            std::string file_name = "tests/TEST_7.txt";
            genTest(file_name);

            std::vector<std::pair<std::string, std::string>> tests = {{"tests/TEST_7.txt", "results/TEST_7.txt"}}; 
            std::vector<int> population_sizes = {200}; 
            std::vector<double> mutation_rates = {0.0001};
            std::vector<double> new_rundom_chromosome_coefs = {0.01};
            std::vector<GEN_MET ::StartPopulationType> start_population_types = {GEN_MET ::StartPopulationType::RANDOM_POPULATION};
            std::vector<GEN_MET ::PopulationMutateType> population_mutate_types = {GEN_MET ::PopulationMutateType::ALL_POPULATION};
            std::vector<GEN_MET ::GeneticMethod::TrainType> train_types = {
                // GEN_MET ::TrainType::EVOLUTION_MULTIPLE_POPULATIONS,
                // GEN_MET ::TrainType::EVOLUTION_ONE_POPULATION,
                GEN_MET ::TrainType::POOLING_MULTIPLE_POPULATIONS};
            std::vector<size_t> maxes_generations = {5};
            std::vector<size_t> max_work_times = {3};
            size_t num_runs = 1;
            size_t depth = 4;
            
            customKnapsackTest(
                tests,
                population_sizes,
                mutation_rates,
                new_rundom_chromosome_coefs,
                start_population_types,
                population_mutate_types,
                train_types,
                maxes_generations,
                max_work_times,
                num_runs,
                depth);

        #else
            std::string test_file_name_pref = "tests/";
            std::string dump_file_name_pref = "results/";

            std::string test_file_path = test_file_name_pref + argv[1];
            std::string dump_file_path = dump_file_name_pref + argv[1];
            size_t max_generations = std::stoull(argv[3]);
            size_t max_work_time = std::stoull(argv[4]);
            size_t num_runs = std::stoull(argv[5]);

            size_t num = std::stoull(argv[2]);
            
            randomTest(test_file_path, dump_file_path, num, max_generations, max_work_time, num_runs);
        #endif

        
        
    }
    catch(std::exception& ex)
    {
        std::cout << ex.what();
    }

    #ifdef BENCHMARK
        Benchmark::logRes(std::cout);
    #endif
}