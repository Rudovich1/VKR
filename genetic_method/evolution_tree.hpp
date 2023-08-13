#pragma once

#include "types.hpp"
#include "interfaces.hpp"

namespace GeneticAlgorithm
{
    namespace Conditions
    {
        template<typename GeneType>
        struct BaseConditions
        {
            typename Interfaces::fitnessFunction<GeneType> fitnessFunction_;
            typename Interfaces::proximityFunction<GeneType> proximityFunction_;
            typename Interfaces::selectionFunction<GeneType> selectionFunction_;
            typename Interfaces::mutationFunction<GeneType> mutationFunction_;
            typename Interfaces::crossingoverFunction<GeneType> crossingoverFunction_;
            typename Interfaces::conditionsForStoppingFunction<GeneType> conditionsForStoppingFunction_;
            typename Interfaces::anyFunction<GeneType> anyFunction_;
            size_t buffer_size_;
            size_t generation_size_; 
            size_t chromosome_size_;

            BaseConditions(
                typename Interfaces::fitnessFunction<GeneType> fitnessFunction,
                typename Interfaces::proximityFunction<GeneType> proximityFunction,
                typename Interfaces::selectionFunction<GeneType> selectionFunction,
                typename Interfaces::mutationFunction<GeneType> mutationFunction,
                typename Interfaces::crossingoverFunction<GeneType> crossingoverFunction,
                typename Interfaces::conditionsForStoppingFunction<GeneType> conditionsForStoppingFunction,
                typename Interfaces::anyFunction<GeneType> anyFunction,
                size_t buffer_size, size_t generation_size, size_t chromosome_size);

            BaseConditions(const BaseConditions<GeneType>& base_conditions);
        };

        // template<typename GeneType>
        // struct Conditions
        // {
        //     BaseData<GeneType> base_data_;

        //     Conditions(const BaseData<GeneType>& base_data);
        //     Conditions(const Conditions<GeneType>& conditions);
        // };

        // template<typename GeneType>
        // struct StartConditions: public Conditions<GeneType>
        // {
        //     typename Interfaces::startGenerationFunction<GeneType> startGenerationFunction_;

        //     StartConditions(const BaseData<GeneType>& base_data, typename Interfaces::startGenerationFunction<GeneType> startGenerationFunction);
        //     StartConditions(const StartConditions<GeneType>& conditions);
        // };

        // template<typename GeneType>
        // struct PoolingConditions: public Conditions<GeneType>
        // {
        //     typename Interfaces::poolingPopulations<GeneType> poolingPopulations_;

        //     PoolingConditions(const BaseData<GeneType>& base_data, typename Interfaces::poolingPopulations<GeneType> poolingPopulations_);
        //     PoolingConditions(const PoolingConditions<GeneType>& conditions);
        // };


        template<typename GeneType>
        BaseConditions<GeneType>::BaseConditions(
            typename Interfaces::fitnessFunction<GeneType> fitnessFunction,
            typename Interfaces::proximityFunction<GeneType> proximityFunction,
            typename Interfaces::selectionFunction<GeneType> selectionFunction,
            typename Interfaces::mutationFunction<GeneType> mutationFunction,
            typename Interfaces::crossingoverFunction<GeneType> crossingoverFunction,
            typename Interfaces::conditionsForStoppingFunction<GeneType> conditionsForStoppingFunction,
            typename Interfaces::anyFunction<GeneType> anyFunction,
            size_t buffer_size, size_t generation_size, size_t chromosome_size):
            fitnessFunction_(fitnessFunction), proximityFunction_(proximityFunction), selectionFunction_(selectionFunction), 
            mutationFunction_(mutationFunction), crossingoverFunction_(crossingoverFunction), conditionsForStoppingFunction_(conditionsForStoppingFunction),
            anyFunction_(anyFunction), buffer_size_(buffer_size), generation_size_(generation_size), chromosome_size_(chromosome_size) {}

        template<typename GeneType>
        BaseConditions<GeneType>::BaseConditions(const BaseConditions<GeneType>& base_conditions):
            fitnessFunction_(base_conditions.fitnessFunction_), proximityFunction_(base_conditions.proximityFunction_),
            selectionFunction_(base_conditions.selectionFunction_), mutationFunction_(base_conditions.mutationFunction_), 
            crossingoverFunction_(base_conditions.crossingoverFunction_), conditionsForStoppingFunction_(base_conditions.conditionsForStoppingFunction_),
            anyFunction_(base_conditions.anyFunction_), buffer_size_(base_conditions.buffer_size_), generation_size_(base_conditions.generation_size_), 
            chromosome_size_(base_conditions.chromosome_size_) {}

        // template<typename GeneType>
        // Conditions<GeneType>::Conditions(const BaseData<GeneType>& base_data): base_data_(base_data) {}

        // template<typename GeneType>
        // Conditions<GeneType>::Conditions(const Conditions<GeneType>& conditions): base_data_(conditions.base_data_) {}

        // template<typename GeneType>
        // StartConditions<GeneType>::StartConditions(const BaseData<GeneType>& base_data, typename Interfaces::startGenerationFunction<GeneType> startGenerationFunction):
        //     Conditions<GeneType>(base_data), startGenerationFunction_(startGenerationFunction) {}

        // template<typename GeneType>
        // StartConditions<GeneType>::StartConditions(const StartConditions<GeneType>& conditions): 
        //     Conditions<GeneType>(conditions.base_data_), startGenerationFunction_(conditions.startGenerationFunction_) {}

