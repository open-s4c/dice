# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

# ------------------------------------------------------------------------------
# Commands and variables for all LevelDB benchmark variants
# ------------------------------------------------------------------------------
PROJECT!=	readlink -f $(ROOTDIR)/../..
DBDIR=		/tmp/bench.db
COMMAND=	./db_bench --db=$(DBDIR)
OS!=		uname
SOEXT!=		if [ "$(OS)" = "Darwin" ]; \
		then echo .dylib; \
		else echo .so; \
		fi
CONFIGURE=	cmake -S ../../leveldb -DCMAKE_BUILD_TYPE=Release \
			-DHAVE_STD_REGEX=ON -DRUN_HAVE_STD_REGEX=1
BUILD= 		$(MAKE)
PREPARE= 	rm -rf $(DBDIR) && $(COMMAND) --threads=1 --benchmarks=fillseq
RUNCMD= 	$(COMMAND) --threads=1 --benchmarks=readrandom --duration=10
PARSE=		cat $(WORKDIR)/$*.run.log  \
		| grep benchstats \
		| cut -d: -f2 \
		| xargs -n1 echo "$*"';' \
		| tee -a $(WORKDIR)/results.csv
BUNDLE_DIR=	$(PROJECT)/build/bench/lib/

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
BLD.patch=	git apply $(ROOTDIR)/patches/01-leveldb


TARGET+=	populate
DEP.populate=	.patch.bld
DIR.populate=	build/leveldb
CFG.populate= 	$(CONFIGURE)
BLD.populate=	$(BUILD)
RUN.populate= 	$(PREPARE)

TARGET+=	results
PRO.results=	echo 'variant; time_s; count; count_1' > $(WORKDIR)/results.csv

# ------------------------------------------------------------------------------
# Variants
# ------------------------------------------------------------------------------

TARGET+=	baseline
DEP.baseline=	.populate.bld
DIR.baseline=	build/leveldb
CFG.baseline= 	$(CONFIGURE)
BLD.baseline=	$(BUILD)
RUN.baseline= 	$(RUNCMD)
PRO.baseline=	$(PARSE)

TARGET+=	tsan
DEP.tsan=	.baseline.run
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
RUN.tsano=	TSANO_LIBDIR=$(PROJECT)/build/deps/tsano \
		$(PROJECT)/deps/tsano/tsano $(RUNCMD)
PRO.tsano=	$(PARSE)

TARGET+=	core
DEP.core=	.tsan.bld
DIR.core=	$(DIR.tsan)
RUN.core=	$(PROJECT)/scripts/dice $(RUNCMD)
PRO.core=	$(PARSE)

TARGET+=	intercept
DEP.intercept=	.tsan.bld
DIR.intercept=	$(DIR.tsan)
RUN.intercept=	$(PROJECT)/scripts/dice -pthread -tsan $(RUNCMD)
PRO.intercept=	$(PARSE)

TARGET+=	self
DEP.self=	.tsan.bld
DIR.self=	$(DIR.tsan)
RUN.self=	$(PROJECT)/scripts/dice -self -pthread -tsan $(RUNCMD)
PRO.self=	$(PARSE)

TARGET+=	bundle
DEP.bundle=	.tsan.bld
DIR.bundle=	$(DIR.tsan)
RUN.bundle=	$(PROJECT)/scripts/dice \
		-with $(BUNDLE_DIR)/libdice-bundle$(SOEXT) $(RUNCMD)
PRO.bundle=	$(PARSE)

TARGET+=	box
DEP.box=	.tsan.bld
DIR.box=	$(DIR.tsan)
RUN.box=	$(PROJECT)/scripts/dice \
		-with $(BUNDLE_DIR)/libdice-bundle-box$(SOEXT) $(RUNCMD)
PRO.box=	$(PARSE)

# ------------------------------------------------------------------------------
BENCHMK= ../bench.mk
include $(BENCHMK)
