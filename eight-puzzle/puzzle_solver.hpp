#pragma once
#include <vector>
#include <unordered_set>
#include <queue>
#include <stack>
#include <chrono>
#include "puzzle.hpp"

struct puzzle_solver {

	using result_type = std::vector<int>;

	/// @brief Count the number of inversions in the puzzle's digits, except 0.
	/// @param p The puzzle.
	/// @return Number of inversions.
	template <puzzle_size_t row, puzzle_size_t col>
	constexpr static int count_inversions(const puzzle<row, col>& p) {
		int s = 0, n = p.size;
		for (int i = 0; i < n; i++) {
			if (p[i] == 0) continue;
			for (int j = i + 1; j < n; j++) {
				if (p[j] != 0 && p[j] < p[i]) s++;
			}
		}
		return s;
	}

	template <puzzle_size_t row, puzzle_size_t col>
	constexpr static int distance_of_blank(const puzzle<row, col>& a, const puzzle<row, col>& b) {
		return abs(a.zero_pos / col - b.zero_pos / col) + abs(a.zero_pos % col + b.zero_pos % col);
	}

	/// @brief Judge whether the two puzzle states are reachable. 44
	/// @param a One puzzle.
	/// @param b Another puzzle.
	/// @return Whether a can transit to b, or b to a.
	template <puzzle_size_t row, puzzle_size_t col>
	constexpr static bool is_solvable(const puzzle<row, col>& a, const puzzle<row, col>& b) {
		// When row == col and odd, the parity of inversions equals.
		// When row == col and even, the parity of difference of inversions == parity of distance of 0.
		static_assert(row == col);
		int inv_diff = count_inversions(a) - count_inversions(b);
		if constexpr (row & 1) { // odd
			return ~inv_diff & 1;
		} else { // even
			return (inv_diff & 1) == (distance_of_blank(a, b) & 1);
		}
	}

	class solution_log {
		using time_point_t = std::chrono::time_point<std::chrono::high_resolution_clock>;

	private:
		time_point_t __start_tp;
		time_point_t __stop_tp;
		int __max_depth;
		int __nodes_generated;
		int __nodes_extended;
		bool __solved;

	public:
		inline void restart() {
			__max_depth = 0;
			__nodes_generated = 0;
			__nodes_extended = 0;
			__solved = false;
			__start_tp = std::chrono::high_resolution_clock::now();
		}

		inline void stop() {
			__stop_tp = std::chrono::high_resolution_clock::now();
		}

		inline void inc_nodesgen() { __nodes_generated++; }

		inline void inc_nodesext() { __nodes_extended++; }

		inline void check_max_depth(int d) { __max_depth = std::max(d, __max_depth); }

		inline void solved(bool value) { __solved = value; }
		inline bool solved() const { return __solved; }

		inline int nodes_generated() const { return __nodes_generated; }

		inline int nodes_extended() const { return __nodes_extended; }

		inline int max_depth() const { return __max_depth; }

		inline int duration() const {
			return std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(__stop_tp - __start_tp).count();
		}
	};
	solution_log logger;
};

#define SOLVE_ENTER \
	logger.restart(); \
	if (ini == tar) { logger.stop(); logger.solved(true); return {};} \
	size_t htar = tar.hash_code(); \
	std::unordered_set<size_t> vis{ ini.hash_code() }; \
	result_type res; \
	bool found = false;
#define SOLVE_EXIT \
	logger.stop(); \
	logger.solved(found); \
	return res;

/// @brief Puzzle solver using breath-first-search strategy.
struct puzzle_solver_bfs : public puzzle_solver {

	/// @brief Get solution. Requires solvable.
	/// @param ini Initial puzzle state.
	/// @param tar Target puzzle state.
	/// @return Operation sequence.
	template <puzzle_size_t row, puzzle_size_t col>
	result_type operator()(const puzzle<row, col>& ini, const puzzle<row, col>& tar) {
		struct puzzle_node {
			puzzle<row, col> puz;
			int action;
			int prev;
			int steps;
		};

		SOLVE_ENTER
		std::vector<puzzle_node> q{ { ini, -1, -1, 0 } };

		for (size_t i = 0; i < q.size() && !found; i++) {
			logger.inc_nodesext();
			auto cur = q[i];
			for (int k = 0; k < 4; k++) {
				if (!cur.puz.can_move(k)) continue;
				logger.inc_nodesgen();
				logger.check_max_depth(cur.steps + 1);
				puzzle_node nxt{ cur.puz, k, (int)i, cur.steps + 1 };
				nxt.puz.move_blank(k);
				size_t h = nxt.puz.hash_code();
				if (vis.contains(h)) continue;
				q.push_back(nxt);
				vis.insert(h);
				if (h == htar) { found = true; break; }
			}
		}
		// Now the last item is the final state.
		for (int i = q.size() - 1; i != 0; i = q[i].prev) {
			res.push_back(q[i].action);
		}
		std::ranges::reverse(res);
		SOLVE_EXIT
	}
};


