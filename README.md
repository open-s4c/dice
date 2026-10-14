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

Import Dice into your CMake project and configure your library:

```cmake
add_subdirectory(deps/dice EXCLUDE_FROM_ALL)

add_library(foo SHARED observer.c)
add_dice_core(foo "${CMAKE_CURRENT_SOURCE_DIR}/foo.dice")
add_dice_mods(foo self pthread_create malloc)
```

The configuration describes slots, events, chains, and ordering.
`add_dice_core` builds the generator, generates dispatchers in `foo-dice/dice.c`
and IDs in `foo-dice/dice.h`, and adds the dispatchers and ordinary core sources
from `src/dice` directly to `foo`. It is sufficient for a runtime with no
interceptor modules. `add_dice_mods` adds the selected module sources directly
to the same target. These sources are compiled as part of `foo`, with its
compiler options and generated configuration.

Both helpers use `DICE_ROLLED`. Core and interceptor sources are compiled with
the generated header and their appropriate slots. Included configuration files
are tracked automatically; changing them regenerates the output and rebuilds
affected objects. Each configured target has its own generated files and objects.

Your observer selects its slot before including the generated header:

```c
#define DICE_MODULE_SLOT SLOT_OBSERVER
#include "dice.h"

PS_SUBSCRIBE(CAPTURE_BEFORE, EVENT_MALLOC, {
    struct malloc_event *allocation = event;
    /* Handle the allocation. */
})
```

Add `CHECK_ROUTES` to `add_dice_core(foo foo.dice CHECK_ROUTES)` to validate
declared routes at runtime. The helper accepts shared, static, module, and object
libraries, as well as executables. Existing Dice targets remain available.
Call `add_dice_core` before `add_dice_mods`; modules may be added in multiple
calls. Names such as `self` and `pthread_create` select sources and correspond
to configuration slots `dice-self` and `dice-pthread-create`. Hyphens and
underscores are equivalent; the `dice-` prefix is also accepted in module lists.
Selecting a module does not add its routes to the configuration.
Cross builds compile a native generator automatically using `DICE_HOST_CC`.
To use an existing matching native `dice` executable instead, set
`DICE_HOST_EXECUTABLE`.
On macOS, keep interceptors in a shared library: dyld interposition does not
redirect calls originating in the same image as the interceptor.

The complete example declares the standard interceptor routes and an observer
for allocation and thread start/exit events:

```sh
cmake -S examples/roll -B build-example
cmake --build build-example
ctest --test-dir build-example --output-on-failure
```

Use `-DCHECK_ROUTES=ON` when configuring this example to enable route checks.

For a project with separate object-library targets, link each module to the
configuration's interface target, `foo-dice.h`:

```cmake
add_library(foo SHARED)
add_dice_core(foo foo.dice)
add_dice_mods(foo self pthread_create)

add_library(observer.o OBJECT observer.c)
target_link_libraries(observer.o PRIVATE foo-dice.h)
target_link_libraries(foo PRIVATE observer.o)
```

The interface supplies the generated header's include path, generation
dependency, route-check settings, and position-independent compilation. Every
object target using it rebuilds when the header changes. Each source selects
its own `DICE_MODULE_SLOT` before including `dice.h`; use `PS_SUBSCRIBE_SLOT`
for additional slots in the same source. The interface does not link the core
into the module. Plugins may also use it, with `DICE_PLUGIN_MODULE` defined.

### Using the generator directly

The build-tree `dice-roll` executable generates
configuration-specific dispatchers, callback trampolines, and tables in `dice.c`.
The generated `dice.h` defines event, chain, and slot IDs and the subscription
glue. It includes the normal Dice headers; it does not pack runtime sources or
public headers. Dice and libvsync headers are required when compiling it.

```sh
cmake -S . -B build -DDICE_TESTS=OFF -DDICE_BENCHMARKS=OFF
cmake --build build
build/src/cli/dice-roll examples/roll/dice.dice --output generated
```

Use `--name dispatcher` to produce `dispatcher.c` and `dispatcher.h` instead.
Names may contain 1–127 letters, digits, underscores or hyphens. This changes
filenames; C symbols and the API remain the same. Use `--depfile FILE` to
record configuration dependencies, including nested includes, in Make format.
The generator requires the source checkout and reads its templates and LICENSE
directly. It is not installed; import the repository with `add_subdirectory`
to use the CMake helpers.

