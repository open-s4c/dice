# Copyright (C) 2025 Huawei Technologies Co., Ltd.                             #
# SPDX-License-Identifier: 0BSD                                                #

set(OPENS4C_URL "https://github.com/open-s4c")
set(TMPLR_VERSION "1.4.3")
set(TMPLR_URL "${OPENS4C_URL}/tmplr/archive/refs/tags/v${TMPLR_VERSION}.tar.gz")
set(TMPLR_SHA256
    "8b98201053a199e6c7c05ed55973716560bdd76ff10f5e87370a933487dbd710")

if(DEFINED TMPLR_PROGRAM AND EXISTS "${TMPLR_PROGRAM}")
  # Cached values are valid; nothing further needed.
  return()
endif()

execute_process(
  COMMAND
    "${CMAKE_SOURCE_DIR}/cmake/ensure-cmd.sh" -q #
    --workdir "${CMAKE_BINARY_DIR}" #
    --url "${TMPLR_URL}" #
    --sha256 "${TMPLR_SHA256}" #
    "tmplr" "${TMPLR_VERSION}"
  OUTPUT_VARIABLE TMPLR_PROGRAM
  OUTPUT_STRIP_TRAILING_WHITESPACE
  RESULT_VARIABLE ENSURE_TMPLR_RC)
if(NOT ENSURE_TMPLR_RC EQUAL 0)
  message(FATAL_ERROR "Failed to ensure tmplr ${TMPLR_VERSION} is available")
endif()
message(STATUS "tmplr v${TMPLR_VERSION}: ${TMPLR_PROGRAM}")

# increase maximum value length
set(TMPLR_PROGRAM ${TMPLR_PROGRAM} -l 1024)
