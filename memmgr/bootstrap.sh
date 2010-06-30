#! /bin/sh

cd `dirname $0`

# on some platforms, you have "g" versions of some of these tools instead,
# ie glibtoolize instead of libtoolize..
find_tool() {
   which $1 2> /dev/null || which g$1 2> /dev/null
}

aclocal=`find_tool aclocal`
libtoolize=`find_tool libtoolize`
automake=`find_tool automake`
autoconf=`find_tool autoconf`
autoheader=`find_tool autoheader`

## removed by ndec during debianization, since utils is dupplicated for now
# don't want to make a separate package for testlib, but could not find a way
# to include this source from another directory
#rm tests/testlib.c
#ln -s `pwd`/../utils/testlib.c tests/testlib.c

mkdir -p config && $autoheader && $aclocal && $libtoolize --copy --force && $automake --copy --add-missing --foreign && $autoconf

