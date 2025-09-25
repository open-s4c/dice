# ------------------------------------------------------------------------------
# Commands and variables for strachapixel benchmark variants
# ------------------------------------------------------------------------------

RASTERIZATION=	rasterization-practical-implementation
RUNCMD= 	./raster3d

# ------------------------------------------------------------------------------
# clone level, apply patches, build rendertri
# ------------------------------------------------------------------------------

URL=		https://github.com/scratchapixel/scratchapixel-code.git
TAG=		47dd2e8

TARGET+=	clone
DIR.clone=	source
CFG.clone=	git clone $(URL) . && \
		git checkout $(TAG) && \
		git apply $(ROOTDIR)/patches/benchmarking.patch


TARGET+=	vanilla
DEP.vanilla=	.clone.bld
DIR.vanilla=	vanilla
CFG.vanilla=	cmake ../$(DIR.clone)/$(RASTERIZATION) \
			-DCMAKE_BUILD_TYPE=Release \
			-DCMAKE_CXX_COMPILER=$(CXX)
BLD.vanilla=	$(MAKE) -j 8

TARGET+=	sanitized
DEP.sanitized=	.clone.bld
DIR.sanitized=	sanitized
CFG.sanitized=	cmake ../$(DIR.clone)/$(RASTERIZATION) \
			-DCMAKE_BUILD_TYPE=Release \
			-DCMAKE_CXX_COMPILER=$(CXX) \
			-DCMAKE_CXX_FLAGS=-fsanitize=thread \
			-DCMAKE_EXE_LINKER_FLAGS=$(LDFLAGS)
BLD.sanitized=	$(MAKE) -j 8

# ------------------------------------------------------------------------------
# Variants
# ------------------------------------------------------------------------------
REPEAT=		5
DEPBUILD=	.sanitized.bld
TARGET+=	header

TARGET+=	baseline
DEP.baseline=	.vanilla.bld
DIR.baseline=	$(DIR.vanilla)
RUN.baseline=	$(TIMED) -r $(REPEAT) -- $(RUNCMD)
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
