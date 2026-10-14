#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "slots.h"

struct solver {
    struct slot *slots;
    size_t count;
    bool (*edges)[SLOT_LIMIT];
    int indegree[SLOT_LIMIT], color[SLOT_LIMIT], path[SLOT_LIMIT];
    int order[SLOT_LIMIT], norder;
    int64_t latest[SLOT_LIMIT];
    char *error;
    size_t size;
};

static bool
visit(struct solver *s, int v, int depth)
{
    if (s->color[v] == 2)
        return true;
    if (s->color[v] == 1) {
        size_t used =
            (size_t)snprintf(s->error, s->size, "slot dependency cycle: ");
        int start = 0;
        while (s->path[start] != v)
            start++;
        for (int i = start; i <= depth && used < s->size; i++) {
            int node = i == depth ? v : s->path[i];
            used +=
                (size_t)snprintf(s->error + used, s->size - used, "%s%s",
                                 i == start ? "" : " -> ", s->slots[node].name);
        }
        return false;
    }
    s->color[v]    = 1;
    s->path[depth] = v;
    for (size_t j = 0; j < s->count; j++)
        if (s->edges[v][j] && !visit(s, (int)j, depth + 1))
            return false;
    s->color[v]           = 2;
    s->order[s->norder++] = v; /* Reverse topological order. */
    return true;
}

static bool search(struct solver *s, size_t done, int64_t position);

static bool
assign(struct solver *s, int v, size_t done, int64_t position)
{
    s->slots[v].number = (int)position;
    for (size_t j = 0; j < s->count; j++)
        if (s->edges[v][j])
            s->indegree[j]--;
    if (search(s, done + 1, position + 1))
        return true;
    for (size_t j = 0; j < s->count; j++)
        if (s->edges[v][j])
            s->indegree[j]++;
    s->slots[v].number = -1;
    return false;
}

/* Unit-size slots, precedence constraints and exact fixed positions. Deadline
 * ordering is a heuristic only: backtracking preserves completeness when a
 * particular topological order cannot fit between forced numbers. */
static bool
search(struct solver *s, size_t done, int64_t position)
{
    if (done == s->count)
        return true;
    if (position > INT_MAX)
        return false;
    int forced = -1, choices[SLOT_LIMIT], count = 0;
    int64_t next_fixed = (int64_t)INT_MAX + 1;
    for (size_t i = 0; i < s->count; i++) {
        if (s->slots[i].number >= 0)
            continue;
        if (s->latest[i] < position)
            return false;
        /* A deadline interval must have enough remaining integer positions. */
        int demand = 0;
        for (size_t j = 0; j < s->count; j++)
            if (s->slots[j].number < 0 && s->latest[j] <= s->latest[i])
                demand++;
        if (demand > s->latest[i] - position + 1)
            return false;
        if (s->slots[i].forced == position)
            forced = (int)i;
        if (s->slots[i].forced > position && s->slots[i].forced < next_fixed)
            next_fixed = s->slots[i].forced;
        if (!s->indegree[i] && s->slots[i].forced < 0) {
            int k = count++;
            while (k && (s->latest[choices[k - 1]] > s->latest[i] ||
                         (s->latest[choices[k - 1]] == s->latest[i] &&
                          strcmp(s->slots[choices[k - 1]].name,
                                 s->slots[i].name) > 0))) {
                choices[k] = choices[k - 1];
                k--;
            }
            choices[k] = (int)i;
        }
    }
    if (forced >= 0)
        return !s->indegree[forced] && assign(s, forced, done, position);
    for (int i = 0; i < count; i++)
        if (assign(s, choices[i], done, position))
            return true;
    if (!count && next_fixed <= INT_MAX)
        return search(s, done, next_fixed);
    return false;
}

int
slots_resolve(struct slot *slots, size_t count,
              bool edges[SLOT_LIMIT][SLOT_LIMIT], char *error, size_t size)
{
    struct solver s = {.slots = slots,
                       .count = count,
                       .edges = edges,
                       .error = error,
                       .size  = size};
    for (size_t i = 0; i < count; i++) {
        slots[i].number = -1;
        for (size_t j = 0; j < i; j++)
            if (slots[i].forced >= 0 && slots[i].forced == slots[j].forced) {
                snprintf(error, size, "slots %s and %s both force number %d",
                         slots[i].name, slots[j].name, slots[i].forced);
                return -1;
            }
        if (!visit(&s, (int)i, 0))
            return -1;
    }
    for (int i = 0; i < s.norder; i++) {
        int v       = s.order[i];
        s.latest[v] = slots[v].forced >= 0 ? slots[v].forced : INT_MAX;
        for (size_t j = 0; j < count; j++) {
            if (!edges[v][j])
                continue;
            s.indegree[j]++;
            if (s.latest[v] >= s.latest[j])
                s.latest[v] = s.latest[j] - 1;
        }
        if (slots[v].forced >= 0 && s.latest[v] < slots[v].forced) {
            snprintf(error, size,
                     "forced number %d for %s contradicts its successors",
                     slots[v].forced, slots[v].name);
            return -1;
        }
    }
    if (!search(&s, 0, 1)) {
        snprintf(error, size,
                 "cannot fit slots between forced numbers while satisfying "
                 "precedence");
        return -1;
    }
    return 0;
}
