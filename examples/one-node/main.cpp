#include "../../knapsack_problem/knapsack_problem.hpp"
#include "../../genetic_method/src/genetic_algorithm.hpp"

#include <chrono>
#include <iostream>
#include <fstream>
#include <memory>
#include <climits>
#include <random>


using namespace GeneticAlgorithm;
using namespace Interfaces;
using namespace Types;

struct FitnessFunction: public FitnessFunctionWrapper<bool, KnapsackProblem&>
{
    virtual std::function<double(const Chromosome<bool>&)> operator()(KnapsackProblem& knapsack_problem) const override
    {
        return [&knapsack_problem](const Chromosome<bool>& chromosome)
        {
            double value = 0., weight = 0.;
            for (size_t i = 0; i < knapsack_problem.size_; ++i)
            {
                if (chromosome.get()[i].get())
                {
                    value += knapsack_problem.items_[i].value_;
                    weight += knapsack_problem.items_[i].weight_;
                }
            }
            if (weight > knapsack_problem.knapsack_capacity_)
            {
                value = 0.;
            }
            return value + 1.;
        };
    }
};

struct StartPopulationFunction: public StartPopulationFunctionWrapper<bool, KnapsackProblem&, size_t>
{
    virtual std::function<Population<bool>()> operator()(KnapsackProblem& knapsack_problem, size_t num_chromosomes) const override
    {
        return [&knapsack_problem, &num_chromosomes]()
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<double> uid(0, 1);
            GeneticAlgorithm::Types::Generation<bool>::Chromosomes_ chromosomes;
            for (size_t i = 0; i < num_chromosomes; ++i)
            {
                GeneticAlgorithm::Types::Chromosome<bool>::Genes_ genes;
                for (size_t j = 0; j < knapsack_problem.size_; ++j)
                {
                    if (uid(gen) < 0.5)
                    {
                        genes.emplace_back(GeneticAlgorithm::Types::Gene<bool>(true));
                    }
                    else
                    {
                        genes.emplace_back(GeneticAlgorithm::Types::Gene<bool>(false));
                    }
                }
                chromosomes.emplace_back(GeneticAlgorithm::Types::Chromosome<bool>(genes));
            }
            Population<bool> population(1);
            population.add(GeneticAlgorithm::Types::Generation<bool>(chromosomes));
            return population;
        };
    }
};

struct MutationFunction: public MutationFunctionWrapper<bool, double, size_t>
{
    virtual std::function<void(Generation<bool>&)> operator()(double mutation_coef, size_t num_new_chromosomes) const override
    {
        return [mutation_coef, num_new_chromosomes](Generation<bool>& generation)
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<double> urd(0., 1.);
            for (size_t i = generation.get().size() - num_new_chromosomes; i < generation.get().size(); ++i)
            {
                for (auto& gene: generation.get()[i].get())
                {
                    if (urd(gen) < mutation_coef)
                    {
                        generation.get()[i].getFitness().makeNull();
                        gene.get() = !gene.get();
                    }
                }
            }
        };
    }
};

template<typename ... Args>
struct BaseCrossingoverFunction: public CrossingoverFunctionWrapper<bool, double, Args ...>
{
    virtual std::function<Generation<bool>(Population<bool>&)> operator()(double crossingover_coef, Args ...) const override = 0;
};

