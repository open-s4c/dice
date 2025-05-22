# Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
# SPDX-License-Identifier: MIT

ROOT!=		pwd
WORK=		$(ROOT)/work
BINGO=		$(ROOT)/../..

# ------------------------------------------------------------------------------
# build dice
# ------------------------------------------------------------------------------

TARGET+=	lib
CFG.lib=	cmake $(BINGO) \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_INSTALL_PREFIX=$(WORK)
BLD.lib=	$(MAKE) install

# ------------------------------------------------------------------------------
# variants
# ------------------------------------------------------------------------------

LIBPATH=	$(WORK)/lib:$(WORK)/lib/dice

TARGET+=	baseline
DEP.baseline=	.lib.bld
BLD.baseline=	$(CC) -o micro $(ROOT)/micro.c \
			-L$(WORK)/lib -I$(WORK)/include -ldice -lpthread
RUN.baseline=   env LD_LIBRARY_PATH=$(LIBPATH) ./micro

# ------------------------------------------------------------------------------
include ../bench.mk
