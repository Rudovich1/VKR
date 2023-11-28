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
        Types::Chromosome<GeneType> calc();

    private:
        std::unique_ptr<EvolutionTree::Node<GeneType>> node_;
    };
} // end namespace GeneticAlgorithm

namespace GeneticAlgorithm
{
    template<typename GeneType>
    General<GeneType>::General(std::unique_ptr<EvolutionTree::Node<GeneType>>&& node): node_(std::move(node)) {}

    template<typename GeneType>
    Types::Chromosome<GeneType> General<GeneType>::calc()
    {
        Types::Population<GeneType> res_population = node_->evolution();
        Types::Chromosome<GeneType>& best_res = res_population.get().back().get()[0];
        for (Types::Chromosome<GeneType>& chormosome: res_population.get().back().get())
        {
            if (chormosome.getFitness().getVal() > best_res.getFitness().getVal()) {best_res = chormosome;}
        }
        return best_res;
    }
} // namespace GeneticAlgorithm