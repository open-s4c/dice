#!/bin/sh
set -eu

variant=$1
timefile=$2
runlog=$3

if grep -Eq '^[^ ]+ [0-9]' "$timefile" 2>/dev/null; then
    cat "$timefile"
    exit 0
fi

run=0
real=""
user=""
sys=""

while read kind value rest; do
    case "$kind" in
        real)
            real=$value
            ;;
        user)
            user=$value
            ;;
        sys)
            sys=$value
            run=$((run + 1))
            printf '%s %d %s %s %s\n' "$variant" "$run" "$real" "$user" "$sys"
            ;;
    esac
done < "$runlog"
