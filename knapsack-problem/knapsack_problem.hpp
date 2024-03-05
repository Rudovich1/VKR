#pragma once

#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>

struct KnapsackProblem
{
    struct Item
    {
        long long weight_;
        long long value_;

        Item(){}
        Item(std::pair<long long, long long> item): weight_(item.first), value_(item.second) {}
    };

    std::vector<Item> items_;
    size_t size_;
    size_t knapsack_capacity_;

    KnapsackProblem(size_t size, const std::vector<std::pair<long long, long long>>& items, size_t knapsack_capacity): 
        size_(size), 
        items_(size), 
        knapsack_capacity_(knapsack_capacity)
    {
        for (size_t i = 0; i < size; ++i)
        {
            items_[i] = items[i];
        }

        std::sort(items_.begin(), items_.end(), [](Item a, Item b){return a.value_ * b.weight_ > b.value_ * a.weight_;});
    }

    std::ostream& operator<< (std::ostream& out)
    {
        out << "Knapsack size: " << size_ << '\n';
        out << "Knapsack capacity: " << knapsack_capacity_ << '\n';
        for (size_t i = 0; i < size_; ++i)
        {
            out << "(" << std::setw(10) << items_[i].weight_ << " | " << std::setw(10) << items_[i].value_ << ")\n";
        }
        return out;
    }

    friend std::ostream& operator<< (std::ostream& out, const KnapsackProblem& knapsack_problem);
};

std::ostream& operator<< (std::ostream& out, const KnapsackProblem& knapsack_problem)
{
    out << "Knapsack size: " << knapsack_problem.size_ << '\n';
    out << "Knapsack capacity: " << knapsack_problem.knapsack_capacity_ << '\n';
    long long w = 0;
    long long v = 0.;
    for (size_t i = 0; i < knapsack_problem.size_; ++i)
    {
        out << "(" << std::setw(10) << knapsack_problem.items_[i].weight_ << " | " << std::setw(10) << knapsack_problem.items_[i].value_ << ")\n";
        w += knapsack_problem.items_[i].weight_;
        v += knapsack_problem.items_[i].value_;
    }
    out << std::setw(25) << std::setfill('-') << '\n';
    out << std::setfill(' ') << std::setw(11) << w << " | " << std::setw(10) << v << '\n'; 
    return out;
}