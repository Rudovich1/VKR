#pragma once

#include "types.hpp"
#include "interfaces.hpp"

#include <string>
#include <memory>
#include <thread>
#include <stdexcept>
#include <functional>
#include <vector>


namespace GeneticAlgorithm
{
    namespace EvolutionTree
    {
        using namespace Interfaces;
        using namespace Types;

        template<class GeneType, class FitnessType>
        struct Node
        {
            Node(std::string id = ""): id_(std::move(id)) {};

            Node& setFitnessFunction(std::shared_ptr<FitnessFunctionWrapper<GeneType, FitnessType>> fun) {return set_(fitnessFunction_, fun);}
            Node& setSelection(std::shared_ptr<SelectionWrapper<GeneType, FitnessType>> fun) {return set_(selection_, fun);}
            Node& setMutation(std::shared_ptr<MutationWrapper<GeneType, FitnessType>> fun) {return set_(mutation_, fun);}
            Node& setCrossingover(std::shared_ptr<CrossingoverWrapper<GeneType, FitnessType>> fun) {return set_(crossingover_, fun);}
            Node& setConditionsForStopping(std::shared_ptr<ConditionsForStoppingWrapper<GeneType, FitnessType>> fun) {return set_(conditionsForStopping_, fun);}
            Node& setAnyFunction(std::shared_ptr<AnyFunctionWrapper<GeneType, FitnessType>> fun) {return set_(anyFunction_, fun);}

            Node& setStartEvolutionLog(std::shared_ptr<StartEvolutionLogWrapper<GeneType, FitnessType>> fun) {return set_(startEvolutionLog_, fun);}
            Node& setEndEvolutionLog(std::shared_ptr<EndEvolutionLogWrapper<GeneType, FitnessType>> fun) {return set_(endEvolutionLog_, fun);}
            Node& setNewGenerationLog(std::shared_ptr<NewGenerationLogWrapper<GeneType, FitnessType>> fun) {return set_(newGenerationLog_, fun);}
            
            Node& setStartNodeLog(std::shared_ptr<StartNodeLogWrapper> fun) {return set_(startNodeLog_, fun);}
            Node& setEndNodeLog(std::shared_ptr<EndNodeLogWrapper> fun) {return set_(endNodeLog_, fun);}

            Node& setStartNodeFunction(std::shared_ptr<StartNodeFunctionWrapper<GeneType, FitnessType>> fun) {return set_(startNodeFunction_, fun);} 
            Node& setEndNodeFunction(std::shared_ptr<EndNodeFunctionWrapper<GeneType, FitnessType>> fun) {return set_(endNodeFunction_, fun);}

            virtual Population<GeneType, FitnessType> evolution() = 0;

        protected:
            template<class Fun>
            void __set_function_error(const std::shared_ptr<Fun>& fun, std::string fun_name) const
            {if (!fun) {std::__throw_logic_error(("not set interface \"" + fun_name + "\" in \"" + id_ + "\"").c_str());}}

            template<class FieldType, class Fun = std::shared_ptr<FieldType>>
            Node& set_(FieldType& field, Fun fun)
            {
                field = std::move(fun);
                return *this;
            }

