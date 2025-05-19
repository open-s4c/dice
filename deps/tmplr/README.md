# `tmplr` - a template replacement tool

`tmplr` is a simple tool to achieve a minimum level of genericity in
[libvsync][] and other projects without resorting to C preprocessor macros.

[libvsync]: https://github.com/open-s4c/libvsync

## Quick Installation

You can use `tmplr` as a script as follows:

    curl -LO https://github.com/open-s4c/tmplr/blob/main/tmplr.c
    mv tmplr.c tmplr
    chmod +x tmplr
    mv tmplr /place/in/path

## Template

`tmplr` reads input files and replaces mappings in template blocks. Template
blocks are marked with `__tmpl_begin` and `__tmpl_end` commands.

For example:

    __tmpl_begin(key=value)
    The following word, key, will be replaced by value.
    __tmpl_end

Iteration mappings may take a single value as in `key=value1` or multiple
values as in `key=[[value1; value2]]`. The list of values is separated by
semicolumn and optionally sorrounded by `[[` and `]]`.  Consider the example:

    __tmpl_begin(key=[[val1;val2]])
    Key --> key
    __tmpl_end

The template expansion will generate one block for each iteration. In the first
iteration, `key=val1`, in the second iteration, the mapping is `key=val2`.

## Command line and mapping override

`tmplr` takes a list of files as input and writes the expanded result to
the standard output. It provides the following flags:
- `-v` for verbose output and
- `-D` to select a single value for an iteration mapping. For example,
  - `-DkeyA=value1`. Other values will be ignored.
  - `-Dkey="value1;value2"` sets key to the list `value1;value2`
- `-P` change command prefix which defaults to `_tmpl`, eg, `-PTEMPLATE`
  assumes commands of the form TEMPLATE_begin, TEMPLATE_end, etc.
- `-i` takes input from stdin in addition to file names. stdin is the last
  input to be processed.


## Valid keys and values

`tmplr` **does not** tokenize the input. Hence, a key "two words" is a
perfectly valid key. Characters such as $ can also be used in keys and values.

The only restriction is that keys cannot contain
- new line: `\n`
- parenthesis: `(` `)`
- comma: `,`
- semicolon: `;`
- nor any `tmplr` commands

Values cannot contain parenthesis, commas nor semicolon.

## Disclaimer

We are aware of similar, more powerful tools such as Jinja, Mustache and M4.
`tmplr` follows three design principles:

- simplicity: as simple and maintainable as possible
- dependency freedom: no additonal language which will get deprecated
- c-syntax transperency: annotation should not interfer with the LSP
