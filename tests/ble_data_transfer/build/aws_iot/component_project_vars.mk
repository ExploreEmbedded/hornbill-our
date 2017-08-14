# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += $(IDF_PATH)/components/aws_iot/include $(IDF_PATH)/components/aws_iot/aws-iot-device-sdk-embedded-C/include
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/aws_iot -laws_iot
COMPONENT_LINKER_DEPS += 
COMPONENT_SUBMODULES += $(IDF_PATH)/components/aws_iot/aws-iot-device-sdk-embedded-C
COMPONENT_LIBRARIES += aws_iot
aws_iot-build: 
