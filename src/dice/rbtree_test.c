/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <stdio.h>
#include <string.h>

#include <dice/rbtree.h>

struct file_entry {
    struct rbnode path_node;
    struct rbnode size_node;
    const char *path;
    size_t size;
};

int
cmp_by_path(const struct rbnode *a, const struct rbnode *b)
{
    const struct file_entry *ea =
        container_of(a, const struct file_entry, path_node);
    const struct file_entry *eb =
        container_of(b, const struct file_entry, path_node);
    return strcmp(ea->path, eb->path);
}

int
cmp_by_size(const struct rbnode *a, const struct rbnode *b)
{
    const struct file_entry *ea =
        container_of(a, const struct file_entry, size_node);
    const struct file_entry *eb =
        container_of(b, const struct file_entry, size_node);
    return (ea->size > eb->size) - (ea->size < eb->size);
}

void
print_by_path(const struct rbtree *tree)
{
    for (struct rbnode *n = rbtree_min(tree); n; n = rbtree_next(n)) {
        const struct file_entry *e =
            container_of(n, const struct file_entry, path_node);
        printf("%s (size %zu)\n", e->path, e->size);
    }
}

int
main()
{
    struct rbtree path_tree, size_tree;
    rbtree_init(&path_tree, cmp_by_path);
    rbtree_init(&size_tree, cmp_by_size);

    struct file_entry entries[] = {
        {.path = "file1.txt", .size = 300},
        {.path = "file2.txt", .size = 200},
        {.path = "file3.txt", .size = 100},
    };

    for (int i = 0; i < 3; ++i) {
        rbtree_insert(&path_tree, &entries[i].path_node);
        rbtree_insert(&size_tree, &entries[i].size_node);
    }

    print_by_path(&path_tree);

    struct file_entry key = {.path = "file2.txt"};
    struct rbnode *found  = rbtree_find(&path_tree, &key.path_node);
    if (found) {
        struct file_entry *entry =
            container_of(found, struct file_entry, path_node);
        printf("--> %s\n", entry->path);
        rbtree_remove(&path_tree, &entry->path_node);
        rbtree_remove(&size_tree, &entry->size_node);
    }

    print_by_path(&path_tree);

    return 0;
}
