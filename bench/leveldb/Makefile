# ------------------------------------------------------------------------------
# Commands and variables for all LevelDB benchmark variants
# ------------------------------------------------------------------------------
DBDIR=		/tmp/bench.db
COMMAND=	./db_bench --db=$(DBDIR)
BUNDLE_DIR=	$(PROJECT)/build/bench/lib/
RUNCMD= 	$(COMMAND) --threads=1 --benchmarks=readrandom --num=5000000
REPEAT=		5
SOEXT!=		if [ "$(uname)" = "Darwin" ]; \
		then echo .dylib; \
		else echo .so; fi

# ------------------------------------------------------------------------------
# clone level, apply patches, build, populate db
# ------------------------------------------------------------------------------

URL=		https://github.com/google/leveldb.git
TAG=		1.23

TARGET+=	clone
DIR.clone=	leveldb
CFG.clone=	git clone --branch $(TAG) --depth 1 $(URL) . && \
		git submodule update --init --recursive

TARGET+=	vanilla
DEP.vanilla=	.clone.cfg
DIR.vanilla=	vanilla
CFG.vanilla=	cmake -S ../$(DIR.clone) \
			-DCMAKE_BUILD_TYPE=Release \
			-DCMAKE_CXX_COMPILER=$(CXX) \
			-DHAVE_STD_REGEX=ON \
			-DRUN_HAVE_STD_REGEX=1
BLD.vanilla=	$(MAKE) -j 8

TARGET+=	sanitized
DEP.sanitized=	.clone.cfg
DIR.sanitized=	sanitized
CFG.sanitized=	cmake -S ../$(DIR.clone) \
			-DCMAKE_BUILD_TYPE=Release \
			-DCMAKE_CXX_COMPILER=$(CXX) \
			-DCMAKE_CXX_FLAGS=$(CXXFLAGS_EXTRA) \
			-DCMAKE_EXE_LINKER_FLAGS=$(LDFLAGS) \
			-DHAVE_STD_REGEX=ON \
			-DRUN_HAVE_STD_REGEX=1
BLD.sanitized=	$(MAKE) -j 8

TARGET+=	populate
DEP.populate=	.vanilla.bld
DIR.populate=	$(DIR.vanilla)
RUN.populate= 	rm -rf $(DBDIR) && $(COMMAND) --threads=1 --benchmarks=fillseq


DEPBUILD=	.populate.run

# ------------------------------------------------------------------------------
# Variants
# ------------------------------------------------------------------------------
TARGET+=	header

TARGET+=	baseline
DEP.baseline=	$(DEPBUILD)
DIR.baseline=	$(DIR.vanilla)
RUN.baseline= 	$(TIMED) -r $(REPEAT) -- env $(RUNCMD)
PRO.baseline=	$(PARSE)

TARGET+=	tsan
DEP.tsan=	$(DEPBUILD)
DIR.tsan=	$(DIR.sanitized)
RUN.tsan=	$(TIMED) -r $(REPEAT) -- \
			env TSAN_OPTIONS=report_bugs=0 $(RUNCMD)
PRO.tsan=	$(PARSE)
# Linux: if TSAN fails with "FATAL: ThreadSanitizer: unexpected memory mapping"
# Try running this
#	sudo sysctl vm.mmap_rnd_bits=30

TARGET+=	tsano
DEP.tsano=	$(DEPBUILD)
DIR.tsano=	$(DIR.sanitized)
RUN.tsano=	$(TIMED) -r $(REPEAT) -- $(TSANOCMD) $(RUNCMD)
PRO.tsano=	$(PARSE)

TARGET+=	core
DEP.core=	$(DEPBUILD)
DIR.core=	$(DIR.sanitized)
RUN.core=	$(TIMED) -r $(REPEAT) -- $(DICECMD) $(RUNCMD)
PRO.core=	$(PARSE)

TARGET+=	intercept
DEP.intercept=	$(DEPBUILD)
DIR.intercept=	$(DIR.sanitized)
RUN.intercept=	$(TIMED) -r $(REPEAT) -- $(DICECMD) \
			-pthread -malloc -cxa -tsan \
			$(RUNCMD)
PRO.intercept=	$(PARSE)

TARGET+=	self
DEP.self=	$(DEPBUILD)
DIR.self=	$(DIR.sanitized)
RUN.self=	$(TIMED) -r $(REPEAT) -- $(DICECMD) \
			-pthread -malloc -cxa -tsan -self \
			$(RUNCMD)
PRO.self=	$(PARSE)

TARGET+=	bundle
DEP.bundle=	$(DEPBUILD)
DIR.bundle=	$(DIR.sanitized)
RUN.bundle=	$(TIMED) -r $(REPEAT) -- $(DICECMD) \
			-with $(BUNDLE_DIR)/libdice-bundle$(SOEXT) \
			$(RUNCMD)
PRO.bundle=	$(PARSE)

TARGET+=	box
DEP.box=	$(DEPBUILD)
DIR.box=	$(DIR.sanitized)
RUN.box=	$(TIMED) -r $(REPEAT) -- $(DICECMD) \
			-with $(BUNDLE_DIR)/libdice-bundle-box$(SOEXT) \
			$(RUNCMD)
PRO.box=	$(PARSE)

# ------------------------------------------------------------------------------
BENCHMK= ../bench.mk
include $(BENCHMK)
include $(ROOTDIR)/../common.mk
