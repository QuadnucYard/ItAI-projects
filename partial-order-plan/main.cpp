#pragma GCC optimize(3)
#include "planning.hpp"
#include "planning-o.hpp"
#include "../common/utils.hpp"
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
	for (std::string task; std::getline(ftasks, task); ) {
		fmt::print(fmt::fg(fmt::color::orange), "Task: {}\n", task);
		PartialOrderPlanning P;
		P.read_task(fs::path("data") / task);
		{
			fmt::print("Perform forward...\n");
			// auto ans1 = P.forward_search()
			auto t = timeit([&]() { return P.forward_search(); }); // Replace with backward
			fmt::print(fmt::fg(fmt::color::light_cyan), "[Result] ");
			fmt::print(fmt::fg(fmt::color::yellow_green), "Duration: {:.3f}ms ", t.count() / 1e6f);
			fmt::print(fmt::fg(fmt::color::slate_blue), "Nodes generated: {} ", P.log.nodes_generated);
			fmt::print(fmt::fg(fmt::color::steel_blue), "States generated: {}\n", P.log.nodes_generated);
		}
	}
	return 0;
}