struct HomogeneousCrossingoverFunction: public BaseCrossingoverFunction<size_t>
{
    virtual std::function<Generation<bool>(Population<bool>&)> operator()(double crossingover_coef, size_t num_marriage_couples) const override
    {
        if (crossingover_coef > 1. || crossingover_coef < 0.)
        {
            throw std::runtime_error("crossingover coefficient != (0..1)");
        }
        return [crossingover_coef, num_marriage_couples](Population<bool>& population)
        {
            const Generation<bool>& last_generation = population.get().back();
            Generation<bool>::Chromosomes_ chromosomes;
            size_t generation_size = last_generation.get().size();
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<size_t> uid(0, generation_size - 1);
            std::uniform_real_distribution<double> urd(0., 1.);

            for (size_t i = 0; i < num_marriage_couples; ++i)
            {
                size_t ch1_num = uid(gen), ch2_num = uid(gen);
                const GeneticAlgorithm::Types::Chromosome<bool>& ch1 = last_generation.get()[ch1_num];
                const GeneticAlgorithm::Types::Chromosome<bool>& ch2 = last_generation.get()[ch2_num];
                if (urd(gen) > crossingover_coef)
                {
                    if (ch1.getFitness().getVal() > ch2.getFitness().getVal())
                    {
                        chromosomes.emplace_back(ch1);
                    }
                    else
                    {
                        chromosomes.emplace_back(ch2);
                    }
                }
                else
                {
                    double ch_coef = ch1.getFitness().getVal() / (ch1.getFitness().getVal() + ch2.getFitness().getVal());
                    GeneticAlgorithm::Types::Chromosome<bool>::Genes_ genes;
                    for (size_t j = 0; j < ch1.get().size(); ++j)
                    {
                        if (urd(gen) <= ch_coef)
                        {
                            genes.emplace_back(ch1.get()[j]);
                        }
                        else
                        {
                            genes.emplace_back(ch2.get()[j]);
                        }
                    }
                    chromosomes.emplace_back(genes);
                }
            }

            return Generation<bool>(std::move(chromosomes));
        };
    }
};

struct SelectionFunction: public SelectionFunctionWrapper<bool, size_t>
{
    virtual std::function<Generation<bool>(const Generation<bool>&)> operator()(size_t num_chromosomes) const override
    {
        return [num_chromosomes](const Generation<bool>& generation)
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            double sum = 0.;
            std::vector<double> pref_sum(1, 0.);
            for (const auto& i: generation.get())
            {
                pref_sum.emplace_back(pref_sum.back() + i.getFitness().getVal() + 1);
            }
            std::uniform_real_distribution<double> urd(0., pref_sum.back());
            GeneticAlgorithm::Types::Generation<bool>::Chromosomes_ chromosomes;
            for (size_t i = 0; i < num_chromosomes; ++i)
            {
                double res = urd(gen);
                size_t index = std::lower_bound(pref_sum.begin(), pref_sum.end(), res) - pref_sum.begin() - 1;
                chromosomes.emplace_back(generation.get()[index]);
            }
            return Generation<bool>(std::move(chromosomes));
        };
    }
};

struct ConditionsForStopping: public ConditionsForStoppingWrapper<bool, std::chrono::steady_clock::time_point, std::chrono::seconds>
{
    virtual std::function<bool(const Population<bool>&)> operator()(std::chrono::steady_clock::time_point start_time, std::chrono::seconds work_time) const override
    {
        return [&start_time, &work_time](const Population<bool>&)
        {
            if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start_time) > work_time)
            {
                return true;
            }
            return false;
        };
    }
};


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

    std::unique_ptr<GeneticAlgorithm::EvolutionTree::PopulationNode<bool>> node = 
        std::make_unique<GeneticAlgorithm::EvolutionTree::PopulationNode<bool>>("node");

    node->setFitnessFunction(FitnessFunction()(kp));
    node->setMutationFunction(MutationFunction()(mutation_coef, chromosome_size));
    node->setSelectionFunction(SelectionFunction()(generation_size));
    node->setCrossingoverFunction(HomogeneousCrossingoverFunction()(cross_coef, num_marriage_couples));
    node->setConditionsForStoppingFunction(ConditionsForStopping()(start_time, work_time));
    node->setStartPopulationFunction(StartPopulationFunction()(kp, generation_size));

    GeneticAlgorithm::General<bool> general(std::move(node));
    auto result = general.calc();

    std::ofstream out("log.txt");
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