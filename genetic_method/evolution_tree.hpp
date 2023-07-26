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
        class UnaryNode: public Node
        {
            Node* node_;
            
            public:

            UnaryNode(const Data::Conditions<GeneType>& conditions, Node* node);

            Types::Generation<GeneType> selection() override;
        };

        template<typename GeneType>
        class BinaryNode: public Node
        {
            Node* node1_, node2_;
            
            public:

            BinaryNode(const PoolingConditions<GeneType>& conditions, Node* node1, Node* node2);

            Types::Generation<GeneType> selection() override;
        };

        template<typename GeneType>
        class PopulationNode: public Node
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