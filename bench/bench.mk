# Copyright (c) 2025 Diogo Behrens
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
# OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.
# ------------------------------------------------------------------------------
# bench.mk - a terrible benchmark framework for stuborn developers
#   version: 0.2.1
#   license: 0BSD
#
# Quick start:
#
#     make -f bench.mk example
#
# Actions:
#
#     make -f bench.mk help
#
# Define a benchmark Makefile:
#
# 1. Add a target to TARGET variable:
#     TARGET+= example
# 2. Add the target command:
#     RUN.example= echo "hello world" && ls
# 3. Include bench.mk file at last:
#     include bench.mk
#
# Variables controlling a target `foo`:
#
# - DIR.foo: directory where commands of foo execute. default: WORKDIR/.foo
# - CFG.foo: target config, eg, /path/to/configure
# - BLD.foo: target build, eg, make
# - RUN.foo: target run command, eg, ./target --option 1
# - PRO.foo: processing of results, eg, cat .$*.run.log | grep elapsed-time
# - DEP.foo: dependencies of the target, eg, .bar.bld
#
# One can use $* to refer to the target, for example,
#
#    FILTER_RESULTS= cat .$*.run.log | grep elapsed-time >> results.dat
#    ...
#    PRO.foo=	$(FILTER_RESULTS)
#    ...
#    PRO.bar=	$(FILTER_RESULTS)
#
# FILTER_RESULTS can be used in multiple targets. It will read
# WORKDIR/.TARGET.run.log and grep for the elapsed-time line and append the
# results in WORKDIR/results.dat
#
# Special variables available with bench.mk:
#
# - WORKDIR: points to the parent work directory where the targets are created
# - ROOTDIR: points to the directory where the benchmark Makefile is stored
# - BENCHMK: points to bench.mk, default './bench.mk'. If you set this variable,
#   you have to add 'include $(BENCHMK)' to your Makefile.
#
# Command line options:
#
# - TARGET: select a subset of targets, eg, make run TARGET="foo bar"
# - VERBOSE: output results to stdout as well, eg, make run VERBOSE=1
# - FORCE: force action to execute even if it was successfully executed before,
#   eg, make run FORCE=1 VERBOSE=1 TARGET=bar
#
# Do not use these variables for other purposes in your Makefile.
#
# Besides these, other "hidden" variables are _MAKE, _TARGET, _ACTION, _CMD,
# _DIR, and _LOG. Setting these will break this bench.mk.
#
# Limitations:
#
# - bench.mk requires the benchmark Makefile be called 'Makefile'
#
# ------------------------------------------------------------------------------

# This file is compatible with BSD Make and GNU Make
.POSIX:
.SUFFIXES:
.SECONDARY:

# General configuration
ROOTDIR!=	pwd
WORKDIR=	$(ROOTDIR)/work
MAKEFILE=	$(ROOTDIR)/Makefile
BENCHMK!=	if [ -z "$(BENCHMK)" ]; \
		then readlink -f bench.mk; \
		else readlink -f $(BENCHMK); fi

# User targets
help:
	@echo "Usage: make <action> TARGET=<bench>"
	@echo
	@echo "Actions:"
	@echo " configure       configure benchmark with CFG.bench"
	@echo " build           build benchmark using BLD.bench"
	@echo " run             run benchmark using RUN.bench"
	@echo " process         process results using PRO.bench"
	@echo " example         create an example Makefile"
	@echo " info            show the value of bench.mk variables"

info:
	@echo "== bench.mk config =="
	@echo "   ROOTDIR:  $(ROOTDIR)"
	@echo "   WORKDIR:  $(WORKDIR)"
	@echo "   BENCHMK:  $(BENCHMK)"
	@echo "   MAKEFILE: $(MAKEFILE)"

configure:
	@$(MAKE) cfg
build:
	@$(MAKE) bld
process:
	@$(MAKE) pro
clean:
	rm -rf $(WORKDIR)
