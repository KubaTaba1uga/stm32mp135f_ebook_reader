################################################################################
#
# IT8951 ePaper driver
#
################################################################################

LVGL_VERSION = v9.4.0
LVGL_SITE = https://github.com/lvgl/lvgl.git
LVGL_SITE_METHOD = git
LVGL_DEPENDENCIES = host-pkgconf
LVGL_INSTALL_STAGING = YES
LVGL_CONF_OPTS = -DCONFIG_LV_BUILD_DEMOS=OFF -DCONFIG_LV_BUILD_EXAMPLES=OFF -DCMAKE_C_FLAGS="-Wall -Wextra"

$(eval $(cmake-package))
