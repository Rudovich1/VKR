#include "types.hpp"

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
        Gene<GeneType>::Gene(const Gene<GeneType>::Gene_& gene): data_(gene.data_) {}

        template<typename GeneType>
        Gene<GeneType>::Gene(Gene<GeneType>::Gene_&& gene): data_(std::move(gene.data_)) {}

        template<typename GeneType>
        Gene<GeneType>::Gene_& Gene<GeneType>::operator=(const Gene<GeneType>::Gene_& gene)
        {
            if (this != &gene)
            {
                data_ = gene.data_;
            }
            return *this;
        }

        template<typename GeneType>
        Gene<GeneType>::Gene_& Gene<GeneType>::operator=(Gene<GeneType>::Gene_ &&gene)
        {
            if (this != &gene)
            {
                data_ = std::move(gene.data_);
            }
            return *this;
        }

        template<typename GeneType>
        bool Gene<GeneType>::operator==(const Gene<GeneType>::Gene_& gene) const
        {
            return data_ == gene.data_;
        }

        template<typename GeneType>
        bool Gene<GeneType>::operator!=(const Gene<GeneType>::Gene_& gene) const
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
        Chromosome<GeneType>::Chromosome(const Chromosome<GeneType>::Genes_& genes): genes_(genes), fitness_() {}

        template<typename GeneType>
        Chromosome<GeneType>::Chromosome(Chromosome<GeneType>::Genes_&& genes): genes_(genes), fitness_() {}

        template<typename GeneType>
        Chromosome<GeneType>::Chromosome(const Chromosome<GeneType>::Chromosome_& chromosome): genes_(chromosome.genes_), fitness_(chromosome.fitness_) {}

        template<typename GeneType>
        Chromosome<GeneType>::Chromosome(Chromosome<GeneType>::Chromosome_&& chromosome): genes_(std::move(chromosome.genes_)), fitness_(std::move(chromosome.fitness_)) {}

        template<typename GeneType>
        Chromosome<GeneType>::Chromosome_& Chromosome<GeneType>::operator=(const Chromosome<GeneType>::Chromosome_& chromosome)
        {
            if (this != &chromosome)
            {
                genes_ = chromosome.genes_;
                fitness_ = chromosome.fitness_;
            }
            return *this;
        }

        template<typename GeneType>
        Chromosome<GeneType>::Chromosome_& Chromosome<GeneType>::operator=(Chromosome<GeneType>::Chromosome_&& chromosome)
        {
            if (this != &chromosome)
            {
                genes_ = std::move(chromosome.genes_);
                fitness_ = std::move(chromosome.fitness_);
            }
            return *this;
        }

        template<typename GeneType>
        bool Chromosome<GeneType>::operator==(const Chromosome<GeneType>::Chromosome_& chromosome) const
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
        bool Chromosome<GeneType>::operator!=(const Chromosome<GeneType>::Chromosome_& chromosome) const
        {
            return !(*this == chromosome);
        }

        template<typename GeneType>
        Gene<GeneType>::Gene_& Chromosome<GeneType>::operator[](size_t index)
        {
            return genes_[index];
        }

        template<typename GeneType>
        const Gene<GeneType>::Gene_& Chromosome<GeneType>::operator[](size_t index) const
        {
            return genes_[index];
        }

        template<typename GeneType>
        Generation<GeneType>::Generation(size_t generation_size, size_t chromosome_size): chromosomes_(generation_size, chromosome_size) {}

        template<typename GeneType>
        Generation<GeneType>::Generation(const Generation<GeneType>::Chromosomes_& chromosomes): chromosomes_(chromosomes) {}

        template<typename GeneType>
        Generation<GeneType>::Generation(Generation<GeneType>::Chromosomes_&& chromosomes): chromosomes_(chromosomes) {}

        template<typename GeneType>
        Generation<GeneType>::Generation(const Generation<GeneType>::Generation_& generation): chromosomes_(generation.chromosomes_) {}

        template<typename GeneType>
        Generation<GeneType>::Generation(Generation<GeneType>::Generation_&& generation): chromosomes_(std::move(generation.chromosomes_)) {}
    
        template<typename GeneType>
        Generation<GeneType>::Generation_& Generation<GeneType>::operator=(const Generation<GeneType>::Generation_& generation)
        {
            if (this != &generation)
            {
                chromosomes_ = generation.chromosomes_;
            }
            return *this;
        }

        template<typename GeneType>
        Generation<GeneType>::Generation_& Generation<GeneType>::operator=(Generation<GeneType>::Generation_&& generation)
        {
            if (this != &generation)
            {
                chromosomes_ = std::move(generation.chromosomes_);
            }
            return *this;
        }

        template<typename GeneType>
        bool Generation<GeneType>::operator==(const Generation<GeneType>::Generation_& generation) const
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
        bool Generation<GeneType>::operator!=(const Generation<GeneType>::Generation_& generation) const
        {
            return !(*this == generation);
        }

        template<typename GeneType>
        Chromosome<GeneType>::Chromosome_& Generation<GeneType>::operator[](size_t index)
        {
            return chromosomes_[i];
        }

        template<typename GeneType>
        const Chromosome<GeneType>::Chromosome_& Generation<GeneType>::operator[](size_t index) const
        {
            return chromosomes_[i];
        }

        template<typename GeneType>
        Population<GeneType>::Population(size_t buffer_size): generations_(buffer_size) {}

        template<typename GeneType>
        Population<GeneType>::Population(const Population<GeneType>::Generations_& generations): generations_(generations) {}

        template<typename GeneType>
        Population<GeneType>::Population(Population<GeneType>::Generations_&& generations): generations_(std::move(generations)) {}

        template<typename GeneType>
        Population<GeneType>::Population(const Population<GeneType>::Population_& population): generations_(population.generations_) {}
    
        template<typename GeneType>
        Population<GeneType>::Population(Population<GeneType>::Population_&& population): generations_(std::move(population.generations_)) {}

        template<typename GeneType>
        Generation<GeneType>::Generation_& Population<GeneType>::operator[](size_t index)
        {
            return generations_[index];
        }

        template<typename GeneType>
        const Generation<GeneType>::Generation_& Population<GeneType>::operator[](size_t index) const
        {
            return generations_[index];
        }

        template<typename GeneType>
        void Population<GeneType>::add(const Generation<GeneType>::Generation_& generation)
        {
            generations_.add(generation);
        }

        template<typename GeneType>
        void Population<GeneType>::add(Generation<GeneType>::Generation_&& generation)
        {
            generations_.add(generation);
        }

        template<typename GeneType>
        Generation<GeneType>::Generation_& Population<GeneType>::back()
        {
            return generations_.back();
        }

        template<typename GeneType>
        const Generation<GeneType>::Generation_& Population<GeneType>::back() const
        {
            return generations_.back();
        }

    } // end namespace Types
} // end namespace GeneticAlgorithm