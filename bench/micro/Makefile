# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

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
PRO.summary=	echo variant, nthreads, count, time_s > $(WORKDIR)/results.csv

# ------------------------------------------------------------------------------
# variants
# ------------------------------------------------------------------------------

TARGET+=	micro
DEP.micro=	.summary.run
RUN.micro=	env $(PRFX)_LIBRARY_PATH=$(LIBPATH) $(BINDIR)/micro
PRO.micro=	cat $(WORKDIR)/$*.run.log | $(FORMAT) >> $(WORKDIR)/results.csv

TARGET+=	micro2
DEP.micro2=	.summary.run
RUN.micro2=	env $(PRFX)_LIBRARY_PATH=$(LIBPATH) $(BINDIR)/micro2
PRO.micro2=	cat $(WORKDIR)/$*.run.log | $(FORMAT) >> $(WORKDIR)/results.csv

TARGET+=	micro3
DEP.micro3=	.summary.run
RUN.micro3=	env $(PRFX)_LIBRARY_PATH=$(LIBPATH) $(BINDIR)/micro3
PRO.micro3=	cat $(WORKDIR)/$*.run.log | $(FORMAT) >> $(WORKDIR)/results.csv

# ------------------------------------------------------------------------------
BENCHMK= ../bench.mk
include $(BENCHMK)
