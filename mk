
build=$1
if [ "$build" == "" ]; then
    build="release"
fi
export build

pfolders=()
pfolders+=("tslib")
pfolders+=("tslib_test")
pfolders+=("tsspilib")
pfolders+=("tssocketlib")
pfolders+=("tssocketlib_test")
pfolders+=("spidl")
pfolders+=("spiws")

mkroot=`pwd`

for pf in ${pfolders[@]}; do
	cd "$mkroot/$pf"
	./mk $build
	cd $mkroot
done
