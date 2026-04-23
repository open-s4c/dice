# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: 0BSD

DICE!=		readlink -f $(ROOTDIR)/../..
BINDIR=		$(DICE)/build/bench/micro

OUTPARSE= 	's/.*=\([0-9.]*\) .*=\([0-9.]*\) .*=\([0-9.]*\)s/\1,\2,\3/g'
FORMAT=		grep elapsed | sed $(OUTPARSE) \
		| xargs printf '%s,%s\n' $*

LIBPATH=	$(BINDIR)/../lib:$(BINDIR)
OS!=		uname
PRFX!=		if [ "$(OS)" = "Darwin" ]; \
		then echo DYLD; \
		else echo LD; fi

TARGET+=	summary
SUM.summary=	echo variant, nthreads, count, time_s > $(WORKDIR)/results.csv

# ------------------------------------------------------------------------------
# variants
# ------------------------------------------------------------------------------

TARGET+=	micro
DEP.micro=	.summary.run
RUN.micro=	env $(PRFX)_LIBRARY_PATH=$(LIBPATH) $(BINDIR)/micro
SUM.micro=	cat $(WORKDIR)/$*.run.log | $(FORMAT) >> $(WORKDIR)/results.csv

TARGET+=	micro2
DEP.micro2=	.summary.run
RUN.micro2=	env $(PRFX)_LIBRARY_PATH=$(LIBPATH) $(BINDIR)/micro2
SUM.micro2=	cat $(WORKDIR)/$*.run.log | $(FORMAT) >> $(WORKDIR)/results.csv

TARGET+=	micro3
DEP.micro3=	.summary.run
RUN.micro3=	env $(PRFX)_LIBRARY_PATH=$(LIBPATH) $(BINDIR)/micro3
SUM.micro3=	cat $(WORKDIR)/$*.run.log | $(FORMAT) >> $(WORKDIR)/results.csv

TARGET+=	micro4
DEP.micro4=	.summary.run
RUN.micro4=	env $(PRFX)_LIBRARY_PATH=$(LIBPATH) $(BINDIR)/micro4
SUM.micro4=	cat $(WORKDIR)/$*.run.log | $(FORMAT) >> $(WORKDIR)/results.csv

TARGET+=	micro5
DEP.micro5=	.summary.run
RUN.micro5=	env $(PRFX)_LIBRARY_PATH=$(LIBPATH) $(BINDIR)/micro5
SUM.micro5=	cat $(WORKDIR)/$*.run.log | $(FORMAT) >> $(WORKDIR)/results.csv

# ------------------------------------------------------------------------------
BENCHMK= ../bench.mk
include $(BENCHMK)
