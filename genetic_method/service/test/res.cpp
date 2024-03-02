#include "../../src/genetic_algorithm.hpp"
#include <functional>
#include <vector>
#include <random>
#include <chrono>
#include <memory>
#include <iostream>
#include <algorithm>

std::chrono::steady_clock::time_point start;
std::chrono::seconds work_time(1);

struct SimpleFitness: public GeneticAlgorithm::Interfaces::FitnessFunctionWrapper<double> 
{ 
    virtual std::function<double(const GeneticAlgorithm::Types::Chromosome<double>&)> operator()() const override 
    {
        return [](const GeneticAlgorithm::Types::Chromosome<double>& chromosome) 
        { 
            double x = chromosome.get()[0].get();
			double y = chromosome.get()[1].get();
			return x*x + 2*x + 1 + y*y + 4*y + 4;
        };
    }
};
struct SimpleStartPopulation: public GeneticAlgorithm::Interfaces::StartPopulationFunctionWrapper<double> 
{ 
    virtual std::function<GeneticAlgorithm::Types::Population<double>()> operator()() const override 
    {
        return []() 
        { 
            GeneticAlgorithm::Types::Generation<double> start(100, 2);
			for (auto& chromosome: start.get())
				{chromosome.get()[0].get() = chromosome.get()[1].get() = 0.;}
			GeneticAlgorithm::Types::Population<double> population(2);
			population.add(std::move(start));
			return population;
        };
    }
};
struct SimpleMutation: public GeneticAlgorithm::Interfaces::MutationFunctionWrapper<double> 
{ 
    virtual std::function<void(GeneticAlgorithm::Types::Generation<double>&)> operator()() const override 
    {
        return [](GeneticAlgorithm::Types::Generation<double>& generation) 
        { 
            std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<double> urd(-0.01, 0.01);
			for (auto& chromosome: generation.get()) {
				chromosome.optionalFitness().reset();
				chromosome.get()[0].get() += urd(gen);
				chromosome.get()[1].get() += urd(gen);
			}
        };
    }
};
struct SimpleCrossingover: public GeneticAlgorithm::Interfaces::CrossingoverFunctionWrapper<double> 
{ 
    virtual std::function<GeneticAlgorithm::Types::Generation<double>(const GeneticAlgorithm::Types::Population<double>&)> operator()() const override 
    {
        return [](const GeneticAlgorithm::Types::Population<double>& population) 
        { 
            GeneticAlgorithm::Types::Generation<double>::Chromosomes_ chromosomes_;
			for (auto& chromosome: population.get()[0].get()) {chromosomes_.push_back(chromosome);}
			for (auto& chromosome: population.get()[1].get()) {chromosomes_.push_back(chromosome);}
			return GeneticAlgorithm::Types::Generation<double>(std::move(chromosomes_));
        };
    }
};
struct SimpleSelection: public GeneticAlgorithm::Interfaces::SelectionFunctionWrapper<double> 
{ 
    virtual std::function<GeneticAlgorithm::Types::Generation<double>(GeneticAlgorithm::Types::Generation<double>&)> operator()() const override 
    {
        return [](GeneticAlgorithm::Types::Generation<double>& generation) 
        { 
            GeneticAlgorithm::Types::Generation<double>::Chromosomes_ chromosomes_ = generation.get();
			std::sort(chromosomes_.begin(), chromosomes_.end(), 
				[](const GeneticAlgorithm::Types::Chromosome<double>& a, const GeneticAlgorithm::Types::Chromosome<double>& b)
					{return a.fitness() < b.fitness();});
			chromosomes_.resize(chromosomes_.size() / 2);
			return GeneticAlgorithm::Types::Generation<double>(std::move(chromosomes_));
        };
    }
};
struct SimpleConditionsForStopping: public GeneticAlgorithm::Interfaces::ConditionsForStoppingWrapper<double, std::chrono::steady_clock::time_point&, std::chrono::seconds> 
{ 
    virtual std::function<bool(const GeneticAlgorithm::Types::Population<double>&)> operator()(std::chrono::steady_clock::time_point& start, std::chrono::seconds work_time) const override 
    {
        return [&start, work_time](const GeneticAlgorithm::Types::Population<double>& population) 
        { 
            return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start) > work_time;
        };
    }
};
struct SimpleStartNode: public GeneticAlgorithm::Interfaces::StartNodeFunctionWrapper<double, std::chrono::steady_clock::time_point&> 
{ 
    virtual std::function<void()> operator()(std::chrono::steady_clock::time_point& start) const override 
    {
        return [&start]() 
        { 
            return [&start]() {start = std::chrono::steady_clock::now();};
        };
    }
};


int main()
{
    std::unique_ptr<GeneticAlgorithm::EvolutionTree::PopulationNode<double>> node = 
        std::make_unique<GeneticAlgorithm::EvolutionTree::PopulationNode<double>>("node");
    node->setStartPopulationFunction(SimpleStartPopulation()());
    node->setFitnessFunction(SimpleFitness()());
    node->setSelectionFunction(SimpleSelection()());
    node->setMutationFunction(SimpleMutation()());
    node->setCrossingoverFunction(SimpleCrossingover()());
    node->setConditionsForStoppingFunction(SimpleConditionsForStopping()(start, work_time));
            node->setStartNode(SimpleStartNode()(start));
            
    GeneticAlgorithm::General<double> general(std::make_unique<GeneticAlgorithm::EvolutionTree::PopulationNode<double>>(std::move(node)));
    auto res = general.calc();
    std::cout << res.fitness();

}
