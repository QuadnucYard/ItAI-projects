import json
from converter import convert

rules = {
    "init": list,
    "goal": list,
    "actions": [{
        "name": str,
        "preconds": dict,
        "effects": dict,
    }]
}

r = convert(
    '''{
    { "AFA", "AST" },
    { "ASA" },
    {
		{"RST", {{"AST", true}}, {{"AST", false}, {"ASG", true}}},
		{"RFA", {{"AFA", true}}, {{"AFA", false}, {"AFG", true}}},
		{"PSA", {{"ASG", true}, {"AFA", false}}, {{"ASG", false}, {"ASA", true}}},
		{"LO", {}, {{"ASG", false}, {"ASA", false}, {"AST", false}, {"AFG", false}, {"AFA", false}}},
	}
}''', rules
)
print(json.dumps(r))

r = convert(
    '''{
    {},
    { "RightShoeOn", "LeftShoeOn", },
    {
		{"RightShoe", {{"RightSockOn", true}}, {{"RightShoeOn", true}}},
		{"RightSock", {}, {{"RightSockOn", true}}},
		{"LeftShoe", {{"LeftSockOn", true}}, {{"LeftShoeOn", true}}},
		{"LeftSock", {}, {{"LeftSockOn", true}}},
	}
}''', rules
)
print(json.dumps(r))

r = convert(
    '''{
    { "HaveCake" },
    { "HaveCake", "EatenCake" },
    {
		{"EatCake", {{"HaveCake", true}}, {{"HaveCake", false}, {"EatenCake", true}}},
		{"BakeCake", {{"HaveCake", false}}, {{"HaveCake", true}}},
	}
}''', rules
)
print(json.dumps(r))