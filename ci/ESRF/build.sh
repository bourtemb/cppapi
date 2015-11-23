#!/bin/bash

UX_INSTALL_DIR="/segfs/tango/ci/Tango"
WIN_INSTALL_DIR="//unixhome/segfs/tango/ci/Tango"

#
# Is clean required ?
#

if [ $# = 2 ]
then
	if [ $2 = "clean" ]
	then
		CLEAN="clean"
	fi
fi

BRANCH_DIR=../..

#
# Set some variables according to OS
#

for i in $NODE_LABELS
do
	echo $i
	if [ $i = "ubuntu12.04" ]
	then
		echo "Ubuntu 12.04 OS"
		OS_TYPE="linux"
		OS_SPEC="linux=1 ubuntu=1"
		CPU_BUS="64bits=1"
		MAKE_PATH="make"
		INSTALL_DIR=$UX_INSTALL_DIR
	fi

	if [ $i = "Windows_VC9" ]
	then
		echo "Windows VC9"
		OS_TYPE="Windows"
		OS_SPEC="Windows"
		MAKE_PATH="devenv"
		INSTALL_DIR=$WIN_INSTALL_DIR
	fi

	if [ $i = "Windows_VC10" ]
	then
		echo "Windows VC10 (64 bits)"
		OS_TYPE="Windows"
		OS_SPEC="Windows_64_VC10"
		MAKE_PATH="MSBuild.exe"
		INSTALL_DIR=$WIN_INSTALL_DIR
	fi
	
	if [ $i = "centos5" ]
	then
		echo "centos 5 OS"
		OS_TYPE="linux"
		OS_SPEC="linux=1"
		CPU_BUS="64bits=1"
		MAKE_PATH="make"
		INSTALL_DIR=$UX_INSTALL_DIR
	fi

	if [ $i = "redhate4" ]
	then
		echo "RedHate 4 OS"
		OS_TYPE="linux"
		OS_SPEC="linux=1"
		MAKE_PATH="make"
		INSTALL_DIR=$UX_INSTALL_DIR
	fi

	if [ $i = "redhate5" ]
	then
		echo "RedHate 5 OS"
		OS_TYPE="linux"
		OS_SPEC="linux=1"
		CPU_BUS="64bits=1"
		MAKE_PATH="make"
		INSTALL_DIR=$UX_INSTALL_DIR
	fi

	if [ $i = "debian6" ]
	then
		echo "Debian 6 OS"
		OS_TYPE="linux"
		OS_SPEC="linux=1"
		CPU_BUS="64bits=1"
		MAKE_PATH="make"
		INSTALL_DIR=$UX_INSTALL_DIR
	fi

	if [ $i = "debian7_tango" ]
	then
		echo "Debian 7 OS"
		OS_TYPE="linux"
		OS_SPEC="linux=1"
		CPU_BUS="64bits=1"
		MAKE_PATH="make"
		INSTALL_DIR=$UX_INSTALL_DIR
	fi

	if [ $i = "debian8_tango" ]
	then
		echo "Debian 8 OS"
		OS_TYPE="linux"
		OS_SPEC="linux=1"
		CPU_BUS="64bits=1"
		MAKE_PATH="make"
		INSTALL_DIR=$UX_INSTALL_DIR
	fi
done

pwd

#
# Set library type
#

if [ $OS_TYPE = "Windows" ]
then
	TARGETNAME="Build"
	if [ $LIBTYPE = "shared" ]
	then
		LIBNAME=tango_dll
		OUTFILEBASE="devenv_dll"
	else
		LIBNAME=tango_static
		OUTFILEBASE="devenv_static"
	fi
else
	if [ $LIBTYPE = "shared" ]
	then
		LIBNAME=libtango.so
	else
		LIBNAME=libtango.a
	fi
fi

#
# Set debug type
#

if [ $OS_TYPE = "Windows" ]
then
	if [ $DEBUGMODE = "debug" ]
	then
		DEBUG_MODE="Debug"
		OUTFILE=$OUTFILEBASE"_debug"
	else
		DEBUG_MODE="Release"
		OUTFILE=$OUTFILEBASE"_release"
	fi
else
	if [ $DEBUGMODE = "debug" ]
	then
		DEBUG_MODE="debug=1"
	fi
fi

#
# Build library
#

if [ $OS_TYPE = "Windows" ]
then
	if [ -z $CLEAN ]
	then
		BUILD="/build"
	else
		if [ $OS_SPEC = "Windows_64_VC10" ]
		then
			TARGETNAME="Clean;"$TARGETNAME
		else
			BUILD="/rebuild"
		fi
	fi
	export PREFIX=$INSTALL_DIR
	if [ $OS_SPEC = "Windows_64_VC10" ]
	then
		MAKE_CMD="$MAKE_PATH $LIBNAME/$LIBNAME.vcxproj /p:Platform=x64 /t:$TARGETNAME /p:Configuration=$DEBUG_MODE /v:quiet /flp:LogFile=$OUTFILE;Summary;ShowCommandLine;Verbosity=minimal"
		export PATH=$PATH:/cygdrive/c/Windows/Microsoft.Net/Framework64/v4.0.30319
		cd $BRANCH_DIR/win64/tango_vc10
	else
		MAKE_CMD="$MAKE_PATH winnt_lib.sln /project $LIBNAME $BUILD $DEBUG_MODE /projectconfig $DEBUG_MODE /out $OUTFILE"
		export PATH=$PATH:/cygdrive/c/Program\ Files/Microsoft\ Visual\ Studio\ 9.0/Common7/IDE
		cd $BRANCH_DIR/win32/tango_vc9
	fi
	echo $PATH
	/bin/rm -f $OUTFILE
else
	MAKE_CMD="$MAKE_PATH prefix=$INSTALL_DIR $OS_SPEC $CPU_BUS $DEBUG_MODE $CLEAN $LIBNAME install_include install_link"

# branch dependent path:
	cd $BRANCH_DIR
fi

echo $MAKE_CMD
$MAKE_CMD
RET=$?

#
# On Windows, cat the output file and if compilation is OK, also copy include files
#

if [ $OS_TYPE = "Windows" ]
then
	/bin/cat $OUTFILE
	if [ $RET = 0 ]
	then
		cd ../..
		if [ $OS_SPEC = "Windows_64_VC10" ]
		then
			BASE=$INSTALL_DIR/win64
		else
			BASE=$INSTALL_DIR/win32
		fi
		/bin/mkdir -p $BASE/include
		/bin/mkdir -p $BASE/include/idl
		/bin/cp client/*.h $BASE/include
		/bin/cp client/*.tpp $BASE/include
		/bin/cp server/*.h $BASE/include
		/bin/cp server/*.tpp $BASE/include
		/bin/cp server/idl/*.h $BASE/include/idl
	fi
fi

if [ $RET != 0 ]
then
	exit -1
fi
