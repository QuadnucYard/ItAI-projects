#include <iostream>
#include "puzzle_solver.hpp"
#include "puzzle_utils.hpp"

// #define PRINT_STEPS

void print_log(const puzzle_solver& solver) {
	printf("\tSolved: %s\n\tDuration: %dms\n\tMax depth: %d\n\tNodes generated: %d\n\tNodes extended: %d\n",
		solver.logger.solved() ? "Yes" : "No",
		solver.logger.duration(),
		solver.logger.max_depth(),
		solver.logger.nodes_generated(),
		solver.logger.nodes_extended()
	);
}

/// @brief Print solution steps
/// @param p Initial puzzle.
/// @param steps Steps of moving direction.
template <puzzle_size_t row, puzzle_size_t col>
void print_steps(puzzle<row, col> p, const std::vector<int> steps) {
#ifndef PRINT_STEPS
	return;
#endif
	int s = 0;
	for (auto d : steps) {
		printf("Step %d: %s\n", ++s, get_dir_str(d));
		p.move_blank(d);
		std::cout << p;
	}
	std::cout << std::endl;
}

void create_puzzle_and_solve() {
	int steps;
	//std::cin >> steps;
	steps = 10;
	puzzle3x3 p = random_puzzle3x3();
	std::cout << "Initial state:\n" << p << std::endl;
	puzzle3x3 tar;
	std::cout << "Target state:\n" << tar << std::endl;
	bool solvable = puzzle_solver::is_solvable(p, tar);
	std::cout << "Solvable? " << std::boolalpha << solvable << "\n\n";
	if (!solvable) return;
	{
		std::cout << "[DFS]" << std::endl;
		puzzle_solver_dfs solver;
		auto r = solver(p, tar, 30);
		print_log(solver);
		print_steps(p, r);
	}
	{
		std::cout << "[BFS]" << std::endl;
		puzzle_solver_bfs solver;
		auto r = solver(p, tar);
		print_log(solver);
		print_steps(p, r);
	}
	{
		std::cout << "[Heuristic-misplacement]" << std::endl;
		puzzle_solver_heuristic solver;
		auto r = solver(p, tar, puzzle_solver_heuristic::misplacement_criterion());
		print_log(solver);
		print_steps(p, r);
	}
	{
		std::cout << "[Heuristic-Manhattan]" << std::endl;
		puzzle_solver_heuristic solver;
		auto r = solver(p, tar, puzzle_solver_heuristic::manhattan_criterion());
		print_log(solver);
		print_steps(p, r);
	}

}

int main() {
	create_puzzle_and_solve();
	return 0;
}