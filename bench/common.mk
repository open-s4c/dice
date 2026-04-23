# ROOTDIR is given by bench.mk, it refers to the directory of the benchmark
# PROJECT refers to the dice source directory
PROJECT!=	readlink -f $(ROOTDIR)/../..

# The expected BUILD_DIR of dice is $(PROJECT)/build
BUILD_DIR=	$(PROJECT)/build

# Common dice options and configuration of tsano command
TSANOCMD=	env TSANO_LIBDIR=$(BUILD_DIR)/deps/tsano \
		$(PROJECT)/deps/tsano/tsano

DICECMD=	env DICE_BUILD=$(BUILD_DIR) \
		$(PROJECT)/scripts/dice

# Compiler and linker configuration
OS!=		uname
SOEXT!=		if [ "$(OS)" = "Darwin" ]; then echo .dylib; else echo .so; fi
CC!=		if [ "$(OS)" = "Darwin" ]; then echo clang; else echo gcc; fi
CXX!=		if [ "$(OS)" = "Darwin" ]; then echo clang++; else echo g++; fi
CFLAGS_EXTRA=	-fsanitize=thread
CXXFLAGS_EXTRA=	-fsanitize=thread
# Force clang++ to link libtsan as shared library. (Yes, the flag is libsan)
LDFLAGS!=	if [ "$(CXX)" = "clang++" ]; then echo '-shared-libsan'; fi

# Use timedrun.sh in scripts directory
TIMED= 		$(PROJECT)/scripts/timedrun.sh \
			-n $* -o $(WORKDIR)/$*.time
PARSE=		{ \
			$(PROJECT)/scripts/process-timedrun.sh \
				"$*" \
				$(WORKDIR)/$*.time \
				$(WORKDIR)/$*.run.log; \
		} | tee -a $(WORKDIR)/results.csv

# Add TARGET+=header to initialize the results.csv file
SUM.header=	echo 'variant run real user sys' > $(WORKDIR)/results.csv
