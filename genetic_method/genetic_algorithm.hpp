#pragma once

#include <functional>
#include "types.hpp"
#include "interfaces.hpp"

namespace GeneticAlgorithm
{

    template<typename GeneType>
    class GeneticAlgorithm
    {
        Data::GeneticAlgorithmConditions conditions_;

        public:

        GeneticAlgorithmConditions(Data::GeneticAlgorithmConditions conditions);



    };

} // end namespace GeneticAlgorithm