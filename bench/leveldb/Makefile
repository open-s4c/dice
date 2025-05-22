# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

# ------------------------------------------------------------------------------
# Commands and variables for all LevelDB benchmark variants
# ------------------------------------------------------------------------------
ROOT!=		pwd
DBDIR=		/tmp/bench.db
COMMAND=	./db_bench --db=$(DBDIR)
CONFIGURE=	cmake -S ../../leveldb -DCMAKE_BUILD_TYPE=Release \
			-DHAVE_STD_REGEX=ON -DRUN_HAVE_STD_REGEX=1
BUILD= 		$(MAKE)
PREPARE= 	rm -rf $(DBDIR) && $(COMMAND) --threads=1 --benchmarks=fillseq
RUNCMD= 	$(COMMAND) --threads=1 --benchmarks=readrandom --duration=10
PARSE=		cat $(ROOT)/$*.run.log  \
		| grep benchstats \
		| cut -d: -f2 \
		| xargs -n1 echo "$*"';' \
		| tee -a $(ROOT)/results.csv

# ------------------------------------------------------------------------------
# clone level, apply patches, build dice
# ------------------------------------------------------------------------------

URL=		https://github.com/google/leveldb.git
SHA=		068d5ee1a3ac40dabd00d211d5013af44be55bea

TARGET+=	clone
DIR.clone=	leveldb
CFG.clone=	git clone --recurse-submodules $(URL) .
BLD.clone=	git checkout $(SHA) && git submodule update --init --recursive

TARGET+=	patch
DEP.patch=	.clone.bld
DIR.patch=	leveldb
CFG.patch=	git checkout .
BLD.patch=	git apply $(ROOT)/patches/01-leveldb

# ------------------------------------------------------------------------------
# Variants
# ------------------------------------------------------------------------------

TARGET+=	baseline
DEP.baseline=	.patch.bld
DIR.baseline=	build/leveldb
CFG.baseline= 	$(CONFIGURE)
BLD.baseline=	$(BUILD)
PRE.baseline= 	$(PREPARE)
RUN.baseline= 	$(RUNCMD)
PRO.baseline=	$(PARSE)

TARGET+=	tsan
DEP.tsan=	.baseline.pre
DIR.tsan=	build/leveldb-tsan
CFG.tsan= 	$(CONFIGURE) -DCMAKE_CXX_FLAGS=-fsanitize=thread
BLD.tsan=	$(BUILD)
RUN.tsan=	env TSAN_OPTIONS=report_bugs=0 $(RUNCMD)
PRO.tsan=	$(PARSE)
# Linux: if TSAN fails with "FATAL: ThreadSanitizer: unexpected memory mapping"
# Try running this
#	sudo sysctl vm.mmap_rnd_bits=30

TARGET+=	tsano
DEP.tsano=	.tsan.bld
DIR.tsano=	$(DIR.tsan)
RUN.tsano=	$(ROOT)/../../scripts/tsano $(RUNCMD)
PRO.tsano=	$(PARSE)

TARGET+=	dice
DEP.dice=	.tsan.bld
DIR.dice=	$(DIR.tsan)
RUN.dice=	$(ROOT)/../../scripts/dice $(RUNCMD)
PRO.dice=	$(PARSE)

TARGET+=	builtin
DEP.builtin=	.tsan.bld
DIR.builtin=	$(DIR.tsan)
RUN.builtin=	$(ROOT)/../../scripts/dice-builtin $(RUNCMD)
PRO.builtin=	$(PARSE)

# ------------------------------------------------------------------------------
include ../bench.mk
