#pragma once

#include <vector>
#include <functional>
#include <iostream>
#include <exception>

#include "tools/buffer_type.hpp"
#include "tools/nullable_type.hpp"

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
            using Genes_ = std::vector<Gene<GeneType>>;
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

            Genes_& get();
            const Genes_& get() const;

            NullableType<double>& getFitness();
            const NullableType<double>& getFitness() const;

        private:
            Genes_ genes_;
            NullableType<double> fitness_;
        };

        template<typename GeneType>
        class Generation
        {
        public:
            using Chromosomes_ = std::vector<Chromosome<GeneType>>;
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

            Chromosomes_& get();
            const Chromosomes_& get() const;

        private:
            Chromosomes_ chromosomes_;
        };

        template<typename GeneType>
        class Population
        {
        public:
            using Generations_ = BufferType<Generation<GeneType>>;
            using Population_ = Population<GeneType>;

            Population(size_t buffer_size);
            Population(const Generations_& generations);
            Population(Generations_&& generations);
            Population(const Population_& population);
            Population(Population_&& population);

            Generations_& get();
            const Generations_& get() const;

            Population& add(const Generation<GeneType>& generation);
            Population& add(Generation<GeneType>&& generation);

        private:
            Generations_ generations_;
        };        
    } // end namespace Types
} // end namespace GeneticAlgorithm

namespace GeneticAlgorithm
{
    namespace Types
    {
        template<typename GeneType>
        Gene<GeneType>::Gene(): data_() {}

        template<typename GeneType>
        Gene<GeneType>::Gene(const GeneType &data): data_(data) {}

        template<typename GeneType>
        Gene<GeneType>::Gene(GeneType &&data): data_(data) {}

        template<typename GeneType>
        Gene<GeneType>::Gene(const typename Gene<GeneType>::Gene_& gene): data_(gene.data_) {}

        template<typename GeneType>
        Gene<GeneType>::Gene(typename Gene<GeneType>::Gene_&& gene): data_(std::move(gene.data_)) {}

        template<typename GeneType>
        typename Gene<GeneType>::Gene_& Gene<GeneType>::operator=(const typename Gene<GeneType>::Gene_& gene)
        {
            if (this != &gene)
            {
                data_ = gene.data_;
            }
            return *this;
        }

        template<typename GeneType>
        typename Gene<GeneType>::Gene_& Gene<GeneType>::operator=(typename Gene<GeneType>::Gene_ &&gene)
        {
            if (this != &gene)
            {
                data_ = std::move(gene.data_);
            }
            return *this;
        }

        template<typename GeneType>
        bool Gene<GeneType>::operator==(const typename Gene<GeneType>::Gene_& gene) const
        {
            return data_ == gene.data_;
        }

        template<typename GeneType>
        bool Gene<GeneType>::operator!=(const typename Gene<GeneType>::Gene_& gene) const
        {
            return data_ != gene.data_;
        }

        template<typename GeneType>
        GeneType& Gene<GeneType>::get()
        {
            return data_;
        }

        template<typename GeneType>
        const GeneType& Gene<GeneType>::get() const
        {
            return data_;
        }

        template<typename GeneType>
        Chromosome<GeneType>::Chromosome(size_t size): genes_(size), fitness_() {}

        template<typename GeneType>
        Chromosome<GeneType>::Chromosome(const typename Chromosome<GeneType>::Genes_& genes): genes_(genes), fitness_() {}

        template<typename GeneType>
        Chromosome<GeneType>::Chromosome(typename Chromosome<GeneType>::Genes_&& genes): genes_(genes), fitness_() {}

        template<typename GeneType>
        Chromosome<GeneType>::Chromosome(const typename Chromosome<GeneType>::Chromosome_& chromosome): genes_(chromosome.genes_), fitness_(chromosome.fitness_) {}

        template<typename GeneType>
        Chromosome<GeneType>::Chromosome(typename Chromosome<GeneType>::Chromosome_&& chromosome): genes_(std::move(chromosome.genes_)), fitness_(std::move(chromosome.fitness_)) {}

        template<typename GeneType>
        typename Chromosome<GeneType>::Chromosome_& Chromosome<GeneType>::operator=(const typename Chromosome<GeneType>::Chromosome_& chromosome)
        {
            if (this != &chromosome)
            {
                genes_ = chromosome.genes_;
                fitness_ = chromosome.fitness_;
            }
            return *this;
        }

        template<typename GeneType>
        typename Chromosome<GeneType>::Chromosome_& Chromosome<GeneType>::operator=(typename Chromosome<GeneType>::Chromosome_&& chromosome)
        {
            if (this != &chromosome)
            {
                genes_ = std::move(chromosome.genes_);
                fitness_ = std::move(chromosome.fitness_);
            }
            return *this;
        }

