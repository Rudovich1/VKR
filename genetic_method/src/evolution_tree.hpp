#pragma once

#include "types.hpp"
#include "interfaces.hpp"
#include "log_interfaces.hpp"
#include "evolution_log.hpp"

#include <chrono>
#include <string>
#include <memory>
#include <array>
#include <thread>
#include <atomic>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <stdexcept>
#include <optional>

namespace GeneticAlgorithm
{
    namespace EvolutionTree
    {
        using namespace Interfaces;

        template<class GeneType, class FitnessType>
        struct Node
        {
            Node(std::string id = ""): id_(std::move(id)) {};

            Node& setFitnessFunction(FitnessFunctionWrapper<GeneType, FitnessType>* fun) 
            {
                // TODO .....
                fitnessFunction_ = std::make_shared(fun);
            }
            Node& setSelection(typename Interfaces::selectionFunction<GeneType>);
            Node& setMutation(typename Interfaces::mutationFunction<GeneType>);
            Node& setCrossingover(typename Interfaces::crossingoverFunction<GeneType>);
            Node& setConditionsForStopping( typename Interfaces::conditionsForStoppingFunction<GeneType>);
            Node& setAnyFunction(typename Interfaces::anyFunction<GeneType>);

            Node& setNewGenerationLog(typename LogInterfaces::newGenerationLog<GeneType>);
            Node& setStartNodeLog(typename LogInterfaces::startNodeLog<GeneType>);
            Node& setEndNodeLog(typename LogInterfaces::endNodeLog<GeneType>);

            Node& setEndNode(typename Interfaces::endNode<GeneType>);
            Node& setStartNode(typename Interfaces::startNode<GeneType>);

            void evolution(Types::Population<GeneType>&) const;

            virtual Types::Population<GeneType> evolution(bool is_parallel) = 0;

        protected:
            virtual bool isSetFunctions_() const; 

            std::shared_ptr<FitnessFunctionWrapper<GeneType, FitnessType>> fitnessFunction_;
            std::shared_ptr<SelectionWrapper<GeneType, FitnessType>> selection_;
            std::shared_ptr<MutationWrapper<GeneType, FitnessType>> mutation_;
            std::shared_ptr<CrossingoverWrapper<GeneType, FitnessType>> crossingover_;
            std::shared_ptr<ConditionsForStoppingWrapper<GeneType, FitnessType>> conditionsForStopping_;
            std::shared_ptr<AnyFunctionWrapper<GeneType, FitnessType>> anyFunction_;

            std::shared_ptr<NewGenerationLogWrapper<GeneType, FitnessFunctionWrapper>> newGenerationLog_;
            std::shared_ptr<StartNodeLogWrapper<GeneType, FitnessFunctionWrapper>> startNodeLog_;
            std::shared_ptr<EndNodeLogWrapper<GeneType, FitnessFunctionWrapper>> endNodeLog_;

            std::shared_ptr<EndNodeFunctionWrapper> endNodeFunction_;
            std::shared_ptr<StartNodeFunctionWrapper> startNodeFunction_;

            const std::string id_;
            // mutable std::shared_ptr<NodeLog> node_log_;
        };


        template<typename GeneType>
        struct UnaryNode: public Node<GeneType>
        {
            std::shared_ptr<Node<GeneType>> node_;
            
            UnaryNode(std::string id, std::shared_ptr<Node<GeneType>> node);

            virtual Types::Population<GeneType> evolution(bool is_parallel) override;
        };


        template<typename GeneType, size_t num_nodes>
        struct K_Node: public Node<GeneType>
        {
            std::array<std::shared_ptr<Node<GeneType>>, num_nodes> nodes_;
            std::optional<typename Interfaces::poolingPopulations<GeneType, num_nodes>> poolingPopulations_;
            virtual bool isSetFunctions_() const override;

            K_Node(std::string id, std::array<std::shared_ptr<Node<GeneType>>, num_nodes> nodes);

            K_Node& setPoolingPopulations(typename Interfaces::poolingPopulations<GeneType, num_nodes>);

            virtual Types::Population<GeneType> evolution(bool is_parallel) override;
        };


        template<typename GeneType>
        struct PopulationNode: public Node<GeneType>
        {
            std::optional<typename Interfaces::startPopulationFunction<GeneType>> startPopulationFunction_;
            virtual bool isSetFunctions_() const override;

            PopulationNode(std::string id);

            PopulationNode& setStartPopulationFunction(typename Interfaces::startPopulationFunction<GeneType>);

            virtual Types::Population<GeneType> evolution(bool is_parallel) override;
        };
    } 
    // end namespace EvolutionTree
} 
// end namespace GeneticAlgorithm

