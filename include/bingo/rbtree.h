/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
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
void rbtree_init(struct rbtree *tree, rbcmp_f cmp);
void rbtree_insert(struct rbtree *tree, struct rbnode *node);
void rbtree_remove(struct rbtree *tree, struct rbnode *node);
struct rbnode *rbtree_find(const struct rbtree *tree, const struct rbnode *key);

/* Iterator operations */
struct rbnode *rbtree_min(const struct rbtree *tree);
struct rbnode *rbtree_max(const struct rbtree *tree);
struct rbnode *rbtree_next(const struct rbnode *node);
struct rbnode *rbtree_prev(const struct rbnode *node);

#ifndef container_of
    #define container_of(A, T, M) ((T *)((((size_t)(A)) - offsetof(T, M))))
#endif

#endif /* _RBTREE_H */
