#pragma once

#include "types.hpp"

#include <functional>
#include <string>

namespace GeneticAlgorithm
{
    namespace LogInterfaces
    {
        template<typename GeneType, typename ... Args>
        struct NewGenerationLogWrapper
        {
            virtual std::function<void(const GeneticAlgorithm::Types::Generation<GeneType>&, const std::string& node_id)> operator()(Args...) const = 0;
        };

        template<typename GeneType, typename ... Args>
        struct StartNodeLogWrapper
        {
            virtual std::function<void(const GeneticAlgorithm::Types::Population<GeneType>&, const std::string& node_id)> operator()(Args...) const = 0;
        };

        template<typename GeneType, typename ... Args>
        struct EndNodeLogWrapper
        {
            virtual std::function<void(const GeneticAlgorithm::Types::Population<GeneType>&, const std::string& node_id)> operator()(Args...) const = 0;
        };


        template<typename GeneType>
        using newGenerationLog = std::function<void(const GeneticAlgorithm::Types::Generation<GeneType>&, const std::string& node_id)>;

        template<typename GeneType>
        using startNodeLog = std::function<void(const GeneticAlgorithm::Types::Population<GeneType>&, const std::string& node_id)>;
        
        template<typename GeneType>
        using endNodeLog = std::function<void(const GeneticAlgorithm::Types::Population<GeneType>&, const std::string& node_id)>;
    } 
    // namespace Loger
} 
// namespace GeneticAlgorithm