namespace GeneticAlgorithm
{
    namespace EvolutionTree
    {
        template<typename GeneType>
        Node<GeneType>::Node(std::string id): id_(std::move(id)), node_log_(std::make_shared<NodeLog>(id_))
        {
            if (ids_.find(id_) != ids_.end())
            {
                throw std::logic_error("id \"" + id_ + "\" already exists");
            }
        }

        template<typename GeneType>
        Node<GeneType>& Node<GeneType>::setFitnessFunction(typename Interfaces::fitnessFunction<GeneType> fun)
        {
            fitnessFunction_ = std::move(fun);
            return *this;
        }

        template<typename GeneType>
        Node<GeneType>& Node<GeneType>::setSelectionFunction(typename Interfaces::selectionFunction<GeneType> fun)
        {
            selectionFunction_ = std::move(fun);
            return *this;
        }

        template<typename GeneType>
        Node<GeneType>& Node<GeneType>::setMutationFunction(typename Interfaces::mutationFunction<GeneType> fun)
        {
            mutationFunction_ = std::move(fun);
            return *this;
        }

        template<typename GeneType>
        Node<GeneType>& Node<GeneType>::setCrossingoverFunction(typename Interfaces::crossingoverFunction<GeneType> fun)
        {
            crossingoverFunction_ = std::move(fun);
            return *this;
        }

        template<typename GeneType>
        Node<GeneType>& Node<GeneType>::setConditionsForStoppingFunction(typename Interfaces::conditionsForStoppingFunction<GeneType> fun)
        {
            conditionsForStoppingFunction_ = std::move(fun);
            return *this;
        }

        template<typename GeneType>
        Node<GeneType>& Node<GeneType>::setAnyFunction(typename Interfaces::anyFunction<GeneType> fun)
        {
            anyFunction_ = std::move(fun);
            return *this;
        }

        template<typename GeneType>
        Node<GeneType>& Node<GeneType>::setEndNode(typename Interfaces::endNode<GeneType> fun)
        {
            endNode_ = std::move(fun);
            return *this;
        }

        template<typename GeneType>
        Node<GeneType>& Node<GeneType>::setStartNode(typename Interfaces::startNode<GeneType> fun)
        {
            startNode_ = std::move(fun);
            return *this;
        }

        template<typename GeneType>
        Node<GeneType>& Node<GeneType>::setStartNodeLog(typename LogInterfaces::startNodeLog<GeneType> fun)
        {
            startNodeLog_ = std::move(fun);
            return *this;
        }

        template<typename GeneType>
        Node<GeneType>& Node<GeneType>::setNewGenerationLog(typename LogInterfaces::newGenerationLog<GeneType> fun)
        {
            newGenerationLog_ = std::move(fun);
            return *this;
        }

        template<typename GeneType>
        Node<GeneType>& Node<GeneType>::setEndNodeLog(typename LogInterfaces::endNodeLog<GeneType> fun)
        {
            endNodeLog_ = std::move(fun);
            return *this;
        }

        template<typename GeneType>
        bool Node<GeneType>::isSetFunctions_() const
        {
            return fitnessFunction_.has_value() &&
                conditionsForStoppingFunction_.has_value();
        }

        template<typename GeneType>
        void Node<GeneType>::evolution(Types::Population<GeneType>& population) const
        {
            node_log_->population_log_.start();
            GenerationLog generation_log;

            if (startNodeLog_.has_value()) {startNodeLog_.value()(population, id_);}
            if (startNode_.has_value()) {startNode_.value()();}

            generation_log.start();

            for (Types::Chromosome<GeneType>& chromosome: population.get().back().get())
            {
                if (!chromosome.optionalFitness().has_value())
                {
                    chromosome.optionalFitness() = fitnessFunction_.value()(chromosome);
                }
                generation_log.add(chromosome.fitness());
            }

            generation_log.end();
            node_log_->population_log_.add(generation_log);

            while (!conditionsForStoppingFunction_.value()(population))
            {
                generation_log.start();

                Types::Generation<GeneType> new_generation;
                
                if (crossingoverFunction_.has_value()) {new_generation = crossingoverFunction_.value()(population);}
                else {new_generation = population.get().back();}

                if (mutationFunction_.has_value()) {mutationFunction_.value()(new_generation);}

                for (Types::Chromosome<GeneType>& chromosome: new_generation.get())
                {
                    if (!chromosome.optionalFitness().has_value())
                    {
                        chromosome.optionalFitness() = fitnessFunction_.value()(chromosome);
                    }
                }

                if (selectionFunction_.has_value()) {population.add(selectionFunction_.value()(new_generation));}
                else {population.add(std::move(new_generation));}

                if (anyFunction_.has_value()) {anyFunction_.value()(population);}

                if (newGenerationLog_.has_value()) {newGenerationLog_.value()(population.get().back(), id_);}

                for (const Types::Chromosome<GeneType>& chromosome: population.get().back().get())
                {
                    generation_log.add(chromosome.fitness());
                }
                generation_log.end();
                node_log_->population_log_.add(generation_log);
            }

            if (endNode_.has_value()) {endNode_.value()();}
            if (endNodeLog_.has_value()) {endNodeLog_.value()(population, id_);}

            node_log_->population_log_.end();
        }

