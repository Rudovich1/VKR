#pragma once

#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <iostream>
#include "../knapsack_problem/knapsack_problem.hpp"
#include "../genetic_method/genetic_algorithm.hpp"

struct MyFitnessFunctionWrapper: public GeneticAlgorithm::Interfaces::FitnessFunctionWrapper<bool, KnapsackProblem>
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

struct MyProximityFunctionWrapper: public GeneticAlgorithm::Interfaces::ProximityFunctionWrapper<bool>
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

struct MyStartGenerationFunctionWrapper: public GeneticAlgorithm::Interfaces::StartGenerationFunctionWrapper<bool, KnapsackProblem, size_t>
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
                    if (uid(gen) < 0.05)
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

struct MySelectionFunctionWrapper: public GeneticAlgorithm::Interfaces::SelectionFunctionWrapper<bool, size_t>
{
    virtual std::function<GeneticAlgorithm::Types::Generation<bool>(const GeneticAlgorithm::Types::Generation<bool>&)> operator()(size_t& num_chromosomes) override
    {
        return [num_chromosomes](const GeneticAlgorithm::Types::Generation<bool>& generation)
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            double sum = 0.;
            std::vector<double> pref_sum(1, 0.);
            for (const auto& i: generation.get())
            {
                pref_sum.emplace_back(pref_sum.back() + i.getFitness().getVal());
            }
            std::uniform_real_distribution<double> urd(0., pref_sum.back());
            GeneticAlgorithm::Types::Generation<bool>::Chromosomes_ chromosomes;
            for (size_t i = 0; i < num_chromosomes; ++i)
            {
                double res = urd(gen);
                size_t index = std::lower_bound(pref_sum.begin(), pref_sum.end(), res) - pref_sum.begin() - 1;
                chromosomes.emplace_back(generation.get()[index]);
            }
            return GeneticAlgorithm::Types::Generation<bool>(chromosomes);
        };
    }
};

struct MyConditionsForStoppingWrapper: public GeneticAlgorithm::Interfaces::ConditionsForStoppingWrapper<bool, std::chrono::steady_clock::time_point, std::chrono::seconds>
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

struct MyMutationFunctionWrapper: public GeneticAlgorithm::Interfaces::MutationFunctionWrapper<bool, double>
{
    virtual std::function<void(GeneticAlgorithm::Types::Generation<bool>&)> operator()(double& mutation_coef) override
    {
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
                        gene.get() = !gene.get();
                    }
                }
            }
        };
    }
};

struct MyCrossingoverFunctionWrapper: public GeneticAlgorithm::Interfaces::CrossingoverFunctionWrapper<bool>
{
    virtual std::function<void(GeneticAlgorithm::Types::Generation<bool>&)> operator()() override
    {
        return [](GeneticAlgorithm::Types::Generation<bool>& generation)
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<size_t> uid(0, 1);
            double sum = 0.;
            std::vector<double> pref_sum(1, 0.);
            for (const auto& i: generation.get())
            {
                pref_sum.emplace_back(pref_sum.back() + i.getFitness().getVal());
            }
            std::uniform_real_distribution<double> urd(0., pref_sum.back());
            for (size_t i = 0, size = generation.get().size(); i < size; ++i)
            {
                double chromo1 = urd(gen), chromo2 = urd(gen);
                size_t index1 = std::lower_bound(pref_sum.begin(), pref_sum.end(), chromo1) - pref_sum.begin() - 1;
                size_t index2 = std::lower_bound(pref_sum.begin(), pref_sum.end(), chromo2) - pref_sum.begin() - 1;
                GeneticAlgorithm::Types::Chromosome<bool> new_chromo(generation.get()[index1]);
                for (size_t j = 0; j < new_chromo.get().size(); ++j)
                {
                    if (new_chromo.get()[j] != generation.get()[index2].get()[j])
                    {
                        new_chromo.get()[j].get() = (bool)(uid(gen));
                    }
                }
                generation.get().emplace_back(new_chromo);
            }
        };
    }
};

using ResultType = GeneticAlgorithm::Types::Chromosome<bool>;

struct MyAnyFunctionWrapper: public GeneticAlgorithm::Interfaces::AnyFunctionWrapper<bool, ResultType>
{
    virtual std::function<void(const GeneticAlgorithm::Types::Population<bool>&)> operator()(ResultType& result_type) override
    {
        return [&result_type](const GeneticAlgorithm::Types::Population<bool>& population)
        {
            for (const GeneticAlgorithm::Types::Chromosome<bool>& i: population.get().back().get())
            {
                if (i.getFitness().getVal() > result_type.getFitness().getVal())
                {
                    #ifdef PRINT
                        std::cout << result_type.getFitness().getVal() << '\n';
                    #endif
                    result_type = i;   
                }
            }
        };
    }
};

struct MyResultFunctionWrapper: public GeneticAlgorithm::Interfaces::ResultFunctionWrapper<bool, ResultType, ResultType>
{
    virtual std::function<ResultType(const GeneticAlgorithm::Types::Population<bool>&)> operator()(ResultType& result_type) override
    {
        return [&result_type](const GeneticAlgorithm::Types::Population<bool>& population)
        {
            return result_type;
        };
    }
};