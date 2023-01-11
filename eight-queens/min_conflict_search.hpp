#pragma once
#include "csp.hpp"
#include <chrono>
#include <random>

template <class Variable, class Assignment>
class MinConflictSearch {
public:
	using variable_type = Variable;
	using value_type = Variable::value_type;
	using assignment_type = Assignment;
	using csp_type = CSP<variable_type, assignment_type>;
private:
	const csp_type& csp;
	std::mt19937 rng;

public:
	MinConflictSearch(const csp_type& csp, uint_fast32_t seed): csp(csp), rng(seed) {}
	MinConflictSearch(const csp_type& csp): MinConflictSearch(csp, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) {}

	std::pair<int, assignment_type> operator()(int max_steps = -1) {
		assignment_type current = csp.initial_assignment();
		if (max_steps == -1)
			max_steps = std::numeric_limits<int>::max();
		for (int i = 0; i < max_steps; i++) {
			if (csp.is_solution(current))
				return { i, current };
			auto var = choose_conflict_variable(current);
			auto value = get_min_conflict_value(*var, current);
			if (var != current.end())
				current.reassign(*var, value);
		}
		return { -1, current };
	}

private:
	auto choose_conflict_variable(assignment_type& assignment) {
		int max_conflicts = 0, cnt = 0;
		auto result = assignment.end();
		for (auto it = assignment.begin(); it != assignment.end(); ++it) {
			int conf = assignment.get_conflicts(it->value);
			if (conf > max_conflicts) {
				max_conflicts = conf;
				cnt = 1;
				result = it;
			} else if (conf == max_conflicts && rng() % ++cnt == 0) {
				result = it;
			}
		}
		return result;
	}

	value_type get_min_conflict_value(const variable_type& var, const assignment_type& assignment) {
		int min_conflicts = std::numeric_limits<int>::max(), cnt = 0;
		value_type val;
		for (auto&& v : var.domain()) {
			int conf = assignment.get_conflicts(v);
			if (conf < min_conflicts) {
				min_conflicts = conf;
				cnt = 1;
				val = v;
			} else if (conf == min_conflicts && rng() % ++cnt == 0) {
				val = v;
			}
		}
		return val;
	}

};