#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "slots.h"

#define N 4
static bool edges[SLOT_LIMIT][SLOT_LIMIT];
static struct slot slots[N];
static int order[N];

/* Independent oracle: enumerate every ordering, then place unpinned slots at
 * the earliest available position. Every solution has one of these orderings.
 */
static bool
possible(int depth, unsigned used)
{
    if (depth < N) {
        for (int i = 0; i < N; i++) {
            if (used & (1u << i))
                continue;
            order[depth] = i;
            if (possible(depth + 1, used | (1u << i)))
                return true;
        }
        return false;
    }
    int positions[N], number = 0;
    for (int i = 0; i < N; i++) {
        int v = order[i];
        number++;
        if (slots[v].forced >= 0) {
            if (slots[v].forced < number)
                return false;
            number = slots[v].forced;
        }
        positions[v] = number;
    }
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (edges[i][j] && positions[i] >= positions[j])
                return false;
    return true;
}

static void
check(bool condition, unsigned graph, int a, int b, const char *error)
{
    if (condition)
        return;
    fprintf(stderr, "graph=%u pins=%d,%d: %s\n", graph, a, b, error);
    exit(EXIT_FAILURE);
}

int
main(void)
{
    size_t checked = 0;
    /* Every directed graph on four nodes, including cycles, and every pair
     * of forced numbers on the first/last node (zero means unpinned). */
    for (unsigned graph = 0; graph < (1u << (N * (N - 1))); graph++) {
        unsigned bit = 0;
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                if (i != j)
                    edges[i][j] = (graph & (1u << bit++)) != 0;
        for (int a = 0; a <= 5; a++) {
            for (int b = 0; b <= 5; b++) {
                for (int i = 0; i < N; i++) {
                    snprintf(slots[i].name, NAME_SIZE, "slot%d", i);
                    slots[i].forced = -1;
                }
                slots[0].forced = a ? a : -1;
                slots[3].forced = b ? b : -1;
                bool expected   = possible(0, 0);
                char error[256] = "invalid resolved numbering";
                bool found =
                    slots_resolve(slots, N, edges, error, sizeof(error)) == 0;
                check(found == expected, graph, a, b, error);
                if (found) {
                    for (int i = 0; i < N; i++) {
                        check(slots[i].number > 0 &&
                                  (slots[i].forced < 0 ||
                                   slots[i].number == slots[i].forced),
                              graph, a, b, error);
                        for (int j = 0; j < N; j++) {
                            check(i == j || slots[i].number != slots[j].number,
                                  graph, a, b, error);
                            check(!edges[i][j] ||
                                      slots[i].number < slots[j].number,
                                  graph, a, b, error);
                        }
                    }
                }
                checked++;
            }
        }
    }
    printf("Checked %zu slot constraints against exhaustive search\n", checked);
    return 0;
}
