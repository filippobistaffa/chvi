from scipy.spatial import ConvexHull
import numpy as np


def non_dominated(solutions, verbose=False):
    is_efficient = np.ones(solutions.shape[0], dtype=bool)
    for i, c in enumerate(solutions):
        if is_efficient[i]:
            # Remove dominated points, will also remove itself
            dominated_points = (np.asarray(solutions[is_efficient]) <= c).all(axis=1)
            is_efficient[is_efficient] = np.invert(dominated_points)
            # keep the point itself, otherwise we would get an empty list
            is_efficient[i] = 1
    return solutions[is_efficient]


def get_hull(points, CCS=True):
    #if CCS:
    #    points = non_dominated(np.array(points), verbose=True)
    try:
        hull = ConvexHull(points)
        hull_points = [points[vertex] for vertex in hull.vertices]
    except:
        hull_points = points
    if CCS:
        vertices = non_dominated(np.array(hull_points))
    else:
        vertices = hull_points
    return np.array(vertices)


def translate_hull(point, gamma, hull):
    if len(hull) == 0:
        hull = [point]
    else:
        hull = np.multiply(hull, gamma, casting="unsafe")
        if len(point) > 0:
            hull = np.add(hull, point, casting="unsafe")
    return hull
