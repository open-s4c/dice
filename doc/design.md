---
title: Dice Design Document
author: Diogo Behrens
version: "1.2"
---

# 1. Overview

Dice is a lightweight, highly extensible framework designed to capture
execution events in multithreaded applications with minimal overhead. The
framework leverages function interposition to track critical system calls such
as thread creation, memory allocation, and synchronization primitives (e.g.,
mutexes, condition variables). Dice then distributes these events via a
publish-subscribe (Pubsub) system, allowing subscribers to dynamically respond
to and interact with the events.

The design of Dice emphasizes low overhead and extensibility, allowing easy
integration with a variety of testing, debugging, and monitoring use cases
which require runtime data processing and in-place behavior modification,
including but not limited to systematic concurrency testing, runtime monitoring,
deterministic replay, and data race detection.


## 1.1. Pubsub

Dice's Pubsub organizes event subscriptions in topics called **chains**.
Whenever an event is intercepted, it can be published to one or more chains.
Examples of events that can be intercepted are:

- Thread initialization/termination: Published when a new thread starts or
  terminates.

- Mutex lock/unlock: Published before and after `pthread_mutex_lock`,
  `pthread_mutex_unlock`, and other similar functions are called.

- Malloc/realloc/free: Published before and after standard memory allocation
  functions are called by the application.

- TSan event: Published on every events related to thread sanitizer functions
  such are `__tsan_read8`, `__tsan_exchange`, etc.

Within a chain, events are delivered to the event handlers in the subscription
**slot order** (described below).  Event handlers can keep state and have side
effect as well as change the event content such that following handlers receive
an updated event.  This mechanism allows subscribers to track resource states,
detect concurrency issues, and create complex runtime monitoring systems,
including state machines and deterministic replay systems.


## 1.2. Modules

Dice supports the loading of additional functionality via **modules**, which
can interpose system functions functions to publish events as well as subscribe
for events to act upon receiving them.

Dice provides two core modules: the **Pubsub** module and the **Mempool**
module.  Mempool manages memory for use by other modules, ensuring isolation of
Dice from the application.

Besides these two core modules, offers a few other (optional) modules, for
example, the **Self** module, which manages thread-local storage (TLS) for each
thread, ensuring the correct allocation of resources and avoiding redundant TLS
allocations in interposed functions.

Modules can be loaded as shared libraries via `LD_PRELOAD` mechanism or compiled
with core modules as a single shared library.


## 1.3. Example Use Cases

- Tracer: A subscriber can capture all event types and log them to a file,
  providing a trace of the program's execution. This can be useful for debugging
  and understanding the flow of control in complex multithreaded programs.

- State Machine Monitoring: A subscriber that tracks the state of resources
  (e.g., mutexes, condition variables) and ensures that they transition
  according to the expected behavior, catching violations such as deadlocks or
  invalid state transitions.

- Deterministic Replay: While Dice does not provide built-in deterministic
  replay, it can be extended to implement such a system. For example,
  a "sequencer" module can ensure that only one thread execute user code at
  any point in time.  To replay an execution, the sequencer needs to ensure
  scheduling decisions are deterministic or store in a replay file.  In this
  way, it is possible to replay specific execution scenarios by controlling
  the sequence of events and thread execution, ensuring that the system behaves
  consistently for debugging or testing purposes.


# 2. Pubsub System

The Pubsub (Publish-Subscribe) system is the heart of the event-driven
architecture in Dice. It was designed to facilitate the distribution of
execution events to subscribers in a low-overhead, flexible manner. Pubsub
allows different publisher modules to send event notifications, which are
then processed by interested subscriber modules.


## 2.1. What is Pubsub?

At its core, the Pubsub system provides a mechanism for publishers to broadcast
events to subscribers. Publishers are the modules that generate events, while
subscribers are the modules (or functions) that are interested in reacting to
these events.

Each event in Dice has a type that specifies what kind of event it is.  The
*event type* is identified by an integer `type_id`.  For example, the event
corresponding to a thread being initialized is identified by
`EVENT_THREAD_START`.

Events are published in topics, which are called *chains* and are identified by
an integer `chain_id`. Modules can subscribe for events published in a chain;
subscriptions can be filtered by `type_id` or be triggered for any type of
event using the special type ID `ANY_TYPE`.


## 2.2. Why Pubsub?

The Pubsub system introduces several key advantages:

1. **Decoupling**: The Pubsub mechanism decouples the publishers and
   subscribers, allowing modules to communicate without (much) knowledge of each
   other. This reduces dependencies and makes the system more modular and
   extensible.

