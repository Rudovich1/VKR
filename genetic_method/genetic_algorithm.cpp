#include "genetic_algorithm.hpp"

struct GeneInit: public GeneticAlgorithm::Interfaces::GeneInitFunction<bool>
{
    bool operator()() override
    {
        return false;
    }
};

int main()
{
    GeneticAlgorithm::Gene<bool> gene(GeneInit);

    
}