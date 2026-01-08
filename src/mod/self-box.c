/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */

#include <dice/self.h>
#include <dice/types.h>

thread_id self_id_(struct metadata *md);
DICE_HIDE thread_id
self_id(struct metadata *md)
{
    return self_id_(md);
}

bool self_retired_(struct metadata *md);
DICE_HIDE bool
self_retired(struct metadata *md)
{
    return self_retired_(md);
}

void *self_tls_get_(struct metadata *md, uintptr_t item_key);
DICE_HIDE void *
self_tls_get(struct metadata *md, uintptr_t item_key)
{
    return self_tls_get_(md, item_key);
}

void self_tls_set_(struct metadata *md, uintptr_t item_key, void *ptr,
                   struct tls_dtor dtor);
DICE_HIDE void
self_tls_set(struct metadata *md, uintptr_t item_key, void *ptr,
             struct tls_dtor dtor)
{
    self_tls_set_(md, item_key, ptr, dtor);
}

void *self_tls_(struct metadata *md, const void *global, size_t size);
DICE_HIDE void *
self_tls(struct metadata *md, const void *global, size_t size)
{
    return self_tls_(md, global, size);
}