        template<typename GeneType>
        void Node<GeneType>::cerrInfo_(std::string info) const
        {
            std::cerr << id_ << ": " << info << '\n';
        }

        template<typename GeneType>
        UnaryNode<GeneType>::UnaryNode(std::string id, std::shared_ptr<Node<GeneType>> node): 
            Node<GeneType>(id), node_(std::move(node)) 
        {
            Node<GeneType>::node_log_->ch_node_logs_.push_back(node_->node_log_);
        }

        template<typename GeneType>
        Types::Population<GeneType> UnaryNode<GeneType>::evolution(bool is_parallel)
        {
            Node<GeneType>::node_log_->start();
            Types::Population<GeneType> population = node_->evolution(is_parallel);
            node_.reset();
            Node<GeneType>::node_log_->evolution_start();
            Node<GeneType>::evolution(population);
            Node<GeneType>::node_log_->end();
            return population;
        }

        template<typename GeneType, size_t num_nodes>
        K_Node<GeneType, num_nodes>::K_Node(std::string id,
            std::array<std::shared_ptr<Node<GeneType>>, num_nodes> nodes):
            Node<GeneType>(id), nodes_(std::move(nodes)) 
        {
            for (size_t i = 0; i < num_nodes; ++i)
            {
                Node<GeneType>::node_log_->ch_node_logs_.push_back(nodes_[i]->node_log_);
            }
        }

        template<typename GeneType, size_t num_nodes>
        K_Node<GeneType, num_nodes>& K_Node<GeneType, num_nodes>::setPoolingPopulations(
            typename Interfaces::poolingPopulations<GeneType, num_nodes> fun)
        {
            poolingPopulations_ = std::move(fun);
            return *this;
        }

        template<typename GeneType, size_t num_nodes>
        bool K_Node<GeneType, num_nodes>::isSetFunctions_() const
        {
            return Node<GeneType>::isSetFunctions_() && poolingPopulations_.has_value();
        }

        template<typename GeneType, size_t num_nodes>
        Types::Population<GeneType> K_Node<GeneType, num_nodes>::evolution(bool is_parallel)
        {
            Node<GeneType>::node_log_->start();
            std::array<Types::Population<GeneType>, num_nodes> populations;
            if (is_parallel)
            {
                std::vector<std::thread> treads;

                std::function evolute_node = 
                [is_parallel](Types::Population<GeneType>& population, std::shared_ptr<GeneticAlgorithm::EvolutionTree::Node<GeneType>>& node_ptr)
                {
                    population = node_ptr->evolution(is_parallel);
                };

                for (size_t i = 0; i < num_nodes; ++i)
                {
                    treads.push_back(std::thread(evolute_node, std::ref(populations[i]), std::ref(nodes_[i])));
                }
                for (size_t i = 0; i < num_nodes; ++i)
                {
                    treads[i].join();
                }
            }
            else
            {
                for (size_t i = 0; i < num_nodes; ++i)
                {
                    populations[i] = nodes_[i]->evolution(is_parallel);
                }
            }
            for (size_t i = 0; i < num_nodes; ++i)
            {
                nodes_[i].reset();
            }

            Node<GeneType>::node_log_->evolution_start();
            Types::Population<GeneType> population = poolingPopulations_.value()(populations);
            Node<GeneType>::evolution(population);
            Node<GeneType>::node_log_->end();

            return population;
        }


        template<typename GeneType>
        PopulationNode<GeneType>::PopulationNode(std::string id): Node<GeneType>(id) {}

        template<typename GeneType>
        PopulationNode<GeneType>& PopulationNode<GeneType>::setStartPopulationFunction(
            typename Interfaces::startPopulationFunction<GeneType> fun)
        {
            startPopulationFunction_ = std::move(fun);
            return *this;
        }

        template<typename GeneType>
        bool PopulationNode<GeneType>::isSetFunctions_() const
        {
            return Node<GeneType>::isSetFunctions_() && startPopulationFunction_.has_value();
        }

        template<typename GeneType>
        Types::Population<GeneType> PopulationNode<GeneType>::evolution(bool is_parallel)
        {
            Node<GeneType>::node_log_->start();
            Node<GeneType>::node_log_->evolution_start();
            Types::Population<GeneType> population = startPopulationFunction_.value()();
            Node<GeneType>::evolution(population);
            Node<GeneType>::node_log_->end();
            return population;
        }
    }
    // end namespace EvolutionTree
}
// end namespace GeneticAlgorithm