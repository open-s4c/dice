# `tmplr` - a template replacement tool

`tmplr` is a simple tool to achieve a minimum level of genericity in
[libvsync][], [Dice][], and other projects without resorting to C preprocessor
macros.

[libvsync]: https://github.com/open-s4c/libvsync
[Dice]: https://github.com/open-s4c/dice

## Quick Installation

Build `tmplr` with your favorite POSIX C toolchain (clang or gcc) using the
provided Makefile:

```sh
git clone https://github.com/open-s4c/tmplr.git
cd tmplr
make
sudo make PREFIX=/usr/local install    # optional
```

`make install` drops the binary, static library (`libtmplr.a`), man page, and
public headers (`tmplr.h`, `tmplr/macros.h`) under the chosen prefix. Set
`DESTDIR` when packaging for a distro.

## C library API

`tmplr` also ships a static library interface in `include/tmplr.h`.

```c
#include <tmplr.h>

tmplr_opts opts = {0};
tmplr_ctx *ctx = tmplr_create(&opts);
tmplr_set_override(ctx, "K", "v1;v2");
tmplr_process_file(ctx, "input.in", NULL, NULL);
tmplr_destroy(ctx);
```

For the complete CLI and directive reference, see the bundled man page:

```
man ./tmplr.1
```

## Template

`tmplr` reads input files and replaces mappings in template blocks. Template
blocks are marked with `$_begin` and `$_end` commands.

For example:

    $_begin(key=value)
    The following word, key, will be replaced by value.
    $_end

Iteration mappings may take a single value as in `key=value1` or multiple
values as in `key=[[value1; value2]]`. The list of values is separated by
semicolumn and optionally sorrounded by `[[` and `]]`.  Consider the example:

    $_begin(key=[[val1;val2]])
    Key --> key
    $_end

The template expansion will generate one block for each iteration. In the first
iteration, `key=val1`, in the second iteration, the mapping is `key=val2`.

## Persistent mappings

Outside template blocks you can define mappings that apply everywhere by using
`$_map(KEY, VALUE)` (or `TMPL_map` if you change the prefix). tmplr applies
iteration mappings first and then these persistent mappings, so they behave like
global defaults or overrides for identifiers that appear across many blocks.

## Command line and mapping override

`tmplr` takes a list of files as input and writes the expanded result to
the standard output. It provides the following flags:

- `-b LINES` changes the maximum number of buffered lines per block
  (default `100`). Increase it for large blocks at the cost of more memory.
- `-v` for verbose output and
- `-D` to redefine the list of values that a mapping iterates over. For example,
  `-D keyA="4;5;6"` replaces whatever the template provided, so the block runs
  with 4, then 5, then 6 regardless of the original list.
- `-F` to keep only the intersection between the template’s list and the filter
  list. If the template says `_begin(key=[[1;2;3]])`, passing
  `-F key="1;4"` will iterate only `1` because 4 was not part of the original
  mapping.
- `-P TMPL` change command prefix to `TMPL_begin`, `TMPL_end`, etc.
- `-i` takes input from stdin in addition to file names. stdin is the last
  input to be processed.

## Valid keys and values

`tmplr` **does not** tokenize the input, so multiword keys such as `two words`
are valid and characters like `$` may appear anywhere. Leading and trailing
spaces are stripped when a key is parsed, so `KEY`, ` KEY`, and `KEY ` refer to
the same identifier (do the same for `$_map(...)` and `$_hook(...)` calls). If
you need literal whitespace at the edges, encode it explicitly (for example by
mapping `_SPACE` and referencing that placeholder inside the block).

Keys cannot contain

- new line: `\n`
- parenthesis: `(` `)`
- comma: `,`
- semicolon: `;`
- nor any `tmplr` commands

Values cannot contain parenthesis, commas, or semicolons. Like keys, they are
trimmed at the edges, so surrounding spaces must be injected manually if
needed.

## Disclaimer

We are aware of similar, more powerful tools such as Jinja, Mustache and M4.
`tmplr` follows three design principles:

- simplicity: as simple and maintainable as possible
- dependency freedom: no additonal language which will get deprecated
- c-syntax transperency: annotation should not interfer with the LSP

## Tests

To test `tmplr`, run:

    make test

It has been tested on different Linux flavors, macOS, and NetBSD.

### Coverage

The target `coverage` compiles the tool with coverage option, with that one can
use a tool such as `gcovr` to pretty print the results:

    make coverage
    make test
    # run your coverage tool.

Currently the coverage is 75%.

### Fuzzing `tmplr` with AFL

After installing AFL, compile `tmplr` with `afl-gcc`:

    make CC=afl-gcc

Then run AFL with the tests in the `test/` directory. Since the tests uses
`_tmpl` prefix, you have also to pass that argument to `tmplr`.

    afl-fuzz -i test -o results -m256 -- ./tmplr -P _tmpl @@
