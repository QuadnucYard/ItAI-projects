#include "planning.hpp"
#include <algorithm>
#include <queue>
#include <fstream>
#include <tl/enumerate.hpp>
#include <nlohmann/json.hpp>

void PartialOrderPlanning::read_task(const fs::path& file)
{
	using json = nlohmann::json;
	json root = json::parse(std::ifstream(file));
	for (auto&& v : root["init"])
		m_init_state.emplace(v.get<std::string>(), true);
	for (auto&& v : root["goal"])
		m_goal_state.emplace(v.get<std::string>(), true);
	for (auto&& v : root["actions"]) {
		m_actions.emplace_back(v["name"].get<std::string>(), LiteralList{}, LiteralList{});
		for (auto&& [k, v] : v["preconds"].items())
			m_actions.back().preconds.emplace_back(k, v.get<bool>());
		for (auto&& [k, v] : v["effects"].items())
			m_actions.back().effects.emplace_back(k, v.get<bool>());
	}
}

void PartialOrderPlanning::modify_state(State& state, const LiteralList& mods)
{
	for (auto&& l : mods)
		state[l.name] = l.polarity;
}

PartialOrderPlanning::ResultType PartialOrderPlanning::forward_search()
{
	std::vector<Node> nodes{ {m_init_state, -1, -1} }; // Store all generated nodes

	std::queue<int> open{ {0} };
	std::set<State> close{ nodes[0].state };

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
			// Check if all preconditions are satisfied
			auto checker = [&cur_state](const Literal& t) {
				auto it = cur_state.find(t.name);
				return it != cur_state.end() && it->second == t.polarity;
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

PartialOrderPlanning::ResultType PartialOrderPlanning::backward_search()
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
				if (auto it = prev_state.find(e.name);it != prev_state.end())
					if (it->second == e.polarity)
						prev_state.erase(it);
					else
						return false;
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

std::string PartialOrderPlanning::state_to_string(const State& state) {
	std::string s;
	for (auto&& [k, v] : state) {
		if (v) s += '+';
		else s += '-';
		s += k;
	}
	return s;
}

#include <iostream>

void PartialOrderPlanning::forward_search_g()
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
				auto it = cur_state.find(t.name);
				return it != cur_state.end() && it->second == t.polarity;
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
	// Print graph
	for (auto&& [u, e] : graph) {
		std::cout << "(" << state_to_string(u) << ") ->\n";
		for (auto&& [a, v] : e)
			std::cout << "  [" << m_actions[a].name << "] (" << state_to_string(v) << ")\n";
	}
	return;
}