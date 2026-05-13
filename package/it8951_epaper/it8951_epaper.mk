################################################################################
#
# IT8951 ePaper driver
#
################################################################################

IT8951_EPAPER_VERSION = 86406933d8f22af9fd3f2152b4958610c054b9a8
IT8951_EPAPER_SITE = https://github.com/waveshareteam/IT8951-ePaper.git
IT8951_EPAPER_SITE_METHOD = git
IT8951_EPAPER_DEPENDENCIES = host-pkgconf
IT8951_EPAPER_INSTALL_STAGING = YES

$(eval $(meson-package))

