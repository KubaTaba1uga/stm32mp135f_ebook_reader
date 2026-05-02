#!/bin/bash

set -xeu

mkdir -p ${BINARIES_DIR}/boot

cp ${BINARIES_DIR}/zImage ${BINARIES_DIR}/boot/
cp ${BINARIES_DIR}/stm32mp135f_dk_ebook_reader.dtb ${BINARIES_DIR}/boot/

rm -f ${BINARIES_DIR}/boot.ext2
mke2fs -d ${BINARIES_DIR}/boot ${BINARIES_DIR}/boot.ext2 16M
