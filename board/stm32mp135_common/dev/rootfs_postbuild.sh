#!/bin/bash

set -xeu

mkdir -p ${BINARIES_DIR}/boot

cp ${BINARIES_DIR}/zImage ${BINARIES_DIR}/boot/

find ${BINARIES_DIR} -name '*-ebook-reader.dtb' | while read -r path
do
    cp $path ${BINARIES_DIR}/boot/ebook-reader.dtb
done


# find ${BINARIES_DIR} -name '*-ebook-reader.dtb' -exec cp {} ${BINARIES_DIR}/boot/ebook-reader.dtb

rm -f ${BINARIES_DIR}/boot.ext2
mke2fs -d ${BINARIES_DIR}/boot ${BINARIES_DIR}/boot.ext2 16M
