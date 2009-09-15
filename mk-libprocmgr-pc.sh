#!/bin/sh
# 
# A simple script to generate a libprocmgr.pc with the proper paths..  This
# is only a temporary solution until libprocmgr is autotools-ified
# 
# Usage:
# 
#    mk-libprocmgr-pc.sh /path/to/staging/usr
# 
# the parameter should specify the ${prefix} dir, ie. where the staging area
# is..  it should include the "/usr" if everything will be installed in
# 

PREFIX=$1

cat > $PREFIX/lib/pkgconfig/libprocmgr.pc <<EOF
prefix=$PREFIX
exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib
includedir=\${prefix}/include

Name: libprocmgr
Description: ProcMgr Library
Version: 2.0.0
Requires: 
Libs: -L\${libdir} -lprocmgr
Cflags: -I\${includedir}/syslink
EOF
