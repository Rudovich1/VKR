#pragma once

#include <vector>
#include <iostream>
#include <stdexcept>
#include <optional>
#include <utility>

#include "../tools/suffix.hpp"

namespace HeuristicAlgorithm
{
    namespace GeneticAlgorithm
    {
        namespace Types
        {
            template <class GeneType, class FitnessType>
            class Chromosome
            {
            public:
                using Genes_ = std::vector<GeneType>;
                using Chromosome_ = Chromosome<GeneType, FitnessType>;

                Chromosome() : genes_() {}
                Chromosome(const typename Genes_::allocator_type &__a) : genes_(__a) {}
                Chromosome(typename Genes_::size_type __n) : genes_(__n) {}
                Chromosome(typename Genes_::size_type __n, const typename Genes_::allocator_type &__a) : genes_(__n, __a) {}
                Chromosome(typename Genes_::size_type __n, const GeneType &__x) : genes_(__n, __x) {}
                Chromosome(typename Genes_::size_type __n, const GeneType &__x, const typename Genes_::allocator_type &__a) : genes_(__n, __x, __a) {}
                template <class _Iterator>
                Chromosome(_Iterator __first, _Iterator __last) : genes_(__first, __last) {}
                template <class _Iterator>
                Chromosome(_Iterator __first, _Iterator __last, const typename Genes_::allocator_type &__a) : genes_(__first, __last, __a) {}

                Chromosome(const Genes_ &genes) : genes_(genes) {}
                Chromosome_ &operator=(const Genes_ &genes)
                {
                    genes_ = genes;
                    fitness_.reset();
                    return *this;
                }
                Chromosome(const Chromosome_ &chromosome) : genes_(chromosome.genes_), fitness_(chromosome.fitness_) {}
                Chromosome_ &operator=(const Chromosome_ &chromosome)
                {
                    if (this != &chromosome)
                    {
                        genes_ = chromosome.genes_;
                        fitness_ = chromosome.fitness_;
                    }
                    return *this;
                }

                Chromosome(std::initializer_list<GeneType> __il) : genes_(__il) {}
                Chromosome(std::initializer_list<GeneType> __il, const typename Genes_::allocator_type &__a) : genes_(__il, __a) {}
                Chromosome_ &operator=(std::initializer_list<GeneType> __il)
                {
                    genes_ = __il;
                    fitness_.reset();
                    return *this;
                }

                Chromosome(Genes_ &&genes) : genes_(std::move(genes)) {}
                Chromosome_ &operator=(Genes_ &&genes)
                {
                    genes_ = std::move(genes);
                    fitness_.reset();
                    return *this;
                }
                Chromosome(Chromosome_ &&chromosome) : genes_(std::move(chromosome.genes_)), fitness_(std::move(chromosome.fitness_)) {}
                Chromosome_ &operator=(Chromosome_ &&chromosome)
                {
                    if (this != &chromosome)
                    {
                        genes_ = std::move(chromosome.genes_);
                        fitness_ = std::move(chromosome.fitness_);
                    }
                    return *this;
                }

                Genes_ &get()
                {
                    fitness_.reset();
                    return genes_;
                }
                const Genes_ &get() const { return genes_; }
                const Genes_ &cget() const { return std::as_const(genes_); }

                std::optional<FitnessType> &getFitness() { return fitness_; }
                const std::optional<FitnessType> &getFitness() const { return fitness_; }

            protected:
                Genes_ genes_;
                std::optional<FitnessType> fitness_;
            };

            template <class GeneType, class FitnessType, size_t num_genes>
            class StatChromosome
            {
            public:
                using Genes_ = std::array<GeneType, num_genes>;
                using StatChromosome_ = StatChromosome<GeneType, FitnessType, num_genes>;

                StatChromosome() : genes_({}) {}

                StatChromosome(const Genes_ &genes) : genes_(genes) {}
                StatChromosome_ &operator=(const Genes_ &genes)
                {
                    genes_ = genes;
                    fitness_.reset();
                    return *this;
                }
                StatChromosome(const StatChromosome_ &stat_chromosome) : genes_(stat_chromosome.genes_), fitness_(stat_chromosome.fitness_) {}
                StatChromosome_ &operator=(const StatChromosome_ &stat_chromosome)
                {
                    if (this != &stat_chromosome)
                    {
                        genes_ = stat_chromosome.genes_;
                        fitness_ = stat_chromosome.fitness_;
                    }
                    return *this;
                }

                StatChromosome(std::initializer_list<GeneType> __il) : genes_(__il) {}
                StatChromosome_ &operator=(std::initializer_list<GeneType> __il)
                {
                    genes_ = __il;
                    fitness_.reset();
                    return *this;
                }

