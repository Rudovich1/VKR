#include "../../knapsack-problem/knapsack_problem.hpp"
#include "../../genetic_method/src/evolution_tree.hpp"
#include "evolution_log.hpp"

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


struct Data
{
    Data(KnapsackProblem kp): kp(std::move(kp)) {}

    KnapsackProblem kp;
    long long min_w, max_v;
    size_t generation_size, num_new_pairs;
    double mutation_coef;
    size_t max_times;
};


struct FitnessFunction: public FitnessFunctionWrapper<bool, long long>
{
    FitnessFunction(Data& data): data_(data) {}

    long long operator()(const Chromosome<bool, long long>& ch) override
    {
        double value = 0., weight = 0.;
        for (size_t i = 0; i < data_.kp.size_; ++i)
        {
            if (ch.cget()[i])
            {
                value += data_.kp.items_[i].value_;
                weight += data_.kp.items_[i].weight_;
            }
        }
        if (weight > data_.kp.knapsack_capacity_)
        {
            value = 0;
        }
        return value;
    }

    Data& data_;
};


struct StartPopulation_Zeros: public StartPopulationWrapper<bool, long long>
{
    StartPopulation_Zeros(Data& data): data_(data) {}

    Population<bool, long long> operator()() override
    {
        Generation<bool, long long> generation(data_.generation_size, Chromosome<bool, long long>(data_.kp.size_));
        Population<bool, long long> population(2);
        population.get().push(std::move(generation));
        return population;
    }

    Data& data_;
};


struct Mutation: public MutationWrapper<bool, long long>
{
    Mutation(Data& data): data_(data) {}

    void operator()(Generation<bool, long long>& generation) override
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> urd(0., 1.);
        for (size_t i = generation.get().size() - data_.num_new_pairs; i < generation.get().size(); ++i)
        {
            for (size_t j = 0; j < generation.get()[i].get().size(); ++j)
            {
                if (urd(gen) < data_.mutation_coef)
                {
                    generation.get()[i].get()[j] = !generation.get()[i].get()[j];
                }
            }
        }
    }

    Data& data_;
};


struct Crossingover: public CrossingoverWrapper<bool, long long>
{
    Crossingover(Data& data): data_(data) {}

    Generation<bool, long long> operator()(const Population<bool, long long>& population) override
    {
        const Generation<bool, long long>& last_generation = population.get()[0];
        Generation<bool, long long> generation;
        size_t generation_size = data_.generation_size;
        size_t chromosome_size = data_.kp.size_;
        for (size_t j = 0; j < data_.num_new_pairs / generation_size; ++j)
        {
            for (size_t i = 0; i < generation_size; ++i)
            {
                generation.get().push_back(last_generation.get()[i]);
            }
        }
        return generation;
    }

    Data& data_;
};


struct Selection: public SelectionWrapper<bool, long long>
{
    Selection(Data& data): data_(data) {}

    void operator()(Generation<bool, long long>& generation) override
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::sort(generation.get().begin(), generation.get().end(), [](const Chromosome<bool, long long>& ch1, const Chromosome<bool, long long>& ch2)
        {
            return ch1.getFitness() > ch2.getFitness();
        });
        std::vector<size_t> probabilities(generation.get().size());
        probabilities[0] = generation.get()[0].getFitness().value() + 1ULL;
        for (size_t i = 1; i < generation.get().size(); ++i)
        {
            probabilities[i] = probabilities[i - 1] + generation.get()[i].getFitness().value() + 1ULL;
        }
        std::uniform_int_distribution<size_t> uid(0, probabilities.back());
        Generation<bool, long long> new_generation(data_.generation_size);
        for (size_t i = 0; i < data_.generation_size*0.1; ++i)
        {
            new_generation.get()[i] = generation.get()[i];
        }
        for (size_t i = data_.generation_size*0.1; i < data_.generation_size*0.9; ++i)
        {
            size_t ch_ind = std::lower_bound(probabilities.begin(), probabilities.end(), uid(gen)) - probabilities.begin();
            new_generation.get()[i] = generation.get()[ch_ind];
        }
        for (size_t i = data_.generation_size*0.9; i < data_.generation_size; ++i)
        {
            new_generation.get()[i] = Chromosome<bool, long long>(data_.kp.size_, false);
            new_generation.get()[i].getFitness() = 0;
        }
        std::swap(generation, new_generation);
    }

    Data& data_;
};


struct ConditionsForStoppingImprovement: public ConditionsForStoppingWrapper<bool, long long>
{
    ConditionsForStoppingImprovement(Data& data): data_(data){}

    bool operator()(const Population<bool, long long>& population) override
    {
        cur_times_ += 1;
        for (const auto& chromosome: population.get()[0].get())
        {
            if (chromosome.getFitness() > best_res_)
            {
                cur_times_ = 0;
                best_res_ = chromosome.getFitness().value();
            }
        }
        if (cur_times_ == data_.max_times)
        {
            return true;
        }
        return false;
    }

    Data& data_;
    size_t cur_times_ = 0;
    long long best_res_ = 0;
};


