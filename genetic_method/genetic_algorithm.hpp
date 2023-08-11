#pragma once

#include <functional>
#include "types.hpp"
#include "interfaces.hpp"
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

} // end namespace GeneticAlgorithm