Compile the ordinary core separately against the generated header:

```sh
for core in pubsub mempool pubsub-box; do
    cc -std=gnu11 -D_GNU_SOURCE -fPIC -Iinclude -Ideps/libvsync/include \
        -DDICE_ROLLED=1 -DDICE_ROLL_RUNTIME -DDICE_MODULE_SLOT=0 \
        -include generated/dice.h \
        -c src/dice/$core.c -o generated/$core.o
done
```

`DICE_ROLL_RUNTIME` is an internal core build flag. `pubsub-box.c` supplies
sealed publication when plugins are disabled and compiles to an empty object
otherwise. Do not add the old generated dispatchers or `registry.c`: the
configured dispatchers supply the tables used by the rolled pubsub code.

Build the example's interceptor sources against the same header:

```sh
for module in self pthread_create malloc; do
    slot=$(printf '%s' "$module" | tr '[:lower:]' '[:upper:]')
    cc -std=gnu11 -D_GNU_SOURCE -fPIC -Iinclude -Ideps/libvsync/include \
        -DDICE_ROLLED=1 -DDICE_DISPATCH_MODULE \
        -include generated/dice.h -DDICE_MODULE_SLOT=SLOT_DICE_$slot \
        -c src/mod/$module.c -o generated/$module.o
done
```

Build the runtime, plugin, and application on Linux:

```sh
cc -std=gnu11 -D_GNU_SOURCE -fPIC -shared -DDICE_ROLLED=1 \
    -DDICE_DISPATCH_MODULE -Igenerated -Iinclude -Isrc/dice \
    -Ideps/libvsync/include generated/dice.c examples/roll/observer.c \
    generated/pubsub.o generated/mempool.o generated/pubsub-box.o \
    generated/self.o generated/pthread_create.o generated/malloc.o \
    -pthread -ldl -o customdice.so
cc -std=gnu11 -D_GNU_SOURCE -fPIC -shared -DDICE_ROLLED=1 \
    -Igenerated -Iinclude -Ideps/libvsync/include \
    examples/roll/plugin.c -o plugin.so
cc examples/roll/main.c -pthread -o sample
LD_PRELOAD="$PWD/customdice.so:$PWD/plugin.so" ./sample
```

Both the linked observer and the plugin report the allocation. The CMake example
handles the platform-specific compiler and linker options, including macOS.
When hosting the runtime in an executable, export its API symbols for plugins
(for example, `-rdynamic` on Linux).

Existing source-tree consumers can continue using `add_subdirectory(dice)` and
the `dice.h`, `dice.o`, `dice-box.o`, and `dice-*.o` targets with their existing
`DICE_MAX_TYPES`, `DICE_MAX_CHAINS`, `DICE_LAST_DISPATCH_SLOT`, and
`DICE_DISPATCH_CHAINS` settings. The existing runtime, modules, headers, and
CMake package continue to build and install as before.

Event and chain headers keep their existing numeric definitions unless
`DICE_ROLLED` is defined. The generated `dice.h` defines that flag and all
resolved IDs before providing the event payload types. Downstream headers can
use the same pattern:

```c
#ifndef DICE_ROLLED
#define EVENT_REQUEST 200
#define CHAIN_APPLICATION 11
#endif

struct request_event { int value; };
```

Legacy builds use these numbers unchanged. To migrate, include generated
`dice.h` first, or force-include it in application/module compilation with
`-include /path/to/generated/dice.h`. Existing `<dice/...>` includes can remain
when Dice's source headers are on the include path. Generated mode supplies the
IDs and module macros; the downstream payload structs remain in their headers.
When force-including the header, select the module slot through compiler options
too, for example `-DDICE_MODULE_SLOT=SLOT_PROCESS`.
Declare each downstream endpoint in the configuration: generated mode does not
fall back to undeclared legacy IDs. Built-in Dice IDs and aliases retain their
existing values. Compile `dice.c` normally, and rebuild application modules and
plugins against its matching header; legacy `.o` files are not interchangeable
with generated-runtime objects. Do not link the legacy `dice.h` CMake target
into migrated targets, since it supplies legacy capacity definitions.

