#include "evolution_tree.hpp"

namespace GeneticAlgorithm
{
    namespace Data
    {
        using namespace Interfaces;

        template<typename GeneType>
        Conditions<GeneType>::Conditions(
            FitnessFunction<GeneType> fitnessFunction,
            ProximityFunction<GeneType> proximityFunction,
            SelectionFunction<GeneType> selectionFunction,
            ConditionsForStoppingFunction conditionsForStoppingFunction):
            fitnessFunction_(fitnessFunction), proximityFunction_(proximityFunction), 
            selectionFunction_(selectionFunction), conditionsForStoppingFunction_(conditionsForStoppingFunction) {}

        template<typename GeneType>
        Conditions<GeneType>::Conditions(const Conditions<GeneType>& conditions):
            fitnessFunction_(conditions.fitnessFunction_), proximityFunction_(conditions.proximityFunction_), selectionFunction_(conditions.selectionFunction_), conditionsForStoppingFunction_(conditions.conditionsForStoppingFunction_) {}

        template<typename GeneType>
        StartConditions<GeneType>::StartConditions(
            FitnessFunction<GeneType> fitnessFunction,
            ProximityFunction<GeneType> proximityFunction,
            SelectionFunction<GeneType> selectionFunction,
            StartGenerationFunction<GeneType> startGenerationFunction,
            ConditionsForStoppingFunction conditionsForStoppingFunction):
            Conditions<GeneType>(fitnessFunction, proximityFunction, selectionFunction, conditionsForStoppingFunction), startGenerationFunction_(startGenerationFunction) {}
        
        template<typename GeneType>
        StartConditions<GeneType>::StartConditions(const StartConditions<GeneType>& conditions):
            Conditions<GeneType>(conditions.fitnessFunction_, conditions.proximityFunction_, conditions.selectionFunction_, conditions.conditionsForStoppingFunction_), startGenerationFunction_(conditions.startGenerationFunction_) {}

        template<typename GeneType>
        PoolingConditions<GeneType>::PoolingConditions(
            FitnessFunction<GeneType> fitnessFunction,
            ProximityFunction<GeneType> proximityFunction,
            SelectionFunction<GeneType> selectionFunction,
            PoolingConditions<GeneType> poolingConditions,
            ConditionsForStoppingFunction conditionsForStoppingFunction):
            Conditions<GeneType>(fitnessFunction, proximityFunction, selectionFunction, conditionsForStoppingFunction), poolingConditions_(poolingConditions) {}
    
        template<typename GeneType>
        PoolingConditions<GeneType>::PoolingConditions(const PoolingConditions<GeneType>& conditions):
            Conditions<GeneType>(conditions.fitnessFunction_, conditions.proximityFunction_, conditions.selectionFunction_, conditions.conditionsForStoppingFunction_), poolingConditions_(conditions.poolingConditions_) {}
    
    } // end namespace Data

    namespace EvolutionTree
    {
        template<typename GeneType>
        Node<GeneType>::Node(const Data::Conditions<GeneType>& conditions): conditions_(conditions) {}
    
        template<typename GeneType>
        UnaryNode<GeneType>::UnaryNode(const Data::Conditions<GeneType>& conditions, Node<GeneType>* node): Node<GeneType>(conditions), node_(node) {}

        template<typename GeneType>
        Types::Generation<GeneType> UnaryNode<GeneType>::selection()
        {
            return node_->selection();
        }

        template<typename GeneType>
        BinaryNode<GeneType>::BinaryNode(const Data::PoolingConditions<GeneType>& conditions, Node<GeneType>* node1, Node<GeneType>* node2): Node<GeneType>(conditions), node1_(node1), node2_(node2) {}

        template<typename GeneType>
        PopulationNode<GeneType>::PopulationNode(const Data::StartConditions<GeneType>& conditions): Node<GeneType>(conditions) {}
    }

}