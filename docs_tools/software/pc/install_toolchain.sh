#!/bin/bash

DEST_DIR=/opt/fullhan
RTT_CT_FILE="$(dirname $0)/arm-fullhanv2-eabi-b3.tgz"
RTT_CT_DIR=$DEST_DIR/toolchain
UBOOT_CT_FILE="$(dirname $0)/arm-fullhanv3-linux-uclibcgnueabi-b6.2.tgz"
UBOOT_CT_DIR=$DEST_DIR/toolchain

# ANSI COLORS
COLOR_NORMAL="[0;39m"
COLOR_RED="[1;31m"
COLOR_GREEN="[1;32m"
COLOR_YELLOW="[1;33m"
COLOR_CYAN="[1;36m"

ERR()
{
    echo "${COLOR_RED}ERR: ${1}${COLOR_NORMAL}" >&2
}

ECHO()
{
    echo "${COLOR_CYAN}INFO: ${1}${COLOR_NORMAL}" >&2
}

ROOT_EXEC_PREFIX=
root_exec() {
	ECHO "EXEC ==> $*"
	if [ -z "$ROOT_EXEC_PREFIX" ]; then
		$*
	else
		$ROOT_EXEC_PREFIX "$*"
	fi
	ret=$?
	if [ $ret -ne 0 ]; then
		ECHO "Require root privilege, choose number: " >&2
		select choice in "su" "sudo" "exit" "cancel"
		do
			case $choice in
			su)
				ROOT_EXEC_PREFIX="su -c"
				su -c "$*"
				[ $? -eq 0 ] && break
				;;
			sudo)
				ROOT_EXEC_PREFIX="sudo bash -c"
				sudo bash -c "$*"
				[ $? -eq 0 ] && break
				;;
			cancel)
				exit 0
				;;
			exit)
				exit 1
				;;
			*)
				ECHO "choose one, please retry." >&2
				continue
				;;
			esac

			ERR "Install failed. Try again" >&2
		done
	fi
}

set +e

ECHO "Install rtt toolchian" >&2
root_exec mkdir -p $RTT_CT_DIR
root_exec tar xf $RTT_CT_FILE -C $RTT_CT_DIR

ECHO "Install uboot toolchian" >&2
root_exec mkdir -p $UBOOT_CT_DIR
root_exec tar xf $UBOOT_CT_FILE -C $UBOOT_CT_DIR

if [ -z "$(grep "$RTT_CT_DIR/arm-fullhanv2-eabi-b3/bin" < /etc/profile)" ]; then
	content="\\\\n# $(date)\\\\nexport PATH=$RTT_CT_DIR/arm-fullhanv2-eabi-b3/bin:'$'PATH"
	root_exec "echo -e $content >> /etc/profile"
fi

if [ -z "$(grep "$UBOOT_CT_DIR/arm-fullhanv3-linux-uclibcgnueabi-b6/bin" < /etc/profile)" ]; then
	content="\\\\n# $(date)\\\\nexport PATH=$UBOOT_CT_DIR/arm-fullhanv3-linux-uclibcgnueabi-b6/bin:'$'PATH"
	root_exec "echo -e $content >> /etc/profile"
fi