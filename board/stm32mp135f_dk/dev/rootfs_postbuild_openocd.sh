#!/bin/sh

set -xeu

sh $BR2_EXTERNAL_EBK_READER_PATH/board/stm32mp135f_dk/dev/rootfs_postbuild.sh

$HOST_DIR/bin/stm32wrapper4dbg -s ${BINARIES_DIR}/tf-a-stm32mp135f-dk-ebook-reader.stm32 -d ${BINARIES_DIR}/tf-a-stm32mp135f-dk-ebook-reader-debug.stm32 -b -f

cp ${BINARIES_DIR}/tf-a-stm32mp135f-dk-ebook-reader-debug.stm32 ${BINARIES_DIR}/tf-a-stm32mp135f-dk-ebook-reader.stm32

echo ${BINARIES_DIR}
