################################################################################
#
# stm32wrapper4dbg
#
################################################################################

HOST_STM32WRAPPER4DBG_VERSION = v5.1.2
HOST_STM32WRAPPER4DBG_SITE = $(call github,STMicroelectronics,stm32wrapper4dbg,$(HOST_STM32WRAPPER4DBG_VERSION))

define HOST_STM32WRAPPER4DBG_BUILD_CMDS
	$(MAKE) -C $(@D)
endef

define HOST_STM32WRAPPER4DBG_INSTALL_CMDS
	$(INSTALL) -D -m 0755 $(@D)/stm32wrapper4dbg \
		$(HOST_DIR)/bin/stm32wrapper4dbg
endef

$(eval $(host-generic-package))
