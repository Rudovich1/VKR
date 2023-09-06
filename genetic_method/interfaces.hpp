#pragma once

#include <functional>
#include "types.hpp"

namespace GeneticAlgorithm
{
    namespace Interfaces
    {
        template<typename GeneType, typename ReturnType, typename ... Args>
        struct DataGetterWrapper
        {
            virtual std::function<ReturnType(const Types::Population<GeneType>&)> operator()(Args& ...) = 0;
        }; // TODO

        template<typename GeneType, typename ... Args>
        struct FitnessFunctionWrapper
        {
            virtual std::function<double(const Types::Chromosome<GeneType>&)> operator()(Args& ...) = 0;
        };

        template<typename GeneType, typename ... Args>
        struct ProximityFunctionWrapper
        {
            virtual std::function<double(const Types::Chromosome<GeneType>&, const Types::Chromosome<GeneType>&)> operator()(Args& ...) = 0;
        }; // TODO 

        template<typename GeneType, typename ... Args>
        struct StartGenerationFunctionWrapper
        {
            virtual std::function<Types::Generation<GeneType>()> operator()(Args& ...) = 0;
        };

        template<typename GeneType, typename ... Args>
        struct SelectionFunctionWrapper
        {
            virtual std::function<void(Types::Generation<GeneType>&)> operator()(Args& ...) = 0;
        }; // TODO Generation -> Population

        template<typename GeneType, typename ... Args>
        struct ConditionsForStoppingWrapper
        {
            virtual std::function<bool(const Types::Population<GeneType>&)> operator()(Args& ...) = 0;
        };

        template<typename GeneType, typename ... Args>
        struct PoolingPopulationsWrapper
        {
            virtual std::function<Types::Population<GeneType>(const Types::Population<GeneType>&, const Types::Population<GeneType>&)> operator()(Args& ...) = 0;
        };

        template<typename GeneType, typename ... Args>
        struct MutationFunctionWrapper
        {
            virtual std::function<void(Types::Generation<GeneType>&)> operator()(Args& ...) = 0;
        };

        template<typename GeneType, typename ... Args>
        struct CrossingoverFunctionWrapper
        {
            virtual std::function<void(Types::Generation<GeneType>&)> operator()(Args& ...) = 0;
        };

        template<typename GeneType, typename ResultType, typename ... Args>
        struct ResultFunctionWrapper
        {
            virtual std::function<ResultType(const Types::Population<GeneType>&)> operator()(Args& ...) = 0;
        };

        template<typename GeneType, typename ... Args>
        struct AnyFunctionWrapper
        {
            virtual std::function<void(const Types::Population<GeneType>&)> operator()(Args& ...) = 0;
        };

        template<typename GeneType, typename ... Args>
        struct EndNodeFunctionWrapper
        {
            virtual std::function<void()> operator()(Args& ...) = 0;
        };
        
        template<typename GeneType>
        using fitnessFunction = std::function<double(const Types::Chromosome<GeneType>&)>;

        template<typename GeneType>
        using proximityFunction = std::function<double(const Types::Chromosome<GeneType>&, const Types::Chromosome<GeneType>&)>;

        template<typename GeneType>
        using startGenerationFunction = std::function<Types::Generation<GeneType>()>;

        template<typename GeneType>
        using selectionFunction = std::function<void(Types::Generation<GeneType>&)>;

        template<typename GeneType>
        using poolingPopulations = std::function<Types::Population<GeneType>(const Types::Population<GeneType>&, const Types::Population<GeneType>&)>;

        template<typename GeneType>
        using conditionsForStoppingFunction = std::function<bool(const Types::Population<GeneType>&)>;

        template<typename GeneType>
        using mutationFunction = std::function<void(Types::Generation<GeneType>&)>;

        template<typename GeneType>
        using crossingoverFunction = std::function<void(Types::Generation<GeneType>&)>;

        template<typename GeneType, typename ResultType>
        using resultFunction = std::function<ResultType(const Types::Population<GeneType>&)>;

        template<typename GeneType>
        using anyFunction = std::function<void(const Types::Population<GeneType>&)>;

        template<typename GeneType>
        using endNode = std::function<void()>;

    } // end namespace Interfaces
} // end namespace GeneticAlgorithm