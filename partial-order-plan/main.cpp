#include "planning.hpp"
#include "planning-o.hpp"
#include <fmt/ranges.h>
#include <fstream>

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
	}
	return 0;
}