## Configuration

The input is a declarative S-expression with a required `(schema 1)`:

```scheme
(dice
  (schema 1)
  (include "interceptors.dice")
  (runtime
    (plugins true)
    (mempool-size 209715200))

  (slot capture
    (after dice-self)
    (before process)
    (consumes (CAPTURE_BEFORE EVENT_MALLOC))
    (produces (CHAIN_APPLICATION EVENT_REQUEST)))

  (slot process
    (consumes (CHAIN_APPLICATION EVENT_REQUEST))
    (produces (CHAIN_RESULTS EVENT_RESPONSE))))
```

Events and chains are inferred from every `consumes` and `produces` endpoint.
Their declarations are optional: use `(events (EVENT_RESPONSE 200))` or
`(chains (CHAIN_RESULTS 12))` inside `dice` to pin IDs, or `(events EVENT_UNUSED)`
to declare an otherwise unused name. An endpoint can list multiple event types:
`(consumes (CHAIN_APPLICATION EVENT_REQUEST EVENT_RESPONSE))`.

Hyphens and underscores are equivalent in configuration symbols: `dice-self`
and `dice_self` refer to the same slot, and `EVENT-MALLOC` means `EVENT_MALLOC`.
This also applies to fields such as `mempool-size`.
Names are normalized to underscores in generated C identifiers
and query output. Declaring both spellings of the same name is a duplicate.
Quoted strings, including file paths, are preserved exactly.

`ANY_EVENT` and `CHAIN_DICE_CONTROL` are always defined as zero in their
respective ID spaces, even in an empty configuration. Neither requires a
declaration, and zero is reserved: these names cannot be reassigned or used by
custom IDs. Legacy builds also define `CHAIN_CONTROL` as an alias for
`CHAIN_DICE_CONTROL`.

Slots receive unique positive numbers satisfying `before` and `after` constraints.
Add `(number 32)` to pin a slot. Dependency cycles, duplicate numbers, and forced
numbers that cannot satisfy the order abort generation without replacing existing
output. Event flows do not imply ordering constraints; only `before` and `after`
do. Ordering applies when handlers share a dispatched chain and event.

Implement the configured consumers in C files including the generated `dice.h`.
Select the slot using its generated macro before including the header:

```c
#define DICE_MODULE_SLOT SLOT_PROCESS
#include "dice.h"

PS_SUBSCRIBE(CHAIN_APPLICATION, EVENT_REQUEST, {
    /* Handle event; publish EVENT_RESPONSE on CHAIN_RESULTS if appropriate. */
    return PS_OK;
})
```

Compile these files together with `dice.c`. With plugins enabled, each application
consumer has a weak generated handler that invokes registered callbacks, or does
nothing when none are registered. A linked strong handler replaces that callback
wrapper, so its callbacks are not also called. With plugins disabled, every
consumer needs a linked handler. Undeclared direct consumers fail at compile
time. `produces` describes the topology; the C handler still performs the
publication. Payload structures stay in application headers.

Strong handlers must participate in the same link as `dice.c`. Include their
object files explicitly (CMake OBJECT targets work); a weak fallback alone will
not cause the linker to extract an implementation from a static archive. Preload
modules use callback registration through the generated header.

Compile configured modules that link with the core using `-DDICE_DISPATCH_MODULE`.
In release builds, their `PS_PUBLISH` calls use the internal publish path without
rechecking IDs. `ps_publish` calls and function pointers keep the public API's
validation. With `DICE_CHECK_ROUTES=1`, both publication forms validate the
declared routes. Plugins and applications linked against a shared Dice runtime
use the public path; do not give those publishers `DICE_DISPATCH_MODULE`.

- Module sources are supplied by the build, with their slots and routes declared
  explicitly in the configuration. There is no `embed` option. See
  `examples/roll/interceptors.dice` for the standard self, thread, and allocation
  routes. Names such as `dice-self` are ordinary slot names; they do not select
  source files or add routes automatically.
