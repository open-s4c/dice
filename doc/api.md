# Dice Public API Overview

This guide summarizes the headers under `include/dice/` so you can locate the
right building blocks when writing interceptors or subscribers.

## Core Concepts

- `types.h`: defines `chain_id`, `type_id`, `metadata_t`, and constants such as
  `MAX_TYPES`, `MAX_CHAINS`, and `CHAIN_CONTROL`; defines the `thread_id` type
  and sentinel values (`NO_THREAD`, `ANY_THREAD`, `MAIN_THREAD`).
- `chains/`: standard chain identifiers. `intercept.h` introduces
  `INTERCEPT_EVENT`, `INTERCEPT_BEFORE`, `INTERCEPT_AFTER`; `capture.h`
  provides the corresponding republished chains (`CAPTURE_*`).
- `events/`: payload definitions and event identifiers. Each header groups a
  family of events (`events/thread.h`, `events/malloc.h`, `events/memaccess.h`,
  etc.). Include the relevant header to interpret `EVENT_*` payloads inside
  your handlers.

## Pubsub and Module Glue

- `pubsub.h`: the main runtime interface. Exposes `ps_publish`,
  `ps_subscribe`, helper macros (`PS_PUBLISH`, `EVENT_PAYLOAD`), and the
  `ps_callback_f` signature.
- `module.h`: convenience macros for module initialization and subscription.
  `DICE_MODULE_INIT`/`DICE_MODULE_FINI` wrap constructor hooks,
  and `PS_SUBSCRIBE` registers handlers with the slot
  (`DICE_MODULE_SLOT` defaults to 9999).
- `handler.h`: emits the inline handler stubs used by `PS_SUBSCRIBE`. Most code
  relies on this indirectly via `module.h`.
- `dispatch.h`: declares the fast-path dispatch hooks that builtin modules use.
  When you compile a module with `DICE_MODULE_SLOT < MAX_BUILTIN_SLOTS`
  the generated dispatch tables call your handler without going through the
  callback list.

## Runtime Facilities

- `self.h`: per-thread metadata helpers provided by the Self module. Use
  `self_id`, `self_tls`, `self_tls_get`, and `self_tls_set` to manage TLS and
  retrieve Dice thread identifiers.
- `mempool.h`: lock-protected slab allocator used by Dice internals. Modules
  should prefer `mempool_alloc/realloc/free` over `malloc` to avoid reentrancy.
- `now.h`: monotonic time helpers (`now`, `in_sec`, `to_timespec`) for
  benchmarks or diagnostics.
- `rbtree.h`: minimal intrusive red-black tree wrapper used by Self and other
  components that need ordered maps.

## Interposition and Diagnostics

- `interpose.h`: platform-specific macros (`INTERPOSE`, `REAL`, `REAL_DECL`)
  for wrapping libc and pthread functions while still calling the original.
- `log.h`: logging macros (`log_debug`, `log_info`, `log_warn`, `log_fatal`)
  and configuration controls (`LOG_LEVEL`, `LOG_PREFIX`, `LOG_UNLOCKED`).
- `compiler.h`: shared compiler attributes (`DICE_CTOR`, `DICE_HIDE`, `likely`,
  etc.) used across Dice headers.

## Chains and Events at a Glance

When writing a handler:

1. Include the chain header (`dice/chains/capture.h` or
   `dice/chains/intercept.h`).
2. Include the relevant event header (for example, `dice/events/malloc.h`).
3. Subscribe with `PS_SUBSCRIBE(<CHAIN>, <EVENT_ID>, { … })`.
4. Use `EVENT_PAYLOAD` or the struct definition from the event header to inspect
   the payload.

Example:

```c
#include <dice/chains/capture.h>
#include <dice/events/mutex.h>
#include <dice/module.h>

PS_SUBSCRIBE(CAPTURE_BEFORE, EVENT_MUTEX_LOCK, {
    struct mutex_event *ev = EVENT_PAYLOAD(ev);
    /* inspect ev->mutex, ev->owner, etc. */
    return PS_OK;
});
```

Consult the individual headers in `include/dice/events/` for field-level
documentation of each payload.
