#pragma once
#include "decision_tree.hpp"
#include <iostream>

namespace qy::ai {

	void print_attributes(const attr_list& attributes) {
		for (auto&& attribute : attributes) {
			std::cout << attribute << " ";
		}
		std::cout << std::endl;
	}

	void print_example(const Example& example) {
		std::cout << "data: ";
		for (auto&& [attr, value] : example.data) {
			std::cout << value << " ";
		}
		std::cout << " label: ";
		std::cout << example.label << std::endl;
	}

	void print_examples(const ExampleList& examples) {
		std::ranges::for_each(examples, print_example);
	}


	void print_spacer(size_t number) {
		for (size_t i = 0; i < number; i++) {
			std::cout << '\t';
		}
	}

	void print_tree(const DecisionTree::Node* root, int depth = 0) {
		if (root->options.empty()) {
			print_spacer(depth);
			std::cout << root->attribute << " = " << root->value << "\n";
			return;
		}
		print_spacer(depth);
		std::cout << "Split on " << root->attribute << " =>\n";
		for (auto it = root->options.begin(); it != root->options.end(); ++it) {
			print_spacer(depth);
			std::cout << "Case " << it->first << ":\n";
			print_tree(it->second, depth + 1);
		}
	}

	void print_tree(const DecisionTree& tree) {
		std::cout << "Decision Tree:\n";
		print_tree(tree.root(), 0);
	}

	double evaluate(const std::vector<int>& pred, const std::vector<int>& gt) {
		int correct = 0;
		for (size_t i = 0; i < pred.size(); i++) {
			if (pred[i] == gt[i]) correct++;
		}
		return (double)correct / pred.size();
	}

} // namespace qy::ai