2. **Flexibility**: The event-driven model allows for the addition of new event
   types and chains without modifying the core system. New subscribers can be
   added at compile time or at the startup of the runtime.

3. **Minimal Overhead**: Pubsub is designed with efficiency in mind.
   Subscriptions are established early during the module initialization, and
   the publishing of events occurs with minimal cost, i.e., the content of the
   events is passed by reference and the callbacks can be either function
   pointers or direct dispatches (see Section 6.2).

4. **Customizability**: Subscribers can specify exactly what events they want
   to react to, whether it's all events in a chain or specific events for a
   particular chain and event type.


## 2.3. How Pubsub Works

1. **Subscription**: A subscription specifies which chains and event types the
   subscriber is interested and how the event is going to be handled (event
   handler).

   There are two ways how to subscribe for events in Dice. The most common
   option is to let the subscriber module register with the Pubsub system by
   calling `ps_subscribe` at module initialization.

   Besides the chain ID and the event type ID, `ps_subscribe` takes as
   arguments a pointer the the event handler function and a subscription
   slot order.

   The second option is to define the event handler function with a predefined
   function name and compile it together with Dice. Refer to Section X.X for
   more information about module composition and handler subscriptions.

2. **Publishing**: Publishers (such as intercept modules or the Self module)
   publish events by calling `PS_PUBLISH(chain_id, type_id, void*,
   metadata_t*)`. The publisher specifies the chain, event type, event payload,
   and a potentially `NULL` metadata object.

3. **Event handlers**: The event handler function is where the action happens
   for the subscriber. Upon receiving an event, the handler can inspect and
   process the event payload. For example, a subscriber might log the event,
   modify its state, or even initiate further actions based on the event.
   In general, event handlers receive four arguments:

  - `chain_id chain`: The ID identifying the chain.
  - `type_id type`: The ID identifying the event type.
  - `void *event`: A generic pointer that represents the actual event data
    or **payload**. The actual type of this pointer must be agreed between
    publisher and subscriber and can be determined from `type`. The
    Pubsub module is oblivious to the particular event type.
  - `metadata_t *md`: An opaque data structure used by each chain to send
    metadata to the subscribers. Actual type defined by `chain`.

4. **Chain broadcast**: The ordering of delivery of events to handlers is
   controlled by the chain. When an event is published, it travels through
   the chain of subscribers in the slot order. This order is determined
   during subscription. This allows for flexible control over event flow
   and processing.

The Pusbsub system in Dice differs from the standard definition of Pubsub (GoF
design pattern) in several ways:

- **Chains**: Subscribers are organized in handler chains (not topics), i.e.,
  when an event is published to a chain, the subscribers receive the event one
  after another in the order defined in the chain.
- **Interruptions**: Event handlers can control whether the event is further
  propagated to subsequent handlers by returnig `PS_OK` to continue the
  chain or `PS_STOP_CHAIN` to interrupt it.
- **Synchronous**: The publisher **blocks** until all subscribers have handled the
  event or the chain has been interrupted. In fact, the thread executing the
  publish code also executes the subscription handlers, one by one.

These three aspects allow Dice's Pubsub to build powerful patterns such as
defining phases of computation, republishing events in other chains, realizing
when all subscribers of a chain have learned about an event, etc.


## 2.5. Event Flow Overview

```
app thread
   │ intercepts pthread_mutex_lock
   ▼
interceptor publishes INTERCEPT_BEFORE (EVENT_MUTEX_LOCK)
   │
   ▼
Self module republishes CAPTURE_BEFORE + metadata
   │
   ▼
subscriber handles CAPTURE_BEFORE (EVENT_MUTEX_LOCK)
   │
   └─► optional republish or STOP_CHAIN
```

Use this sequence as a reference when adding new handlers or tracing how events
propagate between chains.


## 2.4. Interception chains

The meaning of `chain_id` and `type_id` is given by convention between publishers
and subscribers.  The Pubsub system is oblivious to their meaning.  The header
file `<dice/intercept.h>` defines three chains: `INTERCEPT_BEFORE`,
`INTERCEPT_AFTER`, `INTERCEPT_EVENT`. All Dice modules that intercept syscalls
and other functions publish events to these three chains following this
convention:

- `INTERCEPT_BEFORE`: a function/operation is about to be
  invoked. The exact function or operation is typically reflected in the
  `type_id` or inside the event payload. A publication to `INTERCEPT_BEFORE`
  must be followed by a publication to `INTERCEPT_AFTER`.

