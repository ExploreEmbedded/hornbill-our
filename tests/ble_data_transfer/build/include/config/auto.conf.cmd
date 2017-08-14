deps_config := \
	/f/esp-idf/components/app_trace/Kconfig \
	/f/esp-idf/components/aws_iot/Kconfig \
	/f/esp-idf/components/bt/Kconfig \
	/f/esp-idf/components/esp32/Kconfig \
	/f/esp-idf/components/ethernet/Kconfig \
	/f/esp-idf/components/fatfs/Kconfig \
	/f/esp-idf/components/freertos/Kconfig \
	/f/esp-idf/components/log/Kconfig \
	/f/esp-idf/components/lwip/Kconfig \
	/f/esp-idf/components/mbedtls/Kconfig \
	/f/esp-idf/components/openssl/Kconfig \
	/f/esp-idf/components/spi_flash/Kconfig \
	/f/esp-idf/components/bootloader/Kconfig.projbuild \
	/f/esp-idf/components/esptool_py/Kconfig.projbuild \
	/f/esp-idf/components/partition_table/Kconfig.projbuild \
	/f/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
