enabled() {
	if [ -z "$1" ] || [ "$1" != "yes" ]; then
		return 1
	else
		return 0
	fi
}

in_list() {
	needle=$1
	shift
	for item in $@; do
		if [ "$item" = "$needle" ]; then
			return 0
		fi
	done
	return 1
}

