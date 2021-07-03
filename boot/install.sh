 
check () {
	if [ ! -f "$1" ]; then
		echo ""                                                   1>&2
		echo " *** Missing file: $1"                              1>&2
		echo ' *** You need to run "make" before "make install".' 1>&2
		echo ""                                                   1>&2
		exit 1
	fi
}
