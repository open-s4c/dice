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
REPEAT=3

# when checking whether one tag is indeed faster then the others, we subtract
# a "safety coefficient" which we consider to be only noise in the measurement
# by default 300ms.
VARIABILITY=0.3


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
	cmake --build "$DIR/build" -j \
		-t dice  -t micro-dice \
		-t micro -t micro2 -t micro3
}

run() {
	DIR=$1
	for i in $(seq 1 $REPEAT); do
		$MAKE -C "$DIR/bench/micro" run process VERBOSE=1 FORCE=1
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
	| grep -E "(micro3|time_s)" \
	| cut -d, -f1,2,3,6 > $RESULTS.tmp
	mv $RESULTS.tmp $RESULTS
	echo
}

check() {
	TAGS="$1"
	CC="$2"
	BETTER="$3"
	echo "== Check $BETTER is better or equal =="
	count=0
	s=$VARIABILITY
	for cc in $CC; do
		X=$(cat results.csv \
			| grep $BETTER \
			| grep $cc \
			| head -n 1 \
			| cut -d, -f4)
		for tag in $TAGS; do
			if [ "$tag" = "$BETTER" ]; then
				continue
			fi
			Y=$(cat results.csv \
				| grep $tag \
				| grep $cc \
				| head -n 1 \
				| cut -d, -f4)

			echo -n "   [$cc] $BETTER($X) x $tag($Y) : "
			Z=$(echo "($X-$s) <= $Y" | bc -l)
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
fi
if in_list sum $actions; then
	summary "$TAGS" "$CC"
fi
if in_list show $actions; then
	show_sum
fi
if in_list check $actions; then
	check "$TAGS" "$CC" "$BETTER"
fi
