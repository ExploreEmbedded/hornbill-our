deps_config := \
	/home/sandeep/esp32_bm/esp-idf/components/bt/Kconfig \
	/home/sandeep/esp32_bm/esp-idf/components/esp32/Kconfig \
	/home/sandeep/esp32_bm/esp-idf/components/ethernet/Kconfig \
	/home/sandeep/esp32_bm/esp-idf/components/freertos/Kconfig \
	/home/sandeep/esp32_bm/esp-idf/components/log/Kconfig \
	/home/sandeep/esp32_bm/esp-idf/components/lwip/Kconfig \
	/home/sandeep/esp32_bm/esp-idf/components/mbedtls/Kconfig \
	/home/sandeep/esp32_bm/esp-idf/components/openssl/Kconfig \
	/home/sandeep/esp32_bm/esp-idf/components/spi_flash/Kconfig \
	/home/sandeep/apps/hornbill-our/hornbill-our-firmware/main/Kconfig \
	/home/sandeep/esp32_bm/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/sandeep/esp32_bm/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/sandeep/esp32_bm/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/sandeep/esp32_bm/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
