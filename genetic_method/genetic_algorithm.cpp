#include "genetic_algorithm.hpp"

namespace GeneticAlgorithm
{
    template<typename GeneType, typename ResultType>
    General<GeneType, ResultType>::General(EvolutionTree::Node<GeneType>* node, Interfaces::resultFunction<GeneType, ResultType> resultFunction):
        node_(node), resultFunction_(resultFunction) {}

    template<typename GeneType, typename ResultType>
    ResultType General<GeneType, ResultType>::calc()
    {
        return resultFunction_(node_->evolution());
    }
}