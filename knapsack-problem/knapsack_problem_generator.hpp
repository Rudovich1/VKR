#pragma once

#include "knapsack_problem.hpp"
#include <random>
#include <vector>
#include <fstream>

class KnapsackProblemGenerator
{
    public:
    
    static KnapsackProblem generateProblem(
        size_t size, 
        int min_weight,
        int max_weight,
        double min_value,
        double max_value,
        int min_knapsack_size,
        int max_knapsack_size)
    {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::vector<std::pair<int, double>> items(size);

        for (size_t i = 0; i < size; ++i)
        {
            
            items[i] = std::make_pair(std::uniform_int_distribution<int>(min_weight, max_weight)(gen),
             std::uniform_real_distribution<double>(min_value, max_value)(gen));
        }

        int knapsack_size = std::uniform_int_distribution<int>(min_knapsack_size, max_knapsack_size)(gen);

        return KnapsackProblem(size, items, knapsack_size);
    }

    static KnapsackProblem loadProblem(std::string file_path)
    {
        std::ifstream in(file_path);

        size_t size;
        int knapsack_size;
        in >> size >> knapsack_size;

        std::vector<std::pair<int, double>> items(size);

        for (size_t i = 0; i < size; ++i)
        {
            in >> items[i].first >> items[i].second;
        }

        in.close();

        return KnapsackProblem(size, items, knapsack_size);
    }

    static void saveProblem(KnapsackProblem& knapsack_problem, std::string file_name)
    {
        std::ofstream out(file_name);

        out << knapsack_problem.size_ << " " << knapsack_problem.knapsack_capacity_ << '\n';
        for (size_t i = 0; i < knapsack_problem.size_; ++i)
        {
            out << knapsack_problem.items_[i].weight_ << " " << knapsack_problem.items_[i].value_ << '\n';
        }

        out.close();
    }
};