#pragma once

#include "types.hpp"
#include "interfaces.hpp"

namespace GeneticAlgorithm
{
    namespace Data
    {
        using namespace Interfaces;

        template<typename GeneType>
        struct BaseData
        {
            fitnessFunction<GeneType> fitnessFunction_;
            proximityFunction<GeneType> proximityFunction_;
            selectionFunction<GeneType> selectionFunction_;
            mutationFunction<GeneType> mutationFunction_;
            crossingoverFunction<GeneType> crossingoverFunction_;
            conditionsForStoppingFunction<GeneType> conditionsForStoppingFunction_;
            size_t buffer_size_;
            size_t generation_size_; 
            size_t chromosome_size_;

            BaseData(
                fitnessFunction<GeneType> fitnessFunction,
                proximityFunction<GeneType> proximityFunction,
                selectionFunction<GeneType> selectionFunction,
                mutationFunction<GeneType> mutationFunction,
                crossingoverFunction<GeneType> crossingoverFunction,
                conditionsForStoppingFunction<GeneType> conditionsForStoppingFunction,
                size_t buffer_size, size_t generation_size, size_t chromosome_size);

            BaseData(const BaseData<GeneType>& base_data);
        };

        template<typename GeneType>
        struct Conditions
        {
            BaseData<GeneType> base_data_;

            Conditions(const BaseData<GeneType>& base_data);
            Conditions(const Conditions<GeneType>& conditions);
        };

        template<typename GeneType>
        struct StartConditions: public Conditions<GeneType>
        {
            startGenerationFunction<GeneType> startGenerationFunction_;

            StartConditions(const BaseData<GeneType>& base_data, startGenerationFunction<GeneType> startGenerationFunction);
            StartConditions(const StartConditions<GeneType>& conditions);
        };

        template<typename GeneType>
        struct PoolingConditions: public Conditions<GeneType>
        {
            poolingPopulations<GeneType> poolingPopulations_;

            PoolingConditions(const BaseData<GeneType>& base_data, poolingPopulations<GeneType> poolingPopulations_);
            PoolingConditions(const PoolingConditions<GeneType>& conditions);
        };

    } // end namespace Data

    namespace Evolution
    {
        template<typename GeneType>
        void evolution(Types::Population<GeneType>& population, Data::Conditions<GeneType>& conditions);
        
    } // end namespace Evolution

    namespace EvolutionTree
    {
        template<typename GeneType>
        class Node
        {
            Data::Conditions<GeneType>& conditions_;

        public:
            Node(const Data::Conditions<GeneType>& conditions); 
            virtual Types::Population<GeneType> evolution() = 0;
        };

        template<typename GeneType>
        class UnaryNode: public Node<GeneType>
        {
            Node<GeneType>* node_;
            
        public:
            UnaryNode(const Data::Conditions<GeneType>& conditions, Node<GeneType>* node);
            Types::Population<GeneType> evolution() override;
        };

        template<typename GeneType>
        class BinaryNode: public Node<GeneType>
        {
            Node<GeneType>* node1_, node2_;
            
        public:
            BinaryNode(const Data::PoolingConditions<GeneType>& conditions, Node<GeneType>* node1, Node<GeneType>* node2);
            Types::Population<GeneType> evolution() override;
        };

        template<typename GeneType>
        class PopulationNode: public Node<GeneType>
        {
        public:
            PopulationNode(const Data::StartConditions<GeneType>& conditions);
            Types::Population<GeneType> evolution() override;
        };

        template<typename GeneType, int numNode>
        class K_Node: public Node
        {  
        }; // TODO

    } // end namespace EvolutionTree

} // end namespace GeneticAlgorithm