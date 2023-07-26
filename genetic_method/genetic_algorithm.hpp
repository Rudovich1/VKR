#pragma once

#include <vector>
#include <functional>
#include <iostream>

#include "../tools/buffer_type.hpp"

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
    struct Conditions
    {
        std::function
        size_t population_size_;
        size_t chromosome_size_;
        size_t buffer_size_;

    };

    template<typename GeneType>
    class Population
    {
        using Generation_ = Generation<GeneType>;
        using Generations_ = BufferType<Generation_>;

        Generations_ generations_;

        public:

        Population();
        Population(const Generations_& generation);
        Population(Generation_&& generation);
        Population(const Population& population);
        Population(Population&& population);

        void operator=(const Population& population);
        void operator=(Population&& population);

        Generation_& operator[](size_t index);
    };

    namespace Interfaces
    {
        template<class Type>
        struct Tag
        {
            using type = Type; 
            constexpr Tag(){}
        };

        template<typename ResultType, typename ... Args>
        std::function<ResultType()> functionWrapper(typename Tag<std::function<ResultType(Args ...)>>::type fun, Args ... args)
        {
            return [fun, args ...](){return fun(args ...);};
        }

        template<typename GeneType, typename ReturnType>
        struct DataGetter
        {
            virtual ReturnType operator()(const Population<GeneType>& population) = 0;
        };

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

        template<typename ... Args>
        struct StartPopulationFunction
        {
            virtual void operator()(Args...) = 0;
        };

        template<typename ... Args>
        struct StopConditions
        {
            virtual bool operator() (Args...) = 0;
        };
    }


    template<typename GeneType, typename ... Args>
    class GeneticAlgorithm
    {
        
           
    };
}
