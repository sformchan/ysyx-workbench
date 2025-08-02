KCONFIG_CONFIG := .config

menuconfig:
	scripts/kconfig/build/mconf Kconfig

autoconf.h: .config
	@mkdir -p include/config
	@cat .config | sed 's/^CONFIG_/#define CONFIG_/g' > include/config/autoconf.h

defconfig:
	scripts/kconfig/build/conf --defconfig=defconfig Kconfig

clean:
	rm -f .config include/config/autoconf.h

.PHONY: menuconfig defconfig clean
