#pragma once

#include "knapsack_problem.hpp"
#include "knapsack_problem_generator.hpp"
#include "..\tools\benchmark.hpp"
#include "..\tools\loger.hpp"
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <queue>
#include <set>
#include <chrono>
#include <sstream>

#define _WORKTIME_(time) std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - time).count()

class KnapsackProblemSolver
{
public:
    struct GeneticMethod
    {
        enum StartPopulationType
        {
            RANDOM_POPULATION = 0,
            MUTATE_POPULATION,
            ONE_ITEM_POPULATION
        };

        friend std::ostream& operator<< (std::ostream& out, const StartPopulationType& start_population_type)
        {
            switch(start_population_type) 
            {
                case RANDOM_POPULATION: return (out << "Rundom population");
                case MUTATE_POPULATION: return (out << "Mutate population");
                case ONE_ITEM_POPULATION: return (out << "One item population");
            }
            return (out);
        }

        enum PopulationMutateType
        {
            ALL_POPULATION = 0,
            ONLY_NEW_CHROMOSOME
        };

        friend std::ostream& operator<< (std::ostream& out, const PopulationMutateType& population_mutate_type)
        {
            switch(population_mutate_type) 
            {
                case ALL_POPULATION: return (out << "All population");
                case ONLY_NEW_CHROMOSOME: return (out << "Only new chromosome");
            }
            return (out);
        }

        enum TrainType
        {
            EVOLUTION_ONE_POPULATION = 0,
            EVOLUTION_MULTIPLE_POPULATIONS,
            POOLING_MULTIPLE_POPULATIONS
        };

        friend std::ostream& operator<< (std::ostream& out, const TrainType& train_type)
        {
            switch(train_type) 
            {
                case EVOLUTION_ONE_POPULATION: return (out << "Evolution one population");
                case EVOLUTION_MULTIPLE_POPULATIONS: return (out << "Evolution multiple populations");
                case POOLING_MULTIPLE_POPULATIONS: return (out << "Pooling multiple populations");
            }
            return (out);
        }

        std::vector<KnapsackProblem::Item> items_;
        int max_weight_;
        int population_size_;
        double mutation_rate_;
        double new_rundom_chromosome_coef_;
        StartPopulationType start_population_type_;
        PopulationMutateType population_mutate_type_;
        TrainType train_type_;
        size_t max_generations_;
        size_t max_work_time_;
        double max_fit = 0.;
        std::chrono::steady_clock::time_point start_time_;

        using Gene = bool;

        struct Chromosome
        {
            Chromosome(){}
            Chromosome(size_t size): genes(size){}

            std::vector<Gene> genes;
            double fitness = 0.;
        };

        struct Population
        {
            Population(){}
            Population(size_t population_size, size_t chromosome_size): chromosomes(population_size, Chromosome(chromosome_size)){} 

            std::vector<Chromosome> chromosomes;
            double sum_fitnesses = 0.;
            double max_fitness = 0.;
        };

        GeneticMethod(
            const KnapsackProblem& knapsack_problem, 
            int population_size, 
            double mutation_rate,
            double new_rundom_chromosome_coef,
            StartPopulationType start_population_type,
            PopulationMutateType population_mutate_type,
            TrainType train_type,
            size_t max_generations,
            size_t max_work_time): 
            
            max_weight_(knapsack_problem.knapsack_capacity_),
            items_(knapsack_problem.items_),
            population_size_(population_size),
            mutation_rate_(mutation_rate),
            max_generations_(max_generations),
            new_rundom_chromosome_coef_(new_rundom_chromosome_coef),
            start_population_type_(start_population_type),
            population_mutate_type_(population_mutate_type),
            train_type_(train_type),
            max_work_time_(max_work_time),
            start_time_(std::chrono::steady_clock::now()) {}

        void clear()
        {
            max_fit = 0.;
        }
   
        void initChromosome(Chromosome& chromosome)
        {
            max_fit = std::max(max_fit, chromosome.fitness);
            
            #ifdef KNAPSACK_TEST
                _CHROMOSOME_DUMP_(chromosome.fitness, "");
            #endif
        } 

        void fitness(Chromosome& chromosome) 
        {
            int w = 0;
            chromosome.fitness = 0.;
            for (int i = 0; i < chromosome.genes.size(); i++) 
            {
                if (chromosome.genes[i]) 
                {
                    w += items_[i].weight_;
                    chromosome.fitness += items_[i].value_;
                }
            }
            if (w > max_weight_) 
            {
                chromosome.fitness = 0.;
            }
            initChromosome(chromosome);
        }

