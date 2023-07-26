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
    }
}