#!/bin/sh

DIR=`dirname $0`
HOST=arm-none-linux-gnueabi

# 0) Gather requirements
# =============================================================================

# path to target filesystem
echo "Enter PREFIX (currently '$PREFIX'):\c"
read VALUE
export PREFIX=${VALUE:=$PREFIX}

# path to userspace-syslink git root
echo "Enter path to userspace-syslink (currently '$USERSPACE_SYSLINK'):\c"
read VALUE
export USERSPACE_SYSLINK=${VALUE:=$USERSPACE_SYSLINK}

# path to tiler-userspace git root
export TILER_USERSPACE=`readlink -f $DIR`

# 4) Need path to toolchain

#.. first find gcc
TOOL=`which ${HOST}-gcc`
if [ ! -e $TOOL ]
then
    echo "Could not find gcc"
    exit
fi

TOOLBIN=`dirname $TOOL`
echo "Enter tool path (currently '$TOOLBIN'):\c"
read VALUE
TOOLBIN=${VALUE:=$TOOLBIN}

echo TOOLBIN           is ${TOOLBIN}
echo PREFIX            is ${PREFIX}
echo TILER_USERSPACE   is ${TILER_USERSPACE}
echo USERSPACE_SYSLINK is ${USERSPACE_SYSLINK}

export PATH=${TOOLBIN}:$PATH

#.. find libgcc
TOOL=`which ${HOST}-gcc`
TOOLDIR=`dirname $TOOL`
LIBGCC=$TOOLDIR/../lib/gcc/$HOST/*/libgcc.a
if [ ! -e $LIBGCC ]
then
    echo "Could not find libgcc.a"
    exit
fi
echo Found libgcc.a in $LIBGCC
LIBRT=$TOOLDIR/../$HOST/libc/lib/librt*.so
if [ ! -e $LIBRT ]
then
    echo "Could not find librt.so"
    exit
fi
echo Found librt.so in $LIBRT
LIBPTHREAD=$TOOLDIR/../$HOST/libc/lib/libpthread*.so
if [ ! -e $LIBPTHREAD ]
then
    echo "Could not find libpthread.so"
    exit
fi
echo Found libpthread.so in $LIBPTHREAD


# 1) First you need to build memmgr
# =============================================================================

#.. uncomment to include our unit tests as well
ENABLE_UNIT_TESTS=--enable-unit-tests

#.. uncomment to export the tilermgr.h header - this is currently needed by
#   syslink
ENABLE_TILERMGR=--enable-tilermgr

cd ${TILER_USERSPACE}/memmgr
./bootstrap.sh
./configure --prefix ${PREFIX} --host ${HOST} ${ENABLE_UNIT_TESTS} ${ENABLE_TILERMGR}
make
make install

# 2) Second you need to build syslink
# =============================================================================

#.. need libgcc.a, librt.so and libpthread.so
mkdir -p ${PREFIX}/lib
cp $LIBGCC ${PREFIX}/lib
cp `dirname $LIBRT`/librt*.so* ${PREFIX}/lib
cp `dirname $LIBPTHREAD`/libpthread*.so* ${PREFIX}/lib

#.. syslink prefix needs a target subdirectory, so we will create link to the
#   parent
if [ ! -d ${PREFIX}/target ]; then ln -s ${PREFIX} ${PREFIX}/target; fi

cd ${USERSPACE_SYSLINK}/api/src
make
make install
cd -
cd ${USERSPACE_SYSLINK}/daemons
make TILER_INC_PATH=${TILER_USERSPACE}/memmgr
make install
cd -

# 3) Third you need to build d2cmap library
# =============================================================================

cd ${TILER_USERSPACE}/d2c
export PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig
./mk-libprocmgr-pc.sh ${PREFIX} ${USERSPACE_SYSLINK}
./bootstrap.sh
./configure --prefix ${PREFIX} --host ${HOST} ${ENABLE_UNIT_TESTS}
make
make install

cd -
