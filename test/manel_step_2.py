import numpy as np


def reverse_lex(lex):

    def find_n(n, lex):

        for i in range(len(lex)):
            if n == lex[i]:
                return i

    return [find_n(i, lex) for i in range(len(lex))]


def lex_max(hull, lex=0, iWantIndex=True):

    hull = np.array(hull)

    if lex == 0:
        # if no specified lexicographic ordering, it uses the following default one:
        n_objectives = len(hull[0])  # 3 objectives in this examples
        lex = [i - 1 for i in range(n_objectives, 0, -1)]  # ordering: obj_2 > obj_1 > obj_0

    ordered_hull = hull[:, lex]
    antilex = reverse_lex(lex)

    for i in range(len(lex)):
        first_max = np.max(ordered_hull, axis=0)[i]
        next_hull = []

        for j in ordered_hull:
            if j[i] >= first_max:
                next_hull.append(j)

        ordered_hull = next_hull

    lex_point = np.array(ordered_hull[0])
    lex_point = lex_point[antilex]

    if iWantIndex:
        for i in range(len(hull)):
            its_the_same = True
            for j in range(len(lex_point)):
                its_the_same *= lex_point[j] == hull[i][j]

            if its_the_same:
                return i

    return lex_point


if __name__ == "__main__":

    ## Example computation of STEP 2

    # example convex hull
    hull = [[5.75, 0., 0., ],
                [5.9375, 0., -0.9375],
                [6.3125, 0., -3.4375],
                [6.5, 0., -5.3125],
                [6.75, 0., -9.0625],
                [7., 0., -13.75],
                [8.25, -10., 0., ],
                [8.75, -10., -2.5],
                [9., -25., 0., ],
                [9., -10., -5., ],
                [9.125, -30., 0., ],
                [9.5, -30., -1.25],
                [9.75, -30., -2.5],
                [10., -30., -5., ]]


    # By default the algorithm uses the ordering: obj_2 > obj_1 > obj_0, which is OK for our experiments.

    lex_index = lex_max(hull)  # compute the INDEX of the point that maximises the lex_order

    print(lex_index)
