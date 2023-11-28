#pragma once

#include "types.hpp"
#include "interfaces.hpp"

#include <chrono>
#include <string>
#include <sstream>
#include <memory>
#include <array>

namespace GeneticAlgorithm
{
    namespace EvolutionTree
    {
        template<typename GeneType>
        class Node
        {
        protected:
            typename Interfaces::fitnessFunction<GeneType> fitnessFunction_;
            typename Interfaces::selectionFunction<GeneType> selectionFunction_;
            typename Interfaces::mutationFunction<GeneType> mutationFunction_;
            typename Interfaces::crossingoverFunction<GeneType> crossingoverFunction_;
            typename Interfaces::conditionsForStoppingFunction<GeneType> conditionsForStoppingFunction_;
            typename Interfaces::anyFunction<GeneType> anyFunction_;
            typename Interfaces::endNode<GeneType> endNode_;
            std::string id_;

        public:
            Node(std::string id);

            Node& setFitnessFunction(typename Interfaces::fitnessFunction<GeneType>);
            Node& setSelectionFunction(typename Interfaces::selectionFunction<GeneType>);
            Node& setMutationFunction(typename Interfaces::mutationFunction<GeneType>);
            Node& setCrossingoverFunction(typename Interfaces::crossingoverFunction<GeneType>);
            Node& setConditionsForStoppingFunction( typename Interfaces::conditionsForStoppingFunction<GeneType>);
            Node& setAnyFunction(typename Interfaces::anyFunction<GeneType>);
            Node& setEndNode(typename Interfaces::endNode<GeneType>);

            void evolution(Types::Population<GeneType>&);

            virtual Types::Population<GeneType> evolution() = 0;
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
            typename Interfaces::poolingPopulations<GeneType, num_nodes> poolingPopulations_;

        public:
            K_Node(std::string id, std::array<std::unique_ptr<Node<GeneType>>, num_nodes>&& nodes);

            K_Node& setPoolingPopulations(typename Interfaces::poolingPopulations<GeneType, num_nodes>);

            virtual Types::Population<GeneType> evolution() override;
        };


        template<typename GeneType>
        class BinaryNode: public K_Node<GeneType, 2> {};


        template<typename GeneType>
        class PopulationNode: public Node<GeneType>
        {
        protected:
            typename Interfaces::startPopulationFunction<GeneType> startPopulationFunction_;

        public:
            PopulationNode(std::string id);

            PopulationNode& setStartPopulationFunction(typename Interfaces::startPopulationFunction<GeneType>);

            virtual Types::Population<GeneType> evolution() override;
        };
    } // end namespace EvolutionTree
} // end namespace GeneticAlgorithm

namespace GeneticAlgorithm
{
    namespace DefaultFunctions
    {
        template<typename GeneType>
        struct DefaultFitnessFunctionWrapper final: public Interfaces::FitnessFunctionWrapper<GeneType>
        {
            std::function<double(const Types::Chromosome<GeneType>&)> operator()() const override
            {
                return [](const Types::Chromosome<GeneType>&) {return 0.;};
            }
        };

        template<typename GeneType>
        struct DefaultStartPopulationFunctionWrapper final: public Interfaces::StartPopulationFunctionWrapper<GeneType>
        {
            std::function<Types::Population<GeneType>()> operator()() const override
            {
                return []()
                {
                    return Types::Population<GeneType>(1).add(Types::Generation<GeneType>(1, 1));
                };
            }
        };

        template<typename GeneType>
        struct DefaultSelectionFunctionWrapper final: public Interfaces::SelectionFunctionWrapper<GeneType>
        {
            std::function<Types::Generation<GeneType>(const Types::Generation<GeneType>&)> operator()() const override
            {
                return [](const Types::Generation<GeneType>& generation) {return generation;};
            }
        };

        template<typename GeneType>
        struct DefaultConditionsForStoppingWrapper final: public Interfaces::ConditionsForStoppingWrapper<GeneType>
        {
            std::function<bool(const Types::Population<GeneType>&)> operator()() const override
            {
                return [](const Types::Population<GeneType>&) {return true;};
            }
        };

        template<typename GeneType, size_t num_populations>
        struct DefaultPoolingPopulationsWrapper final: public Interfaces::PoolingPopulationsWrapper<GeneType, num_populations>
        {
            std::function<Types::Population<GeneType>(const std::array<Types::Population<GeneType>, num_populations>&)> operator()() const override
            {
                return [](const std::array<Types::Population<GeneType>, num_populations>& populations) {return populations[0];};
            }
        };

        template<typename GeneType>
        struct DefaultMutationFunctionWrapper final: public Interfaces::MutationFunctionWrapper<GeneType>
        {
            std::function<void(Types::Generation<GeneType>&)> operator()() const override
            {
                return [](Types::Generation<GeneType>&) {};
            }
        };

