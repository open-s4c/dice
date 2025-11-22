#!/bin/sh
set -e

TAGS="HEAD main"
TAGS_SET=false
CC="gcc clang"
CC_SET=false
MAKE=make
REPO=$(readlink -f $(dirname "$0")/..)
LOG=regression.log
RESULTS=results.csv
CURBUILD=$REPO/build
BENCH=micro3

# Number of repetitions of each TAG/CC/BENCHMARK combination
REPEAT=5

# When checking whether one tag is indeed faster then the others, we compare
# them like this:
#
#     TARGET / BASELINE <= coefficient
#
# The "safety coefficient" represents the measurement noise (default 5%).
VARIABILITY=0.05


if [ -z "$TAGS" ]; then
	echo "error TAGS: no tags set (make help for usage)"
	exit 1
fi
if [ -z "$CC" ]; then
	echo "error CC: compiler not set (make help for usage)"
	exit 1
fi

in_list() {
	needle=$1
	shift
	haystack="$@"
	for val in $haystack; do
		if [ "$val" = "$needle" ]; then return 0; fi
	done
	return 1
}

info() {
	echo "== Dice performance regression test =="
	echo "   REPO:  $REPO"
	echo "   TAGS:  $TAGS"
	echo "   CC:    $CC"
	echo "   LOG:   $LOG"
	echo "   CSV:   $RESULTS"
	echo "   MAKE:  $MAKE"
}

help() {
	echo "regression.sh -- Dice performance regression test"
	echo
	echo "USAGE"
	echo "  regression.sh [FLAGS] <ACTION> [<ACTION> ...]"
	echo
	echo "ACTIONS"
	echo "  info       Show variable information"
	echo "  run        Run benchmarks"
	echo "  sum        Summarize results in a .csv"
	echo "  show       Show results with miller"
	echo
	echo "FLAGS"
	echo " -t TAGS     Tags, branches or SHA1 (default \"$TAGS\")"
	echo " -c COMP     Compiler(s) to build Dice (default \"$CC\")"
	echo " -u URL      Git repository URL (default \"$REPO\")"
	echo " -r REPEAT   Number of repetitions (default \"$REPEAT\")"
	echo " -f FILE     Results file (Default \"$RESULTS\")"
	echo " -b BENCH    Micro benchmark selection (Default \"$BENCH\")"
	echo " -h          This help message"
	echo
}

clone() {
	REPO=$1
	TAG=$2
	DIR=$3
	if ! test -d "$DIR"; then
		git clone "$REPO" "$DIR"
	fi
	(cd $DIR && git checkout "$TAG")
}

config() {
	DIR=$1
	CC=$2
	cmake -S"$DIR" -B"$DIR/build" \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_C_COMPILER=$CC
}

build() {
	DIR=$1
	cmake --build "$DIR/build" -j -t dice  -t $BENCH
}

run() {
	DIR=$1
	for i in $(seq 1 $REPEAT); do
		cp -r $REPO/bench/bench.mk $REPO/bench/common.mk $DIR/bench/
		cp -r $REPO/bench/micro/Makefile $DIR/bench/micro/Makefile
		$MAKE -C "$DIR/bench/micro" \
			VERBOSE=1 FORCE=1 \
			LIBPATH=$DIR/build/bench/micro \
			BINDIR=$CURBUILD/bench/micro \
			run process
		cp "$DIR/bench/micro/work/results.csv" "$DIR/results-$i.csv"
	done
}

regression() {
	TAGS="$1"
	CC="$2"
	echo "== Run benchmarks =="
	echo > $LOG
	for tag in $TAGS; do
		for cc in $CC; do
			dir="work-$tag-$cc"
			echo -n "   $tag $cc ... "
			clone "$REPO" $tag $dir >> $LOG 2>&1
			config $dir $cc >> $LOG
			build $dir >> $LOG
			run  $dir >> $LOG
			echo "done"
		done
	done
	echo
}

