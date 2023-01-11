#pragma once
#include <vector>

template <class Variable, class Assignment>
struct CSP {
	using variable_type = Variable;
	using value_type = Variable::value_type;
	using variable_collection = std::vector<variable_type>;
	using assignment_type = Assignment;

	virtual assignment_type initial_assignment() const = 0;

	/// @return True for satisfaction. 
	virtual bool constraint(const value_type& v1, const value_type& v2) const = 0;
	virtual bool consistent(const value_type& value, const assignment_type& assignment) const = 0;
	virtual bool is_solution(const assignment_type& assignment) const = 0;
};