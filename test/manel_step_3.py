from mip import Model, xsum, maximize


def minimal_weight_computation(hull, chosen_index, verbose=False, individual_weight=0, epsilon=0.001):

    return minimal_weight_computation_all_states(hull, [chosen_index], verbose, None, individual_weight, epsilon)


def minimal_weight_computation_all_states(hull, chosen_indexes, verbose=False, chosen_states=None, individual_weight=1, epsilon=0.001):

    chosen_hulls = list()

    if chosen_states is not None:
        I = range(len(hull[chosen_states[0]][0]))

        for state in chosen_states:
            chosen_hulls.append(hull[state])

        J = range(len(chosen_indexes))
    else:
        I = range(len(hull[0]))
        chosen_hulls.append(hull)
        J = [0]

    m = Model("knapsack")

    if not verbose:
        m.verbose = 0
    w = [m.add_var() for _ in I]

    m.objective = maximize(xsum(xsum(-chosen_hulls[j][chosen_indexes[j]][i] * w[i] for i in I) for j in J))

    for k in range(len(chosen_indexes)):

        if chosen_states is not None:
            hull_state = hull[chosen_states[k]]
        else:
            hull_state = chosen_hulls[0]

        chosen_point = hull_state[chosen_indexes[k]]

        for j in range(len(hull_state)):
            if j != chosen_indexes[k]:
                m += xsum(w[i] * hull_state[j][i] for i in I) + epsilon <= xsum(w[i] * chosen_point[i] for i in I)

    m += w[individual_weight] == 1

    for weight in w:
        m += weight >= epsilon
    m.optimize()

    #selected = [i for i in I]
    #print("selected items: {}".format(selected))
    minimal_weights = [w[i].x for i in I]

    return minimal_weights


if __name__ == "__main__":

    hull = [[1, 5, 4, 2],
            [7, 1, 1, 8],
            [3, 4, 3, 8],
            [3, 5, 4, 1],
            [5, 1, 5, 6],
            [2, 4, 3, 8]]

    lex_index = 2
    weights = minimal_weight_computation(hull, lex_index) # weights for which the point at lex_index is the optimum
    print("---")
    print(weights)


    ## Now we prove that indeed that is the correct solution
    weights[1] = 1.34
    weights[2] = 0.00
    weights[3] = 0.20

    def dot_product(w, v):
        z = 0

        for i in range(len(v)):
            z += w[i] * v[i]

        return z

    for i in range(len(hull)):
        print(hull[i], dot_product(weights, hull[i]))

    print("---")