example:
	@if [ -f Makefile ]; \
	then echo "error: Makefile already exists"; exit 1; fi
	@echo "TARGET+=\texample" > Makefile
	@echo "RUN.example=\techo 'Hello World'" >> Makefile
	@echo "include\t\tbench.mk" >> Makefile
	@echo "Created the following Makefile:"
	cat Makefile
	@echo -----------------------------------------
	@echo "Run 'make run' to execute the run action of the benchmark"
	$(MAKE) -s run
	@echo -----------------------------------------
	@echo "A work directory was created with the results"
	ls $(WORKDIR)
	@echo -----------------------------------------
	@echo "The output of the `run` action is in .TARGET.run.log"
	cat $(WORKDIR)/.example.run.log
	@echo -----------------------------------------
	@echo "Run 'make clean' to remove the work directory"
	$(MAKE) -s clean

# ------------------------------------------------------------------------------
# Action bootstrap
# ------------------------------------------------------------------------------
.SUFFIXES: .enable .dir .cfg .bld .run .pro

$(WORKDIR):
	mkdir -p $(WORKDIR)
	cp $(BENCHMK) $(WORKDIR)

dir cfg bld run pro: $(WORKDIR)
	@if [ -z "$(TARGET)" ]; then $(MAKE) help; fi
	@for B in $(TARGET); do                              \
		$(MAKE) -C $(WORKDIR) -f $(MAKEFILE)         \
		_MAKE="$(MAKE) -C $(WORKDIR) -f $(MAKEFILE)" \
		ROOTDIR=$(ROOTDIR) BENCHMK=$(BENCHMK)        \
		_dispatch _TARGET=$$B _ACTION=$@;            \
	done

_dispatch: $(DEP.$(_TARGET))
	@if [ ! -f ".$(_TARGET).enable" ]; then \
		touch .$(_TARGET).enable;       \
	fi
	@if [ ! -z "$(FORCE)" ]; then         \
		rm -f .$(_TARGET).$(_ACTION); \
	fi
	@$(_MAKE) .$(_TARGET).$(_ACTION)

# ------------------------------------------------------------------------------
# Inference rules
# ------------------------------------------------------------------------------

.enable.dir:
	@if [ -z "$(DIR$*)" ]; then           \
		mkdir -p $(WORKDIR)/$*;       \
	else                                  \
		mkdir -p $(WORKDIR)/$(DIR$*); \
	fi
	@touch $@

.dir.cfg:
	@$(_MAKE) _exec _TARGET=$* _DIR="$(DIR$*)" _CMD="$(CFG$*)" _ACTION=$@
	@touch $@

.cfg.bld:
	@$(_MAKE) _exec _TARGET=$* _DIR="$(DIR$*)" _CMD="$(BLD$*)" _ACTION=$@
	@touch $@

.bld.run:
	@$(_MAKE) _exec _TARGET=$* _DIR="$(DIR$*)" _CMD="$(RUN$*)" _ACTION=$@
	@touch $@

.run.pro:
	@$(_MAKE) _exec _TARGET=$* _DIR="$(DIR$*)" _CMD="$(PRO$*)" _ACTION=$@
	@touch $@

# ------------------------------------------------------------------------------
# Generic _execution targets
# ------------------------------------------------------------------------------

# arguments: _CMD _ACTION _DIR _TARGET
_exec:
	@if [ "$(_CMD)" != "" ]; then \
		$(_MAKE) _exec-dir;    \
	fi

_exec-dir:
	@if [ -z "$(_DIR)" ]; then                             \
		$(_MAKE) _exec-mode _DIR=$(WORKDIR)/$(_TARGET) \
			_LOG=$(_ACTION).log ;                  \
	else                                                   \
		$(_MAKE) _exec-mode _DIR=$(WORKDIR)/$(_DIR)    \
			_LOG=$(_ACTION).log;                   \
	fi

_exec-mode:
	@echo -n "$(_ACTION) ... "
	@if [ -z "$(VERBOSE)" ]; then   \
		$(_MAKE) _exec-quiet;   \
	else 	                       \
		$(_MAKE) _exec-verbose; \
	fi
	@echo done

_exec-verbose:
	(cd $(_DIR) && $(_CMD)) 2>&1 | tee $(_LOG)
_exec-quiet:
	@(cd $(_DIR) && $(_CMD)) > $(_LOG) 2>&1 || (cat $(_LOG) && false)