- `INTERCEPT_AFTER`: a function/operation has been called and has
  returned. A publication to `INTERCEPT_AFTER` must follow a publication to
  `INTERCEPT_BEFORE`.

- `INTERCEPT_EVENT`: a function/operation is being called. Publications to
  `INTERCEPT_EVENT` represent the function/operation itself.

A module that publishes to one of these intercept chains is called
*interceptor*.

To better understand the convention described above, consider the interception
of `malloc`.  The interceptor code would look similar to this:

```c
void *malloc(size_t n) {
    struct malloc_event ev = {.n = n};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MALLOC, &ev, NULL);
    ev.ret = REAL(malloc, n); // call the actual implementation
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MALLOC, &ev, NULL);
    return ev.ret;
}
```

The publications to `INTERCEPT_BEFORE` and `INTERCEPT_AFTER` happen around the
call to the real malloc function.

Consider another example: `__tsan_read8`. Calls to this function are injected
by the TSAN compiler pass before plain accesses to memory locations. The
function itself does not perform the reading of the memory location, it simply
informs TSAN runtime the address of the memory location that is going to be
read. Dice replaces the TSAN runtime and can use such functions to publish
events like this:

```c
void __tsan_read8(void *addr) {
    struct memaccess_event ev = {.addr = addr, .size = 8};
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, &ev, NULL);
}
```

Here there is no reason to publish multiple events because there is no real
action occuring inside `__tsan_read8`. In such, we use `INTERCEPT_EVENT`.

Usually, interceptors either publish an event to the chain pair
`INTERCEPT_BEFORE` and `INTERCEPT_AFTER` or to the `INTERCEPT_EVENT` chain. The
formers are used when an external function is called, e.g., `pthread_create`;
the latter is used when the intercepted function is fully implemented in the
subscribers, e.g., user annotations.


# 3. Memory Pool

The Mempool is a foundational component in Dice, designed to isolate the memory
allocation of the application from Dice's runtime and provide a dependency-free
pool of memory.


## 3.1. What is Mempool?

The Mempool is a custom memory management system that pre-allocates a chunk of
memory at the start of the program's execution. This large block of memory is
then used by all modules that require memory during their execution.


## 3.2. Why Mempool?

There are mainly two problems that Mempool solves:


### Application-runtime Memory Isolation

The execution of an application could be influced by the memory allocation
pattern inside Dice, and vice-versa. In particular, if Dice is used to trace or
control a potentially buggy application, Dice allocations might hide bugs in
the application, and bugs in the application might affect Dice execution.
Moreover, if Dice is used to create a record/replay environment, the application
must be able to allocate precisely the same addresses on replay as it has done
on record run.


### Dependency-free thread-local storage

When a thread requires thread-local memory, it calls a series of functions
provided by the system, which often use some sort of synchronization, e.g., via
`pthread_mutex`. Unfortunately, one common use-case of Dice is precisely
monitoring calls involving `pthread_mutex`.  Furthermore, subscribers very often
need thread-local storage (TLS), e.g., to keep track of the events they observe.

Consider the following scenario:

1. An application calls `pthread_mutex_lock`.
2. An interceptor publishes the event `EVENT_MUTEX_LOCK` in `INTERCEPT_BEFORE`.
3. A subscriber is triggered with that event and requests TLS memory to store
   a call-count information.
4. The TLS management triggers another call to `pthread_mutex_lock`, which is
   again intercepted.

Now, to avoid publishing the same event again and entering in an infinite loop,
the interceptor would need to keep the information that it is already in the
process of publishing an event. And here is the catch: that information has to
be stored in TLS memory!

Mempool allocates a memory chunk on startup from the main thread. After startup,
other threads can safely get memory from the Mempool without having the risk of
entering in a infinite loop.  The Self module (section 4) provides a abstraction
layer above the mempool to simplify the handling of TLS management.


## 3.3. How Mempool Works

1. Initialization: The Mempool is initialized early in the program execution
   by the main thread. This ensures that a large block of memory is available
   to all modules from the outset. The Mempool is allocated using system-level
   memory management techniques but is managed internally within Dice.

2. Memory Allocation: When a module requires memory, it requests a chunk of
   memory from the Mempool using `mempool_alloc` instead of the standard
   `malloc`.

3. Deallocation: When memory is no longer needed, it is returned to the Mempool.
   This is done through a specialized function, often `mempool_free`, which
   ensures that memory is properly cleaned up and made available for future use.

