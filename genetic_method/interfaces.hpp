#pragma once

#include "genetic_algorithm.hpp"

namespace GeneticAlgorithm
{
    namespace Interfaces
    {
        template<typename GeneType, typename ReturnType, typename ... Args>
        struct DataGetterWrapper
        {
            virtual std::function<ReturnType(const Types::Population<GeneType>&)> operator()(Args&& ...) = 0;
        };

        template<typename GeneType, typename ... Args>
        struct FitnessFunctionWrapper
        {
            virtual std::function<double(const Types::Chromosome<GeneType>&)> operator()(Args&& ...) = 0;
        };

        template<typename GeneType, typename ... Args>
        struct ProximityFunctionWrapper
        {
            virtual std::function<double(const Types::Chromosome<GeneType>&, const Types::Chromosome<GeneType>&)> operator()(Args&& ...) = 0;
        };

        template<typename GeneType, typename ... Args>
        struct StartGenerationFunctionWrapper
        {
            virtual std::function<Types::Generation<GeneType>()> operator()(Args&& ...) = 0;
        };

        template<typename GeneType, typename ... Args>
        struct SelectionFunctionWrapper
        {
            virtual std::function<Types::Generation<GeneType>(const Types::Generation<GeneType>&)> operator()(Args&& ...) = 0;
        };

        template<typename ... Args>
        struct ConditionsForStoppingWrapper
        {
            virtual std::function<bool()> operator() (Args&& ...) = 0;
        };

        template<typename GeneType, typename ... Args>
        struct PoolingPopulationsWrapper
        {
            virtual std::function<Types::Population<GeneType>(const Types::Population<GeneType>&, const Types::Population<GeneType>&)> operator()(Args&& ...) = 0;
        };
        
        template<typename GeneType>
        using FitnessFunction = std::function<double(const Types::Chromosome<GeneType>&)>;
        template<typename GeneType>
        using ProximityFunction = std::function<double(const Types::Chromosome<GeneType>&, const Types::Chromosome<GeneType>&)>;
        template<typename GeneType>
        using StartGenerationFunction = std::function<Types::Generation<GeneType>()>;
        template<typename GeneType>
        using SelectionFunction = std::function<Types::Generation<GeneType>(const Types::Generation<GeneType>&)>;
        template<typename GeneType>
        using PoolingPopulations = std::function<Types::Population<GeneType>(const Types::Population<GeneType>&, const Types::Population<GeneType>&)>;
        using ConditionsForStoppingFunction = std::function<bool()>;

    } // end namespace Interfaces
} // end namespace GeneticAlgorithm