                StatChromosome(Genes_ &&genes) : genes_(std::move(genes)) {}
                StatChromosome_ &operator=(Genes_ &&genes)
                {
                    genes_ = std::move(genes);
                    fitness_.reset();
                    return *this;
                }
                StatChromosome(StatChromosome_ &&stat_chromosome) : genes_(std::move(stat_chromosome.genes_)), fitness_(std::move(stat_chromosome.fitness_)) {}
                StatChromosome_ &operator=(StatChromosome &&stat_chromosome)
                {
                    if (this != &stat_chromosome)
                    {
                        genes_ = std::move(stat_chromosome.genes_);
                        fitness_ = std::move(stat_chromosome.fitness_);
                    }
                    return *this;
                }

                Genes_ &get()
                {
                    fitness_.reset();
                    return genes_;
                }
                const Genes_ &get() const { return genes_; }
                const Genes_ &cget() const { return std::as_const(genes_); }

                std::optional<FitnessType> &getFitness() { return fitness_; }
                const std::optional<FitnessType> &getFitness() const { return fitness_; }

            protected:
                Genes_ genes_;
                std::optional<FitnessType> fitness_;
            };

            template <class GeneType, class FitnessType>
            class Generation
            {
            public:
                using Chromosome_ = Chromosome<GeneType, FitnessType>;
                using Chromosomes_ = std::vector<Chromosome_>;
                using Generation_ = Generation<GeneType, FitnessType>;

                Generation() : chromosomes_() {}
                Generation(const typename Chromosomes_::allocator_type &__a) : chromosomes_(__a) {}
                Generation(typename Chromosomes_::size_type __n) : chromosomes_(__n) {}
                Generation(typename Chromosomes_::size_type __n, const typename Chromosomes_::allocator_type &__a) : chromosomes_(__n, __a) {}
                Generation(typename Chromosomes_::size_type __n, const Chromosome_ &__x) : chromosomes_(__n, __x) {}
                Generation(typename Chromosomes_::size_type __n, const Chromosome_ &__x, const typename Chromosomes_::allocator_type &__a) : chromosomes_(__n, __x, __a) {}
                template <class _Iterator>
                Generation(_Iterator __first, _Iterator __last) : chromosomes_(__first, __last) {}
                template <class _Iterator>
                Generation(_Iterator __first, _Iterator __last, const typename Chromosomes_::allocator_type &__a) : chromosomes_(__first, __last, __a) {}

                Generation(const Chromosomes_ &chromosomes) : chromosomes_(chromosomes) {}
                Generation_ &operator=(const Chromosomes_ &chromosomes)
                {
                    chromosomes_ = chromosomes;
                    return *this;
                }
                Generation(const Generation_ &generation) : chromosomes_(generation.chromosomes_) {}
                Generation_ &operator=(const Generation_ &generation)
                {
                    if (this != &generation)
                    {
                        chromosomes_ = generation.chromosomes_;
                    }
                    return *this;
                }

                Generation(std::initializer_list<Chromosome_> __il) : chromosomes_(__il) {}
                Generation(std::initializer_list<Chromosome_> __il, const typename Chromosomes_::allocator_type &__a) : chromosomes_(__il, __a) {}
                Generation_ &operator=(std::initializer_list<Chromosome_> __il)
                {
                    chromosomes_ = __il;
                    return *this;
                }

                Generation(Chromosomes_ &&chromosomes) : chromosomes_(std::move(chromosomes)) {}
                Generation_ &operator=(Chromosomes_ &&chromosomes)
                {
                    chromosomes_ = std::move(chromosomes);
                    return *this;
                }
                Generation(Generation_ &&generation) : chromosomes_(std::move(generation.chromosomes_)) {}
                Generation_ &operator=(Generation_ &&generation)
                {
                    if (this != &generation)
                    {
                        chromosomes_ = std::move(generation.chromosomes_);
                    }
                    return *this;
                }

                Chromosomes_ &get() { return chromosomes_; }
                const Chromosomes_ &get() const { return chromosomes_; }

            private:
                Chromosomes_ chromosomes_;
            };

            template <class GeneType, class FitnessType, size_t num_genes, size_t num_chromosomes>
            class StatGeneration
            {
            public:
                using StatChromosome_ = StatChromosome<GeneType, FitnessType, num_genes>;
                using StatChromosomes_ = std::array<StatChromosome_, num_chromosomes>;
                using StatGeneration_ = StatGeneration<GeneType, FitnessType, num_genes, num_chromosomes>;

                StatGeneration() = default;