Each bucket in the pool uses fixed slab sizes (32 bytes and up) so returned
blocks maintain native pointer alignment. For workloads that require different
alignment guarantees you can tune the `sizes_` table in `src/dice/mempool.c`
and rebuild.

Implementation details live in `src/dice/mempool.c`; the public API is
documented in `include/dice/mempool.h`.

```
process start
   │
   ├─► mempool_init() allocates slab arena (main thread)
   │
   ├─► worker threads call mempool_alloc()
   │       │
   │       ├─► pop slab from freelist if available
   │       └─► extend arena when freelist empty
   │
   └─► modules return memory via mempool_free(), pushing slab back
```

The diagram highlights the single lock protecting slab lists—plan allocations
for latency-sensitive code accordingly.


# 4. Self Module

The Self module is one of the key components in Dice, providing essential
thread-local storage (TLS) management. It acts as the first subscriber and
plays a critical role in ensuring that each thread gets its own isolated TLS
space. Unless the user reimplements an equivalent component, Self shall be
loaded with Dice before any other user module.


## 4.1. What is the Self Module?

The Self module stays in the middle between interceptors (Section 2.4) and
subscribers. It manages thread-local storage (TLS), ensuring that each thread
has a dedicated storage area for maintaining its private data. The TLS is
intended to be safely used by subscribers, avoiding platform-specific details
of pthread TLS. The Self module itself employs the TLS to keep track of the
thread ID as well as guarding the execution from recursive publications.


## 4.2. Why Self?

The Self module addresses a few important needs:

1. TLS Management: By managing TLS space, the Self module ensures that each
   thread has a clean, isolated memory region for storing thread-specific data,
   preventing contamination from other threads or system components.

2. Thread Initialization and Finalization: The Self module reacts to thread
   lifecycle events, allocating and deallocating TLS space as necessary, and
   ensuring the correct handling of thread-specific data.

3. Prevention of Infinite Loops: It ensures that there is no infinite recursion
   in certain cases, such as when a thread calls into a subscriber while
   interacting with the event system.


## 4.3. How Self Works

1. Thread Initialization: When a new thread is created (including the main
   thread), the first event published by the thread triggers the initialization
   of the Self module, which in turn publishes a `EVENT_SELF_INIT`. So,
   for any thread, one can always expect to receive such an event as the very
   first event of the thread.  The initilization allocates thread-specific data
   (such as an array of pointers for TLS) from the Mempool (Section 3). It also
   assigns a unique thread ID to the current thread. The thread ID is an atomic
   counter starting from value 1. The thread ID 0 is reserved to represent
   `NO_THREAD`.

2. Thread Finalization: When a thread finishes execution, the Self module
   receives an `EVENT_THREAD_EXIT` event. It is responsible for cleaning
   up the TLS data associated with the thread, using mempool-free to deallocate
   memory. A `EVENT_SELF_FINI` is published once the TLS of a thread is
   reclaimed. The `EVENT_SELF_FINI` of the main thread is sent with `atexit`
   hooks of the main program, but **cannot be guaranteed** to be the absolute
   last piece of code executed in the program.

3. TLS Allocation: The Self module allocates TLS only when it receives a
   `EVENT_THREAD_START` event. This ensures that TLS is only allocated when
   necessary and prevents redundant allocations. If no TLS data is found for a
   thread (e.g., if it has already been finalized), the Self module
   **interrupts the chain**, preventing further processing.

4. Guard Against Recursion: The Self module implements a guard mechanism to
   prevent recursive calls that might arise from the event system itself. For
   example, if a subscriber publishes an event while processing another event,
   the system prevents an infinite loop by interrupting the chain.

The events `EVENT_THREAD_START` and `EVENT_THREAD_EXIT` are published by the
`dice-pthread_create` module of Dice, or can be published by user-defined
interceptors.


## 4.4. Capture chains

The Self module subscribes to all intercept chains and republishes the
events in equivalent **capture chains**: `CAPTURE_BEFORE`, `CAPTURE_AFTER`,
`CAPTURE_EVENT`.  When republishing to these chains, the Self module sends a
**self-specific metadata** along with the event.  This metadata can be used
by any subscriber to query for TLS data as well as thread ID without any extra
cost.  The functions provided for these functionalities are in `dice/self.h`:

- `self_id(metadata_t *md)` returns the Dice's thread ID (starting from 1).
- `self_tls(metadata_t *md, const void *key, size_t size)` returns a pointer to
  a thread-local memory of `size` identified by `key`.

