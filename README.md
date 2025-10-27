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
- Thread-local storage and memory pooling for performance

## Quickstart

To use Dice:

1. Build Dice and its modules:

    cmake -B build
    cmake --build build

2. Call your program setting LD_PRELOAD:

    env LD_PRELOAD=/path/to/libdice.so:/path/to/module.so:... \
        ./your_program

3. Or use the Dice script to do that for you:

    scripts/dice -pthread -self -malloc -tsan ./your_program

Subscribers and modules will now intercept and handle events during execution.
The script `scripts/dice` can be used to select, which modules you want to load
with Dice.  If you prefer composing the command line manually and you are a
macOS user, you should replace `LD_PRELOAD` with `DYLD_INSERT_LIBRARIES`.


## Architecture Overview

Dice is composed of several modules. Its core modules include

- **Mempool**: Centralized memory manager for modules and threads.
- **Pubsub**: Event routing system based on chains and event types.
- **Self**: Manages thread-local storage and handles thread lifecycle events.

Beside these modules, Dice provice several intercept modules such as

- `pthread-create`: Hooks `pthread_create` and `pthread_join`
- `pthread-mutex`: Hooks mutex lock/unlock operations
- `pthread-cond`: Hooks condition variables
- `malloc`: Hooks `malloc`, `free`, `calloc`, `realloc`
- `cxa`: Hooks C++ guard functions (`__cxa_guard_acquire`, etc.)
- `sem`: Hooks POSIX semaphore functions
- `tsan`: Hooks thread sanitizer calls

## Use Cases

- **Execution Tracing**: Log detailed sequences of events for debugging.
- **Race Detection**: Use TSan integration to expose data races.
- **State Machine Monitoring**: Validate synchronization protocol correctness.
- **Deterministic Replay**: Control execution to replicate bugs.
- **Systematic Testing**: Explore thread interleavings to find concurrency bugs.

## Building and Installation

```sh
git clone https://your.repo/dice.git
cd dice
cmake -B build -DCMAKE_INSTALL_PREFIX=<PREFIX>
cmake --build build
cmake --install build
```

The installation prefix is `/usr/local` by default. Core and modules are
compiled as shared libraries and installed as follows:

- Dice core: `<PREFIX>/lib/libdice.so`
- Modules: `<PREFIX>/lib/dice/<MODULE>.so`, eg, `<PREFIX>/lib/dice/malloc.so`

Ensure `<PREFIX>/lib` is in your `LD_LIBRARY_PATH`. Note that if you are using
macOS, you have to set `DYLD_LIBRARY_PATH` and `DYLD_INSERT_LIBRARIES` instead.

Dice also provides a simple shell script (see `scripts/dice`), which simplifies
the preloading of Dice and `libtsano`.  The `dice` script is installed in
`<PREFIX>/bin`.

## Further information

See examples in the `examples` directory.

- Detailed architecture notes: see `doc/design.md`.
- Header-level API reference: see `doc/api.md`.
- Contribution checklist (style, tests, benchmarks): see `doc/contributing.md`.
- Benchmark workflow: see `doc/benchmarking.md`.
- Test layout and commands: see `doc/testing.md`.

---

## License

[0BSD License](LICENSE)
