`libtsano` implements an empty TSAN interface. Atomic operations behave as
expected compiler builtins and seq_cst barriers.

Use `tsano` script to start a program with `libtsano` instead of `libtsan`.
