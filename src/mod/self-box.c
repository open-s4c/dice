/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */

#include <dice/self.h>
#include <dice/thread_id.h>
#include <dice/types.h>

thread_id self_id_(metadata_t *md);
DICE_HIDE thread_id
self_id(metadata_t *md)
{
    return self_id_(md);
}

bool self_retired_(metadata_t *md);
DICE_HIDE bool
self_retired(metadata_t *md)
{
    return self_retired_(md);
}

void *self_tls_get_(metadata_t *md, uintptr_t item_key);
DICE_HIDE void *
self_tls_get(metadata_t *md, uintptr_t item_key)
{
    return self_tls_get_(md, item_key);
}

void self_tls_set_(metadata_t *md, uintptr_t item_key, void *ptr,
                   struct tls_dtor dtor);
DICE_HIDE void
self_tls_set(metadata_t *md, uintptr_t item_key, void *ptr,
             struct tls_dtor dtor)
{
    self_tls_set_(md, item_key, ptr, dtor);
}

void *self_tls_(metadata_t *md, const void *global, size_t size);
DICE_HIDE void *
self_tls(metadata_t *md, const void *global, size_t size)
{
    return self_tls_(md, global, size);
}
