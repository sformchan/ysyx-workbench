KCONFIG_CONFIG := .config

menuconfig:
	tools/kconfig/build/mconf Kconfig
	tools/kconfig/build/conf --syncconfig Kconfig

autoconf.h: .config
	@mkdir -p include/config
	@grep -E '^CONFIG_' .config | \
	sed -E 's/^(CONFIG_[^=]+)=y/#define \1 1/' | \
	sed -E 's/^(CONFIG_[^=]+)=n|# CONFIG_[^ ]+ is not set/\/\* \1 is not set \*\//' | \
	sed -E 's/^(CONFIG_[^=]+)=(.+)/#define \1 \2/' \
	> include/config/autoconf.h

defconfig:
	tools/kconfig/build/conf --defconfig=defconfig Kconfig

clean:
	rm -f .config include/config/autoconf.h

.PHONY: menuconfig defconfig clean

