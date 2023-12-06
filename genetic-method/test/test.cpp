#include "../../knapsack-problem/knapsack_problem.hpp"
#include "../src/genetic_algorithm.hpp"

#include <chrono>
#include <iostream>
#include <fstream>
#include <memory>
#include <climits>
#include <random>
#include <mutex>

using namespace GeneticAlgorithm;
using namespace Interfaces;
using namespace Types;
using namespace std;


const size_t NUM_POPULATIONS = 10;
size_t generation_size = 10;
size_t num_marriage_couples = generation_size;
double mutation_coef = 0.01;
double cross_coef = 0.8;
size_t buffer_size = 1;
size_t chromosome_size = 5;
KnapsackProblem kp(5, {{1, 1}, {1, 2}, {2, 1}, {2, 2}, {3, 3}}, 3);
std::chrono::seconds work_time(1);
std::mutex cout_mut;


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
        return [&knapsack_problem, num_chromosomes]()
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
                        genes.emplace_back(std::move(GeneticAlgorithm::Types::Gene<bool>(true)));
                    }
                    else
                    {
                        genes.emplace_back(std::move(GeneticAlgorithm::Types::Gene<bool>(false)));
                    }
                }
                chromosomes.emplace_back(std::move(GeneticAlgorithm::Types::Chromosome<bool>(genes)));
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
                        generation.get()[i].optionalFitness().reset();
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
    virtual std::function<Generation<bool>(const Population<bool>&)> operator()(double crossingover_coef, Args ...) const override = 0;
};

