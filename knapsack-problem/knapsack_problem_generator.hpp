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
        long long min_weight,
        long long max_weight,
        long long min_value,
        long long max_value,
        size_t min_knapsack_size,
        size_t max_knapsack_size)
    {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::vector<std::pair<long long, long long>> items(size);

        for (size_t i = 0; i < size; ++i)
        {
            items[i] = std::make_pair(std::uniform_int_distribution<long long>(min_weight, max_weight)(gen),
             std::uniform_int_distribution<long long>(min_value, max_value)(gen));
        }

        int knapsack_size = std::uniform_int_distribution<long long>(min_knapsack_size, max_knapsack_size)(gen);

        return KnapsackProblem(size, items, knapsack_size);
    }

    static KnapsackProblem generateProblemNorm(
        size_t size, 
        long long M_w,
        long long S_w,
        long long M_v,
        long long S_v,
        size_t knapsack_size)
    {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::vector<std::pair<long long, long long>> items(size);
        
        auto w_dist = std::normal_distribution<double>(M_w, S_w);
        auto v_dist = std::normal_distribution<double>(M_v, S_w);

        for (size_t i = 0; i < size; ++i)
        {
            auto w = (long long)w_dist(gen);
            auto v = (long long)v_dist(gen);
            if (w < 0 || v < 0) {--i; continue;}
            items[i] = std::make_pair(w, v);
        }

        return KnapsackProblem(size, items, knapsack_size);
    }

    static KnapsackProblem loadProblem(std::string file_path)
    {
        std::ifstream in(file_path);

        size_t size;
        size_t knapsack_size;
        in >> size >> knapsack_size;

        std::vector<std::pair<long long, long long>> items(size);

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