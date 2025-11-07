.POSIX:

TAG1=
TAG2=

all: check build run process
	@echo "== regression: '$(TAG1)' vs '$(TAG2)' =="
	@cat work/results.csv

check:
	@if [ -z "$(TAG1)" ] || [ -z "$(TAG2)" ]; then \
		echo "error: git tags not given"; \
		echo "Usage: make TAG1=<some tag> TAG2=<another tag>"; \
		exit 1; \
	fi

# ------------------------------------------------------------------------------
# Generic configuration
# ------------------------------------------------------------------------------
DICE!=		readlink -f $(ROOTDIR)/../..

TARGET+=	summary
PRO.summary=	echo tag, variant, nthreads, count, time_s \
		> $(WORKDIR)/results.csv

WITH.gcc=	-DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
WITH.clang=	-DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
CONFIGURE=	cmake -Bbuild -DCMAKE_BUILD_TYPE=Release
BUILD=		cmake --build build -j \
		-t dice  -t micro-dice \
		-t micro -t micro2 -t micro3
RUNCMD=		$(MAKE) -C bench/micro \
			ROOTDIR=$(WORKDIR)/$(DIR$*) \
			WORKDIR=$(WORKDIR)/$(DIR$*)/bench/micro/work \
			MAKEFILE=$(WORKDIR)/$(DIR$*)/bench/micro/Makefile \
			DICE=$(WORKDIR)/$(DIR$*) \
			info run process VERBOSE=1

PROCESS=	cat $(WORKDIR)/$(DIR$*)/bench/micro/work/results.csv \
		| grep -v variant \
		| xargs -n1 printf '%s,%s\n' $* \
		>> $(WORKDIR)/results.csv

# ------------------------------------------------------------------------------
# variants
# ------------------------------------------------------------------------------

TARGET+=	tag1-gcc
DIR.tag1-gcc=	$(TAG1)-gcc
CFG.tag1-gcc=	git clone --branch $(TAG1) $(DICE) . \
		&& $(CONFIGURE) $(WITH.gcc)
BLD.tag1-gcc=	$(BUILD)
RUN.tag1-gcc=	$(RUNCMD)
PRO.tag1-gcc=	$(PROCESS)

TARGET+=	tag2-gcc
DIR.tag2-gcc=	$(TAG2)-gcc
CFG.tag2-gcc=	git clone --branch $(TAG2) $(DICE) . \
		&& $(CONFIGURE) $(WITH.gcc)
BLD.tag2-gcc=	$(BUILD)
RUN.tag2-gcc=	$(RUNCMD)
PRO.tag2-gcc=	$(PROCESS)

TARGET+=	tag1-clang
DIR.tag1-clang=	$(TAG1)-clang
CFG.tag1-clang=	git clone --branch $(TAG1) $(DICE) . \
		&& $(CONFIGURE) $(WITH.clang)
BLD.tag1-clang=	$(BUILD)
RUN.tag1-clang=	$(RUNCMD)
PRO.tag1-clang=	$(PROCESS)

TARGET+=	tag2-clang
DIR.tag2-clang=	$(TAG2)-clang
CFG.tag2-clang=	git clone --branch $(TAG2) $(DICE) . \
		&& $(CONFIGURE) $(WITH.clang)
BLD.tag2-clang=	$(BUILD)
RUN.tag2-clang=	$(RUNCMD)
PRO.tag2-clang=	$(PROCESS)

# ------------------------------------------------------------------------------
# include bench.mk
# ------------------------------------------------------------------------------
BENCHMK= ../bench.mk
include $(BENCHMK)
