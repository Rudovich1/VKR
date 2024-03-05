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
        template<typename GeneType>
        class Node
        {
        protected:
            static inline std::unordered_set<std::string> ids_;

            std::optional<typename Interfaces::fitnessFunction<GeneType>> fitnessFunction_;
            std::optional<typename Interfaces::selectionFunction<GeneType>> selectionFunction_;
            std::optional<typename Interfaces::mutationFunction<GeneType>> mutationFunction_;
            std::optional<typename Interfaces::crossingoverFunction<GeneType>> crossingoverFunction_;
            std::optional<typename Interfaces::conditionsForStoppingFunction<GeneType>> conditionsForStoppingFunction_;
            std::optional<typename Interfaces::anyFunction<GeneType>> anyFunction_;
            std::optional<typename Interfaces::endNode<GeneType>> endNode_;
            std::optional<typename Interfaces::startNode<GeneType>> startNode_;

            std::optional<typename LogInterfaces::newGenerationLog<GeneType>> newGenerationLog_;
            std::optional<typename LogInterfaces::startNodeLog<GeneType>> startNodeLog_;
            std::optional<typename LogInterfaces::endNodeLog<GeneType>> endNodeLog_;

            const std::string id_;

            void cerrInfo_(std::string) const;
            virtual bool isSetFunctions_() const; 

        public:
            Node(std::string id);

            Node& setFitnessFunction(typename Interfaces::fitnessFunction<GeneType>);
            Node& setSelectionFunction(typename Interfaces::selectionFunction<GeneType>);
            Node& setMutationFunction(typename Interfaces::mutationFunction<GeneType>);
            Node& setCrossingoverFunction(typename Interfaces::crossingoverFunction<GeneType>);
            Node& setConditionsForStoppingFunction( typename Interfaces::conditionsForStoppingFunction<GeneType>);
            Node& setAnyFunction(typename Interfaces::anyFunction<GeneType>);
            Node& setEndNode(typename Interfaces::endNode<GeneType>);
            Node& setStartNode(typename Interfaces::startNode<GeneType>);

            Node& setNewGenerationLog(typename LogInterfaces::newGenerationLog<GeneType>);
            Node& setStartNodeLog(typename LogInterfaces::startNodeLog<GeneType>);
            Node& setEndNodeLog(typename LogInterfaces::endNodeLog<GeneType>);

            void evolution(Types::Population<GeneType>&) const;

            virtual Types::Population<GeneType> evolution() = 0;

            mutable NodeLog node_log_;
        };


        template<typename GeneType>
        class UnaryNode: public Node<GeneType>
        {
        protected:
            std::unique_ptr<Node<GeneType>> node_;
            
        public:
            UnaryNode(std::string id, std::unique_ptr<Node<GeneType>>&& node);

            virtual Types::Population<GeneType> evolution() override;
        };


        template<typename GeneType, size_t num_nodes>
        class K_Node: public Node<GeneType>
        {
        protected:
            std::array<std::unique_ptr<Node<GeneType>>, num_nodes> nodes_;
            std::optional<typename Interfaces::poolingPopulations<GeneType, num_nodes>> poolingPopulations_;
            virtual bool isSetFunctions_() const override;

        public:
            K_Node(std::string id, std::array<std::unique_ptr<Node<GeneType>>, num_nodes>&& nodes);

            K_Node& setPoolingPopulations(typename Interfaces::poolingPopulations<GeneType, num_nodes>);

            virtual Types::Population<GeneType> evolution() override;
        };


        template<typename GeneType>
        class PopulationNode: public Node<GeneType>
        {
        protected:
            std::optional<typename Interfaces::startPopulationFunction<GeneType>> startPopulationFunction_;
            virtual bool isSetFunctions_() const override;

        public:
            PopulationNode(std::string id);

            PopulationNode& setStartPopulationFunction(typename Interfaces::startPopulationFunction<GeneType>);

            virtual Types::Population<GeneType> evolution() override;
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
        Node<GeneType>::Node(std::string id): id_(std::move(id)), node_log_(id_)
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
            PopulationLog population_log;
            population_log.start();
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
            population_log.add(generation_log);

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
                population_log.add(generation_log);
            }

            if (endNode_.has_value()) {endNode_.value()();}
            if (endNodeLog_.has_value()) {endNodeLog_.value()(population, id_);}

            population_log.end();
            node_log_.population_log_ = std::move(population_log);
        }

        template<typename GeneType>
        void Node<GeneType>::cerrInfo_(std::string info) const
        {
            std::cerr << id_ << ": " << info << '\n';
        }

        template<typename GeneType>
        UnaryNode<GeneType>::UnaryNode(std::string id, std::unique_ptr<Node<GeneType>>&& node): 
            Node<GeneType>(id), node_(std::move(node)) {}

        template<typename GeneType>
        Types::Population<GeneType> UnaryNode<GeneType>::evolution()
        {
            Node<GeneType>::node_log_.start();
            Types::Population<GeneType> population = node_->evolution();
            Node<GeneType>::node_log_.ch_node_logs_.push_back(std::move(node_->node_log_));
            node_.reset();
            Node<GeneType>::evolution(population);
            Node<GeneType>::node_log_.end();
            return population;
        }

        template<typename GeneType, size_t num_nodes>
        K_Node<GeneType, num_nodes>::K_Node(std::string id,
            std::array<std::unique_ptr<Node<GeneType>>, num_nodes>&& nodes):
            Node<GeneType>(id), nodes_(std::move(nodes)) {}

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
        Types::Population<GeneType> K_Node<GeneType, num_nodes>::evolution()
        {
            Node<GeneType>::node_log_.start();
            std::array<Types::Population<GeneType>, num_nodes> populations;
            std::vector<std::thread> treads;

            std::function evolute_node = 
            [](Types::Population<GeneType>& population, std::unique_ptr<GeneticAlgorithm::EvolutionTree::Node<GeneType>>& node_ptr)
            {
                population = std::move(node_ptr->evolution());
            };

            for (size_t i = 0; i < num_nodes; ++i)
            {
                treads.push_back(std::thread(evolute_node, std::ref(populations[i]), std::ref(nodes_[i])));
            }
            for (size_t i = 0; i < num_nodes; ++i)
            {
                treads[i].join();
            }
            Types::Population<GeneType> population = poolingPopulations_.value()(populations);
            for (size_t i = 0; i < num_nodes; ++i)
            {
                Node<GeneType>::node_log_.ch_node_logs_.push_back(std::move(nodes_[i]->node_log_));
                nodes_[i].reset();
            }
            
            Node<GeneType>::evolution(population);
            Node<GeneType>::node_log_.end();
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
        Types::Population<GeneType> PopulationNode<GeneType>::evolution()
        {
            Node<GeneType>::node_log_.start();
            Types::Population<GeneType> population = startPopulationFunction_.value()();
            Node<GeneType>::evolution(population);
            Node<GeneType>::node_log_.end();
            return population;
        }
    }
    // end namespace EvolutionTree
}
// end namespace GeneticAlgorithm