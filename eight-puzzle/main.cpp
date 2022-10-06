#include <iostream>
#include "test_utils.hpp"

bool need_print_steps = false;

struct solution_method {
	constexpr static int
		dfs = 1 << 0,
		bfs = 1 << 1,
		heu_mis = 1 << 2,
		heu_man = 1 << 3,
		all = dfs | bfs | heu_mis | heu_man;
};

void custom_test(const puzzle3x3& p, int methods) {
	std::cout << "Initial state:\n" << p;
	puzzle3x3 tar;
	std::cout << "Target state:\n" << tar;
	bool solvable = puzzle_solver::is_solvable(p, tar);
	std::cout << "Solvable? " << std::boolalpha << solvable << "\n";
	if (!solvable) return;
	if (methods & solution_method::dfs) {
		std::cout << "[DFS]" << std::endl;
		puzzle_solver_dfs solver;
		auto r = solver(p, tar, 30);
		print_log(solver);
		if (need_print_steps) print_steps(p, r);
	}
	if ((int)methods & (int)solution_method::bfs) {
		std::cout << "[BFS]" << std::endl;
		puzzle_solver_bfs solver;
		auto r = solver(p, tar);
		print_log(solver);
		if (need_print_steps) 	print_steps(p, r);
	}
	if ((int)methods & (int)solution_method::heu_mis) {
		std::cout << "[Heuristic-misplacement]" << std::endl;
		puzzle_solver_heuristic solver;
		auto r = solver(p, tar, puzzle_solver_heuristic::misplacement_criterion());
		print_log(solver);
		if (need_print_steps) print_steps(p, r);
	}
	if ((int)methods & (int)solution_method::heu_man) {
		std::cout << "[Heuristic-Manhattan]" << std::endl;
		puzzle_solver_heuristic solver;
		auto r = solver(p, tar, puzzle_solver_heuristic::manhattan_criterion());
		print_log(solver);
		if (need_print_steps) print_steps(p, r);
	}
}

puzzle3x3 create_solvable_puzzle(const puzzle3x3 tar = {}) {
	puzzle3x3 p = random_puzzle3x3();
	while (!puzzle_solver::is_solvable(p, tar)) {
		p = random_puzzle3x3();
	}
	return p;
}

void test_many(int t, bool solvable, int methods = solution_method::all) {
	puzzle3x3 tar{};
	while (t--) {
		custom_test(solvable ? create_solvable_puzzle(tar) : random_puzzle3x3(), methods);
	}
}

int main() {
	test_many(5, true, solution_method::dfs | solution_method::heu_man);
	return 0;
}