---
title: Dice Design Document
author: Diogo Behrens
version: "0.1"
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
Possible intercepted events are:

- Thread initialization/termination: Published when a new thread starts or
  terminates.

- Mutex lock/unlock: Published before and after `pthread_mutex_lock`,
  `pthread_mutex_unlock`, and other similar functions are called.

- Malloc/realloc/free: Published before and after standard memory allocation
  functions are called by the application.

- TSan event: Published on every events related to thread sanitizer functions
  such are `__tsan_read8`, `__tsan_exchange`, etc.

Within a chain, events are delivered to the callbacks in the subscription order.
Callbacks can has keep state and have side effect. They can also change the
event content such that following callabacks receive an updated event.  This
mechanism allows subscribers to track resource states, detect concurrency
issues, and create complex runtime monitoring systems, including state machines
and deterministic replay systems.

## 1.2. Modules

Dice supports the loading of additional functionality via **modules**, which
can interpose functions to publish events as well as subscribe for events to
act upon receiving them.

Dice provides two core modules: the **Pubsub** module and the **Mempool**
module.  Mempool manages memory for use by other modules, ensuring isolation of
Dice from the application.

Besides these two core modules, offers a few other modules, for example, the
**self** module, which manages thread-local storage (TLS) for each thread,
ensuring the correct allocation of resources and avoiding redundant TLS
allocations in interposed functions.

Modules can be loaded as shared libraries via `LD_PRELOAD` mechanism or compiled
with core modules as a single shared library.

## 1.3. Example Use Cases

- Tracer: A subscriber can capture all event types and log them to a file,
  providing a trace of the program’s execution. This can be useful for debugging
  and understanding the flow of control in complex multithreaded programs.

- State Machine Monitoring: A subscriber that tracks the state of resources
  (e.g., mutexes, condition variables) and ensures that they transition
  according to the expected behavior, catching violations such as deadlocks or
  invalid state transitions.

- Deterministic Replay: While Dice does not provide built-in deterministic
  replay, it offers the building blocks necessary to implement such a system.
  Using event data and the switcher (which controls thread execution order), it
  is possible to replay specific execution scenarios by controlling the sequence
  of events and thread execution, ensuring that the system behaves consistently
  for debugging or testing purposes.

# 2. Pubsub System

The Pubsub (Publish-Subscribe) system is the heart of the event-driven
architecture in Dice. It was designed to facilitate the distribution of
execution events to subscribers in a low-overhead, flexible manner. Pubsub
allows different modules (publishers) to send event notifications, which are
then processed by interested subscribers.


## 2.1. What is Pubsub?

At its core, the Pubsub system provides a mechanism for publishers to broadcast
events to subscribers. Publishers are the modules that generate events, while
subscribers are the modules (or functions) that are interested in reacting to
these events.

Each event in Dice has a type that specifies what kind of event it is.  The
event type is identified by an integer `type_id`.  For example, the event
corresponding to a thread being initialized is identified by
`EVENT_THREAD_INIT`.

Events are published in topics, which are called *chains* in Dice and are
identified by an integer `chain_id`. Modules can subscribe for events published
in a chain; subscriptions can be filtered by `type_id` or be triggered for any
type of event.


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
   pointers or direct calls (TODO: see section x.x).

4. **Customizability**: Subscribers can specify exactly what events they want
   to react to, whether it’s all events in a chain or specific events for a
   particular chain and event type.


## 2.3. How Pubsub Works

1. **Subscription**: At initialization, modules (subscribers) register with the
   Pubsub system by specifying which chains and event types they want to
   receive. This is done by calling a function that establishes the
   subscription. Event types can be specific or general (e.g., `ANY_EVENT`).
   Each subscription specifies a callback function, which will be called when
   an event is published. The callback receives four arguments:

  - `chain_id chain`: The ID identifying the chain.
  - `type_id type`: The ID identifyfing the event type.
  - `void *event`: A generic pointer that represents the actual event data
    or **payload**. The actual type of this pointer must be agreed between
    publisher and subscriber and can be determined from `type`. The
    Pubsub module is oblivious to the particular event type.
  - `metadata_t *md`: An opaque data structure used by each chain to send
    metadata to the subscribers. Actual type defined by `chain`.

