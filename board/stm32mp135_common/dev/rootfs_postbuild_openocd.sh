#!/bin/sh

set -xeu

sh $BR2_EXTERNAL_EBK_READER_PATH/board/stm32mp135_common/dev/rootfs_postbuild.sh

find ${BINARIES_DIR} -name 'tf-a-*.stm32' | while read -r path
do
    if [ "$(basename "$path")" = 'tf-a-ebook-reader-debug.stm32' ]
    then
        continue
    fi
    
    file_name=$(basename $path .stm32)
    dst_path=${BINARIES_DIR}/${file_name}-debug.stm32
    rm -f $dst_path
    $HOST_DIR/bin/stm32wrapper4dbg -s $path -d $dst_path -b
done
