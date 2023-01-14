#pragma once
#include "basic-types.hpp"
#include <map>
#include <set>

class PartialOrderPlanningO
{
	using State = std::set<std::string>;
	using ResultType = std::vector<std::string_view>;

	struct Node {
		State state;
		int act;
		int prev;
	};

public:
	void read_task();

	ResultType forward_search();

private:
	void modify_state(State& state, const LiteralList& mods);

	State m_init_state;
	State m_goal_state;
	std::vector<Action> m_actions;
};