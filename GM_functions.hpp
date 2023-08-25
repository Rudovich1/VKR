#pragma once 

#include "knapsack_problem/knapsack_problem.hpp"
#include "genetic_method/genetic_algorithm.hpp"
#include <climits>
#include <random>
#include <exception>
#include <chrono>

namespace FitnessFunctions
{
    struct Base_FitnessFunction: public GeneticAlgorithm::Interfaces::FitnessFunctionWrapper<bool, KnapsackProblem>
    {
        virtual std::function<double(const GeneticAlgorithm::Types::Chromosome<bool>&)> operator()(KnapsackProblem& knapsack_problem) override
        {
            return [&knapsack_problem](const GeneticAlgorithm::Types::Chromosome<bool>& chromosome)
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

    struct Penalty_FitnessFunction: public GeneticAlgorithm::Interfaces::FitnessFunctionWrapper<bool, KnapsackProblem, double>
    {
        virtual std::function<double(const GeneticAlgorithm::Types::Chromosome<bool>&)> operator()(KnapsackProblem& knapsack_problem, double& penalty_coef) override
        {
            return [&knapsack_problem, &penalty_coef](const GeneticAlgorithm::Types::Chromosome<bool>& chromosome)
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
                double res = std::max(0., value - penalty_coef * std::max(weight - knapsack_problem.knapsack_capacity_, 0.));
                return res;
            };
        }
    };

