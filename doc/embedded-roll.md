# Embedding a rolled Dice runtime

For integrations that package Dice core and selected modules into a single
archive (including `dice-rs`), CMake can build an **opt-in object target**
without adding legacy dispatcher objects to the same archive:

```sh
cmake -S dice -B build-roll \
  -DDICE_TESTS=OFF -DDICE_BENCHMARKS=OFF \
  -DDICE_EXTENSION_DIRS=/path/to/craps/dice-extension \
  -DDICE_ROLL_CONFIG=/path/to/craps/dice-extension/roll/craps.dice \
  -DDICE_ROLL_IMPORT_PATHS=dice=/path/to/craps/dice-extension/roll \
  '-DDICE_ROLL_MODULES=self;pthread_create;malloc;fcntl'
cmake --build build-roll --target dice-roll-runtime
```

The CMake object target is `dice-roll-runtime`; generated files live in
`build-roll/dice-roll-runtime-dice/`. Its companion interface target is
`dice-roll-runtime-dice.h`.

The Roll configuration must import the module descriptions that correspond to
the compiled modules. The import path for externally maintained module
descriptions should use the `dice=` namespace, because `add_dice_mods`
assigns slots named `SLOT_DICE_<MODULE>`.

**Legacy and rolled runtime objects must not be combined in a single binary.**
Consume only the objects under `CMakeFiles/dice-roll-runtime.dir/`, rather
than recursively archiving all CMake `.o` files.

When replaying pre-existing traces, pin event IDs in the `.dice` description:
`(events (EVENT_FOO 123))`. Leave callback support enabled when Rust registers
handlers dynamically through `ps_subscribe`. Record/replay validation is
required before enabling sealing or disabling callback support.
