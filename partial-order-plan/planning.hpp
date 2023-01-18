#pragma once
#include "quatset.hpp"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

namespace qy::ai
{
	struct Action {
		std::string name;
		quatset preconds;
		quatset preconds_mask;
		quatset effects;
		quatset effects_mask;
	};

	class PartialOrderPlanning
	{
	public:
		using State = quatset;
		using ResultType = std::vector<int>;	// Type of search result
		using GraphType = std::map<State, std::map<int, State>>;

		struct Node {
			State state;
			int act;	// Index of preformed action
			int prev;	// Index of previous node
		};

		int literal2id(const std::string& s);
		std::string state_to_string(const State& state) const;

		inline const Action& get_action(int i) const { return m_actions[i]; }
		inline State init_state() const { return m_init_state; }
		inline State goal_state() const { return m_goal_state; }

		/// @brief Read planning task from JSON file.
		/// @param file Path of the file.
		void read_task(const fs::path& file);

		std::optional<ResultType> forward_search() const;
		std::optional<ResultType> backward_search() const;
		GraphType forward_search_g() const;

	private:
		ResultType backtrack(const std::vector<Node>& nodes, int cur_idx, bool reversed = false) const;

		State m_init_state;
		State m_goal_state;
		std::vector<Action> m_actions;
		bool m_init_omit;
		bool m_goal_omit;
		bool m_backward_strict;

		std::vector<std::string> m_literals;
	};

} // namespace qy::ai