        template<typename GeneType>
        struct DefaultCrossingoverFunctionWrapper final: public Interfaces::CrossingoverFunctionWrapper<GeneType>
        {
            std::function<Types::Generation<GeneType>(Types::Population<GeneType>&)> operator()() const override
            {
                return [](Types::Population<GeneType>& population) {return population.get().back();};
            }
        };

        // template<typename GeneType>
        // struct DefaultResultFunctionWrapper final: public Interfaces::ResultFunctionWrapper<GeneType, double>
        // {
        //     std::function<double(const Types::Population<GeneType>&)> operator()() const override
        //     {
        //         return [](const Types::Population<GeneType>&) {return 0.;};
        //     }
        // };

        template<typename GeneType>
        struct DefaultAnyFunctionWrapper final: public Interfaces::AnyFunctionWrapper<GeneType>
        {
            std::function<void(const Types::Population<GeneType>&)> operator()() const override
            {
                return [](const Types::Population<GeneType>&) {};
            }
        };

        template<typename GeneType>
        struct DefaultEndNodeFunctionWrapper final: public Interfaces::EndNodeFunctionWrapper<GeneType>
        {
            std::function<void()> operator()() const override
            {
                return [](){};
            }
        };
    } // namespace DefaultFunctions

    namespace EvolutionTree
    {
        template<typename GeneType>
        Node<GeneType>::Node(std::string id): 
            id_(id),
            fitnessFunction_(DefaultFunctions::DefaultFitnessFunctionWrapper<GeneType>()()),
            selectionFunction_(DefaultFunctions::DefaultSelectionFunctionWrapper<GeneType>()()),
            mutationFunction_(DefaultFunctions::DefaultMutationFunctionWrapper<GeneType>()()),
            crossingoverFunction_(DefaultFunctions::DefaultCrossingoverFunctionWrapper<GeneType>()()),
            conditionsForStoppingFunction_(DefaultFunctions::DefaultConditionsForStoppingWrapper<GeneType>()()),
            anyFunction_(DefaultFunctions::DefaultAnyFunctionWrapper<GeneType>()()),
            endNode_(DefaultFunctions::DefaultEndNodeFunctionWrapper<GeneType>()()) {}

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
        void Node<GeneType>::evolution(Types::Population<GeneType>& population)
        {
            for (Types::Chromosome<GeneType>& chromosome: population.get().back().get())
            {
                if (chromosome.getFitness().isNull())
                {
                    chromosome.getFitness().setVal(fitnessFunction_(chromosome));
                }
            }
            while (!conditionsForStoppingFunction_(population))
            {
                Types::Generation<GeneType> new_generation = crossingoverFunction_(population);
                mutationFunction_(new_generation);
                for (Types::Chromosome<GeneType>& chromosome: new_generation.get())
                {
                    if (chromosome.getFitness().isNull())
                    {
                        chromosome.getFitness().setVal(fitnessFunction_(chromosome));
                    }
                }
                population.add(selectionFunction_(new_generation));
                anyFunction_(population);
            }
        } 


        template<typename GeneType>
        UnaryNode<GeneType>::UnaryNode(std::string id, std::unique_ptr<Node<GeneType>>&& node): 
            Node<GeneType>(id), node_(node) {}

        template<typename GeneType>
        Types::Population<GeneType> UnaryNode<GeneType>::evolution()
        {
            Types::Population<GeneType> population = node_->evolution();
            Node<GeneType>::evolution(population);
            this->endNode_();
            return population;
        }


        template<typename GeneType, size_t num_nodes>
        K_Node<GeneType, num_nodes>::K_Node(std::string id,
            std::array<std::unique_ptr<Node<GeneType>>, num_nodes>&& nodes):
            Node<GeneType>(id), nodes_(nodes) {}

        template<typename GeneType, size_t num_nodes>
        K_Node<GeneType, num_nodes>& K_Node<GeneType, num_nodes>::setPoolingPopulations(
            typename Interfaces::poolingPopulations<GeneType, num_nodes> fun)
        {
            poolingPopulations_ = std::move(fun);
            return *this;
        }

        template<typename GeneType, size_t num_nodes>
        Types::Population<GeneType> K_Node<GeneType, num_nodes>::evolution()
        {
            std::array<Types::Population<GeneType>, num_nodes> populations(0);
            for (size_t i = 0; i < num_nodes; ++i)
            {
                populations[i] = nodes_[i]->evolution();
            }
            Types::Population<GeneType> population = poolingPopulations_(populations);
            Node<GeneType>::evolution(population);
            this->endNode_();
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
        Types::Population<GeneType> PopulationNode<GeneType>::evolution()
        {
            Types::Population<GeneType> population = startPopulationFunction_();
            Node<GeneType>::evolution(population);
            return population;
        }
    } // end namespace EvolutionTree
} // end namespace GeneticAlgorithm