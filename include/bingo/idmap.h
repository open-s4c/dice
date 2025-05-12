/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_IDMAP_H
#define BINGO_IDMAP_H

#include <stddef.h>
#include <stdint.h>

typedef struct idmap {
    size_t capacity;
    size_t size;
    uint64_t *items;
} idmap_t;

void idmap_init_cap(idmap_t *idmap, size_t cap);
void idmap_init(idmap_t *idmap);
void idmap_fini(idmap_t *idmap);
size_t idmap_size(idmap_t *idmap);
size_t idmap_find(idmap_t *idmap, uint64_t id);
size_t idmap_insert(idmap_t *idmap, uint64_t id);
size_t idmap_remove(idmap_t *idmap, uint64_t id);

#endif /* BINGO_IDMAP_H */
