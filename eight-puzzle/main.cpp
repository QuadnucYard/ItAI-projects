#include <iostream>
#include <random>
#include <stdexcept>
#include <ctime>
#include "puzzle_solver.hpp"

/// @brief Generate random puzzle of size by move blank block by specific steps. The quality is poor.
/// @tparam row Row of puzzle.
/// @tparam col Column of puzzle.
/// @param steps Steps of move.
/// @return The random puzzle.
template <puzzle_size_t row, puzzle_size_t col>
puzzle<row, col> random_puzzle(int steps) {
	static std::default_random_engine rng(time(0));
	puzzle<row, col> puz;
	bool f[4];
	int prev = -1;
	while (steps--) {
		int c = 0;
		for (int i = 0; i < 4; i++) f[i] = i + prev != 3 && puz.can_move(i);
		c = f[0] + f[1] + f[2] + f[3];
		std::uniform_int_distribution distrib(0, c - 1);
		for (int i = 0, j = distrib(rng); i < 4; i++) {
			if (f[i]) {
				if (j == 0) puz.move_blank(i);
				j--;
			}
		}
	}
	return puz;
}

/// @brief Generate random puzzle of size from random permutation.
/// @tparam row Row of puzzle.
/// @tparam col Column of puzzle.
/// @return The random puzzle.
template <puzzle_size_t row, puzzle_size_t col>
puzzle<row, col> random_puzzle_ex() {
	static std::default_random_engine rng(time(0));
	std::array<int8_t, row* col> d;
	std::iota(d.begin(), d.end(), 0);
	std::ranges::shuffle(d, rng);
	return puzzle<row, col>(d);
}

auto random_puzzle3x3 = random_puzzle_ex<3, 3>;

/// @brief Get direction string.
/// @param d Direction index.
/// @return Direction string.
auto get_dir_str(int d) {
	switch (d) {
	case 0: return "down";
	case 1: return "right";
	case 2: return "up";
	case 3: return "left";
	}
	throw std::invalid_argument("Invalid direction index");
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

int main() {
	int steps;
	//std::cin >> steps;
	steps = 10;
	puzzle3x3 p = random_puzzle3x3();
	std::cout << "Initial state:\n" << p << std::endl;
	puzzle3x3 tar;
	std::cout << "Target state:\n" << tar << std::endl;
	bool solvable = puzzle_solver::is_solvable(p, tar);
	std::cout << "Solvable? " << std::boolalpha << solvable << "\n\n";
	if (!solvable) return 0;
	{
		std::cout << "[DFS]" << std::endl;
		puzzle_solver_dfs solver;
		auto r = solver(p, tar, 25);
		print_steps(p, r);
	}
	{
		std::cout << "[BFS]" << std::endl;
		puzzle_solver_bfs solver;
		auto r = solver(p, tar);
		print_steps(p, r);
	}
	{
		std::cout << "[Heuristic-misplacement]" << std::endl;
		puzzle_solver_heuristic solver;
		auto r = solver(p, tar, puzzle_solver_heuristic::misplacement_criterion());
		print_steps(p, r);
	}
	{
		std::cout << "[Heuristic-Manhattan]" << std::endl;
		puzzle_solver_heuristic solver;
		auto r = solver(p, tar, puzzle_solver_heuristic::manhattan_criterion());
		print_steps(p, r);
	}

	return 0;
}