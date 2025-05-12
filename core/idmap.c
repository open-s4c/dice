/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include <bingo/compiler.h>
#include <bingo/idmap.h>
#include <bingo/log.h>
#include <bingo/mempool.h>

#define IDMAP_INIT_SIZE 4

void
idmap_init_cap(idmap_t *idmap, size_t cap)
{
    assert(idmap);
    *idmap = (idmap_t){
        .items    = (uint64_t *)mempool_alloc(sizeof(uint64_t) * cap),
        .capacity = cap,
        .size     = 0,
    };
}

void
idmap_init(idmap_t *idmap)
{
    idmap_init_cap(idmap, IDMAP_INIT_SIZE);
}

void
idmap_fini(idmap_t *idmap)
{
    assert(idmap);
    if (idmap->items)
        mempool_free(idmap->items);
    memset(idmap, 0, sizeof(idmap_t));
}

size_t
idmap_size(idmap_t *idmap)
{
    return idmap->size;
}

static inline size_t
_idmap_find(idmap_t *idmap, uint64_t id, size_t *empty)
{
    assert(idmap);
    assert(idmap->size == 0 || idmap->items);
    if (empty)
        *empty = idmap->size;

    for (size_t i = 0; i < idmap->size; i++) {
        if (idmap->items[i] == id)
            return i;
        if (empty && idmap->items[i] == 0)
            *empty = i;
    }
    return idmap->size;
}

size_t
idmap_find(idmap_t *idmap, uint64_t id)
{
    return _idmap_find(idmap, id, 0);
}


size_t
idmap_insert(idmap_t *idmap, uint64_t id)
{
    size_t next;
    size_t idx = _idmap_find(idmap, id, &next);
    if (idx < idmap->size)
        return idx;
    if (next < idx) {
        idmap->items[next] = id;
        return next;
    }
    if (idx >= idmap->capacity)
        return idx;
    idmap->items[idx] = id;
    return idmap->size++;
}

size_t
idmap_remove(idmap_t *idmap, uint64_t id)
{
    size_t idx = idmap_find(idmap, id);
    if (idx == idmap->size)
        return idmap->size;
    idmap->items[idx] = 0;
    if (idx + 1 == idmap->size)
        idmap->size--;
    return idx;
}