/// @brief Puzzle solver using depth-first-search strategy.
struct puzzle_solver_dfs : public puzzle_solver {

	/// @brief Get solution. Requires solvable.
	/// @param ini Initial puzzle state.
	/// @param tar Target puzzle state.
	/// @param limit Limit to depth.
	/// @return Operation sequence.
	template <puzzle_size_t row, puzzle_size_t col>
	result_type operator()(const puzzle<row, col>& ini, const puzzle<row, col>& tar, int limit) {
		SOLVE_ENTER

		const auto& dfs = [&](const puzzle<row, col>& cur) {
			const auto& s = [&](auto&& self, const puzzle<row, col>& cur) -> void {
				logger.inc_nodesext();
				for (int k = 0; k < 4; k++) {
					if (!cur.can_move(k)) continue;
					logger.inc_nodesgen();
					logger.check_max_depth(res.size() + 1);
					puzzle nxt{ cur };
					nxt.move_blank(k);
					size_t h = nxt.hash_code();
					if (vis.contains(h)) continue;
					res.push_back(k);
					vis.insert(h);
					if (h == htar) { found = true; break; }
					if (res.size() < limit) self(self, nxt);
					if (found) return;
					res.pop_back();
				}
			};
			return s(s, cur);
		};

		dfs(ini);
		SOLVE_EXIT
	}

};

/// @brief @brief Puzzle solver using heuristic search strategy.
struct puzzle_solver_heuristic : public puzzle_solver {

	/// @brief Criterion of number of misplaced digits.
	struct misplacement_criterion {
		template <puzzle_size_t row, puzzle_size_t col>
		int operator()(const puzzle<row, col>& cur, const puzzle<row, col>& tar) {
			int ans = 0;
			for (int i = 0; i < cur.size; i++) {
				ans += cur[i] != tar[i];
			}
			return ans;
		}
	};

	/// @brief Criterion of the Manhattan distance of each  
	struct manhattan_criterion {
		template <puzzle_size_t row, puzzle_size_t col>
		int operator()(const puzzle<row, col>& cur, const puzzle<row, col>& tar) {
			int pos[cur.size]; // Position of i in cur.
			int ans = 0;
			for (int i = 0; i < cur.size; i++) pos[cur[i]] = i;
			for (int i = 0; i < tar.size; i++) { // Enumerate pos
				if (tar[i] == 0) continue;
				int zx = pos[tar[i]] / col, zy = pos[tar[i]] % col;
				int tx = i / col, ty = i % col;
				ans += abs(zx - tx) + abs(zy - ty);
			}
			return ans;
		}
	};

	/// @brief Get solution. Requires solvable.
	/// @param ini Initial puzzle state.
	/// @param tar Target puzzle state.
	/// @param eval_func Evaluation function.
	/// @return Operation sequence.
	template <puzzle_size_t row, puzzle_size_t col, class Func>
	result_type operator()(const puzzle<row, col>& ini, const puzzle<row, col>& tar, Func eval_func) {
		struct puzzle_node {
			puzzle<row, col> puz;
			int fcost;
			int gcost;
			int action;
			int prev;
			bool operator< (const puzzle_node& o) const {
				return fcost + gcost > o.fcost + o.gcost;
			}
		};

		SOLVE_ENTER
		std::vector<puzzle_node> close;
		std::priority_queue<puzzle_node> open;
		open.push({ ini, 0, 0, -1, -1 });

		while (!open.empty() && !found) {
			logger.inc_nodesext();
			auto cur = open.top();
			open.pop();
			close.push_back(cur);
			for (int k = 0; k < 4; k++) {
				if (!cur.puz.can_move(k)) continue;
				logger.inc_nodesgen();
				logger.check_max_depth(cur.fcost + 1);
				puzzle_node nxt{ cur.puz, cur.fcost + 1, eval_func(cur.puz, tar), k, (int)close.size() - 1 };
				nxt.puz.move_blank(k);
				size_t h = nxt.puz.hash_code();
				if (vis.contains(h)) continue;
				open.push(nxt);
				vis.insert(h);
				if (h == htar) { close.push_back(nxt); found = true; break; }
			}
		}
		// Now the last item is the final state.
		for (int i = close.size() - 1; i != 0; i = close[i].prev) {
			res.push_back(close[i].action);
		}
		std::ranges::reverse(res);
		SOLVE_EXIT
	}
};

#undef SOLVE_ENTER
#undef SOLVE_EXIT