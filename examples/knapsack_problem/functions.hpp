#include "../../knapsack-problem/knapsack_problem.hpp"
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
using namespace EvolutionTree;


struct FitnessFunction: public FitnessFunctionWrapper<bool, const KnapsackProblem&, double, double>
{
    virtual std::function<double(const Chromosome<bool>&)> operator()(const KnapsackProblem& knapsack_problem, double min_w, double max_v) const override
    {
        return [&knapsack_problem, min_w, max_v](const Chromosome<bool>& chromosome)
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
                value -= (max_v / min_w) * (weight - knapsack_problem.knapsack_capacity_);
                if (value < 0)
                {
                    value = 0;
                }
            }
            return value;
        };
    }
};


struct StartPopulation_Zeros: public StartPopulationFunctionWrapper<bool, const KnapsackProblem&, size_t>
{
    virtual std::function<Population<bool>()> operator()(const KnapsackProblem& knapsack_problem, size_t num_chromosomes) const override
    {
        return [&knapsack_problem, num_chromosomes]()
        {
            GeneticAlgorithm::Types::Generation<bool>::Chromosomes_ chromosomes;
            for (size_t i = 0; i < num_chromosomes; ++i)
            {
                chromosomes.push_back(Chromosome<bool>(knapsack_problem.size_));
            }
            Population<bool> population(2);
            population.add(GeneticAlgorithm::Types::Generation<bool>(std::move(chromosomes)));
            return population;
        };
    }
};


struct MutationFunction: public MutationFunctionWrapper<bool, double, size_t>
{
    virtual std::function<void(Generation<bool>&)> operator()(double mutation_coef, size_t num_new_pairs) const override
    {
        return [mutation_coef, num_new_pairs](Generation<bool>& generation)
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<double> urd(0., 1.);
            for (size_t i = generation.get().size() - num_new_pairs; i < generation.get().size(); ++i)
            {
                for (auto& gene: generation.get()[i].get())
                {
                    if (urd(gen) < mutation_coef)
                    {
                        gene.get() = !gene.get();
                        generation.get()[i].optionalFitness().reset();
                    }
                }
            }
        };
    }
};


struct CrossingoverFunction: public CrossingoverFunctionWrapper<bool, size_t>
{
    virtual std::function<Generation<bool>(const Population<bool>&)> operator()(size_t num_new_pairs) const override
    {
        return [num_new_pairs](const Population<bool>& population)
        {
            const Generation<bool>& last_generation = population.get().back();
            Generation<bool>::Chromosomes_ chromosomes;
            size_t generation_size = last_generation.get().size();
            std::random_device rd;
            std::mt19937 gen(rd());
            std::vector<size_t> probabilities(generation_size);
            probabilities[0] = last_generation.get()[0].fitness() + 1ULL;
            for (size_t i = 1; i < generation_size; ++i)
            {
                probabilities[i] = probabilities[i - 1] + last_generation.get()[i].fitness() + 1ULL;
            }
            for (size_t i = 0; i < generation_size; ++i)
            {
                chromosomes.push_back(last_generation.get()[i]);
            }
            std::uniform_int_distribution<size_t> uid(0, probabilities.back());
            std::uniform_real_distribution<double> urd(0., 1.);
            for (size_t i = 0; i < num_new_pairs; ++i)
            {
                size_t ch1_ind = std::lower_bound(probabilities.begin(), probabilities.end(), uid(gen)) - probabilities.begin();
                size_t ch2_ind = std::lower_bound(probabilities.begin(), probabilities.end(), uid(gen)) - probabilities.begin();
                const auto& ch1 = last_generation.get()[ch1_ind];
                const auto& ch2 = last_generation.get()[ch2_ind];
                Chromosome<bool>::Genes_ genes(ch1.get().size());
                double prob_ch1 = (ch1.fitness() + 1) / (ch1.fitness() + 1 + ch2.fitness() + 1);
                for (size_t j = 0; j < ch1.get().size(); ++j)
                {
                    if (urd(gen) < prob_ch1)
                    {
                        genes[i] = ch1.get()[i];
                    }
                    else
                    {
                        genes[i] = ch2.get()[i];
                    }
                }
                chromosomes.push_back(Chromosome<bool>(std::move(genes)));
            }
            return Generation<bool>(std::move(chromosomes));
        };
    }
};


struct SelectionFunction: public SelectionFunctionWrapper<bool, size_t>
{
    virtual std::function<Generation<bool>(Generation<bool>&)> operator()(size_t num_chromosomes) const override
    {
        return [num_chromosomes](const Generation<bool>& generation)
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::vector<size_t> probabilities(generation.get().size());
            probabilities[0] = generation.get()[0].fitness() + 1ULL;
            for (size_t i = 1; i < generation.get().size(); ++i)
            {
                probabilities[i] = probabilities[i - 1] + generation.get()[i].fitness() + 1ULL;
            }
            std::uniform_int_distribution<size_t> uid(0, probabilities.back());
            Generation<bool>::Chromosomes_ chromosomes(num_chromosomes);
            for (size_t i = 0; i < num_chromosomes; ++i)
            {
                size_t ch_ind = std::lower_bound(probabilities.begin(), probabilities.end(), uid(gen)) - probabilities.begin();
                chromosomes[i] = generation.get()[ch_ind];
            }
            return Generation<bool>(std::move(chromosomes));
        };
    }
};


