#pragma once

#include <vector>
#include <functional>
#include <iostream>
#include <exception>

#include "../tools/buffer_type.hpp"
#include "../tools/nullable_type.hpp"

namespace GeneticAlgorithm
{
    namespace Types
    {
        template<typename GeneType>
        class Gene
        {
        public:
            using Gene_ = Gene<GeneType>;

            Gene();
            Gene(const GeneType& data);
            Gene(GeneType&& data);
            Gene(const Gene_& gene);
            Gene(Gene_&& gene);

            Gene_& operator=(const Gene_& gene);
            Gene_& operator=(Gene_&& gene);

            bool operator==(const Gene_& gene) const;
            bool operator!=(const Gene_& gene) const;

            GeneType& get();
            const GeneType& get() const;

        private:
            GeneType data_;
        };

        template<typename GeneType>
        class Chromosome
        {
        public:
            using Genes_ = std::vector<Gene<GeneType>::Gene_>;
            using Chromosome_ = Chromosome<GeneType>;

            Chromosome(size_t size);
            Chromosome(const Genes_& genes);
            Chromosome(Genes_&& genes);
            Chromosome(const Chromosome_& chromosome);
            Chromosome(Chromosome_&& chromosome);

            Chromosome_& operator=(const Chromosome_& chromosome);
            Chromosome_& operator=(Chromosome_&& chromosome);

            bool operator==(const Chromosome_& chromosome) const;
            bool operator!=(const Chromosome_& chromosome) const;

            Gene<GeneType>::Gene_& operator[](size_t index);
            const Gene<GeneType>::Gene_& operator[](size_t index) const;

        private:
            Genes_ genes_;
            NullableType<double> fitness_;
        };

        template<typename GeneType>
        class Generation
        {
        public:
            using Chromosomes_ = std::vector<Chromosome<GeneType>::Chromosome_>;
            using Generation_ = Generation<GeneType>;

            Generation(size_t generation_size, size_t chromosome_size);
            Generation(const Chromosomes_& chromosomes);
            Generation(Chromosomes_&& chromosomes);
            Generation(const Generation_& generation);
            Generation(Generation_&& generation);

            Generation_& operator=(const Generation_& generation);
            Generation_& operator=(Generation_&& generation);
            
            bool operator==(const Generation_& generation) const;
            bool operator!=(const Generation_& generation) const;

            Chromosome<GeneType>::Chromosome_& operator[](size_t index);
            const Chromosome<GeneType>::Chromosome_& operator[](size_t index) const;

        private:
            Chromosomes_ chromosomes_;
        };

        template<typename GeneType>
        class Population
        {
        public:
            using Generations_ = BufferType<Generation<GeneType>::Generation_>;
            using Population_ = Population<GeneType>;

            Population(size_t buffer_size);
            Population(const Generations_& generations);
            Population(Generations_&& generations);
            Population(const Population_& population);
            Population(Population_&& population);

            Generation<GeneType>::Generation_& operator[](size_t index);
            const Generation<GeneType>::Generation_& operator[](size_t index) const;

            void add(const Generation<GeneType>::Generation_& generation);
            void add(Generation<GeneType>::Generation_&& generation);

            Generation<GeneType>::Generation_& back();
            const Generation<GeneType>::Generation_& back() const;

        private:
            Generations_ generations_;
        };
    } // end namespace Types
} // end namespace GeneticAlgorithm
