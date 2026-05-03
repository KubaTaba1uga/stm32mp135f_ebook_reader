#!/bin/sh

set -xeu

sh $BR2_EXTERNAL_EBK_READER_PATH/board/stm32mp135_common/dev/rootfs_postbuild.sh

find ${BINARIES_DIR} -name 'tf-a-*.stm32' | while read -r path
do
    file_name=basename($path)
    dst_path=${BINARIES_DIR}/${file_name}-debug.stm32
    $HOST_DIR/bin/stm32wrapper4dbg -s $path -d $dst_path -b -f
    cp $dst_path $path
done


# # $HOST_DIR/bin/stm32wrapper4dbg -s ${BINARIES_DIR}/tf-a-stm32mp135f-dk-ebook-reader.stm32 -d ${BINARIES_DIR}/tf-a-stm32mp135f-dk-ebook-reader-debug.stm32 -b -f

# cp ${BINARIES_DIR}/tf-a-stm32mp135f-dk-ebook-reader-debug.stm32 ${BINARIES_DIR}/tf-a-stm32mp135f-dk-ebook-reader.stm32

echo ${BINARIES_DIR}
