#pragma once
#include "example.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace qy::ai {

	class Dataset {
	public:
		Dataset(const fs::path& path, bool labeled) : _labeled(labeled) {
			std::ifstream inFile(path);
			if (inFile.fail()) {
				throw std::runtime_error("Cannot read file: " + path.string());
			}
			std::string line;
			std::getline(inFile, line);
			this->attributes = get_values(line);
			for (int row = 1; std::getline(inFile, line); row++) {
				this->raw_values.push_back(get_values(line));
				this->examples.push_back(build_example(get_values(line)));
			}
			if (labeled) this->attributes.pop_back();
		}

	private:
		std::vector<std::string> get_values(const std::string& line) const {
			std::stringstream ss(line);
			std::string value;
			std::vector<std::string> values;
			while (std::getline(ss, value, ',')) {
				values.push_back(value);
			}
			return values;
		}

		bool get_label(const std::string& value) const {
			return value == "1";
		}

		Example build_example(std::vector<std::string> values) const {
			attr_t raw_label;
			bool label = false;
			if (_labeled) {
				label = get_label(values.back());
			}
			attr_map value_map;
			for (size_t i = 0; i < values.size() - label; i++)
				value_map[attributes[i]] = values[i];
			return Example(std::move(value_map), _labeled ? attributes.back() : attr_t{}, label);
		}

	public:
		std::vector<std::string> attributes;
		std::vector<std::vector<std::string>> raw_values;
		std::vector<Example> examples;
		bool _labeled;
	};
}