    struct StablePenalty_FitnessFunction: public GeneticAlgorithm::Interfaces::FitnessFunctionWrapper<bool, KnapsackProblem>
    {
        virtual std::function<double(const GeneticAlgorithm::Types::Chromosome<bool>&)> operator()(KnapsackProblem& knapsack_problem) override
        {
            return [&knapsack_problem](const GeneticAlgorithm::Types::Chromosome<bool>& chromosome)
            {
                double value = 0., weight = 0.;
                double max_value = INT_MIN, min_weight = INT_MAX;
                for (size_t i = 0; i < knapsack_problem.size_; ++i)
                {
                    if (chromosome.get()[i].get())
                    {
                        value += knapsack_problem.items_[i].value_;
                        weight += knapsack_problem.items_[i].weight_;
                        max_value = std::max(max_value, knapsack_problem.items_[i].value_);
                        min_weight = std::min(min_weight, (double)knapsack_problem.items_[i].weight_);
                    }
                }
                double res = std::max(0., value - (max_value / min_weight) * std::max(weight - knapsack_problem.knapsack_capacity_, 0.));
                return res;
            };
        }
    };
}

namespace ProximityFunctions
{
    struct Base_ProximityFunction: public GeneticAlgorithm::Interfaces::ProximityFunctionWrapper<bool>
    {
        virtual std::function<double(const GeneticAlgorithm::Types::Chromosome<bool>&, const GeneticAlgorithm::Types::Chromosome<bool>&)> operator()() override
        {
            return [](const GeneticAlgorithm::Types::Chromosome<bool>& ch1, const GeneticAlgorithm::Types::Chromosome<bool>& ch2)
            {
                double res = 0.;
                for (size_t i = 0, size = ch1.get().size(); i < size; ++i)
                {
                    if (ch1.get()[i] != ch2.get()[i])
                    {
                        res += 1.;
                    }
                }
                return res;
            };
        }
    };
}

namespace StartGenerationFunctions
{
    struct Random_StartGenerationFunction: public GeneticAlgorithm::Interfaces::StartGenerationFunctionWrapper<bool, KnapsackProblem, size_t>
    {
        virtual std::function<GeneticAlgorithm::Types::Generation<bool>()> operator()(KnapsackProblem& knapsack_problem, size_t& num_chromosomes) override
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
                return GeneticAlgorithm::Types::Generation<bool>(chromosomes);
            };
        }
    };

    struct Mutate_StartGenerationFunction: public GeneticAlgorithm::Interfaces::StartGenerationFunctionWrapper<bool, KnapsackProblem, size_t, double>
    {
        virtual std::function<GeneticAlgorithm::Types::Generation<bool>()> operator()(KnapsackProblem& knapsack_problem, size_t& num_chromosomes, double& mutate_coef) override
        {
            if (mutate_coef > 1. || mutate_coef < 0.)
            {
                throw std::runtime_error("mutate coefficient != (0..1)");
            }
            return [&knapsack_problem, &num_chromosomes, &mutate_coef]()
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
                        if (uid(gen) < mutate_coef)
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
                return GeneticAlgorithm::Types::Generation<bool>(chromosomes);
            };
        }
    };

    struct OneItem_StartGenerationFunction: public GeneticAlgorithm::Interfaces::StartGenerationFunctionWrapper<bool, KnapsackProblem>
    {
        virtual std::function<GeneticAlgorithm::Types::Generation<bool>()> operator()(KnapsackProblem& knapsack_problem) override
        {
            return [&knapsack_problem]()
            {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_real_distribution<double> uid(0, 1);
                GeneticAlgorithm::Types::Generation<bool>::Chromosomes_ chromosomes;
                for (size_t i = 0; i < knapsack_problem.size_; ++i)
                {
                    GeneticAlgorithm::Types::Chromosome<bool>::Genes_ genes;
                    for (size_t j = 0; j < knapsack_problem.size_; ++j)
                    {
                        if (i == j)
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
                return GeneticAlgorithm::Types::Generation<bool>(chromosomes);
            };
        }
    };
}

namespace MutationFunctions
{
    struct AllChromosome_MutationFunction: public GeneticAlgorithm::Interfaces::MutationFunctionWrapper<bool, double>
    {
        virtual std::function<void(GeneticAlgorithm::Types::Generation<bool>&)> operator()(double& mutation_coef) override
        {
            if (mutation_coef > 1. || mutation_coef < 0.)
            {
                throw std::runtime_error("mutation coefficient != (0..1)");
            }
            return [mutation_coef](GeneticAlgorithm::Types::Generation<bool>& generation)
            {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_real_distribution<double> urd(0., 1.);
                for (auto& chromosome: generation.get())
                {
                    for (auto& gene: chromosome.get())
                    {
                        if (urd(gen) < mutation_coef)
                        {
                            chromosome.getFitness().makeNull();
                            gene.get() = !gene.get();
                        }
                    }
                }
            };
        }
    };

    struct OnlyNewChromosome_MutationFunction: public GeneticAlgorithm::Interfaces::MutationFunctionWrapper<bool, double, size_t>
    {
        virtual std::function<void(GeneticAlgorithm::Types::Generation<bool>&)> operator()(double& mutation_coef, size_t& num_new_chromosomes) override
        {
            return [mutation_coef, num_new_chromosomes](GeneticAlgorithm::Types::Generation<bool>& generation)
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
}

namespace CrossingoverFunctions
{
    template<typename ... Args>
    struct Base_CrossingoverFunction: public GeneticAlgorithm::Interfaces::CrossingoverFunctionWrapper<bool, double, Args ...>
    {
        virtual std::function<void(GeneticAlgorithm::Types::Generation<bool>&)> operator()(double& crossingover_coef, Args& ...) override = 0;
    };

    struct Homogeneous_CrossingoverFunction: public Base_CrossingoverFunction<size_t>
    {
        virtual std::function<void(GeneticAlgorithm::Types::Generation<bool>&)> operator()(double& crossingover_coef, size_t& num_marriage_couples) override
        {
            if (crossingover_coef > 1. || crossingover_coef < 0.)
            {
                throw std::runtime_error("crossingover coefficient != (0..1)");
            }
            return [crossingover_coef, num_marriage_couples](GeneticAlgorithm::Types::Generation<bool>& generation)
            {
                size_t generation_size = generation.get().size();
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<size_t> uid(0, generation_size - 1);
                std::uniform_real_distribution<double> urd(0., 1.);

                for (size_t i = 0; i < num_marriage_couples; ++i)
                {
                    size_t ch1_num = uid(gen), ch2_num = uid(gen);
                    const GeneticAlgorithm::Types::Chromosome<bool>& ch1 = generation.get()[ch1_num];
                    const GeneticAlgorithm::Types::Chromosome<bool>& ch2 = generation.get()[ch2_num];
                    if (urd(gen) > crossingover_coef)
                    {
                        if (ch1.getFitness().getVal() > ch2.getFitness().getVal())
                        {
                            generation.get().emplace_back(ch1);
                        }
                        else
                        {
                            generation.get().emplace_back(ch2);
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
                        generation.get().emplace_back(genes);
                    }
                }
            };
        }
    };
}

namespace SelectionFunctions
{
    struct Base_SelectionFunction: public GeneticAlgorithm::Interfaces::SelectionFunctionWrapper<bool, size_t>
    {
        virtual std::function<void(GeneticAlgorithm::Types::Generation<bool>&)> operator()(size_t& num_chromosomes) override
        {
            return [num_chromosomes](GeneticAlgorithm::Types::Generation<bool>& generation)
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
                generation.get() = std::move(chromosomes);
            };
        }
    };
}

namespace ConditionsForStopping
{
    struct Time_ConditionsForStopping: public GeneticAlgorithm::Interfaces::ConditionsForStoppingWrapper<bool, std::chrono::steady_clock::time_point, std::chrono::seconds>
    {
        virtual std::function<bool(const GeneticAlgorithm::Types::Population<bool>&)> operator()(std::chrono::steady_clock::time_point& start_time, std::chrono::seconds& work_time) override
        {
            return [&start_time, &work_time](const GeneticAlgorithm::Types::Population<bool>& population)
            {
                if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start_time) > work_time)
                {
                    return true;
                }
                return false;
            };
        }
    };

    struct NoImprovements_ConditionsForStopping: public GeneticAlgorithm::Interfaces::ConditionsForStoppingWrapper<bool, size_t, size_t, double>
    {
        virtual std::function<bool(const GeneticAlgorithm::Types::Population<bool>&)> operator()(size_t& num_no_improvements, size_t& counter, double& best_fitness) override
        {
            return [num_no_improvements, &counter, &best_fitness](const GeneticAlgorithm::Types::Population<bool>& population)
            {
                for (auto& chromosome: population.get().back().get())
                {
                    if (chromosome.getFitness().getVal() > best_fitness)
                    {
                        best_fitness = chromosome.getFitness().getVal();
                        counter = 0;
                    }
                }
                ++counter;
                if (counter >= num_no_improvements)
                {
                    return true;
                }
                return false;
            };
        }
    };
}

namespace ResultFunctions
{

    double getScore(const GeneticAlgorithm::Types::Chromosome<bool>& ch, const KnapsackProblem& kp)
    {
        double value = 0., weight = 0.;
        for (size_t i = 0; i < ch.get().size(); ++i)
        {
            if (ch.get()[i].get())
            {
                value += kp.items_[i].value_;
                weight += kp.items_[i].weight_;
            }
        }
        if (weight > kp.knapsack_capacity_)
        {
            value = 0.;
        }
        return value;
    }

    struct BestChromosomeInLastGeneration_ResultFunction: public GeneticAlgorithm::Interfaces::ResultFunctionWrapper<bool, GeneticAlgorithm::Types::Chromosome<bool>, KnapsackProblem>
    {
        virtual std::function<GeneticAlgorithm::Types::Chromosome<bool>(const GeneticAlgorithm::Types::Population<bool>&)> operator()(KnapsackProblem& kp) override
        {
            return [&kp](const GeneticAlgorithm::Types::Population<bool>& population)
            {
                GeneticAlgorithm::Types::Chromosome<bool> res = population.get().back().get()[0];
                double best_value = getScore(res, kp);
                for (auto& chromosome: population.get().back().get())
                {
                    double chromosome_value = getScore(chromosome, kp);
                    if (chromosome_value > best_value)
                    {
                        res = chromosome;
                        best_value = chromosome_value;
                    }
                }
                return res;
            };
        }
    };

    struct BestChromosome_ResultFunction: public GeneticAlgorithm::Interfaces::ResultFunctionWrapper<bool, GeneticAlgorithm::Types::Chromosome<bool>, GeneticAlgorithm::Types::Chromosome<bool>>
    {
        virtual std::function<GeneticAlgorithm::Types::Chromosome<bool>(const GeneticAlgorithm::Types::Population<bool>&)> operator()(GeneticAlgorithm::Types::Chromosome<bool>& result_type) override
        {
            return [&result_type](const GeneticAlgorithm::Types::Population<bool>& population)
            {
                return result_type;
            };
        }
    };
}

namespace AnyFunctions
{
    struct SaveBestResult_Function: public GeneticAlgorithm::Interfaces::AnyFunctionWrapper<bool, GeneticAlgorithm::Types::Chromosome<bool>>
    {
        virtual std::function<void(const GeneticAlgorithm::Types::Population<bool>&)> operator()(GeneticAlgorithm::Types::Chromosome<bool>& result_type) override
        {
            return [&result_type](const GeneticAlgorithm::Types::Population<bool>& population)
            {
                for (const GeneticAlgorithm::Types::Chromosome<bool>& i: population.get().back().get())
                {
                    if (i.getFitness().getVal() > result_type.getFitness().getVal())
                    {
                        result_type = i;   
                    }
                }
            };
        }
    };
    
}

namespace PoolingPopulations
{
    struct BasePoolingPopulations: public GeneticAlgorithm::Interfaces::PoolingPopulationsWrapper<bool>
    {
        virtual std::function<GeneticAlgorithm::Types::Population<bool>(const GeneticAlgorithm::Types::Population<bool>&, const GeneticAlgorithm::Types::Population<bool>&)> operator()()
        {
            return [](const GeneticAlgorithm::Types::Population<bool>& pop1, const GeneticAlgorithm::Types::Population<bool>& pop2)
            {
                GeneticAlgorithm::Types::Population<bool> res(1);
                size_t num_chromosomes = pop1.get().back().get().size();
                GeneticAlgorithm::Types::Generation<bool> pooling_generation(pop1.get().back());
                for (auto& chromosome: pop2.get().back().get())
                {
                    pooling_generation.get().emplace_back(chromosome);
                }
                SelectionFunctions::Base_SelectionFunction()(num_chromosomes)(pooling_generation);
                res.add(pooling_generation);
                return res;
            };
        }
    };
}

namespace EndNode
{
    struct BaseEndNodeFunction: public GeneticAlgorithm::Interfaces::EndNodeFunctionWrapper<bool, size_t, double, GeneticAlgorithm::Types::Chromosome<bool>, size_t, std::chrono::steady_clock::time_point>
    {
        virtual std::function<void()> operator()(size_t& counter, double& best_fitness, GeneticAlgorithm::Types::Chromosome<bool>& tmp_res, size_t& num_node, std::chrono::steady_clock::time_point& start) override
        {
            return [&counter, &best_fitness, &tmp_res, &num_node, &start]()
            {
                counter = 0;
                best_fitness = 0.;
                tmp_res = GeneticAlgorithm::Types::Chromosome<bool>(tmp_res.get().size());
                tmp_res.getFitness().setVal(0.);
                #ifdef PRINT
                    std::cout << "nodes left: " << --num_node << " time: " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count() << '\n';
                #endif
                start = std::chrono::steady_clock::now();
            };
        }
    };
}