struct HomogeneousCrossingoverFunction: public BaseCrossingoverFunction<size_t>
{
    virtual std::function<Generation<bool>(const Population<bool>&)> operator()(double crossingover_coef, size_t num_marriage_couples) const override
    {
        if (crossingover_coef > 1. || crossingover_coef < 0.)
        {
            throw std::runtime_error("crossingover coefficient != (0..1)");
        }
        return [crossingover_coef, num_marriage_couples](const Population<bool>& population)
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
                    if (ch1.fitness() > ch2.fitness())
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
                    double ch_coef = ch1.fitness() / (ch1.fitness() + ch2.fitness());
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
    virtual std::function<Generation<bool>(Generation<bool>&)> operator()(size_t num_chromosomes) const override
    {
        return [num_chromosomes](const Generation<bool>& generation)
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            double sum = 0.;
            std::vector<double> pref_sum(1, 0.);
            for (const auto& i: generation.get())
            {
                pref_sum.emplace_back(pref_sum.back() + i.fitness() + 1);
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

struct ConditionsForStopping: public ConditionsForStoppingWrapper<bool, std::shared_ptr<std::chrono::steady_clock::time_point>, std::chrono::seconds>
{
    virtual std::function<bool(const Population<bool>&)> operator()(std::shared_ptr<std::chrono::steady_clock::time_point> start_ptr, std::chrono::seconds work_time) const override
    {
        return [start_ptr, work_time](const Population<bool>&)
        {
            if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - *start_ptr) > work_time)
            {
                return true;
            }
            return false;
        };
    }
};

struct PoolingFunction: public PoolingPopulationsWrapper<bool, NUM_POPULATIONS, size_t>
{
    virtual std::function<Population<bool>(std::array<Population<bool>, NUM_POPULATIONS>&)> operator()(size_t generation_size) const override
    {
        return [generation_size](const std::array<Population<bool>, NUM_POPULATIONS>& populations)
        {
            Generation<bool>::Chromosomes_ generation;
            for (const auto& chromosome: populations[0].get().back().get())
            {
                generation.push_back(chromosome);
            }
            for (const auto& chromosome: populations[1].get().back().get())
            {
                generation.push_back(chromosome);
            }
            std::sort(generation.begin(), generation.end(), [](const Chromosome<bool>& a, const Chromosome<bool>& b)
            {
                return a.fitness() > b.fitness();
            });
            if (generation.size() > generation_size)
            {
                generation.resize(generation_size, GeneticAlgorithm::Types::Chromosome<bool>(0));
            }
            Population<bool> population(populations[0].get().buff_size());
            population.add(Generation<bool>(std::move(generation)));
            return population;
        };
    }
};

struct StartNodeFunction: public StartNodeFunctionWrapper<bool, std::shared_ptr<std::chrono::steady_clock::time_point>>
{
    virtual std::function<void()> operator()(std::shared_ptr<std::chrono::steady_clock::time_point> start_ptr) const override
    {
        return [start_ptr]() {*start_ptr = std::chrono::steady_clock::now();};
    }
};

struct EndNodeFunction: public EndNodeFunctionWrapper<bool, std::shared_ptr<std::chrono::steady_clock::time_point>>
{
    virtual std::function<void()> operator()(std::shared_ptr<std::chrono::steady_clock::time_point> start_ptr) const override
    {
        return [start_ptr]() {std::cerr << "work time: " << 
            std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - *start_ptr).count() << '\n';};
    }
};

struct EndNodeLog: public Loger::EndNodeLogWrapper<bool, std::mutex&>
{
    virtual std::function<void(const Population<bool>&, const string&)> operator()(std::mutex& mut) const override
    {
        return [&mut](const Population<bool>& population, const string& id)
        {
            double mean = 0.;
            for (const auto& chromosome: population.get().back().get())
            {
                mean += max(0., chromosome.fitness() - 1);
            }
            mean /= population.get().back().get().size();

            const std::lock_guard<std::mutex> lg(mut);
            std::cout << "in \"" << id << "\" end mean: " << mean << '\n';
        };
    }
};


GeneticAlgorithm::EvolutionTree::Node<bool>& setFuns(GeneticAlgorithm::EvolutionTree::Node<bool>& node)
{
    std::shared_ptr start_work = std::make_shared<std::chrono::steady_clock::time_point>();
    node.setFitnessFunction(FitnessFunction()(kp)).
        setMutationFunction(MutationFunction()(mutation_coef, chromosome_size)).
        setSelectionFunction(SelectionFunction()(generation_size)).
        setCrossingoverFunction(HomogeneousCrossingoverFunction()(cross_coef, num_marriage_couples)).
        setConditionsForStoppingFunction(ConditionsForStopping()(start_work, work_time)).
        setStartNode(StartNodeFunction()(start_work)).
        // setStartNodeLog(StartNodeLog()(cout_mut)).
        setEndNodeLog(EndNodeLog()(cout_mut));
    return node;
}

GeneticAlgorithm::EvolutionTree::PopulationNode<bool>& setFuns(GeneticAlgorithm::EvolutionTree::PopulationNode<bool>& node)
{
    setFuns((GeneticAlgorithm::EvolutionTree::Node<bool>&) 
        node.setStartPopulationFunction(StartPopulationFunction()(kp, generation_size)));
    return node;
}

GeneticAlgorithm::EvolutionTree::K_Node<bool, NUM_POPULATIONS>& setFuns(GeneticAlgorithm::EvolutionTree::K_Node<bool, NUM_POPULATIONS>& node)
{
    setFuns((GeneticAlgorithm::EvolutionTree::Node<bool>&) 
        node.setPoolingPopulations(PoolingFunction()(generation_size)));
    return node;
}

int main()
{
    try
    {
        std::array<std::unique_ptr<GeneticAlgorithm::EvolutionTree::Node<bool>>, NUM_POPULATIONS> nodes;

        for (size_t i = 0; i < NUM_POPULATIONS; ++i)
        {
            std::unique_ptr<GeneticAlgorithm::EvolutionTree::PopulationNode<bool>> node = 
                std::make_unique<GeneticAlgorithm::EvolutionTree::PopulationNode<bool>>("node" + std::to_string(i + 1));
            setFuns(*node);

            nodes[i] = std::move(node);
        }

        std::unique_ptr<GeneticAlgorithm::EvolutionTree::K_Node<bool, NUM_POPULATIONS>> pool_node = 
            std::make_unique<GeneticAlgorithm::EvolutionTree::K_Node<bool, NUM_POPULATIONS>>("pool_node", std::move(nodes)); 
        setFuns(*pool_node);

        std::unique_ptr<GeneticAlgorithm::EvolutionTree::UnaryNode<bool>> final_node =
            std::make_unique<GeneticAlgorithm::EvolutionTree::UnaryNode<bool>>("final_node", std::move(pool_node));
        setFuns(*final_node);
        
        GeneticAlgorithm::General<bool> general(std::move(final_node));
        auto result = general.calc();

        std::cout << kp;
        std::cout << "Result: " << result.fitness() - 1 << '\n';
        for (auto i: result.get())
        {
            std::cout << i.get() << " ";
        }
    }
    catch(std::logic_error le)
    {
        std::cerr << "logic error: " << le.what();
    }
    catch(...)
    {
        std::cerr << "unknown error";
    }
}