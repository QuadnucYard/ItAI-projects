#include "planning.hpp"
#include "planning-o.hpp"
#include <fmt/ranges.h>
#include <fstream>

using namespace qy::ai;

void print_graph(const PartialOrderPlanning& p, const PartialOrderPlanning::GraphType& graph) {
	for (auto&& [u, e] : graph) {
		fmt::print("({}) ->\n", p.state_to_string(u));
		for (auto&& [a, v] : e)
			fmt::print("  [{}] ({})\n", p.get_action(a).name, p.state_to_string(v));
	}
}

int main() {
	fs::current_path(fs::absolute(fs::path(__FILE__).parent_path()));

	PartialOrderPlanningO planning_o;
	planning_o.read_task();
	auto ans0 = planning_o.forward_search();
	fmt::print("{}\n", ans0);

	std::ifstream ftasks("data/tasks.txt");
	std::string task;
	while (std::getline(ftasks, task)) {
		PartialOrderPlanning planning;
		planning.read_task(fs::path("data") / task);
		auto ans1 = planning.forward_search();
		fmt::print("{}\n", ans1);
		auto ans2 = planning.backward_search();
		fmt::print("{}\n", ans2);
		print_graph(planning, planning.forward_search_g());
	}
	return 0;
}