- `(runtime (plugins true))` is the default. Every declared consumer has a weak
  callback fallback unless its handler is linked into the runtime. Declare slots
  with `(slot monitor (consumes (CAPTURE_EVENT EVENT_THREAD_START)))`.
  Plugins include this runtime's generated `dice.h`, define `DICE_PLUGIN_MODULE`
  and `DICE_MODULE_SLOT` (for example, `SLOT_MONITOR`), and use `PS_SUBSCRIBE`
  or `ps_subscribe`. Linked modules use direct handlers by default.
  The OS loads plugins through `LD_PRELOAD` /
  `DYLD_INSERT_LIBRARIES`. Registration happens at startup, before worker threads
  publish events.
- With `(runtime (plugins false))`, callback storage, weak fallback handlers and
  the trailing callback phase are omitted. Every declared consumer handler must
  be linked into the runtime; callback registration is rejected.
- Standard Dice IDs remain fixed. Automatic event IDs start at 128 and chain IDs
  at 7, skipping occupied IDs in alphabetical name order. Explicit IDs may use
  any unused number in 1..65534. Adding automatic names can change assignments:
  pin IDs when stability across configurations matters. All resolved IDs and
  `SLOT_*` macros appear in `dice.h`.
- Delivery has two phases. Slots through `DICE_LAST_KNOWN_SLOT` (`N`) run through
  the generated dispatcher in slot order, mixing strong handlers and callback
  fallbacks. Unknown plugin slots must be greater than `N` and run afterward in
  the existing ordered callback lists. `N` is global across chains and includes
  produces-only slots. Registrations into undeclared gaps at or below `N` are
  rejected in all builds.
- When `DICE_MODULE_SLOT` is omitted, plugins use `DICE_DEFAULT_PLUGIN_SLOT`:
  10000 or `N + 1`, whichever is larger. If `N` is `INT_MAX`, no unknown slot is
  available. Plugins can use only the generated event and chain IDs.
- For each configured chain and slot, an explicitly declared event takes
  precedence over `ANY_EVENT`. Generation selects exactly one handler; returning
  `PS_HANDLER_OFF` does not invoke the wildcard. This also applies when the
  selected handler is a callback wrapper with no registered callback. Events
  without an explicit declaration use the wildcard handler when present.
  `PS_STOP_CHAIN` stops both later direct handlers and plugins. Dynamic callback
  lists retain their existing delivery semantics.
- `(include "modules/events.dice")` loads another `(dice ...)` expression,
  relative to the including file. Included files may omit `schema`; define
  `runtime` in only one file. Unknown forms, duplicate IDs/endpoints, invalid
  references, and include cycles are rejected before generated files are replaced.
- Semicolons introduce comments. Strings support `\n`, `\r`, `\t`, `\\` and `\"`
  escapes. There is no evaluation, macro expansion, or executable configuration.
- Configured slots using `DICE_MODULE_INIT` also declare
  `(consumes (CHAIN_DICE_CONTROL EVENT_DICE_INIT))`.

Dispatch switches contain only explicitly subscribed event cases. Wildcards use
the default branch, so a plugin publishing a declared event does not require a
Cartesian expansion.

Generated dispatchers link with separately compiled `src/dice/pubsub.c` and
`src/dice/mempool.c`. Pubsub provides initialization, slot-ordered callback
registration, delivery, and the public runtime API. Builds with `(plugins false)`
also link `src/dice/pubsub-box.c` for dispatch-only delivery. Generation supplies
the dispatcher, event/chain tables, and configuration checks, plus weak callback
wrappers when plugins are enabled. Callback lists use compact indexes when
public IDs are sparse.

Runtime and plugins must use the same generated header. The header carries a
configuration/template fingerprint, checked when a plugin loads and registers.
It detects different generated configurations; it does not describe application
payload layouts or compensate for incompatible compiler options. Rebuild the
runtime and its plugins together after changing the configuration.

For test builds, compile the runtime, application modules, and plugins with
`-DDICE_CHECK_ROUTES=1`. Publications through `ps_publish` or `PS_PUBLISH` must
match the calling slot's `produces`; callback registration through `ps_subscribe`
must match the receiving slot's `consumes`. A violation terminates the process
with the slot, chain, and event in the diagnostic, even if the caller ignores
return codes. `PS_SUBSCRIBE` in a checked plugin also checks its declaration at
compile time. Static consumers are checked at compile time in all builds.