struct PoolingPopulations: public PoolingPopulationsWrapper<bool, long long>
{
    PoolingPopulations(Data& data): data_(data) {}

    Population<bool, long long> operator()(std::vector<Population<bool, long long>>& populations) override
    {
        Generation<bool, long long> generation;
        for (size_t i = 0; i < populations.size() - 1; ++i)
        {
            Generation<bool, long long> tmp = populations[i].get()[0].get();
            std::sort(tmp.get().begin(), tmp.get().end(), [](const Chromosome<bool, long long>& a, const Chromosome<bool, long long>& b)
            {
                return a.getFitness().value() > b.getFitness().value();
            });
            for (size_t j = 0; j < data_.generation_size / populations.size(); ++j)
            {
                generation.get().push_back(std::move(tmp.get()[j]));
            }
        }
        Generation<bool, long long> tmp = populations.back().get()[0].get();
        std::sort(tmp.get().begin(), tmp.get().end(), [](const Chromosome<bool, long long>& a, const Chromosome<bool, long long>& b)
        {
            return a.getFitness().value() > b.getFitness().value();
        });
        for (size_t j = 0; j < data_.generation_size / populations.size() + (data_.generation_size % populations.size()); ++j)
        {
            generation.get().push_back(std::move(tmp.get()[j]));
        }
        Population<bool, long long> res(2);
        res.get().push(std::move(generation));
        return res;
    }

    Data& data_;
};


struct StartEvolutionLog: public StartEvolutionLogWrapper<bool, long long>
{
    StartEvolutionLog(NodeLog& node_log): node_log_(node_log) {}

    void operator()(const Population<bool, long long>& generation, const std::string& id) override
    {
        node_log_.evolution_start();
    }

    NodeLog& node_log_;
};

struct EndEvolutionLog: public EndEvolutionLogWrapper<bool, long long>
{
    EndEvolutionLog(NodeLog& node_log): node_log_(node_log) {}

    void operator()(const Population<bool, long long>& generation, const std::string& id) override
    {
        node_log_.evolution_end();
    }

    NodeLog& node_log_;
};

struct NewGenerationLog: public NewGenerationLogWrapper<bool, long long>
{
    NewGenerationLog(NodeLog& node_log): node_log_(node_log) {}

    void operator()(const Generation<bool, long long>& generation, const std::string& id) override
    {
        GenerationLog generation_log;
        generation_log.start();
        for (const auto& chromosome: generation.get())
        {
            generation_log.add(chromosome.getFitness().value());
        }
        generation_log.end();
        node_log_.population_log_.add(std::move(generation_log));
    }

    NodeLog& node_log_;
};


struct StartNodeLog: public StartNodeLogWrapper
{
    StartNodeLog(NodeLog& node_log): node_log_(node_log) {}

    void operator()(const std::string& id) override
    {
        node_log_.start();
    }

    NodeLog& node_log_;
};

struct EndNodeLog: public EndNodeLogWrapper
{
    EndNodeLog(NodeLog& node_log): node_log_(node_log) {}

    void operator()(const std::string& id) override
    {
        node_log_.end();
    }

    NodeLog& node_log_;
};


Node<bool, long long>& setFuns(Node<bool, long long>& node, Data& data)
{
    node.setFitnessFunction(std::make_shared<FitnessFunction>(FitnessFunction(data))).
        setMutation(std::make_shared<Mutation>(Mutation(data))).
        setCrossingover(std::make_shared<Crossingover>(Crossingover(data))).
        setSelection(std::make_shared<Selection>(Selection(data))).
        setConditionsForStopping(std::make_shared<ConditionsForStoppingImprovement>(ConditionsForStoppingImprovement(data)));
    return node;
}

Node<bool, long long>& setLogFuns(Node<bool, long long>& node, NodeLog& node_log)
{
    node.setStartEvolutionLog(std::make_shared<StartEvolutionLog>(StartEvolutionLog(node_log))).
        setNewGenerationLog(std::make_shared<NewGenerationLog>(NewGenerationLog(node_log))).
        setEndEvolutionLog(std::make_shared<EndEvolutionLog>(EndEvolutionLog(node_log))).
        setStartNodeLog(std::make_shared<StartNodeLog>(StartNodeLog(node_log))).
        setEndNodeLog(std::make_shared<EndNodeLog>(EndNodeLog(node_log)));
    return node;
}


PopulationNode<bool, long long>& setFuns(PopulationNode<bool, long long>& node, Data& data)
{
    setFuns((Node<bool, long long>&)node, data);
    node.setStartPopulation(std::make_shared<StartPopulation_Zeros>(StartPopulation_Zeros(data)));
    return node;
}


UnaryNode<bool, long long>& setFuns(UnaryNode<bool, long long>& node, Data& data)
{
    setFuns((Node<bool, long long>&)node, data);
    return node;
}


K_Node<bool, long long>& setFuns(K_Node<bool, long long>& node, Data& data)
{
    setFuns((Node<bool, long long>&)node, data);
    node.setPoolingPopulations(std::make_shared<PoolingPopulations>(PoolingPopulations(data)));
    return node;
}