        template<typename GeneType>
        bool Chromosome<GeneType>::operator==(const typename Chromosome<GeneType>::Chromosome_& chromosome) const
        {
            if (genes_.size() != chromosome.genes_.size())
            {
                // TODO throw ....
                throw std::exception();
            }
            for (size_t i = 0, size = genes_.size(); i < size; ++i)
            {
                if (genes_[i] != chromosome.genes_[i])
                {
                    return false;
                }
            }
            return true;
        }

        template<typename GeneType>
        bool Chromosome<GeneType>::operator!=(const typename Chromosome<GeneType>::Chromosome_& chromosome) const
        {
            return !(*this == chromosome);
        }

        template<typename GeneType>
        typename Chromosome<GeneType>::Genes_& Chromosome<GeneType>::get()
        {
            fitness_.makeNull();
            return genes_;
        }

        template<typename GeneType>
        const typename Chromosome<GeneType>::Genes_& Chromosome<GeneType>::get() const
        {
            return genes_;
        }

        template<>
        const typename Chromosome<bool>::Genes_& Chromosome<bool>::get() const
        {
            return genes_;
        }

        template<typename GeneType>
        NullableType<double>& Chromosome<GeneType>::getFitness()
        {
            return fitness_;
        }

        template<typename GeneType>
        const NullableType<double>& Chromosome<GeneType>::getFitness() const
        {
            return fitness_;
        }

        template<typename GeneType>
        Generation<GeneType>::Generation(size_t generation_size, size_t chromosome_size): chromosomes_(generation_size, chromosome_size) {}

        template<typename GeneType>
        Generation<GeneType>::Generation(const typename Generation<GeneType>::Chromosomes_& chromosomes): chromosomes_(chromosomes) {}

        template<typename GeneType>
        Generation<GeneType>::Generation(typename Generation<GeneType>::Chromosomes_&& chromosomes): chromosomes_(chromosomes) {}

        template<typename GeneType>
        Generation<GeneType>::Generation(const typename Generation<GeneType>::Generation_& generation): chromosomes_(generation.chromosomes_) {}

        template<typename GeneType>
        Generation<GeneType>::Generation(typename Generation<GeneType>::Generation_&& generation): chromosomes_(std::move(generation.chromosomes_)) {}
    
        template<typename GeneType>
        typename Generation<GeneType>::Generation_& Generation<GeneType>::operator=(const typename Generation<GeneType>::Generation_& generation)
        {
            if (this != &generation)
            {
                chromosomes_ = generation.chromosomes_;
            }
            return *this;
        }

        template<typename GeneType>
        typename Generation<GeneType>::Generation_& Generation<GeneType>::operator=(typename Generation<GeneType>::Generation_&& generation)
        {
            if (this != &generation)
            {
                chromosomes_ = std::move(generation.chromosomes_);
            }
            return *this;
        }

        template<typename GeneType>
        bool Generation<GeneType>::operator==(const typename Generation<GeneType>::Generation_& generation) const
        {
            if (chromosomes_.size() != generation.chromosomes_.size())
            {
                // TODO throw ....
            }
            for (size_t i = 0, size = chromosomes_.size(); i < size; ++i)
            {
                if (chromosomes_[i] != generation.chromosomes_[i])
                {
                    return false;
                }
            }
            return true;
        }

        template<typename GeneType>
        bool Generation<GeneType>::operator!=(const typename Generation<GeneType>::Generation_& generation) const
        {
            return !(*this == generation);
        }

        template<typename GeneType>
        typename Generation<GeneType>::Chromosomes_& Generation<GeneType>::get()
        {
            return chromosomes_;
        }

        template<typename GeneType>
        const typename Generation<GeneType>::Chromosomes_& Generation<GeneType>::get() const
        {
            return chromosomes_;
        }

        template<typename GeneType>
        Population<GeneType>::Population(size_t buffer_size): generations_(buffer_size) {}

        template<typename GeneType>
        Population<GeneType>::Population(const typename Population<GeneType>::Generations_& generations): generations_(generations) {}

        template<typename GeneType>
        Population<GeneType>::Population(typename Population<GeneType>::Generations_&& generations): generations_(std::move(generations)) {}

        template<typename GeneType>
        Population<GeneType>::Population(const typename Population<GeneType>::Population_& population): generations_(population.generations_) {}
    
        template<typename GeneType>
        Population<GeneType>::Population(typename Population<GeneType>::Population_&& population): generations_(std::move(population.generations_)) {}

        template<typename GeneType>
        typename Population<GeneType>::Generations_& Population<GeneType>::get()
        {
            return generations_;
        }

        template<typename GeneType>
        const typename Population<GeneType>::Generations_& Population<GeneType>::get() const
        {
            return generations_;
        }

        template<typename GeneType>
        Population<GeneType>& Population<GeneType>::add(const Generation<GeneType>& generation)
        {
            generations_.add(generation);
            return *this;
        }

        template<typename GeneType>
        Population<GeneType>& Population<GeneType>::add(Generation<GeneType>&& generation)
        {
            generations_.add(generation);
            return *this;
        }
    } // end namespace Types
} // end namespace GeneticAlgorithm