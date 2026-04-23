# Copyright (c) 2025-2026 Diogo Behrens
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
#   version: 0.3.0
#   license: 0BSD
#
# Changelog:
# - v0.3.0: renamed PRO.* / process / .pro to SUM.* / summary / .sum
#
# Description:
# bench.mk is a small self-contained benchmark driver for GNU Make and BSD
# Make. The examples below use `make`, but `bmake` should work the same way.
#
# Usage:
#
# 1. Copy this file into your project.
# 2. In your benchmark Makefile, set BENCHMK and include it:
#
#     BENCHMK= ../bench.mk
#     include ${BENCHMK}
#
# 3. Add a target to TARGET:
#
#     TARGET+= example
#
# 4. Define one or more stages for that target:
#
#     CFG.example= ...
#     BLD.example= ...
#     RUN.example= ...
#     SUM.example= ...
#
# 5. Run it:
#
#     make run TARGET=example
#
# Quick start:
#
#     make -f bench.mk example
#
# Actions:
#
#     make -f bench.mk help
#
# Variables controlling a target `foo`:
#
# - DIR.foo: directory where commands of foo execute. default: WORKDIR/.foo
# - CFG.foo: target config, eg, /path/to/configure
# - BLD.foo: target build, eg, make
# - RUN.foo: target run command, eg, ./target --option 1
# - SUM.foo: summarize results, eg, cat .$*.run.log | grep elapsed-time
# - DEP.foo: dependencies of the target, eg, .bar.bld
#
# Generated files, stamp files, and logs are kept under WORKDIR.
#
# One can use $* to refer to the target, for example,
#
#    FILTER_RESULTS= cat .$*.run.log | grep elapsed-time >> results.dat
#    ...
#    SUM.foo=	${FILTER_RESULTS}
#    ...
#    SUM.bar=	${FILTER_RESULTS}
#
# FILTER_RESULTS can be used in multiple targets. It will read
# WORKDIR/.TARGET.run.log and grep for the elapsed-time line and append the
# results in WORKDIR/results.dat
#
# Special variables available with bench.mk:
#
# - WORKDIR: points to the parent work directory where the targets are created
# - ROOTDIR: points to the directory where the benchmark Makefile is stored
# - BENCHMK: points to bench.mk. Benchmark Makefiles should set this explicitly
#   and include it via 'include ${BENCHMK}'.
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
ROOTDIR=	$(shell pwd)
ROOTDIR!=	pwd
WORKDIR=	${ROOTDIR}/work
MAKEFILE=	${ROOTDIR}/Makefile
BENCHMK?=	bench.mk

# User targets
help:
	@printf '%s\n' "Usage: make <action> TARGET=<bench>"
	@printf '%s\n' ""
	@printf '%s\n' "Actions:"
	@printf '%s\n' " configure       configure benchmark with CFG.bench"
	@printf '%s\n' " build           build benchmark using BLD.bench"
	@printf '%s\n' " run             run benchmark using RUN.bench"
	@printf '%s\n' " summary         summarize results using SUM.bench"
	@printf '%s\n' " example         create an example Makefile"
	@printf '%s\n' " info            show the value of bench.mk variables"

info:
	@printf '%s\n' "== bench.mk config =="
	@printf '%s\n' "   ROOTDIR:  ${ROOTDIR}"
	@printf '%s\n' "   WORKDIR:  ${WORKDIR}"
	@printf '%s\n' "   BENCHMK:  ${BENCHMK}"
	@printf '%s\n' "   MAKEFILE: ${MAKEFILE}"

configure:
	@${MAKE} cfg
build:
	@${MAKE} bld
summary:
	@${MAKE} sum
clean:
	rm -rf ${WORKDIR}
example:
	@if [ -f Makefile ]; \
	then printf '%s\n' "error: Makefile already exists"; exit 1; fi
	@printf '%s\n' "BENCHMK=	bench.mk" > Makefile
	@printf '%s\n' "TARGET+=	example" >> Makefile
	@printf '%s\n' "RUN.example=	echo 'Hello World'" >> Makefile
	@printf '%s\n' 'include		$${BENCHMK}' >> Makefile
	@printf '%s\n' "Created the following Makefile:"
	@cat Makefile
	@printf '%s\n' -----------------------------------------
	@printf '%s\n' "Run 'make run' to execute the benchmark"
	@${MAKE} -s run
	@printf '%s\n' -----------------------------------------
	@printf '%s\n' "A work directory was created with the results"
	@ls -A ${WORKDIR}
	@printf '%s\n' -----------------------------------------
	@printf '%s\n' "The output of the run action is in .example.run.log"
	@cat ${WORKDIR}/.example.run.log
	@printf '%s\n' -----------------------------------------
	@printf '%s\n' "Run 'make clean' to remove the work directory"
	@${MAKE} -s clean

