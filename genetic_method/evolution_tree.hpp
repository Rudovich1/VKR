#pragma once

#include "types.hpp"
#include "interfaces.hpp"

namespace GeneticAlgorithm
{
    namespace Data
    {
        using namespace Interfaces;

        template<typename GeneType>
        struct Conditions
        {
            FitnessFunction<GeneType> fitnessFunction_;
            ProximityFunction<GeneType> proximityFunction_;
            SelectionFunction<GeneType> selectionFunction_;
            ConditionsForStoppingFunction conditionsForStoppingFunction_;

            Conditions(
                FitnessFunction<GeneType> fitnessFunction,
                ProximityFunction<GeneType> proximityFunction,
                SelectionFunction<GeneType> selectionFunction,
                ConditionsForStoppingFunction conditionsForStoppingFunction);
            Conditions(const Conditions<GeneType>& conditions);
        };

        template<typename GeneType>
        struct StartConditions: public Conditions<GeneType>
        {
            StartGenerationFunction<GeneType> startGenerationFunction_;

            StartConditions(
                FitnessFunction<GeneType> fitnessFunction,
                ProximityFunction<GeneType> proximityFunction,
                SelectionFunction<GeneType> selectionFunction,
                StartGenerationFunction<GeneType> startGenerationFunction,
                ConditionsForStoppingFunction conditionsForStoppingFunction
            );
            StartConditions(const StartConditions<GeneType>& conditions);
        };

        template<typename GeneType>
        struct PoolingConditions: public Conditions<GeneType>
        {
            PoolingConditions<GeneType> poolingConditions_;

            PoolingConditions(
                FitnessFunction<GeneType> fitnessFunction,
                ProximityFunction<GeneType> proximityFunction,
                SelectionFunction<GeneType> selectionFunction,
                PoolingConditions<GeneType> poolingConditions,
                ConditionsForStoppingFunction conditionsForStoppingFunction
            );
            PoolingConditions(const PoolingConditions<GeneType>& conditions);
        };

    } // end namespace Data

    namespace EvolutionTree
    {
        template<typename GeneType>
        class Node
        {
            Data::Conditions<GeneType>& conditions_;

            public:

            Node(const Data::Conditions<GeneType>& conditions); 

            virtual Types::Generation<GeneType> selection() = 0;
        };

        template<typename GeneType>
        class UnaryNode: public Node<GeneType>
        {
            Node<GeneType>* node_;
            
            public:

            UnaryNode(const Data::Conditions<GeneType>& conditions, Node<GeneType>* node);

            Types::Generation<GeneType> selection() override;
        };

        template<typename GeneType>
        class BinaryNode: public Node<GeneType>
        {
            Node<GeneType>* node1_, node2_;
            
            public:

            BinaryNode(const PoolingConditions<GeneType>& conditions, Node<GeneType>* node1, Node<GeneType>* node2);

            Types::Generation<GeneType> selection() override;
        };

        template<typename GeneType>
        class PopulationNode: public Node<GeneType>
        {
            public:

            PopulationNode(const StartConditions<GeneType>& conditions)

            Types::Generation<GeneType> selection() override;
        };

        template<typename GeneType, int numNode>
        class K_Node: public Node
        {
            
        }; // TODO

    } // end namespace EvolutionTree
} // end namespace GeneticAlgorithm