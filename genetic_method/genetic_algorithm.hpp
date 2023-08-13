#pragma once

#include <functional>
#include "evolution_tree.hpp"

namespace GeneticAlgorithm
{

    template<typename GeneType, typename ResultType>
    class General
    {
        public:

        General(EvolutionTree::Node<GeneType>* node, Interfaces::resultFunction<GeneType, ResultType> resultFunction);

        ResultType calc();

        private:
        
        EvolutionTree::Node<GeneType>* node_;
        Interfaces::resultFunction<GeneType, ResultType> resultFunction_;
    };


    template<typename GeneType, typename ResultType>
    General<GeneType, ResultType>::General(EvolutionTree::Node<GeneType>* node, Interfaces::resultFunction<GeneType, ResultType> resultFunction):
        node_(node), resultFunction_(resultFunction) {}

    template<typename GeneType, typename ResultType>
    ResultType General<GeneType, ResultType>::calc()
    {
        return resultFunction_(node_->evolution());
    }

} // end namespace GeneticAlgorithm