Unless the user is deploying a component reimplementing the functionality of
the Self module, user modules should subscribe to `CAPTURE_` chains instead of
the equivalent `INTERCEPT_` chains.


## 4.5. Example Use Case: File Descriptor Management

One example of how the Self module can be used is for managing file descriptors
on a per-thread basis. Each thread can store its own file descriptor in its TLS
space, ensuring that threads do not interfere with each other's file operations.
A subscriber could intercept system calls like open or close, logging the event
and associating file descriptors with the correct thread's TLS.


# 5. Interpose Modules

In addition to the core Dice components, several interpose modules are provided
with Dice. These modules are designed to intercept and modify the behavior of
various system functions, allowing for detailed monitoring, testing, and
debugging. Interposition is a powerful technique that allows developers to hook
into existing system calls and modify their behavior or capture specific events.


## 5.1. What is Interposition?

Interposition refers to the technique of intercepting calls to system functions
or library functions and inserting custom behavior. In Dice, this technique is
applied to various system functions using shared libraries and dynamic linking
mechanisms like `LD_PRELOAD`.

By using interposition, Dice can intercept functions like `pthread_create`,
`malloc`, `free`, and other system-level functions without modifying the source
code of the application.


## 5.2. How Interposition Works with `LD_PRELOAD`

The `LD_PRELOAD` environment variable is a mechanism in Unix-like systems that
allows users to load shared libraries before others. When an application is run,
the dynamic linker checks the `LD_PRELOAD` variable and loads any libraries
listed in it before the default system libraries. This allows Dice to interpose
on functions without modifying the application's code or the system libraries
themselves.

Each interpose module in Dice targets a specific set of functions. For example,
`dice-pthread_create` module intercepts calls to `pthread_create` and
`pthread_exit`, while the `dice-malloc` intercepts memory allocation functions
like `malloc`, `free`, `calloc`, and `realloc`.

When an application calls an intercepted function, the corresponding interpose
module is triggered. For example, when `pthread_create` is called, the interpose
module publishes events using the Pubsub system. For example, when
`pthread_create` is intercepted, the event `EVENT_THREAD_CREATE` is published.
Via a trampoline function, passed to the real `pthread_create`, the new thread
also publishes a `EVENT_THREAD_START` event.  Similarly, events like
`EVENT_MUTEX_LOCK`, `EVENT_MUTEX_UNLOCK`, `EVENT_MALLOC`, and `EVENT_FREE` can
be intercepted and published to the appropriate chains.

Notes that on macOS the environment variable to control library preloading is
called `DYLD_INSERT_LIBRARIES`.


## 5.3. Interpose Modules in Dice

- `dice-pthread_create`: Publishes `EVENT_THREAD_CREATE`, `EVENT_THREAD_START`,
  and `EVENT_THREAD_EXIT`. Load it whenever you need thread lifecycle events or
  the Self module; exported via `-pthread` in `scripts/dice`.
- `dice-pthread_mutex`: Emits `EVENT_MUTEX_LOCK`, `_UNLOCK`, and try-lock
  variants. Use alongside the Self module to build lock-order checkers.
- `dice-pthread_cond`: Covers `pthread_cond_wait`, `signal`, and `broadcast`
  paths, generating wait/wake events for scheduling analyses.
- `dice-malloc`: Hooks `malloc`, `calloc`, `realloc`, and `free`, publishing
  allocation events that can be correlated with stack traces or leak detectors.
- `dice-cxa`: Wraps C++ guard helpers so constructors run under Dice control;
  useful when intercepting static initializers or C++ singletons.
- `dice-sem`: Tracks POSIX semaphore operations (`sem_wait`, `sem_post`,
  `sem_trywait`) for workloads that use semaphores instead of mutexes.
- `dice-tsan`: Substitutes the libtsan frontend; publishes memory-access events
  and is required for the `tsano` record/replay toolchain.
- `dice-stacktrace`: Augments capture metadata with call stacks. Automatically
  loaded with `-tsan` via `scripts/dice` but can be preloaded independently.
- `dice-self`: Manages TLS for subscribers. Subscribe to capture chains after
  loading this module or link it in as builtin (`-self`).

The stock `scripts/dice` wrapper enables the appropriate mix of these modules
based on flags like `-pthread`, `-malloc`, and `-tsan`. Custom preload strings
should follow the same order: core `libdice`, intercept modules, then user
plugins.


