#pragma GCC optimize(3)
#include "planning.hpp"
#include "planning-o.hpp"
#include <fmt/ranges.h>
#include <fmt/color.h>
#include <fstream>
#include <ranges>

using namespace qy::ai;

void print_graph(const PartialOrderPlanning& p, const PartialOrderPlanning::GraphType& graph) {
	for (auto&& [u, e] : graph) {
		fmt::print("({}) ->\n", p.state_to_string(u));
		for (auto&& [a, v] : e)
			fmt::print("  [{}] ({})\n", p.get_action(a).name, p.state_to_string(v));
	}
}

void print_actions(const PartialOrderPlanning& p, const PartialOrderPlanning::ResultType& result) {
	fmt::print("{}\n", result | std::views::transform([&p](auto&& t) { return p.get_action(t).name; }));
}

void print_actions(const PartialOrderPlanning& p, const std::optional<PartialOrderPlanning::ResultType>& result) {
	if (result)
		print_actions(p, result.value());
	else
		fmt::print("No result\n");
}

void print_chain(const PartialOrderPlanning& p, const PartialOrderPlanning::ResultType& result) {
	auto s = p.init_state();
	fmt::print("({})", p.state_to_string(s));
	for (auto&& i : result) {
		auto& a = p.get_action(i);
		s.modify(a.effects, a.effects_mask);
		fmt::print("->[{}]->({})", a.name, p.state_to_string(s));
	}
	fmt::print("\n");
}

int main() {
	fs::current_path(fs::absolute(fs::path(__FILE__).parent_path()));

	/* PartialOrderPlanningO planning_o;
	planning_o.read_task();
	auto ans0 = planning_o.forward_search();
	fmt::print("{}\n", ans0); */

	std::ifstream ftasks("data/tasks.txt");
	std::string task;
	while (std::getline(ftasks, task)) {
		fmt::print(fmt::fg(fmt::color::orange), "Task: {}\n", task);
		PartialOrderPlanning P;
		P.read_task(fs::path("data") / task);
		fmt::print("Perform forward...\n");
		auto ans1 = P.forward_search();
		fmt::print("Perform backward...\n");
		auto ans2 = P.backward_search();
		print_actions(P, ans1);
		// print_chain(P, ans1);
		print_actions(P, ans2);
		// print_chain(P, ans2);
		//print_graph(P, P.forward_search_g());
	}
	return 0;
}