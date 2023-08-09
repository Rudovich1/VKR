#include "evolution_tree.hpp"

namespace GeneticAlgorithm
{
    namespace Data
    {
        using namespace Interfaces;

        template<typename GeneType>
        BaseData<GeneType>::BaseData(
            fitnessFunction<GeneType> fitnessFunction,
            proximityFunction<GeneType> proximityFunction,
            selectionFunction<GeneType> selectionFunction,
            mutationFunction<GeneType> mutationFunction,
            crossingoverFunction<GeneType> crossingoverFunction,
            conditionsForStoppingFunction<GeneType> conditionsForStoppingFunction,
            size_t buffer_size, size_t generation_size, size_t chromosome_size):
            fitnessFunction_(fitnessFunction), proximityFunction_(proximityFunction), selectionFunction_(selectionFunction), 
            mutationFunction_(mutationFunction), crossingoverFunction_(crossingoverFunction), conditionsForStoppingFunction_(conditionsForStoppingFunction),
            buffer_size_(buffer_size), generation_size_(generation_size), chromosome_size_(chromosome_size) {}

        template<typename GeneType>
        BaseData<GeneType>::BaseData(const BaseData<GeneType>& base_data):
            fitnessFunction_(base_data.fitnessFunction_), proximityFunction_(base_data.proximityFunction_),selectionFunction_(base_data.selectionFunction_), 
            mutationFunction_(base_data.mutationFunction_), crossingoverFunction_(base_data.crossingoverFunction_), conditionsForStoppingFunction_(base_data.conditionsForStoppingFunction_),
            buffer_size_(base_data.buffer_size_), generation_size_(base_data.generation_size_), chromosome_size_(base_data.chromosome_size_) {}

        template<typename GeneType>
        Conditions<GeneType>::Conditions(const BaseData<GeneType>& base_data): base_data_(base_data) {}

        template<typename GeneType>
        Conditions<GeneType>::Conditions(const Conditions<GeneType>& conditions): base_data_(conditions.base_data_) {}

        template<typename GeneType>
        StartConditions<GeneType>::StartConditions(const BaseData<GeneType>& base_data, startGenerationFunction<GeneType> startGenerationFunction):
            Conditions<GeneType>(base_data), startGenerationFunction_(startGenerationFunction) {}

        template<typename GeneType>
        StartConditions<GeneType>::StartConditions(const StartConditions<GeneType>& conditions): 
            Conditions<GeneType>(conditions.base_data_), startGenerationFunction_(conditions.startGenerationFunction_) {}

        template<typename GeneType>
        PoolingConditions<GeneType>::PoolingConditions(const BaseData<GeneType>& base_data, poolingPopulations<GeneType> poolingPopulations):
            Conditions<GeneType>(base_data), poolingPopulations_(poolingPopulations) {} 
    
        template<typename GeneType>
        PoolingConditions<GeneType>::PoolingConditions(const PoolingConditions<GeneType>& conditions):
            Conditions<GeneType>(conditions.base_data_), poolingPopulations_(conditions.poolingPopulations_) {}

    } // end namespace Data

    namespace Evolution
    {
        template<typename GeneType>
        void evolution(Types::Population<GeneType>& population, Data::Conditions<GeneType>& conditions)
        {
            while (!conditions.base_data_.conditionsForStoppingFunction_(population))
            {
                Types::Generation<GeneType> new_generation = population.back();
                conditions.base_data_.crossingoverFunction_(new_generation);
                conditions.base_data_.mutationFunction_(new_generation);
                for (Types::Chromosome<GeneType>& chromosome: new_generation.chromosomes_)
                {
                    if (chromosome.fitness_.isNull)
                    {
                        chromosome.fitness_ = conditions.base_data_.fitnessFunction_(chromosome);
                    }
                }
                population.add(conditions.base_data_.selectionFunction_(new_generation));
            }   
        }

    } // end namespace Evolution

    namespace EvolutionTree
    {
        template<typename GeneType>
        Node<GeneType>::Node(const Data::Conditions<GeneType>& conditions): conditions_(conditions) {}
    
        template<typename GeneType>
        UnaryNode<GeneType>::UnaryNode(const Data::Conditions<GeneType>& conditions, Node<GeneType>* node): Node<GeneType>(conditions), node_(node) {}

        template<typename GeneType>
        Types::Population<GeneType> UnaryNode<GeneType>::evolution()
        {
            Types::Population<GeneType> population = node_->evolution();
            Evolution::evolution(population, conditions_);
            return population;
        }

        template<typename GeneType>
        BinaryNode<GeneType>::BinaryNode(const Data::PoolingConditions<GeneType>& conditions, Node<GeneType>* node1, Node<GeneType>* node2): Node<GeneType>(conditions), node1_(node1), node2_(node2) {}

        template<typename GeneType>
        Types::Population<GeneType> BinaryNode<GeneType>::evolution()
        {
            Types::Population<GeneType> population = conditions_.poolingPopulations_(node1_->evolution(), node2_->evolution());
            Evolution::evolution(population, conditions_);
            return population;
        }

        template<typename GeneType>
        PopulationNode<GeneType>::PopulationNode(const Data::StartConditions<GeneType>& conditions): Node<GeneType>(conditions) {}

        template <typename GeneType>
        Types::Population<GeneType> PopulationNode<GeneType>::evolution()
        {
            Types::Population<GeneType> population(conditions_.buffer_size_);
            population.add(conditions_.startGenerationFunction_());
            Evolution::evolution(population, conditions_);
            return population;
        }
    } // end namespace EvolutionTree

} // end namespace GeneticAlgorithm