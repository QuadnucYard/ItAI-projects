#pragma GCC optimize(3)
#include "csp_queens.hpp"
#include <iostream>

void print_solution(const QueensAssignment& a) {
	for (int i = 0; auto && q : a) {
		for (int j = 0; j < a.size(); j++)
			std::cout << ".@"[j == q.value.col];
		std::cout << '\n';
		i++;
	}
}

void single_display(int n, int print_level = 0) {
	auto mcs = MCSQueens(CSPQueens(n));
	auto st = std::chrono::high_resolution_clock::now();
	auto sol = mcs(-1);
	auto et = std::chrono::high_resolution_clock::now();
	auto dur = std::chrono::duration_cast<std::chrono::microseconds>(et - st).count();
	printf("steps: %d\n", sol.first);
	printf("duration: %.3fms\n", dur / 1e3);
	if (print_level <= 1) {
		for (auto x : sol.second)
			printf("(%d,%d) ", x.value.row, x.value.col);
		printf("\n");
		if (sol.first > 0 && print_level <= 0)
			print_solution(sol.second);
	}
}

void benchmark_steps() {
	for (int i = 4; i <= 64; i += 4) { // Number of queens
		CSPQueens csp{ i };
		MCSQueens mcs{ csp, 114514u };
		printf("%d", i);
		for (int k = 50; k <= 500; k += 50) {
			int failures = 0;
			for (int j = 0; j < 1000; j++) {
				auto sol = mcs(k);
				failures += sol.first == -1;
			}
			printf("\t%d", failures);
		}
		printf("\n");
	}
}

int main() {
	// benchmark_steps();
	single_display(10000, 2);
	return 0;
}