2. **Publishing**: Publishers (such as intercept modules or the Self module)
   publish events by calling `PS_PUBLISH(chain_id, type_id, void*, metadata_t*)`. The
   publisher specifies the chain, event type, event payload, and a potentially
   `NULL` metadata object.

3. **Callback Handling**: The callback function is where the action happens for
   the subscriber. Upon receiving an event, the callback can inspect and process
   the event payload. For example, a subscriber might log the event, modify its
   state, or even initiate further actions based on the event.

4. **Chain Control**: The ordering of events is controlled by the subscription
   chain. When an event is published, it travels through the chain of
   subscribers in the order of their subscription. This order is determined by
   the loading order of the modules (based on constructor attributes and
   priorities). This allows for flexible control over event flow and processing.

The Pusbsub system in Dice differs from the standard definition of Pubsub (GoF
design pattern) in several ways:

- **Chains**: Subscribers are organized in callback chains (not topics), i.e.,
  when an event is published to a chain, the subscribers receive the event one
  after another in the order of subscription.
- **Interruptions**: Callbacks can control whether the event is further
  propagated to subsequent subscriptions by returnig `PS_CB_OK` to continue the
  chain or `PS_CB_STOP` to interrupt it.
- **Synchronous**: The publisher blocks until all subscribers have handled the
  event or the chain has been interrupted.

These three aspects allow Dice's Pubsub to build powerful patterns such as
defining phases of computation, republishing events in other chains, realizing
when all subscribers of a chain have learned about an event, etc.


## 2.4. Interception chains

The meaning of `chain_id` and `type_id` is given by convetion between publishers
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

To better understand the convention described here, consider the interception
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
memory at the start of the program’s execution. This large block of memory is
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


# 4. Self Module

The Self module is one of the key components in Dice, providing essential
thread-local storage (TLS) management and handling thread initialization and
finalization events. It acts as the first subscriber and plays a critical role
in ensuring that each thread gets its own isolated TLS space. Unless the user
reimplements an equivalent component, Self shall be loaded with Dice before any
other user module.


## 4.1. What is the Self Module?

The Self module manages thread-specific data, ensuring that each thread has a
dedicated storage area for maintaining its TLS data. The Self module is invoked
when a new thread is created or when a thread finishes execution.


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

1. Thread Initialization: When a new thread is created, the Self module
   receives an `EVENT_THREAD_INIT`. Upon receiving this event, it
   allocates thread-specific data (such as an array of pointers for TLS) from
   the Mempool. It also assigns a unique thread ID to the current thread, which
   is managed atomically to ensure correct synchronization.

2. Thread Finalization: When a thread finishes execution, the Self module
   receives an `EVENT_THREAD_FINI` event. It is responsible for cleaning
   up the TLS data associated with the thread, using mempool-free to deallocate
   memory.

3. TLS Allocation: The Self module allocates TLS only when it receives a
   `EVENT_THREAD_INIT` event. This ensures that TLS is only allocated when
   necessary and prevents redundant allocations. If no TLS data is found for a
   thread (e.g., if it has already been finalized), the Self module
   **interrupts the chain**, preventing further processing.

4. Guard Against Recursion: The Self module implements a guard mechanism to
   prevent recursive calls that might arise from the event system itself. For
   example, if a subscriber publishes an event while processing another event,
   the system prevents an infinite loop by interrupting the chain.

The events `EVENT_THREAD_INIT` and `EVENT_THREAD_FINI` are published by the
`mod-pthread_create` module of Dice, or can be published by user-defined
interceptors.


## 4.4. Capture chains

The Self module subscribes to all intercept chains (section 2.4) and republishes
the events in equivalent **capture chains**: `CAPTURE_BEFORE`, `CAPTURE_AFTER`,
`CAPTURE_EVENT`.  When republishing to these chains, the Self module sends a
**self-specific metadata** along with the event.  This metadata can be used by
any subscriber to query for TLS data as well as thread ID without any extra
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
space, ensuring that threads do not interfere with each other’s file operations.
A subscriber could intercept system calls like open or close, logging the event
and associating file descriptors with the correct thread’s TLS.
