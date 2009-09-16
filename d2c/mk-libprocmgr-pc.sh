#!/bin/sh
# 
# A simple script to generate a libprocmgr.pc with the proper paths..  This
# is only a temporary solution until libprocmgr is autotools-ified
# 
# Usage:
# 
#    mk-libprocmgr-pc.sh /path/to/staging/usr /path/to/userspace-syslink
# 
# the parameter should specify the ${prefix} dir, ie. where the staging area
# is..  it should include the "/usr" if everything will be installed in
# 

PREFIX=$1
ROOT=`readlink -f $2`


PKGCONF=$PREFIX/lib/pkgconfig
PC=$PKGCONF/libprocmgr.pc

# warn if this path is not included in pkgconfig
test "$PKG_CONFIG_PATH" = "$PREFIX/lib/pkgconfig" || echo "Creating file $PC outside of PKG_CONFIG_PATH"

# syslink include files stay in package currently
# drivers are moved to target/lib

mkdir -p $PREFIX/lib/pkgconfig
cat > $PREFIX/lib/pkgconfig/libprocmgr.pc <<EOF
prefix=$PREFIX
exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib
includedir=\${prefix}/include

Name: libprocmgr
Description: ProcMgr Library
Version: 2.0.0
Requires: 
Libs: -L${ROOT}/target/lib -lprocmgr -lutils
Cflags: -I${ROOT}/api/include
EOF
