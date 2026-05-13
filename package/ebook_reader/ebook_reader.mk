################################################################################
#
# Ebook reader app
#
################################################################################

EBOOK_READER_VERSION = v0.0.1
EBOOK_READER_SITE = $(BR2_EXTERNAL_EBK_READER_PATH)/package/ebook_reader
EBOOK_READER_SITE_METHOD = local
EBOOK_READER_DEPENDENCIES = it8951_epaper lvgl host-pkgconf
IT8951_EPAPER_INSTALL_STAGING = YES

$(eval $(meson-package))

