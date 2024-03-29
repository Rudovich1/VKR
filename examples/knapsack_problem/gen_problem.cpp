#include "../../knapsack-problem/knapsack_problem_generator.hpp"
#include "../../knapsack-problem/knapsack_problem_solver.hpp"
#include <fstream>
#include <iomanip>

int main()
{
    auto kp = KnapsackProblemGenerator::generateProblemNorm(1000, 300, 100, 100, 30, 100000);
    KnapsackProblemGenerator::saveProblem(kp, "task1.txt");

    // auto kp = KnapsackProblemGenerator::loadProblem("task1.txt");
    auto dp_res = KnapsackProblemSolver::dynamicProgrammingMethod(kp, 10);
    auto bb_res = KnapsackProblemSolver::branchAndBoundaryMethod(kp, 10);
    auto greedy_res = KnapsackProblemSolver::greedyMethod(kp, 10);

    std::ofstream out("res.txt");
    out << std::setprecision(6) << std::fixed;
    out << "dp: " << dp_res << '\n';
    out << "bb: " << bb_res << '\n';
    out << "greedy: " << greedy_res << '\n';
}