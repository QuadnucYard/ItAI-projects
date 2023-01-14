#pragma once
#include <string>
#include <vector>

struct Literal {
	std::string name;
	bool polarity;
	bool operator== (const Literal& o) const = default;
};

using LiteralList = std::vector<Literal>;

struct Action {
	std::string name;
	LiteralList preconds;
	LiteralList effects;
};