# 6. Builtins and plugins

Dice is designed to be loaded as a shared library with `LD_PRELOAD`.
The core library in Dice has only the Pubsub and the Mempool modules inside.
Additional modules can be loaded in two ways.


## 6.1. Loading Strategies

When Dice is used purely as a preload library you only need `libdice.so` plus
the modules you want to enable. A typical invocation for an application `foo`
looks like:

```
env LD_PRELOAD=/path/to/libdice.so:/path/to/dice-pthread_create.so foo <arg1>
```

macOS users should swap `LD_PRELOAD` for `DYLD_INSERT_LIBRARIES`. Module
constructors register their subscribers during load, so the order of
initialization is controlled via the `DICE_MODULE_SLOT` macro instead of
relying on linker quirks. Lower slots run first, and builtin modules reserve
the range `0..MAX_BUILTIN_SLOTS-1`. Plugin authors should pick slots greater
than that so the core interceptors and the Self module execute before
user code.

For tighter deployments you can link Dice core and a curated set of modules into
a single shared object, e.g.:

```
add_library(libmydice SHARED
    $<TARGET_OBJECTS:dice.o>
    $<TARGET_OBJECTS:dice-pthread_create.o>
    $<TARGET_OBJECTS:dice-pthread_mutex.o>
    $<TARGET_OBJECTS:my_module.o>)
```

The resulting library is still preloaded the same way:

```
env LD_PRELOAD=/path/to/libmydice.so foo <arg1>
```

This approach removes dynamic relocation overhead, enables LTO across modules,
and allows you to hide the Dice interfaces entirely when combined with the box
objects described below.


## 6.2. Dispatch vs Callback Execution

Dice supports two types of subscription:

- **Callback-based subscriptions**: Modules loaded as independent shared
  libraries call `ps_subscribe` during initialization. This is the only
  supported subscription when using `LD_PRELOAD` with separate `.so` files. The
  subscriptions always take the tripple chain/type/slot, where slot represents
  the priority of the callback. Multiple modules can use the same slot for this
  kind of subscription.

- **Dispatch-based subscriptions**: Modules define a handler function following
  the naming convention `ps_dispatch_CCC_EEE_SSS`, where `CCC` is a chain ID,
  `EEE` is a event type ID, and `SSS` is a slot. Among the builtin range of
  slots, only one module can occupy one slot, i.e., no two modules can use
  the same value `SSS`.

In practice, subscriptions are implemented using the `PS_SUBSCRIBE` macro
from the `dice/module.h` header file.  This macro will provide both types
of subscription. The compile-time constant `DICE_MODULE_SLOT` constant is
the slot of the module.  The subscription mechanism used in runtime depends
on whether the module is compiled together with Dice **and** whether
the slot is within the builtin slot range, i.e.,  `DICE_MODULE_SLOT <
MAX_BUILTIN_SLOTS`. If that is the case, then the Pubsub will prefer calling
the defined dispatch function directly instead of using the callback function
pointers.

On the publisher side, both types of subscriptions are served by the same
`ps_publish` function. If two modules subscribe for the same chain and event
type, they are served in the slot order (lower first).


## 6.3. Box Builds and Hidden Interfaces

When we bundle Dice together with a set of builtin modules we often want to
strip the dynamic plugin surface so linkers and optimizers can treat the
package as a sealed unit. The **box** object libraries provide this toggle.

- `src/dice/box.c` (`dice-box.o`) overrides the weak entry points exported by
  `libdice.so`. The box version of `ps_publish` drives the generated dispatch
  tables directly and skips the subscription lists; `ps_subscribe` becomes a
  no-op, and the mempool wrappers forward to the internal symbols while
  remaining hidden from the dynamic symbol table.
- `src/mod/self-box.c` (`dice-self-box.o`) performs the same trick for the Self
  helpers so TLS accessors are only visible inside the bundle.
- Both targets compile with `DICE_HIDE_ALL`, forcing the linker to keep these
  overrides local to the resulting shared object. References inside builtin
  modules still resolve because they are linked in the same library, but no
  symbol is exported for preloadable plugins to latch on.

Link the box objects in addition to `dice.o` when building a monolithic
assembly (e.g., `libdice-bundle-box`). Because builtin modules also include the
generated `dispatch_X.c` fast chain, every publication goes through a switch
table and ends up invoking the correct handler without registering a callback.
Dropping the box objects restores the regular plugin surface (`libdice.so`),
allowing new modules to subscribe via `ps_subscribe` at load time.
