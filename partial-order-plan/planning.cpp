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

	std::string PartialOrderPlanning::state_to_string(const State& state) const
	{
		std::string s;
		for (size_t i = 0; i < m_literals.size(); i++) {
			auto x = state.get(i);
			if (x == quatset::UNKNOWN) continue;
			if (x == quatset::TRUE) s += '+';
			else if (x == quatset::FALSE)s += '-';
			s += m_literals[i];
		}
		return s;
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
			auto& a = m_actions.emplace_back(v["name"].get<std::string>(), 0, 0, 0, 0);
			for (auto&& [k, v] : v["preconds"].items()) {
				int i = literal2id(k);
				a.preconds.setb(i, v.get<bool>());
				a.preconds_mask.set(i, quatset::BOTH);
			}
			for (auto&& [k, v] : v["effects"].items()) {
				int i = literal2id(k);
				a.effects.setb(i, v.get<bool>());
				a.effects_mask.set(i, quatset::BOTH);
			}
		}
		m_init_omit = !root.contains("initFlag") || root["initFlag"].get<bool>();
		m_goal_omit = root.contains("goalFlag") && root["goalFlag"].get<bool>();
		m_backward_strict = !root.contains("backwardStrictEqual") || root["backwardStrictEqual"].get<bool>();
	}

	PartialOrderPlanning::ResultType PartialOrderPlanning::backtrack(const std::vector<Node>& nodes, int cur_idx, bool reversed) const
	{
		ResultType ans;
		for (int i = cur_idx; i != 0; i = nodes[i].prev)
			ans.emplace_back(nodes[i].act);
		if (reversed)
			std::ranges::reverse(ans);
		return ans;
	}

	std::optional<PartialOrderPlanning::ResultType> PartialOrderPlanning::forward_search() const
	{
		// Full initial state
		State init_state = m_init_omit ? m_init_state.unk_as_false(m_literals.size()) : m_init_state;

		std::vector<Node> nodes{ {init_state, -1, -1} }; // Store all generated nodes

		std::queue<int> open{ {0} };
		std::set<State> close{ nodes[0].state };

		while (!open.empty()) {
			int cur_idx = open.front();
			State cur_state = nodes[cur_idx].state;
			open.pop();
			if (cur_state.includes(m_goal_state))
				return backtrack(nodes, cur_idx, true);
			// Extend
			for (auto&& [i, action] : tl::views::enumerate(m_actions)) {
				// Check if all preconditions are satisfied
				if (!cur_state.includes(action.preconds))
					continue;
				// Apply effects
				State next_state = cur_state.modified(action.effects, action.effects_mask);
				// Enqueue
				if (!close.contains(next_state)) {
					open.push(nodes.size());
					nodes.emplace_back(next_state, i, cur_idx);
					close.insert(next_state);
				}
			}
		}
		return std::nullopt;
	}

	std::optional<PartialOrderPlanning::ResultType> PartialOrderPlanning::backward_search() const
	{
		// Full goal state
		State goal_state = m_goal_omit ? m_goal_state.unk_as_false(m_literals.size()) : m_goal_state;

		std::vector<Node> nodes{ {goal_state, -1, -1} }; // Store all generated nodes

		std::queue<int> open{ {0} };
		std::set<State> close{ nodes[0].state };

		while (!open.empty()) {
			int cur_idx = open.front();
			State cur_state = nodes[cur_idx].state;
			open.pop();
			if ((m_backward_strict && cur_state == m_init_state) || (!m_backward_strict && cur_state.includes(m_init_state)))
				return backtrack(nodes, cur_idx, false);
			// Extend
			for (auto&& [i, action] : tl::views::enumerate(m_actions)) {
				// Check if all effects are established
				if ((cur_state & action.effects_mask) & ~action.effects)
					continue;
				// Remove established literals from previous state
				// Apply preconditions
				State prev_state = (cur_state & ~action.effects_mask).modified(action.preconds, action.preconds_mask);
				// Enqueue
				if (!close.contains(prev_state)) {
					open.push(nodes.size());
					nodes.emplace_back(prev_state, i, cur_idx);
					close.insert(prev_state);
				}
			}
		}
		return std::nullopt;
	}

	PartialOrderPlanning::GraphType PartialOrderPlanning::forward_search_g() const
	{
		State init_state = m_init_omit ? m_init_state.unk_as_false(m_literals.size()) : m_init_state;

		std::vector<Node> nodes{ {init_state, -1, -1} }; // Store all generated nodes

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
				if (!cur_state.includes(action.preconds))
					continue;
				// Apply effects
				State next_state = cur_state.modified(action.effects, action.effects_mask);
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