        // template<typename GeneType>
        // PoolingConditions<GeneType>::PoolingConditions(const BaseData<GeneType>& base_data, typename Interfaces::poolingPopulations<GeneType> poolingPopulations):
        //     Conditions<GeneType>(base_data), poolingPopulations_(poolingPopulations) {} 
    
        // template<typename GeneType>
        // PoolingConditions<GeneType>::PoolingConditions(const PoolingConditions<GeneType>& conditions):
        //     Conditions<GeneType>(conditions.base_data_), poolingPopulations_(conditions.poolingPopulations_) {}

    } // end namespace Data

    namespace Evolution
    {
        template<typename GeneType>
        void evolution(Types::Population<GeneType>& population, Conditions::BaseConditions<GeneType>& base_conditions);


        template<typename GeneType>
        void evolution(Types::Population<GeneType>& population, Conditions::BaseConditions<GeneType>& base_conditions)
        {
            for (Types::Chromosome<GeneType>& chromosome: population.get().back().get())
            {
                if (chromosome.getFitness().isNull())
                {
                    chromosome.getFitness().setVal(base_conditions.fitnessFunction_(chromosome));
                }
            }
            while (!base_conditions.conditionsForStoppingFunction_(population))
            {
                Types::Generation<GeneType> new_generation = population.get().back();
                base_conditions.crossingoverFunction_(new_generation);
                base_conditions.mutationFunction_(new_generation);
                for (Types::Chromosome<GeneType>& chromosome: new_generation.get())
                {
                    if (chromosome.getFitness().isNull())
                    {
                        chromosome.getFitness().setVal(base_conditions.fitnessFunction_(chromosome));
                    }
                }
                population.add(base_conditions.selectionFunction_(new_generation));
                base_conditions.anyFunction_(population);
            }   
        }
        
    } // end namespace Evolution

    namespace EvolutionTree
    {
        template<typename GeneType>
        class Node
        {
        protected:
            Conditions::BaseConditions<GeneType> base_conditions_;

        public:
            Node(const Conditions::BaseConditions<GeneType>& base_conditions_); 
            virtual Types::Population<GeneType> evolution() = 0;
        };

        template<typename GeneType>
        class UnaryNode: public Node<GeneType>
        {
            Node<GeneType>* node_;
            
        public:
            UnaryNode(const Conditions::BaseConditions<GeneType>& base_conditions_, Node<GeneType>* node);
            Types::Population<GeneType> evolution() override;
        };

        template<typename GeneType>
        class BinaryNode: public Node<GeneType>
        {
            Node<GeneType>* node1_, node2_;
            typename Interfaces::poolingPopulations<GeneType> poolingPopulations_;
            
        public:
            BinaryNode(const Conditions::BaseConditions<GeneType>& base_conditions, 
                Interfaces::poolingPopulations<GeneType> poolingPopulations, 
                Node<GeneType>* node1, Node<GeneType>* node2);

            Types::Population<GeneType> evolution() override;
        };

        template<typename GeneType>
        class PopulationNode: public Node<GeneType>
        {
            typename Interfaces::startGenerationFunction<GeneType> startGenerationFunction_;

        public:
            PopulationNode(const Conditions::BaseConditions<GeneType>& base_conditions, Interfaces::startGenerationFunction<GeneType> startGenerationFunction);
            Types::Population<GeneType> evolution() override;
        };

        // template<typename GeneType, int numNode>
        // class K_Node: public Node
        // {  
        // }; // TODO


        template<typename GeneType>
        Node<GeneType>::Node(const Conditions::BaseConditions<GeneType>& base_conditions): base_conditions_(base_conditions) {}
    
        template<typename GeneType>
        UnaryNode<GeneType>::UnaryNode(const Conditions::BaseConditions<GeneType>& base_conditions, Node<GeneType>* node): Node<GeneType>(base_conditions), node_(node) {}

        template<typename GeneType>
        Types::Population<GeneType> UnaryNode<GeneType>::evolution()
        {
            Types::Population<GeneType> population = node_->evolution();
            Evolution::evolution(population, this->base_conditions_);
            return population;
        }

        template<typename GeneType>
        BinaryNode<GeneType>::BinaryNode(const Conditions::BaseConditions<GeneType>& base_conditions,
            Interfaces::poolingPopulations<GeneType> poolingPopulations, Node<GeneType>* node1, Node<GeneType>* node2): 
            Node<GeneType>(base_conditions), poolingPopulations_(poolingPopulations), node1_(node1), node2_(node2) {}

        template<typename GeneType>
        Types::Population<GeneType> BinaryNode<GeneType>::evolution()
        {
            Types::Population<GeneType> population = poolingPopulations_(node1_->evolution(), node2_->evolution());
            Evolution::evolution(population, this->base_conditions_);
            return population;
        }

        template<typename GeneType>
        PopulationNode<GeneType>::PopulationNode(const Conditions::BaseConditions<GeneType>& base_conditions, 
            Interfaces::startGenerationFunction<GeneType> startGenerationFunction): 
            Node<GeneType>(base_conditions), startGenerationFunction_(startGenerationFunction) {}

        template <typename GeneType>
        Types::Population<GeneType> PopulationNode<GeneType>::evolution()
        {
            Types::Population<GeneType> population(this->base_conditions_.buffer_size_);
            population.add(startGenerationFunction_());
            Evolution::evolution(population, this->base_conditions_);
            return population;
        }

    } // end namespace EvolutionTree

} // end namespace GeneticAlgorithm