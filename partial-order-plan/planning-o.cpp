#include "planning-o.hpp"
#include <queue>
#include <algorithm>
#include <cassert>
#include <tl/enumerate.hpp>
#include <tl/to.hpp>

void PartialOrderPlanningO::read_task()
{
	/* m_goal_state = {
		"RightShoeOn",
		"LeftShoeOn",
	};
	m_actions = {
		{"RightShoe", {{"RightSockOn", true}}, {{"RightShoeOn", true}}},
		{"RightSock", {}, {{"RightSockOn", true}}},
		{"LeftShoe", {{"LeftSockOn", true}}, {{"LeftShoeOn", true}}},
		{"LeftSock", {}, {{"LeftSockOn", true}}},
	};
	m_init_state = {}; */
	/* m_init_state = { "HaveCake" };
	m_goal_state = { "HaveCake", "EatenCake" };
	m_actions = {
		{"EatCake", {{"HaveCake", true}}, {{"HaveCake", false}, {"EatenCake", true}}},
		{"BakeCake", {{"HaveCake", false}}, {{"HaveCake", true}}},
	}; */
	m_init_state = { "AFA", "AST" };
	m_goal_state = { "ASA" };
	m_actions = {
		{"RST", {{"AST", true}}, {{"AST", false}, {"ASG", true}}},
		{"RFA", {{"AFA", true}}, {{"AFA", false}, {"AFG", true}}},
		{"PSA", {{"ASG", true}, {"AFA", false}}, {{"ASG", false}, {"ASA", true}}},
		{"LO", {}, {{"ASG", false}, {"ASA", false}, {"AST", false}, {"AFG", false}, {"AFA", false}}},
	};
}

void PartialOrderPlanningO::modify_state(State& state, const LiteralList& mods)
{
	for (auto&& l : mods) {
		if (l.polarity)
			state.insert(l.name);
		else
			state.erase(l.name);
	}
}

PartialOrderPlanningO::ResultType PartialOrderPlanningO::forward_search()
{
	std::vector<Node> nodes;

	std::queue<int> open;
	std::set<State> close;

	nodes.emplace_back(m_init_state, -1, -1);
	close.insert(nodes[0].state);
	open.push(0);

	while (!open.empty()) {
		int cur_idx = open.front();
		State cur_state = nodes[cur_idx].state;
		open.pop();
		if (std::ranges::includes(cur_state, m_goal_state)) {
			// Backtrack
			ResultType ans;
			for (int i = cur_idx; i != 0; i = nodes[i].prev)
				ans.emplace_back(m_actions[nodes[i].act].name);
			std::ranges::reverse(ans);
			return ans;
		}
		// Extend
		for (auto&& [i, action] : tl::views::enumerate(m_actions)) {
			if (!std::ranges::all_of(action.preconds, [&cur_state](auto&& t) { return cur_state.contains({ t.name }) == t.polarity; }))
				continue;
			State next_state = cur_state;
			modify_state(next_state, action.effects);
			if (!close.contains(next_state)) {
				open.push(nodes.size());
				nodes.emplace_back(next_state, i, cur_idx);
				close.insert(next_state);
			}
		}
	}
	return {};
}
