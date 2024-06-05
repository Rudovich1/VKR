#pragma once 

#include "interfaces.hpp"
#include "types.hpp"
#include "../algorithm.hpp"

#include <memory>
#include <string>
#include <thread>
#include <optional>
#include <stdexcept>
#include <functional>

namespace HeuristicAlgorithm
{
    namespace GeneticAlgorithm
    {
        using namespace Types;
        using namespace Interfaces;

        template<class GeneType, class FitnessType>
        struct GeneticAlgorithm: public Algorithm<Population<GeneType, FitnessType>, Population<GeneType, FitnessType>>
        {
            using Chromosome_ = Chromosome<GeneType, FitnessType>;
            using Generation_ = Generation<GeneType, FitnessType>;
            using Population_ = Population<GeneType, FitnessType>;

            using FitnessFunction_ = FitnessFunctionWrapper<GeneType, FitnessType>;
            using Selection_ = SelectionWrapper<GeneType, FitnessType>;
            using Mutation_ = MutationWrapper<GeneType, FitnessType>;
            using Crossingover_ = CrossingoverWrapper<GeneType, FitnessType>;
            using ConditionsForStopping_ = ConditionsForStoppingWrapper<GeneType, FitnessType>;
            using AnyFunction_ = AnyFunctionWrapper<GeneType, FitnessType>;
            
            using StartEvolutionLog_ = StartEvolutionLogWrapper<GeneType, FitnessType>;
            using EndEvolutionLog_ = EndEvolutionLogWrapper<GeneType, FitnessType>;
            using NewGenerationLog_ = NewGenerationLogWrapper<GeneType, FitnessType>;

            void setFitnessFunction(std::shared_ptr<FitnessFunction_> fun) { fitnessFunction_ = fun; }
            void setSelection(std::shared_ptr<Selection_> fun) { selection_ = fun; }
            void setMutation(std::shared_ptr<Mutation_> fun) { mutation_ = fun; }
            void setCrossingover(std::shared_ptr<Crossingover_> fun) { crossingover_ = fun; }
            void setConditionsForStopping(std::shared_ptr<ConditionsForStopping_> fun) { conditionsForStopping_ = fun; }
            void setAnyFunction(std::shared_ptr<AnyFunction_> fun) { anyFunction_ = fun; }

            void setStartEvolutionLog(std::shared_ptr<StartEvolutionLog_> fun) { startEvolutionLog_ = fun; }
            void setEndEvolutionLog(std::shared_ptr<EndEvolutionLog_> fun) { endEvolutionLog_ = fun; }
            void setNewGenerationLog(std::shared_ptr<NewGenerationLog_> fun) { newGenerationLog_ = fun; }

            Population_ evolution(Population_& population) override
            {
                __set_interface_error(fitnessFunction_, "Fitness Function");
                __set_interface_error(conditionsForStopping_, "Conditions For Stopping");

                if (startEvolutionLog_)
                {
                    (*startEvolutionLog_)(population);
                }

                if (population.get().size() == 0)
                {
                    std::__throw_logic_error("empty population");
                }

                calc_fitness_(population.get()[0]);

                while (!(*conditionsForStopping_)(population))
                {
                    Generation_ new_generation;

                    if (crossingover_)
                    {
                        new_generation = (*crossingover_)(population);
                    }
                    else
                    {
                        new_generation = population.get()[0];
                    }

                    if (mutation_)
                    {
                        (*mutation_)(new_generation);
                    }

                    calc_fitness_(new_generation);

                    if (selection_)
                    {
                        (*selection_)(new_generation);
                    }
                    if (newGenerationLog_)
                    {
                        (*newGenerationLog_)(new_generation);
                    }

                    population.get().push(std::move(new_generation));

                    if (anyFunction_)
                    {
                        (*anyFunction_)(population);
                    }
                }

                if (endEvolutionLog_)
                {
                    (*endEvolutionLog_)(population);
                }

                return std::move(population);
            }

        protected:
            template <class Interface>
            void __set_interface_error(const std::shared_ptr<Interface> &interface, std::string interface_name) const
            {
                if (!interface)
                {
                    std::__throw_logic_error(("not set interface \"" + interface_name + "\"").c_str());
                }
            }

            void calc_fitness_(Generation_& generation)
            {
                if (is_parallel_)
                {
                    std::vector<std::thread> threads;
                    auto& fitnessFunction = fitnessFunction_;
                    std::function calc_fitness = [&fitnessFunction](Chromosome_ &chromosome)
                    {
                        chromosome.getFitness() = (*fitnessFunction)(chromosome);
                    };

                    for (Chromosome_ &chromosome : generation.get())
                    {
                        if (!chromosome.getFitness().has_value())
                        {
                            threads.push_back(std::thread(calc_fitness, std::ref(chromosome)));
                        }
                    }

                    for (auto &thread: threads)
                    {
                        thread.join();
                    }
                }
                else
                {
                    for (Chromosome_ &chromosome : generation.get())
                    {
                        if (!chromosome.getFitness().has_value())
                        {
                            chromosome.getFitness() = (*fitnessFunction_)(chromosome);
                        }
                    }
                }
            }

            std::shared_ptr<FitnessFunction_> fitnessFunction_;
            std::shared_ptr<Selection_> selection_;
            std::shared_ptr<Mutation_> mutation_;
            std::shared_ptr<Crossingover_> crossingover_;
            std::shared_ptr<ConditionsForStopping_> conditionsForStopping_;
            std::shared_ptr<AnyFunction_> anyFunction_;

            std::shared_ptr<StartEvolutionLog_> startEvolutionLog_;
            std::shared_ptr<EndEvolutionLog_> endEvolutionLog_;
            std::shared_ptr<NewGenerationLog_> newGenerationLog_;
            
            bool is_parallel_;
        };
    }
}