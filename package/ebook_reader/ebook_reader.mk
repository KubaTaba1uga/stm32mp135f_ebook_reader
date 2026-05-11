################################################################################
#
# Ebook reader app
#
################################################################################

EBOOK_READER_VERSION = v0.0.1
EBOOK_READER_SITE = $(BR2_EXTERNAL_EBK_READER_PATH)/package/ebook_reader/subprojects/IT8951-ePaper
EBOOK_READER_SITE_METHOD = local

$(eval $(meson-package))

