# Dice

**Dice** is a lightweight, extensible C framework for capturing and
distributing execution events in multithreaded programs. Designed for low
overhead and high flexibility, Dice enables powerful tooling for runtime
monitoring, concurrency testing, and deterministic replay using a modular
publish-subscribe (pubsub) architecture.

## Features

- Function interposition via `LD_PRELOAD`
- Pubsub-based event distribution
- Modular, pluggable architecture
- Supports thread, memory, and synchronization tracking
- Automatic interception via assembly macros (Autocept)
- Thread-local storage and memory pooling for performance

## Getting Started

To use Dice:

1. **Build Dice and its modules**
2. **Set environment variable**:
   ```sh
   export LD_PRELOAD=/path/to/libdice.so:/path/to/module.so:...
   ```
3. **Run your program**
   ```sh
   ./your_program
   ```

Subscribers and modules will now intercept and handle events during execution.

---

## Architecture Overview

Dice is composed of:

- **Core Modules**: `mempool`, `self`, and `pubsub` provide foundational
  services.
- **Intercept Modules**: Use interposition to hook into key system functions.
- **Autocept**: Automatically intercepts functions using assembly trampolines.
- **Switcher**: Coordinates deterministic thread execution.

---

## Core Modules

- **Mempool**: Centralized memory manager for modules and threads.
- **Pubsub**: Event routing system based on chains and event types.
- **Self**: Manages thread-local storage and handles thread lifecycle events.

## Interpose Modules

- `pthread-create`: Hooks `pthread_create` and `pthread_join`
- `pthread-mutex`: Hooks mutex lock/unlock operations
- `pthread-cond`: Hooks condition variables
- `malloc`: Hooks `malloc`, `free`, `calloc`, `realloc`
- `cxa`: Hooks C++ guard functions (`__cxa_guard_acquire`, etc.)
- `sem`: Hooks POSIX semaphore functions
- `tsan`: Hooks thread sanitizer calls
- `autocept`: Generic interposition using assembly macros

---

## Use Cases

- **Execution Tracing**: Log detailed sequences of events for debugging.
- **Race Detection**: Use TSan integration to expose data races.
- **State Machine Monitoring**: Validate synchronization protocol correctness.
- **Deterministic Replay**: Control execution to replicate bugs.
- **Systematic Testing**: Explore thread interleavings to find concurrency bugs.

---

## Building and Installation

```sh
git clone https://your.repo/dice.git
cd dice
make
```

Modules are compiled as shared libraries. Ensure they are available in your
`LD_LIBRARY_PATH`. Note that if you are using macOS, you have to set
`DYLD_LIBRARY_PATH` and `DYLD_INSERT_LIBRARIES` instead.

---

## Usage

Example: Run an application with Dice intercepting memory and threading events:

```sh
export LD_PRELOAD=/path/to/libdice.so:/path/to/libdice-malloc.so
./your_program
```

You can add your own subscriber module and load it the same way to process events.

---

## Examples

### Logger Subscriber

**`examples/logger.c`**
*A simple subscriber that logs all events to stderr.*

```c
#include <stdio.h>
#include <dice/capture.h>
#include <dice/module.h>

static bool log_callback(token_t token, const void *data, self_t *self) {
    fprintf(stderr, "[logger] Event received: chain=%d, event_id=%d, data=%p\n",
            token->chain, token->event, data);
    return true; // continue chain
}

REGISTER_CALLBACK(ANY_CHAIN, ANY_EVENT, {
    log_callback(token, event, arg);
})
```

**Compile:**
```sh
gcc -fPIC -shared -o libdice-logger.so logger.c -I/path/to/dice/include
```

**Run:**
```sh
env LD_PRELOAD=./libdice.so:./libdice-logger.so ./your_program
```

---

## License

[MIT License](LICENSE)

