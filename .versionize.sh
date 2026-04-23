#!/bin/sh
# Copyright (c) 2025 Diogo Behrens
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
# OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.
set -eu

usage() {
	cat <<'EOF'
Usage:
  versionize.sh          # print resolved version and date
  versionize.sh -i FILE  # replace __VERSION__/__DATE__ in FILE in-place
  versionize.sh -r FILE  # render FILE to stdout with placeholders replaced
  versionize.sh -c       # run "git archive --format=tar HEAD | grep VERSIONIZE"

Options are mutually exclusive. Placeholders are replaced with the project
version (derived from git describe or archive substitution) and the current
date in YYYY-MM-DD format.

Setup: place this script in your repository and add this line to
.gitattributes (adjust path as needed):
  rel/path/to/versionize.sh export-subst
EOF
}
SED=sed
DATE=$(date '+%Y-%m-%d')
VERSIONIZE='$Format:%(describe:tags=true)$'

case "$VERSIONIZE" in
	*'Format:'*)
		VERSIONIZE=$(git describe --tags --always --dirty 2>/dev/null \
			|| echo '')
		;;
	*'%(describe:'*)
		VERSIONIZE='$Format:%h$'
		;;
	*'tag:'*)
		VERSIONIZE=$(printf '%s' "$VERSIONIZE" \
			| $SED -n 's/.*tag: \([^,)]*\).*/\1/p')
		;;
	*)
		true # use the version
	;;
esac

if echo $VERSIONIZE | grep -e "^v[0-9]\+\.[0-9.]\+.*$" >/dev/null 2>&1; then
	VERSIONIZE=$(echo $VERSIONIZE | $SED -n 's/v\(.*\)$/\1/p')
fi

if [ $# -eq 0 ]; then
	printf '%s %s\n' "$VERSIONIZE" "$DATE"
	exit
fi

while [ $# -gt 0 ]; do
	case "$1" in
		-i)
			shift
			FILE=$1
			$SED -I'' -e "s|__VERSION__|$VERSIONIZE|g" \
				  -e "s|__DATE__|$DATE|g" "$FILE"
			exit
			;;
		-r)
			shift
			FILE=$1
			$SED 	-e "s|__VERSION__|$VERSIONIZE|g" \
				-e "s|__DATE__|$DATE|g" "$FILE"
			exit
			;;
		-c)
			if ! git archive --format=tar HEAD | grep -a VERSIONIZE
			then
				printf 'VERSIONIZE marker not found\n' >&2
				exit 1
			fi
			exit
			;;
		*)
			usage >&2
			exit 1
			;;
	esac
done
