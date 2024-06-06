#include "../../src/heuristic_algorithms.hpp"

#include <random>
#include <functional>
#include <chrono>
#include <string>
#include <iostream>

using namespace HeuristicAlgorithm::GeneticAlgorithm::Interfaces;
using namespace HeuristicAlgorithm::GeneticAlgorithm::Types;

using Chromosome_ = Chromosome<double, double>;
using Generation_ = Generation<double, double>;
using Population_ = Population<double, double>;


struct FitnessFunction: public FitnessFunctionWrapper<double, double>
{
    FitnessFunction(std::function<double(double)>& y, std::pair<double, double>& best_res): y_(y), best_res_(best_res) {}

    double operator() (const Chromosome_& chromosome) override
    {
        std::pair res = {chromosome.cget()[0], y_(chromosome.cget()[0])};
        if (best_res_.second > res.second)
        {
            best_res_ = res;
        }
        return res.second;
    }

    std::function<double(double)>& y_;
    std::pair<double, double>& best_res_;
};


struct Mutation: public MutationWrapper<double, double>
{
    Mutation(double& coef): coef_(coef) {}

    void operator() (Generation_& generation) override
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> urd(-coef_, coef_);
        size_t generation_size = generation.get().size();

        for (size_t i = generation_size / 2; i < generation_size; ++i)
        {
            generation.get()[i].get()[0] += urd(gen);
        }
    }

    double& coef_;
}; 


struct Selection: public SelectionWrapper<double, double>
{
    void operator()(Generation_& generation) override
    {
        std::sort(generation.get().begin(), generation.get().end(), [](const Chromosome_& ch1, const Chromosome_& ch2)
        {
            return ch1.getFitness() < ch2.getFitness();
        });
        generation.get().resize(generation.get().size() / 2);
    }
};


struct Crossingover: public CrossingoverWrapper<double, double>
{
    Generation_ operator() (const Population_& population) override
    {
        Generation_ new_generation(population.get()[0].get().size() * 2);
        for (size_t i = 0; i < new_generation.get().size() / 2; ++i)
        {
            new_generation.get()[i] = new_generation.get()[i + new_generation.get().size() / 2] = population.get()[0].get()[i];
        }
        return new_generation;
    }
};


struct ConditionsForStopping: public ConditionsForStoppingWrapper<double, double>
{
    ConditionsForStopping(std::chrono::seconds work_time, std::chrono::steady_clock::time_point& start): work_time_(work_time), start_(start) {}

    bool operator() (const Population_&) override
    {
        if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start_) >= work_time_)
        {
            return true;
        }
        return false;
    }

    std::chrono::seconds work_time_;
    std::chrono::steady_clock::time_point& start_;
};


struct AnyFunction: public AnyFunctionWrapper<double, double>
{
    AnyFunction(double& coef, double step): coef_(coef), step_(step) {}

    void operator() (Population_&) override
    {
        coef_ /= step_;
    }

    double& coef_;
    double step_;
};


struct StartEvolutionLog: public StartEvolutionLogWrapper<double, double>
{
    StartEvolutionLog(std::chrono::steady_clock::time_point& start): start_(start) {}
    void operator() (const Population_& population) override
    {
        std::cout << "----- Start evolution -----\n";
        start_ = std::chrono::steady_clock::now();
    }
    std::chrono::steady_clock::time_point& start_;
};


struct EndEvolutionLog: public EndEvolutionLogWrapper<double, double>
{
    void operator() (const Population_& population) override
    {
        std::cout << "----- End evolution -----\n";
    }
};


struct NewGenerationLog: public NewGenerationLogWrapper<double, double>
{
    NewGenerationLog(std::pair<double, double>& best_res): best_res_(best_res) {}
    void operator() (const Generation_& generation) override
    {
        std::pair<double, double> best_cur_res = {generation.get()[0].cget()[0], generation.get()[0].getFitness().value()};
        for (auto& chromosome: generation.get())
        {
            if (best_cur_res.second > chromosome.getFitness().value())
            {
                best_cur_res = {chromosome.cget()[0], chromosome.getFitness().value()};
            }
        }
        std::cout << "===== cur res: {" + std::to_string(best_cur_res.second) + ";" + std::to_string(best_cur_res.first) + "} | best res: " + std::to_string(best_res_.second) + " =====\n";
        if (best_res_.second > best_cur_res.second)
        {
            best_res_ = best_cur_res;
        }
    }
    std::pair<double, double>& best_res_; 
};


using namespace HeuristicAlgorithm::Graph::Interfaces; 


struct StartNodeLog: public StartNodeLogWrapper<Population_>
{
    void operator() (Population_&, const std::string& id) override
    {
        std::cout << "----- Start node \"" + id + "\" -----\n";
    }
};


struct EndNodeLog: public EndNodeLogWrapper<Population_>
{
    void operator() (Population_&, const std::string& id) override
    {
        std::cout << "----- End node \"" + id + "\" -----\n";
    }
};


struct Deserialization: public DeserializationWrapper<Population_, Population_>
{
    Population_ operator() (std::vector<Population_>& inputs) override
    {
        Population_ new_population(1);
        if (inputs.size() == 0)
        {
            new_population.get().push(Generation_(100, Chromosome_(1, 1000)));
            return new_population;
        }
        new_population.get().push(Generation_());
        for (auto& population: inputs)
        {
            for (auto& chromosome: population.get()[0].get())
            {
                new_population.get()[0].get().push_back(chromosome);
            }
        }
        std::sort(new_population.get()[0].get().begin(), new_population.get()[0].get().end(), [](const Chromosome_& ch1, const Chromosome_& ch2)
        {
            return ch1.getFitness() < ch2.getFitness();
        });
        new_population.get()[0].get().resize(new_population.get()[0].get().size() / inputs.size());
        return new_population;
    }
};


struct Serialization: public SerializationWrapper<Population_, Population_>
{
    Population_ operator() (Population_& output) override
    {
        return Population_(output);
    };
};