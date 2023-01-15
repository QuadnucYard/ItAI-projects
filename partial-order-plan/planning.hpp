#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <filesystem>
#include "quatset.hpp"

namespace fs = std::filesystem;

namespace qy::ai
{
	struct Literal {
		int id;
		bool polarity;
		bool operator== (const Literal& o) const = default;
	};

	using LiteralList = std::vector<Literal>;

	struct Action {
		std::string name;
		LiteralList preconds;
		LiteralList effects;
	};

	class PartialOrderPlanning
	{
	public:
		using State = quatset;
		using ResultType = std::vector<std::string_view>;	// Type of search result
		using GraphType = std::map<State, std::map<int, State>>;

		struct Node {
			State state;
			int act;	// Index of preformed action
			int prev;	// Index of previous node
		};

		int literal2id(const std::string& s);
		void modify_state(State& state, const LiteralList& mods) const;
		std::string state_to_string(const State& state) const;
		const Action& get_action(int i) const;

		/// @brief Read planning task from JSON file.
		/// @param file Path of the file.
		void read_task(const fs::path& file);

		ResultType forward_search() const;
		ResultType backward_search() const;
		GraphType forward_search_g() const;

	private:
		State m_init_state;
		State m_goal_state;
		std::vector<Action> m_actions;

		std::vector<std::string> m_literals;
	};

} // namespace qy::ai
