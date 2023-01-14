#pragma once
#include "basic-types.hpp"
#include <map>
#include <set>
#include <filesystem>

namespace fs = std::filesystem;

class PartialOrderPlanning
{
	using State = std::map<std::string, bool>;
	using ResultType = std::vector<std::string_view>;	// Type of search result

	struct Node {
		State state;
		int act;	// Index of preformed action
		int prev;	// Index of previous node
	};

public:
	/// @brief Read planning task from JSON file.
	/// @param file Path of the file.
	void read_task(const fs::path& file);

	ResultType forward_search();
	ResultType backward_search();

private:
	void modify_state(State& state, const LiteralList& mods);

	State m_init_state;
	State m_goal_state;
	std::vector<Action> m_actions;
};