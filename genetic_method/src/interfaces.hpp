#pragma once

#include "types.hpp"

#include <array>
#include <vector>
#include <string>


namespace GeneticAlgorithm
{
    namespace Interfaces
    {
        using namespace Types;

        template<class ReturnType, class ... Args>
        struct Wrapper {virtual ReturnType operator()(Args...) const = 0;};



        template<class GeneType, class FitnessType>
        struct FitnessFunctionWrapper: public Wrapper<FitnessType, const Chromosome<GeneType, FitnessType>&> {};

        template<class GeneType, class FitnessType>
        struct SelectionWrapper: public Wrapper<void, Generation<GeneType, FitnessType>&> {};

        template<class GeneType, class FitnessType>
        struct ConditionsForStoppingWrapper: public Wrapper<bool, const Population<GeneType, FitnessType>&> {};

        template<class GeneType, class FitnessType>
        struct MutationWrapper: public Wrapper<void, Generation<GeneType, FitnessType>&> {};

        template<class GeneType, class FitnessType>
        struct CrossingoverWrapper: public Wrapper<Generation<GeneType, FitnessType>, const Population<GeneType, FitnessType>&> {};

        template<class GeneType, class FitnessType>
        struct AnyFunctionWrapper: public Wrapper<void, Population<GeneType, FitnessType>&> {};


        template<class GeneType, class FitnessType>
        struct StartPopulationWrapper: public Wrapper<Population<GeneType, FitnessType>> {};


        template<class GeneType, class FitnessType>
        struct PoolingPopulationsWrapper: 
            public Wrapper<Population<GeneType, FitnessType>, std::vector<Population<GeneType, FitnessType>>&> {};


        template<class GeneType, class FitnessType>
        struct NewGenerationLogWrapper: public Wrapper<void, const Generation<GeneType, FitnessType>&, const std::string&> {};

        template<class GeneType, class FitnessType>
        struct StartNodeLogWrapper: public Wrapper<void, const Population<GeneType, FitnessType>&, const std::string&> {};

        template<class GeneType, class FitnessType>
        struct EndNodeLogWrapper: public Wrapper<void, const Population<GeneType, FitnessType>&, const std::string&> {};



        template<class GeneType, class FitnessType, size_t num_genes>
        struct StatFitnessFunctionWrapper: public Wrapper<FitnessType, const StatChromosome<GeneType, FitnessType, num_genes>&> {};

        template<class GeneType, class FitnessType, size_t num_genes, size_t num_chromosomes>
        struct StatSelectionWrapper: public Wrapper<void, StatGeneration<GeneType, FitnessType, num_genes, num_chromosomes>&> {};

        template<class GeneType, class FitnessType, size_t num_genes, size_t num_chromosomes, size_t suffix_size>
        struct StatConditionsForStoppingWrapper: 
            public Wrapper<bool, const StatPopulation<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>&> {};

        template<class GeneType, class FitnessType, size_t num_genes, size_t num_chromosomes>
        struct StatMutationWrapper: public Wrapper<void, StatGeneration<GeneType, FitnessType, num_genes, num_chromosomes>&> {};

        template<class GeneType, class FitnessType, size_t num_genes, size_t num_chromosomes, size_t suffix_size>
        struct StatCrossingoverWrapper: 
            public Wrapper<StatGeneration<GeneType, FitnessType, num_genes, num_chromosomes>, 
                const StatPopulation<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>&> {};

        template<class GeneType, class FitnessType, size_t num_genes, size_t num_chromosomes, size_t suffix_size>
        struct StatAnyFunctionWrapper: 
            public Wrapper<void, StatPopulation<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>&> {};


        template<class GeneType, class FitnessType, size_t num_genes, size_t num_chromosomes, size_t suffix_size>
        struct StatStartPopulationWrapper: 
            public Wrapper<StatPopulation<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>> {};


        template<class GeneType, class FitnessType, size_t num_genes, size_t num_chromosomes, size_t suffix_size, size_t num_populations>
        struct PoolingPopulationsWrapper: 
            public Wrapper<StatPopulation<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>, 
                std::array<StatPopulation<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>, num_populations>&> {};


        template<class GeneType, class FitnessType, size_t num_genes, size_t num_chromosomes>
        struct StatNewGenerationLogWrapper: 
            public Wrapper<void, const StatGeneration<GeneType, FitnessType, num_genes, num_chromosomes>&, const std::string&> {};

        template<class GeneType, class FitnessType, size_t num_genes, size_t num_chromosomes, size_t suffix_size>
        struct StatStartNodeLogWrapper: 
            public Wrapper<void, const StatPopulation<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>&, const std::string&> {};

        template<class GeneType, class FitnessType, size_t num_genes, size_t num_chromosomes, size_t suffix_size>
        struct StatEndNodeLogWrapper: 
            public Wrapper<void, const StatPopulation<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>&, const std::string&> {};



        struct EndNodeFunctionWrapper: public Wrapper<void> {};
        struct StartNodeFunctionWrapper: public Wrapper<void> {};


    } // end namespace Interfaces
} // end namespace GeneticAlgorithm