Known publishers and consumers in a checked build select their declared slot
with `DICE_MODULE_SLOT`. Unknown plugin slots above `N` may publish and consume
any declared chain/event; they have no configured routes to check.
Inside `PS_SUBSCRIBE_SLOT`, publications use that
handler's explicit slot. Helper functions outside a handler use the translation
unit's `DICE_MODULE_SLOT`. An `ANY_EVENT` endpoint allows all declared event types
on that chain; an exact consumption does not permit a wildcard subscription.

Checked builds reject calls through the raw `ps_publish` function pointer because
it carries no slot context. Use a wrapper calling `ps_publish` normally, or
`dice_publish_checked(slot, chain, type, event, md)` when passing an explicit slot.
The configuration fingerprint also rejects mixing checked and unchecked modules.
Without `DICE_CHECK_ROUTES`, per-slot route checks are not compiled in.
Linked dispatch modules use the internal publication path; other publishers
use the public API. Registration still rejects
invalid IDs, undeclared known-slot subscriptions, and incompatible headers.

## Inspecting a configuration

```sh
build/src/cli/dice-roll --check system.dice
build/src/cli/dice-roll --query system.dice
build/src/cli/dice-roll --graph system.dice > topology.dot
dot -Tsvg topology.dot -o topology.svg
```

`query` prints resolved IDs, slots, and their consumed/produced endpoints. `graph`
emits these flows and dashed slot ordering edges in DOT format. It does not infer
publications or forwarding from arbitrary C handler code. SVG rendering uses
optional Graphviz.

## Architecture Overview

Dice is composed of several modules. Its core modules include

- **Mempool**: Centralized memory manager for modules and threads.
- **Pubsub**: Event routing system based on chains and event types.
- **Self**: Manages thread-local storage and handles thread lifecycle events.

Dice also provides interceptor modules such as

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
cmake -B build -DCMAKE_INSTALL_PREFIX=<PREFIX>
cmake --build build
cmake --install build
```

The installation prefix is `/usr/local` by default. Installation includes the
existing headers, `libdice`, interceptor libraries, `tsano`, and the `dice`
preload script. Invocations such as `dice -malloc ./program` are unchanged.
The standalone `dice-roll` generator is available only in the source build.
Installation also provides a CMake package:
`find_package(dice CONFIG REQUIRED)` exposes `dice::dice` and `dice::dice.h`.
The package requires libvsync 4.2 or newer to be discoverable by CMake.

The generator, S-expression parser, and slot resolver are written in C;
no Python is required. The parser lives in `src/cli/sxp.c`
and `src/cli/sxp.h`, with its test in `test/roll/sxp.c`. `libtmplr.a` is linked
into the executable and renders templates loaded from `src/cli/templates`.
The generator reads LICENSE from the source checkout and gets existing built-in
ID values from the Dice headers at compile time. Compiling its output requires
the Dice core sources and Dice/libvsync
headers. The CMake helpers handle these paths when importing Dice with
`add_subdirectory`.

Legacy libraries and the `scripts/dice` preload helper remain available inside
the source build for existing regression tests and source-tree consumers.
Run `ctest --test-dir build --output-on-failure`; the roll tests build generated
runtimes and plugins, check existing CMake compatibility, and verify the
installed libraries, exported targets, and preload command.

## Further information

See examples in the `examples` directory.

- Detailed architecture notes: [doc/design.md](doc/design.md).
- Header-level API reference: [doc/api.md](doc/api.md).
- Contribution checklist (style, tests, etc):
  [doc/contributing.md](doc/contributing.md).
- Benchmark workflow: [doc/benchmarking.md](doc/benchmarking.md).
- Test layout and commands: [doc/testing.md](doc/testing.md).

---

## License

[0BSD License](LICENSE)

Generated `dice.c` and `dice.h` begin with the Dice license and a generation
notice. Run `build/src/cli/dice-roll --licenses` to display the Dice license. The libvsync
license is kept in `deps/LICENSE.libvsync`.