                StatGeneration(const StatChromosomes_ &stat_chromosomes) : stat_chromosomes_(stat_chromosomes) {}
                StatGeneration_ &operator=(const StatChromosomes_ &stat_chromosomes)
                {
                    stat_chromosomes_ = stat_chromosomes;
                    return *this;
                }
                StatGeneration(const StatGeneration_ &stat_generation) : stat_chromosomes_(stat_generation.stat_chromosomes_) {}
                StatGeneration_ &operator=(const StatGeneration_ &stat_generation)
                {
                    if (this != &stat_generation)
                    {
                        stat_chromosomes_ = stat_generation.stat_chromosomes_;
                    }
                    return *this;
                }

                StatGeneration(std::initializer_list<StatChromosome_> __il) : stat_chromosomes_(__il) {}
                StatGeneration_ &operator=(std::initializer_list<StatChromosome_> __il)
                {
                    stat_chromosomes_ = __il;
                    return *this;
                }

                StatGeneration(StatChromosomes_ &&stat_chromosomes) : stat_chromosomes_(std::move(stat_chromosomes)) {}
                StatGeneration_ &operator=(StatChromosomes_ &&stat_chromosomes)
                {
                    stat_chromosomes_ = std::move(stat_chromosomes);
                    return *this;
                }
                StatGeneration(StatGeneration_ &&stat_generation) : stat_chromosomes_(std::move(stat_generation.stat_chromosomes_)) {}
                StatGeneration_ &operator=(StatGeneration_ &&stat_generation)
                {
                    if (this != &stat_generation)
                    {
                        stat_chromosomes_ = std::move(stat_generation.stat_chromosomes_);
                    }
                    return *this;
                }

                StatChromosomes_ &get() { return stat_chromosomes_; }
                const StatChromosomes_ &get() const { return stat_chromosomes_; }

            protected:
                StatChromosomes_ stat_chromosomes_;
            };

            template <class GeneType, class FitnessType>
            class Population
            {
            public:
                using Generation_ = Generation<GeneType, FitnessType>;
                using Generations_ = Suffix<Generation_>;
                using Population_ = Population<GeneType, FitnessType>;

                Population() : generations_() {}
                Population(size_t suffix_size) : generations_(suffix_size) {}

                Population(const Generations_ &generations) : generations_(generations) {}
                Population_ &operator=(const Generations_ &generations)
                {
                    generations_ = generations;
                    return *this;
                }
                Population(const Population_ &population) : generations_(population.generations_) {}
                Population_ &operator=(const Population_ &population)
                {
                    if (this != &population)
                    {
                        generations_ = population.generations_;
                    }
                    return *this;
                }

                Population(Generation_ &&generations) : generations_(std::move(generations)) {}
                Population_ &operator=(Generation_ &&generations)
                {
                    generations_ = std::move(generations);
                    return *this;
                }
                Population(Population_ &&population) : generations_(std::move(population.generations_)) {}
                Population_ &operator=(Population_ &&population)
                {
                    if (this != &population)
                    {
                        generations_ = std::move(population.generations_);
                    }
                    return *this;
                }

                Generations_ &get() { return generations_; }
                const Generations_ &get() const { return generations_; }

            protected:
                Generations_ generations_;
            };

            template <class GeneType, class FitnessType, size_t num_genes, size_t num_chromosomes, size_t suffix_size>
            class StatPopulation
            {
            public:
                using StatGeneration_ = StatGeneration<GeneType, FitnessType, num_genes, num_chromosomes>;
                using StatGenerations_ = StatSuffix<StatGeneration_, suffix_size>;
                using StatPopulation_ = StatPopulation<GeneType, FitnessType, num_genes, num_chromosomes, suffix_size>;

                StatPopulation() : stat_generations_() {}

                StatPopulation(const StatGenerations_ &stat_generations) : stat_generations_(stat_generations) {}
                StatPopulation_ &operator=(const StatGenerations_ &stat_generations)
                {
                    stat_generations_ = stat_generations;
                    return *this;
                }
                StatPopulation(const StatPopulation_ &stat_population) : stat_generations_(stat_population.stat_generations_) {}
                StatPopulation_ &operator=(const StatPopulation_ &stat_population)
                {
                    if (this != &stat_population)
                    {
                        stat_generations_ = stat_population.stat_generations_;
                    }
                    return *this;
                }

                StatPopulation(StatGenerations_ &&stat_generations) : stat_generations_(std::move(stat_generations)) {}
                StatPopulation_ &operator=(StatGenerations_ &&stat_generations) { stat_generations_ = std::move(stat_generations); }
                StatPopulation(StatPopulation_ &&stat_population) : stat_generations_(std::move(stat_population.stat_generations_)) {}
                StatPopulation_ &operator=(StatPopulation_ &&stat_population)
                {
                    if (this != &stat_population)
                    {
                        stat_generations_ = std::move(stat_population.stat_generations_);
                    }
                    return *this;
                }

            protected:
                StatGenerations_ stat_generations_;
            };
        } // end namespace Types
    } // end namespace GeneticAlgorithm
} // end namespace HeuristicAlgorithm