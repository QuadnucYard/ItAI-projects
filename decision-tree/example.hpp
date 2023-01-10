#pragma once
#include <vector>
#include <map>
#include <algorithm>
#include <string>

namespace qy::ai {

	//using attr_t = int;
	//using attr_list = std::vector<attr_t>;
	using attr_t = std::string;
	using attr_list = std::vector<attr_t>;
	using attr_map = std::map<attr_t, attr_t>;

	struct Example {
		Example() = default;
		Example(attr_map data, attr_t raw_label, bool label) :
			data(std::move(data)), raw_label(std::move(raw_label)), label(label) {}

		attr_map data;
		attr_t raw_label;
		bool label;
	};

	using ExampleList = std::vector<Example>;

	int get_positive_count(const ExampleList& examples) {
		return std::ranges::count(examples, true, &Example::label);
	}

	bool have_same_class(const ExampleList& examples) {
		return std::ranges::all_of(examples, [label = examples[0].label](auto const& e) { return e.label == label; });
	}

	std::map<attr_t, ExampleList> group_by(const ExampleList& examples, attr_t attribute) {
		std::map<attr_t, ExampleList> result;
		for (auto&& e : examples) {
			result[e.data.at(attribute)].push_back(e);
		}
		return result;
	}

	std::vector<int> get_labels(const ExampleList& examples) {
		std::vector<int> result;
		for (auto&& e : examples) result.push_back(e.label);
		return result;
	}

} // namespace qy::ai