summary() {
	TAGS="$1"
	CC="$2"

	echo "== Collect summaries =="
	echo "tag,cc,bench,nthreads,count,time_s" > $RESULTS
	for tag in $TAGS; do
		for cc in $CC; do
			dir="work-$tag-$cc"
			echo -n "   $tag $cc ... "
			cat "$dir"/results-*.csv \
			| grep -v variant \
			| xargs -n1 printf '%s,%s\n' "$tag,$cc" >> $RESULTS
			echo "done"
		done
	done
	cat $RESULTS \
	| grep -E "($BENCH|time_s)" \
	| cut -d, -f1,2,3,6 > $RESULTS.tmp
	mv $RESULTS.tmp $RESULTS
	echo
}

check() {
	if ! test -f $RESULTS; then
		echo "error: not results file \"$RESULTS\""
		exit 1
	fi
	TAGS="$(cat $RESULTS | tail +2 | cut -d, -f1 | sort | uniq)"
	CC="$(cat $RESULTS | tail +2 | cut -d, -f2 | sort | uniq)"
	echo "== Parameters in $RESULTS =="
	echo "   TAGS:  $TAGS"
	echo "   CC:    $CC"
	echo

	BETTER="$1"

	if ! echo $TAGS | grep $BETTER > /dev/null; then
		echo "error: $BETTER not in tags of $RESULTS"
		exit 1
	fi

	echo "== Check $BETTER is better or equal =="

	count=0
	s=$VARIABILITY
	for cc in $CC; do

		# Calculate mean of measurements
		VAL=$(cat results.csv \
			| grep $BETTER, \
			| grep $cc, \
			| cut -d, -f4)
		LX=$(echo $VAL | wc -w)
		SX=$(echo $VAL | tr ' ' '\n' | paste -sd+ - | bc -l)

		for tag in $TAGS; do
			if [ "$tag" = "$BETTER" ]; then
				continue
			fi

			# Calculate mean of measurements
			VAL=$(cat results.csv \
				| grep $tag, \
				| grep $cc, \
				| cut -d, -f4)
			LY=$(echo $VAL | wc -w)
			SY=$(echo $VAL | tr ' ' '\n' | paste -sd+ - | bc -l)

			echo -n "   [$cc] $BETTER($SX/$LX) x $tag($SY/$LY) : "
			Z=$(echo "($SX)/($SY)*($LY)/($LX) <= (1+$s)" | bc -l)
			if [ "$Z" = 0 ]; then
				echo "FAIL"
				count=$(echo "$count + 1" | bc -l)
			else
				echo "OK"
			fi
		done
	done
	if [ "$count" != 0 ]; then
		exit 1
	fi
}

show_sum() {
	echo "== Show CSV summary =="
	mlr --icsv --opprint stats1 \
		-a mean -f time_s -g tag,cc,bench \
		then sort -f cc \
		then put '$time_s_mean = fmtnum($time_s_mean, "%.2f")' \
		$RESULTS

}

while [ $# -gt 0 ]; do
	case "$1" in
	-h)
		shift
		help
		exit 0
		;;
	-t)
		shift
		if [ "$TAGS_SET" = false ]; then
			# if the first -t arg, then remove the default value
			TAGS_SET=true
			TAGS="$1"
		else
			TAGS="$TAGS $1"
		fi
		;;
	-c)
		shift
		if [ "$CC_SET" = false ]; then
			# if the first -c arg, then remove the default value
			CC_SET=true
			CC="$1"
		else
			CC="$CC $1"
		fi
		;;
	-u)
		shift
		REPO="$1"
		;;
	-r)
		shift
		REPEAT="$1"
		;;
	-b)
		shift
		BENCH="$1"
		;;
	-m)
		shift
		MAKE="$1"
		;;
	info)
		actions="$actions $1"
		;;
	run)
		actions="$actions $1"
		;;
	sum)
		actions="$actions $1"
		;;
	show)
		actions="$actions $1"
		;;
	check)
		actions="$actions $1"
		shift
		BETTER="$1"
		;;
	*)
		echo "error: unknown flag or action '$1'"
		echo
		help
		exit 1
		;;
	esac
	shift
done

if [ -z "$actions" ]; then
	echo "error: no actions given (-h for help)"
	exit 1
fi

info
echo


if in_list run $actions; then
	regression "$TAGS" "$CC"
	summary "$TAGS" "$CC"
fi
if in_list sum $actions; then
	summary "$TAGS" "$CC"
fi
if in_list show $actions; then
	show_sum
fi
if in_list check $actions; then
	check "$BETTER"
fi
