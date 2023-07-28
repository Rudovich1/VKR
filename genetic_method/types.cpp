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
        Gene<GeneType>::Gene(const Gene& gene): data_(gene.data_) {}

        template<typename GeneType>
        Gene<GeneType>::Gene(Gene&& gene): data_(std::move(gene.data_)) {}

        template<typename GeneType>
        void Gene<GeneType>::operator=(const Gene& gene)
        {
            data_ = gene.data_;
        }

        template<typename GeneType>
        void Gene<GeneType>::operator=(Gene &&gene)
        {
            data_ = std::move(gene.data_);
        }

        template<typename GeneType>
        GeneType& Gene<GeneType>::get()
        {
            return data_;
        }

        template<typename GeneType>
        Chromosome<GeneType>::Chromosome(size_t size): genes_(size) {}

        template<typename GeneType>
        Chromosome<GeneType>::Chromosome(const Genes_& genes): genes_(genes) {}

        template<typename GeneType>
        Chromosome<GeneType>::Chromosome(Genes_&& genes): genes_(genes) {}

        template<typename GeneType>
        Chromosome<GeneType>::Chromosome(const Chromosome& chromosome): genes_(chromosome.genes_) {}

        template<typename GeneType>
        Chromosome<GeneType>::Chromosome(Chromosome&& chromosome): genes_(std::move(chromosome.genes_)) {}

        template<typename GeneType>
        void Chromosome<GeneType>::operator=(const Chromosome& chromosome)
        {
             genes_ = chromosome.genes_;
        }

        template<typename GeneType>
        void Chromosome<GeneType>::operator=(Chromosome&& chromosome)
        {
            genes_ = std::move(chromosome.genes_);
        }

        template<typename GeneType>
        Types::Chromosome<GeneType>::Gene_& Chromosome<GeneType>::operator[](size_t index)
        {
            return genes_[index];
        }

        template<typename GeneType>
        Generation<GeneType>::Generation(size_t generation_size, size_t chromosome_size): 
            chromosomes_(generation_size, chromosome_size) {}

    }
}