struct ConditionsForStoppingImprovement: public ConditionsForStoppingWrapper<
    bool, size_t, std::shared_ptr<size_t>, std::shared_ptr<double>>
{
    virtual std::function<bool(const Population<bool>&)> operator()(
        size_t max_times, std::shared_ptr<size_t> cur_times, std::shared_ptr<double> best_res) const override
    {
        return [max_times, cur_times, best_res](const Population<bool>& population)
        {
            *cur_times += 1;
            for (const auto& chromosome: population.get().back().get())
            {
                if (chromosome.fitness() > *best_res)
                {
                    *cur_times = 0;
                    *best_res = chromosome.fitness();
                }
            }
            if (*cur_times == max_times)
            {
                return true;
            }
            return false;
        };
    }
};

template<size_t num_populations>
struct PoolingFunction: public PoolingPopulationsWrapper<bool, num_populations, size_t>
{
    virtual std::function<Population<bool>(std::array<Population<bool>, num_populations>&)> operator()(size_t num_chromosomes) const override
    {
        return [num_chromosomes](const std::array<Population<bool>, num_populations>& populations)
        {
            Generation<bool>::Chromosomes_ generation;

            for (const Population<bool>& population: populations)
            {
                for (const Chromosome<bool>& chromosome: population.get().back().get())
                {
                    generation.push_back(chromosome);
                }
            }
            std::sort(generation.begin(), generation.end(), [](const Chromosome<bool>& a, const Chromosome<bool>& b)
            {
                return a.fitness() > b.fitness();
            });
            if (generation.size() > num_chromosomes)
            {
                generation.resize(num_chromosomes, GeneticAlgorithm::Types::Chromosome<bool>(0));
            }
            Population<bool> population(populations[0].get().buff_size());
            population.add(Generation<bool>(std::move(generation)));
            return population;
        };
    }
};



Node<bool>& setFuns(
    Node<bool>& node,
    const KnapsackProblem& kp,
    size_t num_chromosomes,
    double mutation_coef,
    size_t num_new_pairs,
    size_t max_not_improvement)
{
    double max_v = LLONG_MIN, min_w = LLONG_MAX;
    for (const auto& item: kp.items_)
    {
        max_v = std::max(max_v, (double)item.value_);
        min_w = std::min(min_w, (double)item.weight_);
    }
    auto cur_times = std::make_shared<size_t>(0);
    auto best_res = std::make_shared<double>(0.);
    node.setFitnessFunction(FitnessFunction()(kp, min_w, max_v)).
        setMutationFunction(MutationFunction()(mutation_coef, num_new_pairs)).
        setCrossingoverFunction(CrossingoverFunction()(num_new_pairs)).
        setSelectionFunction(SelectionFunction()(num_chromosomes)).
        setConditionsForStoppingFunction(ConditionsForStoppingImprovement()(max_not_improvement, cur_times, best_res));
    return node;
}

PopulationNode<bool>& setFuns(
    PopulationNode<bool>& node,
    const KnapsackProblem& kp,
    size_t num_chromosomes,
    double mutation_coef,
    size_t num_new_pairs,
    size_t max_not_improvement)
{
    setFuns((GeneticAlgorithm::EvolutionTree::Node<bool>&)node, kp, num_chromosomes, mutation_coef, num_new_pairs, max_not_improvement);
    node.setStartPopulationFunction(StartPopulation_Zeros()(kp, num_chromosomes));
    return node;
}

UnaryNode<bool>& setFuns(
    UnaryNode<bool>& node,
    const KnapsackProblem& kp,
    size_t num_chromosomes,
    double mutation_coef,
    size_t num_new_pairs,
    size_t max_not_improvement)
{
    setFuns((GeneticAlgorithm::EvolutionTree::Node<bool>&)node, kp, num_chromosomes, mutation_coef, num_new_pairs, max_not_improvement);
    return node;
}

template<size_t num_populations>
K_Node<bool, num_populations>& setFuns(
    K_Node<bool, num_populations>& node,
    const KnapsackProblem& kp,
    size_t num_chromosomes,
    double mutation_coef,
    size_t num_new_pairs,
    size_t max_not_improvement)
{
    setFuns((GeneticAlgorithm::EvolutionTree::Node<bool>&)node, kp, num_chromosomes, mutation_coef, num_new_pairs, max_not_improvement);
    node.setPoolingPopulations(PoolingFunction<num_populations>()(num_chromosomes));
    return node;
}