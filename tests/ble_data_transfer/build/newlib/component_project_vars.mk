# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += $(IDF_PATH)/components/newlib/platform_include $(IDF_PATH)/components/newlib/include
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/newlib -lnewlib $(IDF_PATH)/components/newlib/lib/libc.a $(IDF_PATH)/components/newlib/lib/libm.a
COMPONENT_LINKER_DEPS += $(IDF_PATH)/components/newlib/lib/libc.a $(IDF_PATH)/components/newlib/lib/libm.a
COMPONENT_SUBMODULES += 
COMPONENT_LIBRARIES += newlib
newlib-build: 
