import json
import re


def convert_impl(a, rule):
    if rule in (int, float, bool, str):
        return a
    elif rule == dict:
        return dict(a)
    elif rule == list:
        return list(a)
    elif isinstance(rule, list):
        return [convert_impl(x, rule[0]) for x in a]
    elif isinstance(rule, dict):
        return {k: convert_impl(x, v) for x, (k, v) in zip(a, rule.items())}
    else:
        print(a, rule)


def convert(source: str, rule):
    source = source.replace("{", "[").replace("}", "]")
    source = re.sub(r",\s*]", "]", source)
    try:
        a = json.loads(source)
        return convert_impl(a, rule)
    except Exception as e:
        print(e)
        print(source)
