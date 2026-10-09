# Configured runtimes with Dice Roll

Dice Roll generates event, chain, and slot IDs and the dispatch code for a
configuration. Import the Dice source tree into your project to build a
configured runtime. Existing Dice targets, installed libraries, and preload
scripts remain available for traditional usage.

## Build a configured library

With a Dice checkout at `deps/dice`, add this to your CMake project:

```cmake
add_subdirectory(deps/dice EXCLUDE_FROM_ALL)

add_library(observer SHARED observer.c)
add_dice_core(observer "${CMAKE_CURRENT_SOURCE_DIR}/observer.dice")
add_dice_mods(observer self pthread_create)
```

The configuration imports the selected interceptors and declares the observer:

```scheme
(import (dice self) (dice pthread_create))

(module observer
  (after (dice self))
  (subscribes (CAPTURE_EVENT EVENT_THREAD_START)))
```

In `observer.c`, select the module's implicit slot before including the
generated interface:

```c
#define DICE_MODULE_SLOT SLOT_OBSERVER
#include <dice/roll.h>
#include <stdio.h>

PS_SUBSCRIBE(CAPTURE_EVENT, EVENT_THREAD_START, {
    puts("thread started");
})
```

`add_dice_core` generates `observer-dice/dice.h` and `dice.c` in the build
directory and compiles the dispatch code and ordinary core sources into the
target. `add_dice_mods` compiles the selected interceptor sources into that same
target. Declare their routes with `import` in the configuration as well.
Imports and includes are tracked as build dependencies. Searched directories
are tracked too, so adding a higher-precedence import override regenerates the
configured runtime during an incremental build.

The helpers define `DICE_ROLLED` for configured targets. Include `dice/roll.h`
or the generated `dice.h` before other Dice headers. The generated header also
opts a source into rolled mode when used directly.

On macOS, put interceptors in a shared library so dyld can redirect calls made
by the application.

## Targets and configuration

`add_dice_core` accepts shared, module, and object libraries, and executables.
Static library targets are currently rejected: weak callback fallbacks can
satisfy archive references before the linker extracts strong handler objects.
For an embedded core, use an object library and link that object target directly
into the final executable or shared library. Its generated interface target,
`<target>-dice.h`, supplies the include paths, configuration flags, and
generation dependency to separately compiled consumers.

Each `module` has an implicit slot for its direct `subscribes` and `publishes`
fields. Additional named slots must be nested inside a module. Their generated
names include the full module path, such as `SLOT_OBSERVER_STATS` for a slot
named `stats` inside `observer`. Use `PS_SUBSCRIBE_SLOT` to select an explicit
slot in a source with another default.

Events and chains are inferred from `subscribes` and `publishes`; explicit
`events` and `chains` declarations can introduce names or pin numeric IDs.
`subscribes` and `publishes` are the only route-declaration keywords.
`before` and `after` establish handler order. Missing optional ordering targets
are ignored; use `require` for dependencies that must exist.

Callbacks are enabled by default. Loadable modules use the same generated
header and define `DICE_PLUGIN_MODULE`; load them before initialization if they
subscribe to `EVENT_DICE_INIT`. Slots with callbacks disabled require linked
handlers for their declared subscriptions. Rebuild the runtime and its consumers
together when the configuration changes. Add `CHECK_ROUTES` to `add_dice_core`
to check publications and subscriptions against their declared routes.

Built-in aliases, such as `EVENT_THREAD_CREATE` and `EVENT_PTHREAD_CREATE`,
retain the same numeric ID. Name lookups and registration accept either
spelling; ID-to-name functions select one canonical spelling for diagnostics.

## Use the generator directly

From the Dice checkout, build the native tool explicitly when tests and
benchmarks are disabled. Substitute your configuration's path for
`observer.dice`:

```sh
cmake -S . -B build -DDICE_TESTS=OFF -DDICE_BENCHMARKS=OFF
cmake --build build --target dice-cli
build/src/cli/dice-roll --check observer.dice
build/src/cli/dice-roll --query observer.dice
build/src/cli/dice-roll observer.dice --output generated
```

`--query` reports all configured names and IDs, including built-in aliases.
`--graph` emits DOT output. Use `-I DIRECTORY` or `-I NAMESPACE=DIRECTORY` to
add import roots, and `--depfile FILE` to write configuration dependencies. The
generated C file contains dispatch code and tables; the CMake helpers compile it
with the Dice core and the selected modules.

The generator is a **source-tree tool**. It is not installed by
`cmake --install`, and copying the binary does not make it relocatable. It reads
LICENSE, templates, and bundled module definitions from the checkout path used
when it was built. Keep that checkout available at that path, and reconfigure
and rebuild after moving it. Installed-package generation and relocatable tool
packaging are outside this interface's current scope.

Cross builds prepare a native generator automatically. Set `DICE_HOST_CC` to
select its compiler, or `DICE_HOST_EXECUTABLE` to use an existing matching
native generator that can still access its source checkout.
