/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef _RBTREE_H
#define _RBTREE_H

#include <stddef.h>

enum rbtree_color { RB_RED, RB_BLACK };

struct rbnode {
    struct rbnode *parent;
    struct rbnode *left;
    struct rbnode *right;
    enum rbtree_color color;
};

typedef int (*rbcmp_f)(const struct rbnode *a, const struct rbnode *b);

struct rbtree {
    struct rbnode *root;
    rbcmp_f cmp;
};

/* Tree operations */
/* Initialize an empty tree with the provided comparator. */
void rbtree_init(struct rbtree *tree, rbcmp_f cmp);
/* Insert node into the tree. Nodes must remain valid while stored. */
void rbtree_insert(struct rbtree *tree, struct rbnode *node);
/* Remove node from the tree. Caller releases storage. */
void rbtree_remove(struct rbtree *tree, struct rbnode *node);
/* Find a node equal to key using the tree comparator. */
struct rbnode *rbtree_find(const struct rbtree *tree, const struct rbnode *key);

/* Iterator operations */
/* Return the minimum element or NULL if the tree is empty. */
struct rbnode *rbtree_min(const struct rbtree *tree);
/* Return the maximum element or NULL if the tree is empty. */
struct rbnode *rbtree_max(const struct rbtree *tree);
/* Return the in-order successor or NULL. */
struct rbnode *rbtree_next(const struct rbnode *node);
/* Return the in-order predecessor or NULL. */
struct rbnode *rbtree_prev(const struct rbnode *node);

#ifndef container_of
    #define container_of(A, T, M) ((T *)((((size_t)(A)) - offsetof(T, M))))
#endif

#endif /* _RBTREE_H */
