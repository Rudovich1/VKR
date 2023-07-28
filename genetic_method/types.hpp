#pragma once

#include <vector>
#include <functional>
#include <iostream>

#include "../tools/buffer_type.hpp"

namespace GeneticAlgorithm
{
    namespace Types
    {
        template<typename GeneType>
        class Gene
        {
            GeneType gene_;

        public:

            Gene();
            Gene(const GeneType& data);
            Gene(GeneType&& data);
            Gene(const Gene& gene);
            Gene(Gene&& gene);

            void operator=(const Gene& gene);
            void operator=(Gene&& gene);

            GeneType& get();
        };

        template<typename GeneType>
        class Chromosome
        {
            using Gene_ = Gene<GeneType>;
            using Genes_ = std::vector<Gene_>;

            Genes_ genes_;

        public:

            Chromosome(size_t size);
            Chromosome(const Genes_& genes);
            Chromosome(Genes_&& genes);
            Chromosome(const Chromosome& chromosome);
            Chromosome(Chromosome&& chromosome);

            void operator=(const Chromosome& chromosome);
            void operator=(Chromosome&& chromosome);

            Gene_& operator[](size_t index);

        };

        template<typename GeneType>
        class Generation
        {
            using Chromosome_ = Chromosome<GeneType>;
            using Chromosomes_ = std::vector<Chromosome_>;

            Chromosomes_ chromosomes_;

        public:

            Generation(size_t generation_size, size_t chromosome_size);
            Generation(const Chromosomes_& chromosomes);
            Generation(Chromosomes_&& chromosomes);
            Generation(const Generation& generation);
            Generation(Generation&& generation);

            void operator=(const Generation& generation);
            void operator=(Generation&& generation);

            Chromosome_& operator[](size_t index);
        };

        template<typename GeneType>
        class Population
        {
            using Generation_ = Generation<GeneType>;
            using Generations_ = BufferType<Generation_>;

            Generations_ generations_;

            public:

            Population(size_t generation_size, size_t chromosome_size, size_t buffer_size);
            Population(const Generations_& generation);
            Population(Generation_&& generation);
            Population(const Population& population);
            Population(Population&& population);

            void operator=(const Population& population);
            void operator=(Population&& population);

            Generation_& operator[](size_t index);
        };

    } // end namespace Types

} // end namespace GeneticAlgorithm