        void mutate(Chromosome& chromosome, bool reset = false) 
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> dis(0., 1.);
            chromosome.fitness = 0.;
            int w = 0;
            for (int i = 0; i < chromosome.genes.size(); i++) 
            {
                if (reset)
                {
                    chromosome.genes[i] = false;
                }
                if (dis(gen) <= mutation_rate_) 
                {
                    chromosome.genes[i] = !chromosome.genes[i];
                }
                if (chromosome.genes[i])
                {
                    chromosome.fitness += items_[i].value_;
                    w += items_[i].weight_;
                }
            }
            if (w > max_weight_)
            {
                chromosome.fitness = 0.;
            }
            initChromosome(chromosome);
        }

        void randChromosome(Chromosome& chromosome)
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> res(0, 1);
            chromosome.fitness = 0.;
            int w = 0;
            for (size_t i = 0; i < chromosome.genes.size(); ++i)
            {
                chromosome.genes[i] = res(gen);
                if (chromosome.genes[i])
                {
                    w += items_[i].weight_;
                    chromosome.fitness += items_[i].value_;
                }
            }
            if (w > max_weight_)
            {
                chromosome.fitness = 0.;
            }
            initChromosome(chromosome);
        }

        Population createPopulation() 
        {
            Population population(population_size_, items_.size());
            std::random_device rd;
            std::mt19937 gen(rd());
            if (start_population_type_ == StartPopulationType::MUTATE_POPULATION)
            {
                for (size_t i = 0; i < population_size_; i++) 
                {
                    mutate(population.chromosomes[i], true);
                    population.sum_fitnesses += population.chromosomes[i].fitness;
                    population.max_fitness = std::max(population.max_fitness, population.chromosomes[i].fitness);
                }
            }
            else if (start_population_type_ == StartPopulationType::RANDOM_POPULATION)
            {
                std::uniform_int_distribution<> res(0, 1);
                for (size_t i = 0; i < population_size_; ++i)
                {
                    randChromosome(population.chromosomes[i]);
                    population.sum_fitnesses = population.chromosomes[i].fitness;
                    population.max_fitness = std::max(population.max_fitness, population.chromosomes[i].fitness);
                }
            }
            else if (start_population_type_ == StartPopulationType::ONE_ITEM_POPULATION)
            {
                for (size_t i = 0; i < population_size_; ++i)
                {
                    int w = 0;
                    for (size_t j = 0; j < items_.size(); ++j)
                    {  
                        if (i % items_.size() == j % population_size_)
                        {
                            population.chromosomes[i].genes[j] = true;
                            population.chromosomes[i].fitness += items_[j].value_;
                            w += items_[j].weight_;
                        }
                        else
                        {
                            population.chromosomes[i].genes[j] = false;
                        }
                    }
                    if (w > max_weight_)
                    {
                        population.chromosomes[i].fitness = 0.;
                    }
                    initChromosome(population.chromosomes[i]);
                    population.sum_fitnesses = population.chromosomes[i].fitness;
                    population.max_fitness = std::max(population.max_fitness, population.chromosomes[i].fitness);
                }
            }

            return population;
        }

        std::pair<Chromosome, Chromosome> crossover(const Chromosome& parent1, const Chromosome& parent2) 
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            int sep = std::uniform_int_distribution<>(0, items_.size() - 2)(gen);
            Chromosome child1(items_.size()), child2(items_.size());
            int w1 = 0, w2 = 0;
            for (int i = 0; i < items_.size(); ++i) 
            {
                if (i <= sep)
                {
                    child1.genes[i] = parent1.genes[i];
                    child2.genes[i] = parent2.genes[i];
                }
                else
                {
                    child1.genes[i] = parent2.genes[i];
                    child2.genes[i] = parent1.genes[i];
                }
                if (child1.genes[i])
                {
                    w1 += items_[i].weight_;
                    child1.fitness += items_[i].value_;
                }
                if (child2.genes[i])
                {
                    w2 += items_[i].weight_;
                    child2.fitness += items_[i].value_;
                }
            }

            if (w1 > max_weight_)
            {
                child1.fitness = 0.;
            }
            if (w2 > max_weight_)
            {
                child2.fitness = 0.;
            }

            initChromosome(child1);
            initChromosome(child2);

            return {child1, child2};
        }

        void selection(Population& population, size_t res_population_size) 
        {
            std::random_device rd;
            std::mt19937 gen(rd());

            if (population.sum_fitnesses == 0)
            {
                std::shuffle(population.chromosomes.begin(), population.chromosomes.end(), gen);
                if (population.chromosomes.size() > res_population_size)
                {
                    population.chromosomes.resize(res_population_size);
                }

                population.max_fitness = 0.;
                population.sum_fitnesses = 0.;

                for (size_t i = res_population_size * (1. - new_rundom_chromosome_coef_); i < res_population_size; ++i)
                {
                    mutate(population.chromosomes[i], true);
                    population.sum_fitnesses += population.chromosomes[i].fitness;
                    population.max_fitness = std::max(population.max_fitness, population.chromosomes[i].fitness);
                }
                return;
            }

            std::vector<double> probability(population.chromosomes.size());
            std::uniform_real_distribution<double> prob(0., population.sum_fitnesses);

            Population res_population;
            std::sort(population.chromosomes.begin(), population.chromosomes.end(), [](Chromosome& a, Chromosome& b){return a.fitness > b.fitness;});

            probability[0] = population.chromosomes[0].fitness;
            for (size_t i = 1; i < population.chromosomes.size(); ++i)
            {
                probability[i] = probability[i - 1] + population.chromosomes[i].fitness;
            }
            
            for (size_t i = 0; i < res_population_size * (1. - new_rundom_chromosome_coef_); ++i)
            {
                double genp = prob(gen);
                int tmp = std::lower_bound(probability.begin(), probability.end(), genp) - probability.begin();
                res_population.chromosomes.emplace_back(population.chromosomes[tmp]);
                res_population.max_fitness = std::max(res_population.max_fitness, population.chromosomes[tmp].fitness);
                res_population.sum_fitnesses += population.chromosomes[tmp].fitness;
            }

            for (size_t i = res_population_size * (1. - new_rundom_chromosome_coef_); i < res_population_size; ++i)
            {
                res_population.chromosomes.emplace_back(Chromosome(items_.size()));
                mutate(res_population.chromosomes[i], true);
                res_population.max_fitness = std::max(res_population.max_fitness, res_population.chromosomes[i].fitness);
                res_population.sum_fitnesses += res_population.chromosomes[i].fitness;
            }

            population = std::move(res_population);
        }

        void pooling(Population& pop1, Population& pop2)
        {
            for (auto& i: pop2.chromosomes)
            {
                pop1.chromosomes.emplace_back(i);
            }
            pop1.max_fitness = std::max(pop1.max_fitness, pop2.max_fitness);
            pop1.sum_fitnesses += pop2.sum_fitnesses;
        }

        void fixture(Population& population, size_t max_generations)
        {
            std::random_device rd;
            std::mt19937 gen(rd());

            double local_max_fit = population.max_fitness;
            int count = 0;

            do 
            {
                #if defined(COUT_LOG) || defined(KNAPSACK_TEST)
                    _START_TIMER_;
                #endif

                size_t start_population_size = population.chromosomes.size();

                std::shuffle(population.chromosomes.begin(), population.chromosomes.end(), gen);
                for (size_t i = 0; i < start_population_size - 1; i += 2)
                {
                    auto[ch1, ch2] = crossover(population.chromosomes[i], population.chromosomes[i + 1]);
                    
                    population.chromosomes.emplace_back(ch1);
                    population.chromosomes.emplace_back(ch2);
                }
                population.max_fitness = 0.;
                population.sum_fitnesses = 0.;
                for (size_t i = 0; i < population.chromosomes.size(); ++i)
                {
                    if (population_mutate_type_ == PopulationMutateType::ALL_POPULATION)
                    {
                        mutate(population.chromosomes[i]);
                    }
                    else if (population_mutate_type_ == PopulationMutateType::ONLY_NEW_CHROMOSOME && i >= (population.chromosomes.size() / 2) + 1)
                    {
                        mutate(population.chromosomes[i]);
                    }
                    population.max_fitness = std::max(population.max_fitness, population.chromosomes[i].fitness);
                    population.sum_fitnesses += population.chromosomes[i].fitness;
                }

                selection(population, start_population_size);

                if (population.max_fitness <= local_max_fit)
                {
                    ++count;
                }
                else
                {
                    local_max_fit = population.max_fitness;
                    count = 0;
                }

                #ifdef KNAPSACK_TEST
                    _GENERATION_DUMP_(_STOP_TIMER_, "");
                #endif

                #ifdef COUT_LOG
                    std::cout 
                    << "== Generation ==    "
                    << "Count: " << std::setw(8) << std::left << count << " " 
                    << "Time: " << std::setw(10) << std::left << (std::to_string(_STOP_TIMER_) + "ms.") << " "
                    << "Res: " << std::setw(10) << std::left << population.max_fitness << " "
                    << "Local max res: " << std::setw(10) << std::left << local_max_fit << " "
                    << "Max res: " << std::setw(10) << std::left << max_fit << '\n';
                #endif
            }
            while (count < max_generations && _WORKTIME_(start_time_) < max_work_time_);

            #ifdef KNAPSACK_TEST
                _POPULATION_DUMP_("");
            #endif

            #ifdef COUT_LOG
                std::cout << "\n == End population ==\n\n";
            #endif
        }

        void evolutionOnePopulation()
        {
            Population population = createPopulation();
            fixture(population, ULLONG_MAX);
        }

        void evolutionMultiplePopulations()
        {
            while(_WORKTIME_(start_time_) < max_work_time_)
            {
                Population population = createPopulation();
                fixture(population, max_generations_);
            }
        }

        void poolingMultiplePopulations()
        {
            Population population = createPopulation();
            fixture(population, max_generations_);

            while(_WORKTIME_(start_time_) < max_work_time_) 
            {
                if (population.chromosomes.size() > population_size_)
                {
                    selection(population, population_size_);
                }

                Population next_population = createPopulation();
                fixture(next_population, max_generations_);

                pooling(population, next_population);
                fixture(population, max_generations_);
            }
        }

        double genetic_algorithm() 
        {
            start_time_ = std::chrono::steady_clock::now();
            max_fit = 0.;
            if (train_type_ == TrainType::EVOLUTION_MULTIPLE_POPULATIONS)
            {
                evolutionMultiplePopulations();
            }
            else if (train_type_ == TrainType::EVOLUTION_ONE_POPULATION)
            {
                evolutionOnePopulation();
            }
            else if (train_type_ == TrainType::POOLING_MULTIPLE_POPULATIONS)
            {
                poolingMultiplePopulations();
            }

            #ifdef KNAPSACK_TEST
                _RUN_DUMP_("");
            #endif 

            return max_fit;
        }
    };

    struct BranchAndBoundaryMethod
    {
        struct Node 
        {
            int level;
            double profit;
            int weight;
            double bound;
        };

        friend bool operator<(const Node& a, const Node& b) 
        {
            return a.bound < b.bound;
        }

        double bound(const Node& u, int n, int W, const std::vector<double>& p, const std::vector<int>& w) 
        {
            if (u.weight >= W) 
            {
                return 0;
            }
            double bound = u.profit;
            int j = u.level + 1;
            int totweight = u.weight;
            while ((j < n) && (totweight + w[j] <= W)) 
            {
                totweight += w[j];
                bound += p[j];
                j++;
            }
            if (j < n) 
            {
                bound += (W - totweight) * ((double)p[j] / w[j]);
            }
            return bound;
        }

        double solve(int n, int W, const std::vector<double>& p, const std::vector<int>& w, size_t max_work_time) 
        {
            std::priority_queue<Node> Q;
            Node u, v;
            Q.push({-1, 0, 0, 0});
            double maxprofit = 0.;

            auto start = std::chrono::steady_clock::now();

            while (!Q.empty() && _WORKTIME_(start) < max_work_time) 
            {
                u = Q.top();
                Q.pop();
                if (u.level == -1) 
                {
                    v.level = 0;
                } 
                else if (u.level == n - 1) 
                {
                    continue;
                } 
                else 
                {
                    v.level = u.level + 1;
                }
                v.weight = u.weight + w[v.level];
                v.profit = u.profit + p[v.level];
                if (v.weight <= W && v.profit > maxprofit) 
                {
                    maxprofit = v.profit;
                }
                v.bound = bound(v, n, W, p, w);
                if (v.bound > maxprofit) 
                {
                    Q.push(v);
                }
                v.weight = u.weight;
                v.profit = u.profit;
                v.bound = bound(v, n, W, p, w);
                if (v.bound > maxprofit) 
                {
                    Q.push(v);
                }
            }
            return maxprofit;
        }
    };

    static double dynamicProgrammingMethod(const KnapsackProblem& knapsack_problem, size_t max_work_time = 10)
    {
        #ifdef BENCHMARK
            _START_BENCHMARK_;
        #endif

        std::vector<std::vector<double>> dp(knapsack_problem.size_ + 1, std::vector<double>(knapsack_problem.knapsack_capacity_ + 1));

        auto start = std::chrono::steady_clock::now();
        double ans = 0.;

        for (int i = 0; i <= knapsack_problem.size_; i++) 
        {
            for (int j = 0; j <= knapsack_problem.knapsack_capacity_; j++) 
            {
                if (i == 0 || j == 0) 
                {
                    dp[i][j] = 0;
                } 
                else if (knapsack_problem.items_[i - 1].weight_ <= j) 
                {
                    dp[i][j] = std::max(
                        knapsack_problem.items_[i - 1].value_ + dp[i - 1][j - knapsack_problem.items_[i - 1].weight_], 
                        dp[i - 1][j]);
                } 
                else 
                {
                    dp[i][j] = dp[i - 1][j];
                }
                ans = std::max(ans, dp[i][j]);
                if (_WORKTIME_(start) >= max_work_time)
                {
                    return ans;
                }
            }
        }

        return ans;
    }

    static double branchAndBoundaryMethod(const KnapsackProblem& knapsack_problem, size_t max_work_time = 10) 
    {

        #ifdef BENCHMARK
            _START_BENCHMARK_;
        #endif

        std::vector<double> v(knapsack_problem.size_);
        std::vector<int> w(knapsack_problem.size_);

        for (size_t i = 0; i < knapsack_problem.size_; ++i)
        {
            v[i] = knapsack_problem.items_[i].value_;
            w[i] = knapsack_problem.items_[i].weight_;
        }

        BranchAndBoundaryMethod res;

        return res.solve(knapsack_problem.size_, knapsack_problem.knapsack_capacity_, v, w, max_work_time);
    }

    static double geneticMethod(
        const KnapsackProblem& knapsack_problem, 
        int population_size, 
        double mutation_rate,
        double new_rundom_chromosome_coef,
        GeneticMethod::StartPopulationType start_population_type,
        GeneticMethod::PopulationMutateType population_mutate_type,
        GeneticMethod::TrainType train_type,
        size_t max_generations,
        size_t max_work_time,
        size_t num_runs = 1)
    {

        #ifdef BENCHMARK
            _START_BENCHMARK_;
        #endif

        GeneticMethod gen(
            knapsack_problem, 
            population_size, 
            mutation_rate, 
            new_rundom_chromosome_coef, 
            start_population_type,
            population_mutate_type,
            train_type,
            max_generations,
            max_work_time);

        double res = 0.;

        for (size_t i = 0; i < num_runs; ++i)
        {
            res = std::max(res, gen.genetic_algorithm());
        }

        return res;
    }

    static double greedyMethod(const KnapsackProblem& knapsack_problem, size_t max_work_time = 10)
    {
        #ifdef BENCHMARK
            _START_BENCHMARK_;
        #endif

        auto start = std::chrono::steady_clock::now();

        std::vector<KnapsackProblem::Item> items = knapsack_problem.items_;

        std::sort(items.begin(), items.end(), [](KnapsackProblem::Item a, KnapsackProblem::Item b){return a.value_ * b.weight_ > b.value_ * a.weight_;});
        std::multiset<std::pair<int, double>> st;

        int w = 0;
        double v = 0.;
        double best_v = 0.;

        for (size_t i = 0; i < knapsack_problem.size_ && _WORKTIME_(start) < max_work_time; ++i)
        {
            while(w + knapsack_problem.items_[i].weight_ > knapsack_problem.knapsack_capacity_)
            {
                auto[tmp_w, tmp_v] = *st.begin();
                st.erase(st.begin());
                w -= -tmp_w;
                v -= tmp_v;
            }

            w += knapsack_problem.items_[i].weight_;
            v += knapsack_problem.items_[i].value_;
            st.insert({-knapsack_problem.items_[i].weight_, knapsack_problem.items_[i].value_});
            best_v = std::max(best_v, v);
        }

        return best_v;
    }
};