#!/bin/sh
set -e

repeat=1
args=""
tmpfmt="/tmp/timedrun.XXXX"

add_args() {
	if [ -z "$args" ]; then
		args="$@"
	else
		args="$args $@"
	fi
}

tee_out() {
	if [ -z "$output" ]; then
		cat <&0 >&2
	else
		cat <&0 > $output
	fi
}

usage() {
        echo "Usage: timedrun.sh [options] [--] <command> [<args>]"
        echo
        echo "Options:"
        echo " -r REPEAT   Numer of repetitions (default 1)"
        echo " -f FORMAT   Temporary file format (default $tmpfmt)"
        echo " -n NAME     Prepend a name to each time measurement"
        echo " -o OUTPUT   Output file (default stderr)"
        echo " -h          This help message"
	echo
}

while [ $# -gt 0 ]; do
	case $1 in
        -h|--help)
	        usage
		exit 0
		;;
	-r)
		shift
		repeat=$1
		;;
	-f)
		shift
		tmpfmt=$1
		;;
	-n)
		shift
		name=$1
		;;
	-o)
		shift
		output=$1
		;;
	-- )
		shift
		args=$@
		break
		;;
	*)
		add_args $1
		;;
	esac
	shift
done

if [ -z "$args" ]; then
	usage
	exit 1
fi

tmpall=$(mktemp $tmpfmt)
tmprun=$(mktemp $tmpfmt)

# echo "run real user sys" > $tmpall
# echo "name run real user sys" > $tmpall
for i in $(seq 1 $repeat); do
	time -p sh -c "$args 2>&1" 2> $tmprun
	# ensure the output of this next command is real\t..user\t..\sys\t..
	cat $tmprun \
		| tail -n 3 \
		| tr '\t' ' ' \
		| tr -s ' ' \
		| cut -d' ' -f2 \
		| tr '\n' ' ' \
		| sed -e 's/^/'$i' /' \
		>> $tmpall
	echo >> $tmpall
done

if [ -z "$name" ]; then
	cat $tmpall
else
	cat $tmpall | sed -e '1,$s/^/'"$name"' /'
fi | tee_out

rm -f $tmprun
rm -f $tmpall
