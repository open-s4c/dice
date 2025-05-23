/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <dice/compiler.h>
#include <dice/rbtree.h>

#define LEFT  0
#define RIGHT 1

static void
_rotate(struct rbtree *tree, struct rbnode *x, int dir)
{
    struct rbnode *y = (dir == LEFT) ? x->right : x->left;
    if (dir == LEFT) {
        x->right = y->left;
        if (y->left)
            y->left->parent = x;
        y->left = x;
    } else {
        x->left = y->right;
        if (y->right)
            y->right->parent = x;
        y->right = x;
    }

    y->parent = x->parent;
    if (!x->parent) {
        tree->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }

    x->parent = y;
}

static void
_insert_fixup(struct rbtree *tree, struct rbnode *z)
{
    while (z->parent && z->parent->color == RB_RED) {
        struct rbnode *gp    = z->parent->parent;
        int dir              = (z->parent == gp->left) ? LEFT : RIGHT;
        struct rbnode *uncle = (dir == LEFT) ? gp->right : gp->left;

        if (uncle && uncle->color == RB_RED) {
            z->parent->color = RB_BLACK;
            uncle->color     = RB_BLACK;
            gp->color        = RB_RED;
            z                = gp;
        } else {
            if ((dir == LEFT && z == z->parent->right) ||
                (dir == RIGHT && z == z->parent->left)) {
                z = z->parent;
                _rotate(tree, z, dir);
            }
            z->parent->color = RB_BLACK;
            gp->color        = RB_RED;
            _rotate(tree, gp, !dir);
        }
    }
    tree->root->color = RB_BLACK;
}

DICE_HIDE_IF void
rbtree_init(struct rbtree *tree, rbcmp_f cmp)
{
    tree->root = NULL;
    tree->cmp  = cmp;
}

DICE_HIDE_IF void
rbtree_insert(struct rbtree *tree, struct rbnode *z)
{
    struct rbnode **p = &tree->root, *parent = NULL;

    z->left = z->right = z->parent = NULL;
    z->color                       = RB_RED;

    while (*p) {
        parent = *p;
        if (tree->cmp(z, parent) < 0)
            p = &parent->left;
        else
            p = &parent->right;
    }

    z->parent = parent;
    if (!parent)
        tree->root = z;
    else if (tree->cmp(z, parent) < 0)
        parent->left = z;
    else
        parent->right = z;

    _insert_fixup(tree, z);
}

static struct rbnode *
_minimum(struct rbnode *node)
{
    while (node && node->left)
        node = node->left;
    return node;
}

static void
_transplant(struct rbtree *tree, struct rbnode *u, struct rbnode *v)
{
    if (!u->parent)
        tree->root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    if (v)
        v->parent = u->parent;
}

static void
_remove_fixup(struct rbtree *tree, struct rbnode *x, struct rbnode *x_parent)
{
    while ((!x || x->color == RB_BLACK) && x != tree->root) {
        int left         = (x == x_parent->left);
        struct rbnode *w = left ? x_parent->right : x_parent->left;

        if (w->color == RB_RED) {
            w->color        = RB_BLACK;
            x_parent->color = RB_RED;
            _rotate(tree, x_parent, left ? LEFT : RIGHT);
            w = left ? x_parent->right : x_parent->left;
        }

        if ((!w->left || w->left->color == RB_BLACK) &&
            (!w->right || w->right->color == RB_BLACK)) {
            w->color = RB_RED;
            x        = x_parent;
            x_parent = x->parent;
        } else {
            if ((left && (!w->right || w->right->color == RB_BLACK)) ||
                (!left && (!w->left || w->left->color == RB_BLACK))) {
                if (left && w->left)
                    w->left->color = RB_BLACK;
                if (!left && w->right)
                    w->right->color = RB_BLACK;
                w->color = RB_RED;
                _rotate(tree, w, left ? RIGHT : LEFT);
                w = left ? x_parent->right : x_parent->left;
            }

            w->color        = x_parent->color;
            x_parent->color = RB_BLACK;
            if (left && w->right)
                w->right->color = RB_BLACK;
            if (!left && w->left)
                w->left->color = RB_BLACK;
            _rotate(tree, x_parent, left ? LEFT : RIGHT);
            x = tree->root;
        }
    }

    if (x)
        x->color = RB_BLACK;
}

DICE_HIDE_IF void
rbtree_remove(struct rbtree *tree, struct rbnode *z)
{
    struct rbnode *y = z;
    struct rbnode *x;
    struct rbnode *x_parent;
    enum rbtree_color y_color_orig = y->color;

    if (!z->left) {
        x        = z->right;
        x_parent = z->parent;
        _transplant(tree, z, z->right);
    } else if (!z->right) {
        x        = z->left;
        x_parent = z->parent;
        _transplant(tree, z, z->left);
    } else {
        y            = _minimum(z->right);
        y_color_orig = y->color;
        x            = y->right;
        x_parent     = y->parent == z ? y : y->parent;
        if (y->parent != z) {
            _transplant(tree, y, y->right);
            y->right         = z->right;
            y->right->parent = y;
        }
        _transplant(tree, z, y);
        y->left         = z->left;
        y->left->parent = y;
        y->color        = z->color;
    }

    if (y_color_orig == RB_BLACK)
        _remove_fixup(tree, x, x_parent);
}

DICE_HIDE_IF struct rbnode *
rbtree_find(const struct rbtree *tree, const struct rbnode *key)
{
    struct rbnode *node = tree->root;
    while (node) {
        int cmp = tree->cmp(key, node);
        if (cmp == 0)
            return node;
        node = (cmp < 0) ? node->left : node->right;
    }
    return NULL;
}

DICE_HIDE_IF struct rbnode *
rbtree_min(const struct rbtree *tree)
{
    return _minimum(tree->root);
}

DICE_HIDE_IF struct rbnode *
rbtree_max(const struct rbtree *tree)
{
    struct rbnode *node = tree->root;
    while (node && node->right)
        node = node->right;
    return node;
}

DICE_HIDE_IF struct rbnode *
rbtree_next(const struct rbnode *node)
{
    if (node->right) {
        struct rbnode *n = node->right;
        while (n->left)
            n = n->left;
        return n;
    }

    struct rbnode *p = node->parent;
    while (p && node == p->right) {
        node = p;
        p    = p->parent;
    }
    return p;
}

DICE_HIDE_IF struct rbnode *
rbtree_prev(const struct rbnode *node)
{
    if (node->left) {
        struct rbnode *n = node->left;
        while (n->right)
            n = n->right;
        return n;
    }

    struct rbnode *p = node->parent;
    while (p && node == p->left) {
        node = p;
        p    = p->parent;
    }
    return p;
}
