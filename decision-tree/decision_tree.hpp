#pragma once
#include "example.hpp"
#include "dataset.hpp"
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <functional>
#include <cmath>

namespace qy::ai {

	double binary_entropy(double p) {
		return -p * std::log(p + 1e-9) - (1 - p) * std::log(1 - p + 1e-9);
	}

	attr_list remove_attribute(attr_list attributes, attr_t attribute) {
		std::erase(attributes, attribute);
		return attributes;
	}

	class DecisionTree {

	public:
		struct Node {

			Node(attr_t attribute): Node(attribute, false) {}
			Node(attr_t attribute, bool value): attribute(std::move(attribute)), value(value) {}

			~Node() {
				for (auto&& [a, c] : options)
					delete c;
			}

			attr_t attribute;
			bool value; // 当对应的option为null时选择的值
			std::unordered_map<attr_t, Node*> options;
		};

		DecisionTree(const Dataset& dataset) {
			_root = learn(dataset.attributes, dataset.examples);
		}

		~DecisionTree() {
			delete _root;
		}

		const Node* root() const { return _root; }

		bool classify(const Example& example) const {
			const Node* root = _root;
			while (!root->options.empty()) {
				auto&& a = example.data.at(root->attribute);
				if (!root->options.contains(a))
					return root->value;
				root = root->options.at(a);
			}
			return root->value;
		}

		std::vector<int> classify(const ExampleList& examples) const {
			std::vector<int> result;
			for (auto&& e : examples)
				result.push_back(classify(e));
			return result;
		}

	private:

		double entropy_remain(attr_t attribute, const ExampleList& examples) const {
			double result = 0.0;
			for (auto&& [option, exs] : group_by(examples, attribute)) {
				int pos = get_positive_count(exs), neg = exs.size() - pos;
				double p_k = (double)exs.size() / examples.size();
				double e_k = binary_entropy((double)pos / (pos + neg));
				result += p_k * e_k;
			}
			return result;
		}

		double info_gain(attr_t attribute, const ExampleList& examples) const {
			int p = get_positive_count(examples), n = examples.size();
			return binary_entropy((double)p / (p + n)) - entropy_remain(attribute, examples);
		}

		attr_t importance(const attr_list& attributes, const ExampleList& examples) const {
			return std::ranges::max(attributes, {}, [&](auto&& s) { return info_gain(s, examples); });
		}

		Node* learn(const attr_list& attributes, const ExampleList& examples) const {
			if (attributes.empty()) {
				return plurality_value(examples);
			} else if (have_same_class(examples)) {
				return new Node(examples[0].raw_label, examples[0].label);
			} else {
				attr_t A = importance(attributes, examples); // Most important attribute
				Node* tree = new Node(A);
				for (auto&& [v_k, exs] : group_by(examples, A)) {
					tree->options[v_k] = learn(remove_attribute(attributes, A), exs);
				}
				tree->value = plurality_value(examples);
				return tree;
			}
		}

	private:
		static Node* plurality_value(const ExampleList& examples) {
			int pos = get_positive_count(examples);
			int neg = examples.size() - pos;
			return new Node(examples[0].raw_label, pos >= neg);
		}

	private:
		Node* _root;
	};


} // namespace qy::ai
