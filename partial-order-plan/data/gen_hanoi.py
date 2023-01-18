from itertools import product
import json
from pathlib import Path
from pprint import pprint


def gen_hanoi(n: int):
    actions = []
    data = {
        "init": [f"{i}A" for i in range(n)],
        "goal": [f"{i}C" for i in range(n)],
        "actions": actions,
        "initFlag": True,
        "goalFlag": True,
        "backwardStrictEqual": False,
    }
    for i in range(n):
        for x, y in product(("A", "B", "C"), ("A", "B", "C")):
            if x == y: continue
            actions.append({
                "name": f"{i}:{x}->{y}",
                "preconds": {
                    f"{i}{x}": True,
                    **{f"{j}{x}": False
                       for j in range(i)},
                    **{f"{j}{y}": False
                       for j in range(i)},
                },
                "effects": {
                    f"{i}{x}": False,
                    f"{i}{y}": True,
                },
            })
    # pprint(data, width=120, sort_dicts=False)
    (Path(__file__).absolute().parent / f"hanoi{n}.json").write_text(json.dumps(data))


for i in range(1, 11):
    gen_hanoi(i)