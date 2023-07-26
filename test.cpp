#include "genetic_method/genetic_algorithm.hpp"
#include <iostream>

using namespace GeneticAlgorithm;
using namespace Interfaces;

struct Tmp: public DataGetter<bool, bool>
{
    bool operator()(const Population<bool>& population) override
    {
        return true;
    }
};



int main()
{
    Population<bool> pop;
    Tmp tmp;
    std::function<bool()> fun = [&pop, &tmp](){return tmp(pop);};
}