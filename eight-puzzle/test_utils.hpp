#pragma once
#include <iostream>
#include "puzzle_solver.hpp"
#include "puzzle_utils.hpp"

/// @brief Print log of solver.
/// @param solver Puzzle solver.
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
	int s = 0;
	for (auto d : steps) {
		printf("Step %d: %s\n", ++s, get_dir_str(d));
		p.move_blank(d);
		std::cout << p;
	}
	std::cout << std::endl;
}