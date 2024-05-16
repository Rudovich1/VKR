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

        template <class GeneType, class FitnessType>
        struct Node
        {
            Node(std::string id = "") : id_(std::move(id)){};

            Node &setFitnessFunction(std::shared_ptr<FitnessFunctionWrapper<GeneType, FitnessType>> fun) { return set_(fitnessFunction_, fun); }
            Node &setSelection(std::shared_ptr<SelectionWrapper<GeneType, FitnessType>> fun) { return set_(selection_, fun); }
            Node &setMutation(std::shared_ptr<MutationWrapper<GeneType, FitnessType>> fun) { return set_(mutation_, fun); }
            Node &setCrossingover(std::shared_ptr<CrossingoverWrapper<GeneType, FitnessType>> fun) { return set_(crossingover_, fun); }
            Node &setConditionsForStopping(std::shared_ptr<ConditionsForStoppingWrapper<GeneType, FitnessType>> fun) { return set_(conditionsForStopping_, fun); }
            Node &setAnyFunction(std::shared_ptr<AnyFunctionWrapper<GeneType, FitnessType>> fun) { return set_(anyFunction_, fun); }

            Node &setStartEvolutionLog(std::shared_ptr<StartEvolutionLogWrapper<GeneType, FitnessType>> fun) { return set_(startEvolutionLog_, fun); }
            Node &setEndEvolutionLog(std::shared_ptr<EndEvolutionLogWrapper<GeneType, FitnessType>> fun) { return set_(endEvolutionLog_, fun); }
            Node &setNewGenerationLog(std::shared_ptr<NewGenerationLogWrapper<GeneType, FitnessType>> fun) { return set_(newGenerationLog_, fun); }

            Node &setStartNodeLog(std::shared_ptr<StartNodeLogWrapper> fun) { return set_(startNodeLog_, fun); }
            Node &setEndNodeLog(std::shared_ptr<EndNodeLogWrapper> fun) { return set_(endNodeLog_, fun); }

            Node &setStartNodeFunction(std::shared_ptr<StartNodeFunctionWrapper<GeneType, FitnessType>> fun) { return set_(startNodeFunction_, fun); }
            Node &setEndNodeFunction(std::shared_ptr<EndNodeFunctionWrapper<GeneType, FitnessType>> fun) { return set_(endNodeFunction_, fun); }

            virtual Population<GeneType, FitnessType> evolution() = 0;

        protected:
            template <class Fun>
            void __set_function_error(const std::shared_ptr<Fun> &fun, std::string fun_name) const
            {
                if (!fun)
                {
                    std::__throw_logic_error(("not set interface \"" + fun_name + "\" in \"" + id_ + "\"").c_str());
                }
            }

            template <class FieldType, class Fun = std::shared_ptr<FieldType>>
            Node &set_(FieldType &field, Fun fun)
            {
                field = std::move(fun);
                return *this;
            }

            void evolution_(Population<GeneType, FitnessType> &population)
            {
                __set_function_error(fitnessFunction_, "Fitness Function");
                __set_function_error(conditionsForStopping_, "Conditions For Stopping");

                if (startEvolutionLog_)
                {
                    (*startEvolutionLog_.get())(population, id_);
                }

                for (Chromosome<GeneType, FitnessType> &chromosome : population.get()[0].get())
                {
                    if (!chromosome.getFitness().has_value())
                    {
                        chromosome.getFitness() = (*fitnessFunction_.get())(chromosome);
                    }
                }

                while (!(*conditionsForStopping_.get())(population))
                {
                    Generation<GeneType, FitnessType> new_generation;

                    if (crossingover_)
                    {
                        new_generation = (*crossingover_.get())(population);
                    }
                    else
                    {
                        new_generation = population.get()[0];
                    }

                    if (mutation_)
                    {
                        (*mutation_.get())(new_generation);
                    }

                    for (Chromosome<GeneType, FitnessType> &chromosome : new_generation.get())
                    {
                        if (!chromosome.getFitness().has_value())
                        {
                            chromosome.getFitness() = (*fitnessFunction_.get())(chromosome);
                        }
                    }

                    if (selection_)
                    {
                        (*selection_.get())(new_generation);
                    }
                    if (newGenerationLog_)
                    {
                        (*newGenerationLog_.get())(new_generation, id_);
                    }

                    population.get().push(std::move(new_generation));

                    if (anyFunction_)
                    {
                        (*anyFunction_.get())(population);
                    }
                }

                if (endEvolutionLog_)
                {
                    (*endEvolutionLog_.get())(population, id_);
                }
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

        template <class GeneType, class FitnessType>
        struct UnaryNode : public Node<GeneType, FitnessType>
        {
            UnaryNode(std::shared_ptr<Node<GeneType, FitnessType>> node, std::string id = "") : Node<GeneType, FitnessType>(std::move(id)), node_(std::move(node)) {}

            virtual Population<GeneType, FitnessType> evolution() override
            {
                if (Node<GeneType, FitnessType>::startNodeLog_)
                {
                    (*Node<GeneType, FitnessType>::startNodeLog_.get())(Node<GeneType, FitnessType>::id_);
                }
                Population<GeneType, FitnessType> population = node_->evolution();

                if (Node<GeneType, FitnessType>::startNodeFunction_)
                {
                    (*Node<GeneType, FitnessType>::startNodeFunction_.get())(population);
                }
                Node<GeneType, FitnessType>::evolution_(population);
                if (Node<GeneType, FitnessType>::endNodeFunction_)
                {
                    (*Node<GeneType, FitnessType>::endNodeFunction_.get())(population);
                }

                if (Node<GeneType, FitnessType>::endNodeLog_)
                {
                    (*Node<GeneType, FitnessType>::endNodeLog_.get())(Node<GeneType, FitnessType>::id_);
                }
                return population;
            }

        protected:
            std::shared_ptr<Node<GeneType, FitnessType>> node_;
        };

        template <class GeneType, class FitnessType>
        struct K_Node : public Node<GeneType, FitnessType>
        {
            K_Node(std::vector<std::shared_ptr<Node<GeneType, FitnessType>>> nodes, bool is_parallel, std::string id = "") : Node<GeneType, FitnessType>(std::move(id)), is_parallel_(is_parallel), nodes_(std::move(nodes)) {}

            K_Node &setPoolingPopulations(std::shared_ptr<PoolingPopulationsWrapper<GeneType, FitnessType>> fun)
            {
                poolingPopulations_ = std::move(fun);
                return *this;
            }

            virtual Population<GeneType, FitnessType> evolution() override
            {
                __set_function_error(poolingPopulations_, "Poolong Population");

                if (Node<GeneType, FitnessType>::startNodeLog_)
                {
                    (*Node<GeneType, FitnessType>::startNodeLog_.get())(Node<GeneType, FitnessType>::id_);
                }
                std::vector<Population<GeneType, FitnessType>> populations(nodes_.size());
                if (is_parallel_)
                {
                    std::vector<std::thread> treads;

                    std::function evolute_node =
                        [](Population<GeneType, FitnessType> &population, std::shared_ptr<Node<GeneType, FitnessType>> &node_ptr)
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

                if (Node<GeneType, FitnessType>::startNodeFunction_)
                {
                    (*Node<GeneType, FitnessType>::startNodeFunction_.get())(population);
                }
                Node<GeneType, FitnessType>::evolution_(population);
                if (Node<GeneType, FitnessType>::endNodeFunction_)
                {
                    (*Node<GeneType, FitnessType>::endNodeFunction_.get())(population);
                }

                if (Node<GeneType, FitnessType>::endNodeLog_)
                {
                    (*Node<GeneType, FitnessType>::endNodeLog_.get())(Node<GeneType, FitnessType>::id_);
                }
                return population;
            }

        protected:
            std::vector<std::shared_ptr<Node<GeneType, FitnessType>>> nodes_;
            std::shared_ptr<PoolingPopulationsWrapper<GeneType, FitnessType>> poolingPopulations_;
            bool is_parallel_;
        };

        template <class GeneType, class FitnessType>
        struct PopulationNode : public Node<GeneType, FitnessType>
        {
            PopulationNode(std::string id = "") : Node<GeneType, FitnessType>(std::move(id)) {}

            PopulationNode &setStartPopulation(std::shared_ptr<StartPopulationWrapper<GeneType, FitnessType>> fun)
            {
                startPopulation_ = std::move(fun);
                return *this;
            }

            virtual Population<GeneType, FitnessType> evolution() override
            {
                Node<GeneType, FitnessType>::__set_function_error(startPopulation_, "Start Population");

                if (Node<GeneType, FitnessType>::startNodeLog_)
                {
                    (*Node<GeneType, FitnessType>::startNodeLog_.get())(Node<GeneType, FitnessType>::id_);
                }
                Population<GeneType, FitnessType> population = (*startPopulation_.get())();

                if (Node<GeneType, FitnessType>::startNodeFunction_)
                {
                    (*Node<GeneType, FitnessType>::startNodeFunction_.get())(population);
                }
                Node<GeneType, FitnessType>::evolution_(population);
                if (Node<GeneType, FitnessType>::endNodeFunction_)
                {
                    (*Node<GeneType, FitnessType>::endNodeFunction_.get())(population);
                }

                if (Node<GeneType, FitnessType>::endNodeLog_)
                {
                    (*Node<GeneType, FitnessType>::endNodeLog_.get())(Node<GeneType, FitnessType>::id_);
                }
                return population;
            }

        protected:
            std::shared_ptr<StartPopulationWrapper<GeneType, FitnessType>> startPopulation_;
        };

        template <class GeneType, class FitnessType, size_t num_genes, size_t num_chromosomes, size_t suffix_size>
        struct StatNode
        {
            using Chromosome_ = StatChromosome<GeneType, FitnessType, num_genes>;
            using Generation_ = StatGeneration<GeneType, FitnessType, num_genes, num_chromosomes>;
            using Population_ = StatPopulation<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>;

            using FitnessFunction_ = StatFitnessFunctionWrapper<GeneType, FitnessType, num_genes>;
            using Selection_ = StatSelectionWrapper<GeneType, FitnessType, num_genes, num_chromosomes>;
            using Mutation_ = StatMutationWrapper<GeneType, FitnessType, num_genes, num_chromosomes>;
            using Crossingover_ = StatCrossingoverWrapper<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>;
            using ConditionsForStopping_ = StatConditionsForStoppingWrapper<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>;
            using AnyFunction_ = StatAnyFunctionWrapper<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>;
            using StartEvolutionLog_ = StatStartEvolutionLogWrapper<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>;
            using EndEvolutionLog_ = StatEndEvolutionLogWrapper<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>;
            using NewGenerationLog_ = StatNewGenerationLogWrapper<GeneType, FitnessType, num_genes, num_chromosomes>;
            using StartNodeLog_ = StartNodeLogWrapper;
            using EndNodeLog_ = EndNodeLogWrapper;
            using StartNodeFunction_ = StatStartNodeFunctionWrapper<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>;
            using EndNodeFunction_ = StatEndNodeFunctionWrapper<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>;

            StatNode(std::string id = "") : id_(std::move(id)){};

            StatNode &setFitnessFunction(std::shared_ptr<FitnessFunction_> fun) { return set_(fitnessFunction_, fun); }
            StatNode &setSelection(std::shared_ptr<Selection_> fun) { return set_(selection_, fun); }
            StatNode &setMutation(std::shared_ptr<Mutation_> fun) { return set_(mutation_, fun); }
            StatNode &setCrossingover(std::shared_ptr<Crossingover_> fun) { return set_(crossingover_, fun); }
            StatNode &setConditionsForStopping(std::shared_ptr<ConditionsForStopping_> fun) { return set_(conditionsForStopping_, fun); }
            StatNode &setAnyFunction(std::shared_ptr<AnyFunction_> fun) { return set_(anyFunction_, fun); }

            StatNode &setStartEvolutionLog(std::shared_ptr<StartEvolutionLog_> fun) { return set_(startEvolutionLog_, fun); }
            StatNode &setEndEvolutionLog(std::shared_ptr<EndEvolutionLog_> fun) { return set_(endEvolutionLog_, fun); }
            StatNode &setNewGenerationLog(std::shared_ptr<NewGenerationLog_> fun) { return set_(newGenerationLog_, fun); }

            StatNode &setStartNodeLog(std::shared_ptr<StartNodeLog_> fun) { return set_(startNodeLog_, fun); }
            StatNode &setEndNodeLog(std::shared_ptr<EndNodeLog_> fun) { return set_(endNodeLog_, fun); }

            StatNode &setStartNodeFunction(std::shared_ptr<StartNodeFunction_> fun) { return set_(startNodeFunction_, fun); }
            StatNode &setEndNodeFunction(std::shared_ptr<EndNodeFunction_> fun) { return set_(endNodeFunction_, fun); }

            virtual Population_ evolution() = 0;

        protected:
            template <class Fun>
            void __set_function_error(const std::shared_ptr<Fun> &fun, std::string fun_name) const
            {
                if (!fun)
                {
                    std::__throw_logic_error(("not set interface \"" + fun_name + "\" in \"" + id_ + "\"").c_str());
                }
            }

            template <class FieldType, class Fun = std::shared_ptr<FieldType>>
            StatNode &set_(FieldType &field, Fun fun)
            {
                field = std::move(fun);
                return *this;
            }

            void evolution_(Population_ &population)
            {
                __set_function_error(fitnessFunction_, "Fitness Function");
                __set_function_error(conditionsForStopping_, "Conditions For Stopping");

                if (startEvolutionLog_)
                {
                    (*startEvolutionLog_.get())(population, id_);
                }

                for (Chromosome_ &chromosome : population.get()[0].get())
                {
                    if (!chromosome.getFitness().has_value())
                    {
                        chromosome.getFitness() = (*fitnessFunction_.get())(chromosome);
                    }
                }

                while (!(*conditionsForStopping_.get())(population))
                {
                    Generation_ new_generation;

                    if (crossingover_)
                    {
                        new_generation = (*crossingover_.get())(population);
                    }
                    else
                    {
                        new_generation = population.get()[0];
                    }

                    if (mutation_)
                    {
                        (*mutation_.get())(new_generation);
                    }

                    for (Chromosome_ &chromosome : new_generation.get())
                    {
                        if (!chromosome.getFitness().has_value())
                        {
                            chromosome.getFitness() = (*fitnessFunction_.get())(chromosome);
                        }
                    }

                    if (selection_)
                    {
                        (*selection_.get())(new_generation);
                    }
                    if (newGenerationLog_)
                    {
                        (*newGenerationLog_.get())(new_generation, id_);
                    }

                    population.get().push(std::move(new_generation));

                    if (anyFunction_)
                    {
                        (*anyFunction_.get())(population);
                    }
                }

                if (endEvolutionLog_)
                {
                    (*endEvolutionLog_.get())(population, id_);
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

            std::shared_ptr<StartNodeLog_> startNodeLog_;
            std::shared_ptr<EndNodeLog_> endNodeLog_;

            std::shared_ptr<StartNodeFunction_> startNodeFunction_;
            std::shared_ptr<EndNodeFunction_> endNodeFunction_;

            const std::string id_;
        };

        template <class GeneType, class FitnessType, size_t num_genes, size_t num_chromosomes, size_t suffix_size>
        struct StatUnaryNode : public StatNode<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>
        {
            using StatNode_ = StatNode<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>;

            StatUnaryNode(std::shared_ptr<StatNode_> node, std::string id = "") : StatNode_(std::move(id)), node_(std::move(node)) {}

            virtual StatNode_::Population_ evolution() override
            {
                if (StatNode_::startNodeLog_)
                {
                    (*StatNode_::startNodeLog_.get())(StatNode_::id_);
                }
                StatNode_::Population_ population = node_->evolution();

                if (StatNode_::startNodeFunction_)
                {
                    (*StatNode_::startNodeFunction_.get())(population);
                }
                StatNode_::evolution_(population);
                if (StatNode_::endNodeFunction_)
                {
                    (*StatNode_::endNodeFunction_.get())(population);
                }

                if (StatNode_::endNodeLog_)
                {
                    (*StatNode_::endNodeLog_.get())(StatNode_::id_);
                }
                return population;
            }

        protected:
            std::shared_ptr<StatNode_> node_;
        };

        template <class GeneType, class FitnessType, size_t num_genes, size_t num_chromosomes, size_t suffix_size, size_t num_populations>
        struct StatK_Node : public StatNode<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>
        {
            using StatNode_ = StatNode<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>;

            using PoolingPopulations_ = StatPoolingPopulationsWrapper<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size, num_populations>;

            StatK_Node(std::array<std::shared_ptr<StatNode_>, num_populations> nodes, bool is_parallel, std::string id = "") : StatNode_(std::move(id)), is_parallel_(is_parallel), nodes_(std::move(nodes)) {}

            StatK_Node &setPoolingPopulations(std::shared_ptr<PoolingPopulations_> fun)
            {
                poolingPopulations_ = std::move(fun);
                return *this;
            }

            virtual StatNode_::Population_ evolution() override
            {
                __set_function_error(poolingPopulations_, "Poolong Population");

                if (StatNode_::startNodeLog_)
                {
                    (*StatNode_::startNodeLog_.get())(StatNode_::id_);
                }
                std::array<StatNode_::Population_, num_populations> populations(nodes_.size());
                if (is_parallel_)
                {
                    std::array<std::thread, num_populations> treads;

                    std::function evolute_node =
                        [](StatNode_::Population_ &population, std::shared_ptr<StatNode_> &node_ptr)
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

                StatNode_::Population_ population = poolingPopulations_.get()(populations);

                if (StatNode_::startNodeFunction_)
                {
                    (*StatNode_::startNodeFunction_.get())(population);
                }
                StatNode_::evolution_(population);
                if (StatNode_::endNodeFunction_)
                {
                    (*StatNode_::endNodeFunction_.get())(population);
                }

                if (StatNode_::endNodeLog_)
                {
                    (*StatNode_::endNodeLog_.get())(StatNode_::id_);
                }
                return population;
            }

        protected:
            std::array<std::shared_ptr<StatNode_>, num_population> nodes_;
            std::shared_ptr<PoolingPopulations_> poolingPopulations_;
            bool is_parallel_;
        };

        template <class GeneType, class FitnessType, size_t num_genes, size_t num_chromosomes, size_t suffix_size>
        struct StatPopulationNode : public StatNode<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>
        {
            using StatNode_ = StatNode<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>;

            using StartPopulation_ = StatStartPopulationWrapper<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>;

            StatPopulationNode(std::string id = "") : StatNode_(std::move(id)) {}

            StatPopulationNode &setStartPopulation(std::shared_ptr<StartPopulation_> fun)
            {
                startPopulation_ = std::move(fun);
                return *this;
            }

            virtual StatNode_::Population_ evolution() override
            {
                StatNode_::__set_function_error(startPopulation_, "Start Population");

                if (StatNode_::startNodeLog_)
                {
                    (*StatNode_::startNodeLog_.get())(StatNode_::id_);
                }
                StatNode_::Population_ population = (*startPopulation_.get())();

                if (StatNode_::startNodeFunction_)
                {
                    (*StatNode_::startNodeFunction_.get())(population);
                }
                StatNode_::evolution_(population);
                if (StatNode_::endNodeFunction_)
                {
                    (*StatNode_::endNodeFunction_.get())(population);
                }

                if (StatNode_::endNodeLog_)
                {
                    (*StatNode_::endNodeLog_.get())(StatNode_::id_);
                }
                return population;
            }

        protected:
            std::shared_ptr<StartPopulation_> startPopulation_;
        };

    } // end namespace EvolutionTree
} // end namespace GeneticAlgorithm
