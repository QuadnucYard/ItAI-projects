#pragma once
#include "csp.hpp"
#include "min_conflict_search.hpp"
#include <ranges>

struct position {
	int row;
	int col;

	position(): row(-1), col(-1) {}
	position(int row, int col): row(row), col(col) {}

	bool operator== (const position& o) const = default;
};

struct Queen {
	using value_type = position;
	int n;
	value_type value;
	Queen(int n, int id): n(n), value{ id,id } {}
	auto domain() const {
		return std::views::iota(0, n)
			| std::views::transform([r = this->value.row](auto x) { return position{ r, x }; });
	}
};


struct QueensAssignment {
	std::vector<Queen> vars;
	std::vector<int> state0, state1, state2; // Column, major diagonal, minor diagonal
	int invalid_count;

	QueensAssignment(const std::vector<Queen>& vars): vars(vars),
		state0(vars.size()), state1(vars.size() * 2 - 1), state2(vars.size() * 2 - 1),
		invalid_count(0)
	{
		for (size_t i = 0; i < vars.size(); i++)
			assign(this->vars[i], vars[i].value);
	}

	inline void inc(std::vector<int>& s, int i) {
		if (++s[i] == 2) ++invalid_count;
	}

	inline void dec(std::vector<int>& s, int i) {
		if (--s[i] == 1) --invalid_count;
	}

	void assign(Queen& old, const position& value) {
		old.value = value;
		inc(state0, value.col);
		inc(state1, value.row + value.col);
		inc(state2, value.row - value.col + vars.size() - 1);
	}

	void reassign(Queen& old, const position& value) {
		dec(state0, old.value.col);
		dec(state1, old.value.row + old.value.col);
		dec(state2, old.value.row - old.value.col + vars.size() - 1);
		assign(old, value);
	}

	int get_conflicts(const position& value) const {
		return state0[value.col]
			+ state1[value.row + value.col]
			+ state2[value.row - value.col + vars.size() - 1]
			- 3;
	}

	auto begin() { return vars.begin(); }
	auto begin() const { return vars.begin(); }
	auto end() { return vars.end(); }
	auto end() const { return vars.end(); }
	auto size() const { return vars.size(); }
};

struct CSPQueens: public CSP<Queen, QueensAssignment> {
	int size;

	CSPQueens(int size): size(size) {}

	assignment_type initial_assignment() const override {
		variable_collection vars;
		for (int i = 0; i < size; i++)
			vars.emplace_back(size, i);
		return vars;
	}

	// Not used
	bool constraint(const value_type& v1, const value_type& v2) const override {
		return v1.col != v2.col && v1.row - v1.col != v2.row - v2.col && v1.row + v1.col != v2.row + v2.col;
	}

	// Not used
	bool consistent(const value_type& value, const assignment_type& assignment) const override {
		return false;
	}

	bool is_solution(const assignment_type& assignment) const override {
		return assignment.invalid_count == 0;
	}
};

using MCSQueens = MinConflictSearch<Queen, QueensAssignment>;