            void evolution_(Population<GeneType, FitnessType>& population)
            {
                __set_function_error(fitnessFunction_, "Fitness Function");
                __set_function_error(conditionsForStopping_, "Conditions For Stopping");
                
                if (startEvolutionLog_) {(*startEvolutionLog_.get())(population, id_);}

                for (Chromosome<GeneType, FitnessType>& chromosome: population.get()[0].get())
                {
                    if (!chromosome.getFitness().has_value())
                    {
                        chromosome.getFitness() = (*fitnessFunction_.get())(chromosome);
                    }
                }

                while (!(*conditionsForStopping_.get())(population))
                {
                    Generation<GeneType, FitnessType> new_generation;
                    
                    if (crossingover_) {new_generation = (*crossingover_.get())(population);}
                    else {new_generation = population.get()[0];}

                    if (mutation_) {(*mutation_.get())(new_generation);}

                    for (Chromosome<GeneType, FitnessType>& chromosome: new_generation.get())
                    {
                        if (!chromosome.getFitness().has_value())
                        {
                            chromosome.getFitness() = (*fitnessFunction_.get())(chromosome);
                        }
                    }

                    if (selection_) {(*selection_.get())(new_generation);}
                    if (newGenerationLog_) {(*newGenerationLog_.get())(new_generation, id_);}

                    population.get().push(std::move(new_generation));

                    if (anyFunction_) {(*anyFunction_.get())(population);}
                }

                if (endEvolutionLog_) {(*endEvolutionLog_.get())(population, id_);}
            }

            std::shared_ptr<FitnessFunctionWrapper<GeneType, FitnessType>> fitnessFunction_;
            std::shared_ptr<SelectionWrapper<GeneType, FitnessType>> selection_;
            std::shared_ptr<MutationWrapper<GeneType, FitnessType>> mutation_;
            std::shared_ptr<CrossingoverWrapper<GeneType, FitnessType>> crossingover_;
            std::shared_ptr<ConditionsForStoppingWrapper<GeneType, FitnessType>> conditionsForStopping_;
            std::shared_ptr<AnyFunctionWrapper<GeneType, FitnessType>> anyFunction_;

            std::shared_ptr<StartEvolutionLogWrapper<GeneType, FitnessType>> startEvolutionLog_;
            std::shared_ptr<EndEvolutionLogWrapper<GeneType, FitnessType>> endEvolutionLog_;
            std::shared_ptr<NewGenerationLogWrapper<GeneType, FitnessType>> newGenerationLog_;
            
            std::shared_ptr<StartNodeLogWrapper> startNodeLog_;
            std::shared_ptr<EndNodeLogWrapper> endNodeLog_;

            std::shared_ptr<StartNodeFunctionWrapper<GeneType, FitnessType>> startNodeFunction_;
            std::shared_ptr<EndNodeFunctionWrapper<GeneType, FitnessType>> endNodeFunction_;

            const std::string id_;
        };


        template<class GeneType, class FitnessType>
        struct UnaryNode: public Node<GeneType, FitnessType>
        {   
            UnaryNode(std::shared_ptr<Node<GeneType, FitnessType>> node, std::string id = ""): 
                Node<GeneType, FitnessType>(std::move(id)), node_(std::move(node)) {}

            virtual Population<GeneType, FitnessType> evolution() override
            {
                if (Node<GeneType, FitnessType>::startNodeLog_) {(*Node<GeneType, FitnessType>::startNodeLog_.get())(Node<GeneType, FitnessType>::id_);}
                Population<GeneType, FitnessType> population = node_->evolution();

                if (Node<GeneType, FitnessType>::startNodeFunction_) {(*Node<GeneType, FitnessType>::startNodeFunction_.get())(population);} 
                Node<GeneType, FitnessType>::evolution_(population);
                if (Node<GeneType, FitnessType>::endNodeFunction_) {(*Node<GeneType, FitnessType>::endNodeFunction_.get())(population);}

                if (Node<GeneType, FitnessType>::endNodeLog_) {(*Node<GeneType, FitnessType>::endNodeLog_.get())(Node<GeneType, FitnessType>::id_);}
                return population;
            }

        protected:
            std::shared_ptr<Node<GeneType, FitnessType>> node_;
        };


        template<class GeneType, class FitnessType>
        struct K_Node: public Node<GeneType, FitnessType>
        {
            K_Node(std::vector<std::shared_ptr<Node<GeneType, FitnessType>>> nodes, bool is_parallel, std::string id = ""):
                Node<GeneType, FitnessType>(std::move(id)), is_parallel_(is_parallel), nodes_(std::move(nodes)) {}