# ------------------------------------------------------------------------------
# Action bootstrap
# ------------------------------------------------------------------------------
.SUFFIXES: .enable .dir .cfg .bld .run .sum

${WORKDIR}:
	mkdir -p ${WORKDIR}

dir cfg bld run sum: ${WORKDIR}
	@if [ -z "${TARGET}" ]; then ${MAKE} help; fi
	@for B in ${TARGET}; do                                                \
		${MAKE} -C ${WORKDIR} -f ${MAKEFILE}                           \
		_MAKE="${MAKE} -C ${WORKDIR} -f ${MAKEFILE} ROOTDIR=${ROOTDIR} \
			BENCHMK=${ROOTDIR}/${BENCHMK}"                         \
		ROOTDIR=${ROOTDIR} BENCHMK=${ROOTDIR}/${BENCHMK}               \
		_TARGET=$$B _ACTION=$@ _dispatch;                              \
	done

_dispatch:
	@if [ ! -f ".${_TARGET}.enable" ]; then \
		touch .$(_TARGET).enable;       \
	fi
	@if [ ! -z "${FORCE}" ]; then         \
		rm -f .${_TARGET}.${_ACTION}; \
	fi
	@${_MAKE} _TARGET=.${_TARGET} _deps
	@${_MAKE} .${_TARGET}.${_ACTION}

# ------------------------------------------------------------------------------
# Inference rules
# ------------------------------------------------------------------------------

.enable.dir:
	@if [ -z "${DIR$*}" ]; then           \
		mkdir -p ${WORKDIR}/$*;       \
	else                                  \
		mkdir -p ${WORKDIR}/${DIR$*}; \
	fi
	@touch $@

.dir.cfg:
	@${_MAKE} _TARGET=$* _DIR="${DIR$*}" _CMD="${CFG$*}" _ACTION=$@ _exec
	@touch $@

.cfg.bld:
	@${_MAKE} _TARGET=$* _DIR="${DIR$*}" _CMD="${BLD$*}" _ACTION=$@ _exec
	@touch $@

.bld.run:
	@${_MAKE} _TARGET=$* _DIR="${DIR$*}" _CMD="${RUN$*}" _ACTION=$@ _exec
	@touch $@

.run.sum:
	@${_MAKE} _TARGET=$* _DIR="${DIR$*}" _CMD="${SUM$*}" _ACTION=$@ _exec
	@touch $@

# ------------------------------------------------------------------------------
# Generic _execution targets
# ------------------------------------------------------------------------------

# arguments: _CMD _ACTION _DIR _TARGET
_deps:
	@for D in ${DEP${_TARGET}}; do                                  \
		if [ -z "${FORCE}" ] && [ -f "$$D" ]; then continue; fi;\
		case "$$D" in                                           \
		*.enable) ;;                                            \
		*.*) E=$${D%.*}.enable; touch "$$E" ;;                  \
		esac;                                                   \
			${_MAKE} "$$D" || exit 1;                       \
	done

_exec:
	@if [ "${_CMD}" != "" ]; then \
		${_MAKE} _exec-dir;   \
	fi

_exec-dir:
	@if [ -z "${_DIR}" ]; then                       \
		${_MAKE} _DIR=${WORKDIR}/${_TARGET}      \
			_LOG=${_ACTION}.log _exec-mode ; \
	else                                             \
		${_MAKE} _DIR=${WORKDIR}/${_DIR}         \
			_LOG=${_ACTION}.log _exec-mode;  \
	fi

_exec-mode:
	@printf '%s' "${_ACTION} ... "
	@if [ -z "${VERBOSE}" ]; then   \
		${_MAKE} _exec-quiet;   \
	else 	                        \
		${_MAKE} _exec-verbose; \
	fi
	@printf '%s\n' done

_exec-verbose:
	@printf "#cd %s && %s\n\n" "${_DIR}" "${_CMD}" | tee ${_LOG}
	(cd ${_DIR} && ${_CMD}) 2>&1 | tee -a ${_LOG}

_exec-quiet:
	@printf "#cd %s && %s\n\n" "${_DIR}" "${_CMD}" > ${_LOG}
	@(cd ${_DIR} && ${_CMD}) >> ${_LOG} 2>&1 || (cat ${_LOG} && false)
