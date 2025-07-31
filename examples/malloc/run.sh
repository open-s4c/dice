#!/bin/sh

# Dice and other plugins must be loaded via LD_PRELOAD (on Unixes and Linux) or
# DYLD_INSERT_LIBRARIES (on macOS).

# We assume Dice has been built in the following directory.
BUILD=../../build/src

# libdice contains no real functions besides the pubsub and a mempool.
MODS=${BUILD}/dice/libdice.so

# With the malloc module, we interpose malloc calls and publish events for
# malloc-related calls into INTERCEPT chains.
MODS=${MODS}:${BUILD}/mod/mod-malloc.so

# With the self module, we allow threads to have TLS state
# the Self module subscribe INTERCEPT chains and redirect them into CAPTURE
# chains.
MODS=${MODS}:${BUILD}/mod/mod-self.so

# With the pthread_create module, we interpose thread creation calls and allow
# threads to register with the self module
MODS=${MODS}:${BUILD}/mod/mod-pthread_create.so

# Finally, we add our logger, which subscribes for CAPTURE_BEFORE and
# CAPTURE_AFTER chains of EVENT_MALLOC.
MODS=${MODS}:./libmalloc_logger.so

exec env LD_PRELOAD=${MODS} "$@"

# Note that the order of the modules in LD_PRELOAD is not important as long as
# libdice.so is the left-most library in the string.
