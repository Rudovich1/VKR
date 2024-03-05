#pragma once

#include "evolution_tree.hpp"

#include <functional>

namespace GeneticAlgorithm
{
    template<typename GeneType>
    class General
    {
    public:
        General(std::unique_ptr<EvolutionTree::Node<GeneType>>&& node);
        std::pair<Types::Chromosome<GeneType>, NodeLog> calc();

    private:
        std::unique_ptr<EvolutionTree::Node<GeneType>> node_;
    };
} // end namespace GeneticAlgorithm

namespace GeneticAlgorithm
{
    template<typename GeneType>
    General<GeneType>::General(std::unique_ptr<EvolutionTree::Node<GeneType>>&& node): node_(std::move(node)) {}

    template<typename GeneType>
    std::pair<Types::Chromosome<GeneType>, NodeLog> General<GeneType>::calc()
    {
        Types::Population<GeneType> res_population = node_->evolution();
        NodeLog log = std::move(node_->node_log_);
        node_.reset();

        Types::Chromosome<GeneType>& best_res = res_population.get().back().get()[0];
        for (Types::Chromosome<GeneType>& chormosome: res_population.get().back().get())
        {
            if (chormosome.fitness() > best_res.fitness()) {best_res = chormosome;}
        }
        return std::make_pair(std::move(Types::Chromosome<GeneType>(best_res)), std::move(log));
    }
} // namespace GeneticAlgorithm