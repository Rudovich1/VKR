#pragma once

#include "types.hpp"

#include <functional>
#include <array>
#include <memory>

namespace GeneticAlgorithm
{
    namespace Interfaces
    {
        template<typename GeneType, typename ... Args>
        struct FitnessFunctionWrapper
        {
            virtual std::function<double(const Types::Chromosome<GeneType>&)> operator()(Args ...) const = 0;
        };

        template<typename GeneType, typename ... Args>
        struct StartPopulationFunctionWrapper
        {
            virtual std::function<Types::Population<GeneType>()> operator()(Args ...) const = 0;
        };

        template<typename GeneType, typename ... Args>
        struct SelectionFunctionWrapper
        {
            virtual std::function<Types::Generation<GeneType>(const Types::Generation<GeneType>&)> operator()(Args ...) const = 0;
        };

        template<typename GeneType, typename ... Args>
        struct ConditionsForStoppingWrapper
        {
            virtual std::function<bool(const Types::Population<GeneType>&)> operator()(Args ...) const = 0;
        };

        template<typename GeneType, size_t num_populations, typename ... Args>
        struct PoolingPopulationsWrapper
        {
            virtual std::function<Types::Population<GeneType>(const std::array<Types::Population<GeneType>, num_populations>&)> operator()(Args ...) const = 0;
        };

        template<typename GeneType, typename ... Args>
        struct MutationFunctionWrapper
        {
            virtual std::function<void(Types::Generation<GeneType>&)> operator()(Args ...) const = 0;
        };

        template<typename GeneType, typename ... Args>
        struct CrossingoverFunctionWrapper
        {
            virtual std::function<Types::Generation<GeneType>(Types::Population<GeneType>&)> operator()(Args ...) const = 0;
        };

        // template<typename GeneType, typename ResultType, typename ... Args>
        // struct ResultFunctionWrapper
        // {
        //     virtual std::function<ResultType(const Types::Population<GeneType>&)> operator()(Args& ...) const = 0;
        // };

        template<typename GeneType, typename ... Args>
        struct AnyFunctionWrapper
        {
            virtual std::function<void(const Types::Population<GeneType>&)> operator()(Args ...) const = 0;
        };

        template<typename GeneType, typename ... Args>
        struct EndNodeFunctionWrapper
        {
            virtual std::function<void()> operator()(Args ...) const = 0;
        };
        
        
        template<typename GeneType>
        using fitnessFunction = std::function<double(const Types::Chromosome<GeneType>&)>;

        template<typename GeneType>
        using startPopulationFunction = std::function<Types::Population<GeneType>()>;

        template<typename GeneType>
        using selectionFunction = std::function<Types::Generation<GeneType>(const Types::Generation<GeneType>&)>;

        template<typename GeneType, size_t num_populations>
        using poolingPopulations = std::function<Types::Population<GeneType>(const std::array<Types::Population<GeneType>, num_populations>&)>;

        template<typename GeneType>
        using conditionsForStoppingFunction = std::function<bool(const Types::Population<GeneType>&)>;

        template<typename GeneType>
        using mutationFunction = std::function<void(Types::Generation<GeneType>&)>;

        template<typename GeneType>
        using crossingoverFunction = std::function<Types::Generation<GeneType>(Types::Population<GeneType>&)>;

        // template<typename GeneType, typename ResultType>
        // using resultFunction = std::function<ResultType(const Types::Population<GeneType>&)>;

        template<typename GeneType>
        using anyFunction = std::function<void(const Types::Population<GeneType>&)>;

        template<typename GeneType>
        using endNode = std::function<void()>;

    } // end namespace Interfaces
} // end namespace GeneticAlgorithm