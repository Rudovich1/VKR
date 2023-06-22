#pragma once

#include <vector>
#include <functional>
#include <iostream>

namespace GeneticAlgorithm
{
    template<typename GeneType>
    class Gene
    {
        GeneType data_;

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

    template<typename GeneType, typename ... Args>
    class Chromosome
    {
        using Gene_ = Gene<GeneType>;
        using Genes_ = std::vector<Gene_>;

        Genes_ genes_;
        double fitness_ = 0; 
        bool is_calc_ = false;
        Interfaces::FitnessFunction<GeneType, Args ...>& fitness_func_;

    public:

        Chromosome(size_t size);
        Chromosome(const Genes_& genes);
        Chromosome(Genes_&& genes);
        Chromosome(const Chromosome& chromosome);
        Chromosome(Chromosome&& chromosome);

        void operator=(const Chromosome& chromosome);
        void operator=(Chromosome&& chromosome);

        double getFitness(Args ... args);

        Gene_& operator[](size_t index)
        {
            return genes_[index];
        }

    };

    template<typename GeneType, typename ... Args>
    class Population
    {
        using Chromosome_ = Chromosome<GeneType>;
        using Chromosomes_ = std::vector<Chromosome_>;

        Chromosomes_ chromosomes_;

    };


    namespace Interfaces
    {
        template<typename GeneType, typename ... Args>
        struct FitnessFunction
        {
            virtual double operator()(const Chromosome<GeneType>&, Args ...) = 0;
        };

        template<typename GeneType, typename ... Args>
        struct ProximityFunction
        {
            virtual double operator()(const Chromosome<GeneType>&, const Chromosome<GeneType>&, Args ...) = 0;
        }; 
    }
}
