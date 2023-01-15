#include "planning.hpp"
#include <algorithm>
#include <queue>
#include <fstream>
#include <tl/enumerate.hpp>
#include <nlohmann/json.hpp>

namespace qy::ai
{
	int PartialOrderPlanning::literal2id(const std::string& s)
	{
		if (auto it = std::ranges::find(m_literals, s); it != m_literals.end())
			return it - m_literals.begin();
		m_literals.push_back(s);
		return m_literals.size() - 1;
	}

	void PartialOrderPlanning::modify_state(State& state, const LiteralList& mods) const
	{
		for (auto&& l : mods)
			state.set(l.id, l.polarity + 1);
	}

	std::string PartialOrderPlanning::state_to_string(const State& state) const
	{
		std::string s;
		for (size_t i = 0; i < m_literals.size(); i++) {
			auto x = state.get(i);
			if (x == quatset::TRUE) s += '+';
			else if (x == quatset::FALSE)s += '-';
			s += m_literals[i];
		}
		return s;
	}

	const Action& PartialOrderPlanning::get_action(int i) const
	{
		return m_actions[i];
	}

	void PartialOrderPlanning::read_task(const fs::path& file)
	{
		using json = nlohmann::json;
		json root = json::parse(std::ifstream(file));
		for (auto&& v : root["init"])
			m_init_state.set(literal2id(v.get<std::string>()));
		for (auto&& v : root["goal"])
			m_goal_state.set(literal2id(v.get<std::string>()));
		for (auto&& v : root["actions"]) {
			m_actions.emplace_back(v["name"].get<std::string>(), LiteralList{}, LiteralList{});
			for (auto&& [k, v] : v["preconds"].items())
				m_actions.back().preconds.emplace_back(literal2id(k), v.get<bool>());
			for (auto&& [k, v] : v["effects"].items())
				m_actions.back().effects.emplace_back(literal2id(k), v.get<bool>());
		}
	}

	PartialOrderPlanning::ResultType PartialOrderPlanning::forward_search() const
	{
		std::vector<Node> nodes{ {m_init_state, -1, -1} }; // Store all generated nodes

		std::queue<int> open{ {0} };
		std::set<State> close{ nodes[0].state };

		while (!open.empty()) {
			int cur_idx = open.front();
			State cur_state = nodes[cur_idx].state;
			open.pop();
			if (cur_state.includes(m_goal_state)) {
				// Backtrack
				ResultType ans;
				for (int i = cur_idx; i != 0; i = nodes[i].prev)
					ans.emplace_back(m_actions[nodes[i].act].name);
				std::ranges::reverse(ans);
				return ans;
			}
			// Extend
			for (auto&& [i, action] : tl::views::enumerate(m_actions)) {
				// Check if all preconditions are satisfied
				auto checker = [&cur_state](const Literal& t) {
					return cur_state.get(t.id) == t.polarity + 1;
				};
				if (!std::ranges::all_of(action.preconds, checker))
					continue;
				State next_state = cur_state;
				// Apply effects
				modify_state(next_state, action.effects);
				// Enqueue
				if (!close.contains(next_state)) {
					open.push(nodes.size());
					nodes.emplace_back(next_state, i, cur_idx);
					close.insert(next_state);
				}
			}
		}
		return {};
	}

	PartialOrderPlanning::ResultType PartialOrderPlanning::backward_search() const
	{
		std::vector<Node> nodes{ {m_goal_state, -1, -1} }; // Store all generated nodes

		std::queue<int> open{ {0} };
		std::set<State> close{ nodes[0].state };

		while (!open.empty()) {
			int cur_idx = open.front();
			State cur_state = nodes[cur_idx].state;
			open.pop();
			if (cur_state == m_init_state) {
				// Backtrack
				ResultType ans;
				for (int i = cur_idx; i != 0; i = nodes[i].prev)
					ans.emplace_back(m_actions[nodes[i].act].name);
				return ans;
			}
			// Extend
			for (auto&& [i, action] : tl::views::enumerate(m_actions)) {
				State prev_state = cur_state;
				// Check if all effects are established
				// Remove established literals from previous state
				auto checker = [&](const Literal& e) {
					if (auto x = prev_state.get(e.id); x != 0) {
						if (x != e.polarity + 1)
							return false;
						prev_state.set(e.id, quatset::UNKNOWN);
					}
					return true;
				};
				if (!std::ranges::all_of(action.effects, checker))
					continue;
				// Apply preconditions
				modify_state(prev_state, action.preconds);
				// Enqueue
				if (!close.contains(prev_state)) {
					open.push(nodes.size());
					nodes.emplace_back(prev_state, i, cur_idx);
					close.insert(prev_state);
				}
			}
		}
		return {};
	}

	PartialOrderPlanning::GraphType PartialOrderPlanning::forward_search_g() const
	{
		std::vector<Node> nodes{ {m_init_state, -1, -1} }; // Store all generated nodes

		std::queue<int> open{ {0} };
		std::set<State> close{ nodes[0].state };

		std::map<State, std::map<int, State>> graph;

		while (!open.empty()) {
			int cur_idx = open.front();
			State cur_state = nodes[cur_idx].state;
			open.pop();
			// Extend
			for (auto&& [i, action] : tl::views::enumerate(m_actions)) {
				// Check if all preconditions are satisfied
				auto checker = [&cur_state](const Literal& t) {
					return cur_state.get(t.id) == t.polarity + 1;
				};
				if (!std::ranges::all_of(action.preconds, checker))
					continue;
				State next_state = cur_state;
				// Apply effects
				modify_state(next_state, action.effects);
				// Enqueue
				if (!close.contains(next_state)) {
					open.push(nodes.size());
					nodes.emplace_back(next_state, i, cur_idx);
					close.insert(next_state);
					graph[cur_state][i] = next_state;
				}
			}
		}
		return graph;
	}

} // namespace qy::ai
