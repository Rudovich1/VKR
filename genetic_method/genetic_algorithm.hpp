#pragma once

#include <vector>
#include <functional>
#include <initializer_list> 

template<typename Type>
class Gene
{
    Type data_;

public:

    Gene(){}
    Gene(const Type& data): data_(data) {}
    Gene(Type&& data): data_(data) {}
    Gene(const Gene& gene): data_(gene.data_) {}
    Gene(Gene&& gene): data_(std::move(gene.data_)) {} 

    void operator=(const Gene& gene) {data_ = gene.data_;}
    void operator=(Gene&& gene) {data_ = std::move(gene.data_);}
    void operator=(const Type& data) {data_ = data;}
    void operator=(Type&& data) {data_ = data;}

    Type& get() const {return data_;}
};

template<typename Type>
class Chromosome
{
    using _Gene = Gene<Type>;
    using _Genes = std::vector<_Gene>;

    _Genes genes_;

public:

    Chromosome(){}
    Chromosome(size_t size): genes_(size) {}
    Chromosome(size_t size, const _Gene& gene): genes_(size, gene) {}
    Chromosome(size_t size, _Gene&& gene): genes_(size, gene) {}
    Chromosome(std::initializer_list<Type> init_list)
    {
        for(auto& i: init_list)
        {
            genes_.emplace_back(Gene(i));
        }
    }
    Chromosome(std::initializer_list<_Gene> init_list): genes_(init_list) {}
    Chromosome(const _Genes& genes): genes_(genes) {}
    Chromosome(_Genes&& genes): genes_(genes) {}
    Chromosome(const Chromosome& chromosome): genes_(chromosome.genes_) {}
    Chromosome(Chromosome&& chromosome): genes_(std::move(chromosome.genes_)) {}

    void operator=(std::initializer_list<Type> init_list) {genes_ = init_list;}
    void operator=(std::initializer_list<_Gene> init_list) {genes_ = init_list;}
    void operator=(const _Genes& genes) {genes_ = genes;}
    void operator=(_Genes&& genes) {genes_ = genes;}
    void operator=(const Chromosome& chromosome) {genes_ = chromosome.genes_;}
    void operator=(Chromosome&& chromosome) {genes_ = std::move(chromosome.genes_);}

    size_t size() const {return genes_.size();}

    typename _Genes::iterator begin() {return genes_.begin();}
    typename _Genes::iterator end() {return genes_.end();}

    _Gene& operator[] (size_t index) const {return genes_[index];}
};

template<typename Type>
class Population
{
    using _Chromosome = Chromosome<Type>;
    using _Chromosomes = std::vector<_Chromosome>;

    _Chromosomes chromosomes_;

public:

    Population(){}
    Population(size_t size, const _Chromosome& chromosome): chromosomes_(size, chromosome) {}
    Population(size_t size, _Chromosome&& chromosome): chromosomes_(size, chromosome) {}
    Population(std::initializer_list<std::initializer_list<Type>> init_list) 
    {
        for(auto& i: init_list)
        {
            chromosomes_.emplace_back(_Chromosome(i));
        }
    }
    Population(std::initializer_list<_Chromosome> init_list): chromosomes_(init_list) {}
    Population(const _Chromosomes& chromosomes): chromosomes_(chromosomes) {}
    Population(_Chromosomes&& chromosomes): chromosomes_(chromosomes) {}
    Population(const Population& populations): chromosomes_(populations.chromosomes_) {}
    Population(Population&& population): chromosomes_(std::move(population.chromosomes_)) {}

    void operator=(std::initializer_list<std::initializer_list<Type>> init_list) {chromosomes_ = init_list;}
    void operator=(std::initializer_list<_Chromosome> init_list) {chromosomes_ = init_list;}
    void operator=(const _Chromosomes& chromosomes) {chromosomes_ = chromosomes;}
    void operator=(_Chromosomes&& chromosomes) {chromosomes_ = chromosomes;}
    void operator=(const Population& population) {chromosomes_ = population.chromosomes_;}
    void operator=(Population&& population) {chromosomes_ = std::move(population.chromosomes_);}

    size_t size() const {return chromosomes_.size()};

    _Chromosomes::iterator begin() {return chromosomes_.begin();}
    _Chromosomes::iterator end() {return chromosomes_.end();}

    _Chromosome& operator[] (size_t index) const {return chromosomes_[index];}
};


template<typename Type, typename ... Types>
struct FixtureFunction
{
    virtual double operator()(const Chromosome<Type>&, Types ...) = 0;
};



// class GeneticMethod
// {
//     bool q = false;
    
// };


int main()
{
    Gene gene = true;
    Chromosome chromosome = {gene, gene};
    Chromosome chromosome2 = {Gene(false), Gene(true)};
    Chromosome chromosome3 = {false, true};
    Population population = {chromosome, chromosome};
    Population population2 = {Chromosome{false, false}, Chromosome{true, true}};
    Population population3 = {{false, false}, {true, true}};

}
