#pragma once

#include "evolution_tree.hpp"

#include <functional>
#include <thread>
#include <iostream>

namespace GeneticAlgorithm
{
    template<typename GeneType>
    class General
    {
    public:
        General(std::shared_ptr<EvolutionTree::Node<GeneType>> node, bool is_parallel, bool is_log);
        std::pair<Types::Chromosome<GeneType>, std::shared_ptr<NodeLog>> calc();

    private:
        bool is_parallel_;
        bool is_log_;
        std::shared_ptr<EvolutionTree::Node<GeneType>> node_;
    };
} // end namespace GeneticAlgorithm

namespace GeneticAlgorithm
{
    template<typename GeneType>
    General<GeneType>::General(std::shared_ptr<EvolutionTree::Node<GeneType>> node, bool is_parallel, bool is_log): 
        node_(std::move(node)), is_parallel_(is_parallel), is_log_(is_log) {}

    template<typename GeneType>
    std::pair<Types::Chromosome<GeneType>, std::shared_ptr<NodeLog>> General<GeneType>::calc()
    {
        Types::Population<GeneType> res_population;
        if (is_log_)
        {
            auto runtime_log = std::thread(runtimeLog, std::ref(node_->node_log_));
            res_population = node_->evolution(is_parallel_);
            runtime_log.join();
        }
        else
        {
            res_population = node_->evolution(is_parallel_);
        }
        std::shared_ptr<NodeLog> log = node_->node_log_;
        node_.reset();

        Types::Chromosome<GeneType>& best_res = res_population.get().back().get()[0];
        for (Types::Chromosome<GeneType>& chormosome: res_population.get().back().get())
        {
            if (chormosome.fitness() > best_res.fitness()) {best_res = chormosome;}
        }
        return std::make_pair(std::move(Types::Chromosome<GeneType>(best_res)), std::move(log));
    }
} // namespace GeneticAlgorithm