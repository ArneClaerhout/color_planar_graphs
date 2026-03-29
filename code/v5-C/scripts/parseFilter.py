import sympy
import json
import sys


def simplify(variable, n):
    returnvalue = sympy.sympify(variable).subs(dict(n=n))
    if (returnvalue < 0):
        raise Exception("Incorrect value given: < 0")
    return returnvalue


def to_nauty_args(filter):

    # First, we find the number of vertices
    try:
        n = filter.get("n")
        parts = n.split(":")
        if len(parts) == 1:
            start = end = int(parts[0])
        elif len(parts) == 2:
            start, end = map(int, parts)
        else:
            raise ValueError(f"Invalid range format: {n!r}")
    except:
        start = end = 0

    # Now, the other arguments
    args = []

    # We only check for special nauty options
    for i in range(start, end + 1):
        argsnew = []

        edges = filter.get("e", None)
        if edges is not None:
            argsnew.append(f"-e{simplify(edges, i)}")
            # We make it so the user can choose the number of edges in function of the number of vertices

        min_degree = filter.get("min_degree", None)
        if min_degree is not None and min_degree >= 0:
            argsnew.append(f"-d{min_degree}")

        max_degree = filter.get("max_degree", None)
        if max_degree is not None:
            argsnew.append(f"-D{simplify(max_degree, i)}")

        radius = filter.get("radius", None)
        if radius is not None and radius >= 0:
            argsnew.append(f"-z{radius}")

        diameter = filter.get("diameter", None)
        if diameter is not None and diameter >= 0:
            argsnew.append(f"-Z{diameter}")

        cycles = filter.get("cycles", None)
        if cycles is not None:
            argsnew.append(f"-Y{simplify(cycles, i)}")

        girth = filter.get("girth", None)
        if girth is not None and girth >= 0:
            argsnew.append(f"-g{girth}")

        triangles = filter.get("triangles", None)
        if triangles is not None:
            argsnew.append(f"-T{simplify(triangles, i)}")

        if filter.get("eulerian", None) is not None:
            argsnew.append("-E")

        args.append(argsnew)

    return range(start, end + 1), args


if __name__ == "__main__":

    try:
        # Open the file in read mode
        with open(f'{sys.argv[1]}', 'r') as filter:
            # Parse the JSON file content into directory
            data = json.load(filter)
    except FileNotFoundError:
        raise Exception("The file was not found.")
    except json.JSONDecodeError:
        raise Exception("The file contains invalid JSON.")

    for i, filter in enumerate(data):
        n, args = to_nauty_args(data.get(filter))
        for index, arg in enumerate(args):
            print(n[index], " ".join(arg))
