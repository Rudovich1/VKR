#pragma once 

#include "../../knapsack_problem/knapsack_problem.hpp"
#include "../../genetic_method/genetic_algorithm.hpp"
#include <climits>
#include <random>
#include <chrono>

struct FitnessFunction: public GeneticAlgorithm::Interfaces::FitnessFunctionWrapper<bool, KnapsackProblem>
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

struct StartGenerationFunction: public GeneticAlgorithm::Interfaces::StartGenerationFunctionWrapper<bool, KnapsackProblem, size_t>
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

struct MutationFunction: public GeneticAlgorithm::Interfaces::MutationFunctionWrapper<bool, double, size_t>
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

template<typename ... Args>
struct BaseCrossingoverFunction: public GeneticAlgorithm::Interfaces::CrossingoverFunctionWrapper<bool, double, Args ...>
{
    virtual std::function<void(GeneticAlgorithm::Types::Generation<bool>&)> operator()(double& crossingover_coef, Args& ...) override = 0;
};

struct HomogeneousCrossingoverFunction: public BaseCrossingoverFunction<size_t>
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

struct SelectionFunction: public GeneticAlgorithm::Interfaces::SelectionFunctionWrapper<bool, size_t>
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

struct ConditionsForStopping: public GeneticAlgorithm::Interfaces::ConditionsForStoppingWrapper<bool, std::chrono::steady_clock::time_point, std::chrono::seconds>
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

struct ResultFunction: public GeneticAlgorithm::Interfaces::ResultFunctionWrapper<bool, GeneticAlgorithm::Types::Chromosome<bool>, KnapsackProblem>
{
    virtual std::function<GeneticAlgorithm::Types::Chromosome<bool>(const GeneticAlgorithm::Types::Population<bool>&)> operator()(KnapsackProblem& kp) override
    {
        return [&kp](const GeneticAlgorithm::Types::Population<bool>& population)
        {
            GeneticAlgorithm::Types::Chromosome<bool> res = population.get().back().get()[0];
            double best_value = res.getFitness().getVal();
            for (auto& chromosome: population.get().back().get())
            {
                double chromosome_value = chromosome.getFitness().getVal();
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

struct AnyFunction: public GeneticAlgorithm::Interfaces::AnyFunctionWrapper<bool>
{
    virtual std::function<void(const GeneticAlgorithm::Types::Population<bool>&)> operator()() override
    {
        return [](const GeneticAlgorithm::Types::Population<bool>& population){};
    }
};

struct EndNodeFunction: public GeneticAlgorithm::Interfaces::EndNodeFunctionWrapper<bool>
{
    virtual std::function<void()> operator()() override
    {
        return [](){};
    }
};
