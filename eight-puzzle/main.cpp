#include <iostream>
#include <random>
#include <stdexcept>
#include "puzzle_solver.hpp"
using namespace std;

/// @brief Generate random puzzle of size by move blank block by specific steps.
/// @tparam row Row of puzzle.
/// @tparam col Column of puzzle.
/// @param steps Steps of move.
/// @return The random puzzle.
template <puzzle_size_t row, puzzle_size_t col>
puzzle<row, col> random_puzzle(int steps) {
	static std::default_random_engine rng;
	puzzle<row, col> puz;
	bool f[4];
	while (steps--) {
		int c = 0;
		for (int i = 0; i < 4; i++) f[i] = puz.can_move(i);
		c = f[0] + f[1] + f[2] + f[3];
		std::uniform_int_distribution distrib(0, c - 1);
		for (int i = 0, j = distrib(rng); i < 4; i++) {
			if (f[i]) {
				if (j == 0) puz.move_blank(i);
				else j--;
			}
		}
	}
	return puz;
}

auto random_puzzle3x3 = random_puzzle<3, 3>;

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

int main() {
	puzzle3x3 p = random_puzzle3x3(10);
	std::cout << p;
	puzzle3x3 tar;
	puzzle_solver_heuristic solver;
	printf("solvable? %d\n", puzzle_solver::is_solvable(p, tar));
	auto r = solver(p, tar, puzzle_solver_heuristic::manhattan_criterion());
	auto q = p;
	for (auto x : r) {
		printf("%d %s\n", x, get_dir_str(x));
		q.move_blank(x);
		cout << q;
	}
	printf("\n");
	return 0;
}