            K_Node& setPoolingPopulations(std::shared_ptr<PoolingPopulationsWrapper<GeneType, FitnessType>> fun) 
            {
                poolingPopulations_ = std::move(fun);
                return *this;
            }

            virtual Population<GeneType, FitnessType> evolution() override
            {
                __set_function_error(poolingPopulations_, "Poolong Population");

                if (Node<GeneType, FitnessType>::startNodeLog_) {(*Node<GeneType, FitnessType>::startNodeLog_.get())(Node<GeneType, FitnessType>::id_);}
                std::vector<Population<GeneType, FitnessType>> populations(nodes_.size());
                if (is_parallel_)
                {
                    std::vector<std::thread> treads;

                    std::function evolute_node = 
                    [](Population<GeneType, FitnessType>& population, std::shared_ptr<Node<GeneType, FitnessType>>& node_ptr)
                    {
                        population = node_ptr->evolution();
                    };

                    for (size_t i = 0; i < nodes_.size(); ++i)
                    {
                        treads.push_back(std::thread(evolute_node, std::ref(populations[i]), std::ref(nodes_[i])));
                    }
                    for (size_t i = 0; i < nodes_.size(); ++i)
                    {
                        treads[i].join();
                    }
                }
                else
                {
                    for (size_t i = 0; i < nodes_.size(); ++i)
                    {
                        populations[i] = nodes_[i]->evolution();
                    }
                }

                Population<GeneType, FitnessType> population = poolingPopulations_.get()(populations);

                if (Node<GeneType, FitnessType>::startNodeFunction_) {(*Node<GeneType, FitnessType>::startNodeFunction_.get())(population);} 
                Node<GeneType, FitnessType>::evolution_(population);
                if (Node<GeneType, FitnessType>::endNodeFunction_) {(*Node<GeneType, FitnessType>::endNodeFunction_.get())(population);}

                if (Node<GeneType, FitnessType>::endNodeLog_) {(*Node<GeneType, FitnessType>::endNodeLog_.get())(Node<GeneType, FitnessType>::id_);}
                return population;
            }

        protected:
            std::vector<std::shared_ptr<Node<GeneType, FitnessType>>> nodes_;
            std::shared_ptr<PoolingPopulationsWrapper<GeneType, FitnessType>> poolingPopulations_;
            bool is_parallel_;
        };


        template<class GeneType, class FitnessType>
        struct PopulationNode: public Node<GeneType, FitnessType>
        {
            PopulationNode(std::string id = ""): Node<GeneType, FitnessType>(std::move(id)) {}

            PopulationNode& setStartPopulation(std::shared_ptr<StartPopulationWrapper<GeneType, FitnessType>> fun)
            {
                startPopulation_ = std::move(fun);
                return *this;
            }

            virtual Population<GeneType, FitnessType> evolution() override
            {
                Node<GeneType, FitnessType>::__set_function_error(startPopulation_, "Start Population");

                if (Node<GeneType, FitnessType>::startNodeLog_) {(*Node<GeneType, FitnessType>::startNodeLog_.get())(Node<GeneType, FitnessType>::id_);}
                Population<GeneType, FitnessType> population = (*startPopulation_.get())();

                if (Node<GeneType, FitnessType>::startNodeFunction_) {(*Node<GeneType, FitnessType>::startNodeFunction_.get())(population);} 
                Node<GeneType, FitnessType>::evolution_(population);
                if (Node<GeneType, FitnessType>::endNodeFunction_) {(*Node<GeneType, FitnessType>::endNodeFunction_.get())(population);}

                if (Node<GeneType, FitnessType>::endNodeLog_) {(*Node<GeneType, FitnessType>::endNodeLog_.get())(Node<GeneType, FitnessType>::id_);}
                return population;
            }

        protected:
            std::shared_ptr<StartPopulationWrapper<GeneType, FitnessType>> startPopulation_;
        };
    } // end namespace EvolutionTree
} // end namespace GeneticAlgorithm
