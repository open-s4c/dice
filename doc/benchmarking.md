# Dice Benchmarking Guide

Dice benchmarks are opt-in so they do not affect day-to-day builds. Configure a
workspace with benchmarks enabled, build, and then drive the suites with the
wrapper script:

```sh
cmake -B build -DDICE_BENCHMARKS=ON
cmake --build build
./scripts/benchmark.sh
```

The script fans out to each suite, preserves the raw logs in `bench/*/work/`,
and copies CSV summaries under `results/<host>/<name>/<date>/`.

Individual benchmarks can be started by entering the benchmark directory and
running:

    make build   # builds the benchmark
    make run     # runs the benchmark (assumes Dice is already compiled)
    make process # generates work/results.csv

## Benchmark Suites

- `micro`: Three synthetic publish loops that stress the hot paths of Pubsub.
  `micro` publishes bare events, `micro2` adds capture handlers and TLS via the
  Self module, and `micro3` links against the `micro-dice` bundle to exercise
  generated dispatch code.
- `leveldb`: Builds Google's LevelDB `db_bench` tool and runs `readrandom`
  against a populated database to approximate a storage workload.
- `raytracing`: Renders `theRestOfYourLife` from raytracing.github.io to
  represent a CPU-bound C++ application with heavy threading.
- `scratchapixel`: Rasterizes the Scratchapixel reference scene to cover a
  graphics-style workload with coarse-grained threading.

## Measurement Scenarios

Each suite reuses the same scenario names so results are comparable:

- `baseline`: Upstream program compiled in Release mode with no sanitizers or
  Dice preloads. Serves as the performance reference point.
- `tsan`: Program built with ThreadSanitizer instrumentation and executed with
  the stock libtsan runtime.
- `tsano`: Same instrumentation as `tsan`, but executed through the Dice
  `tsano` launcher to quantify the cost of Dice's replacement runtime.
- `core`: Only `libdice` is preloaded (no extra modules) so we can measure the
  fixed cost of the runtime and loader.
- `intercept`: Loads `libdice` plus the interceptor modules for pthreads,
  malloc/free, C++ guards, and libtsan. Modules are injected as individual
  DSOs via the system loader (the configuration often labelled "elf" in
  reports).
- `self`: Extends `intercept` by also loading `dice-self` to account for TLS
  management overhead when subscribers depend on Self metadata.
- `bundle`: Uses the monolithic `libdice-bundle` produced in `bench/lib`, which
  links Dice core and the same intercept modules into a single shared library
  so dispatch happens through generated switch tables.
- `box`: Uses `libdice-bundle-box` (also under `bench/lib`) to measure the
  fast-path where only dispatch-based modules remain and plugins are absent.
- `cbonly`: Loads `libdice-bundle-cbonly` to isolate the callback-only path and
  compare it with the bundled dispatch implementations.
