ARCH?=x86_64

export SYSTEM_DIR=$(SOURCE_DIR)/system/$(ARCH)

include $(SYSTEM_DIR)/arch.mk

export LDFLAGS=$(foreach d, $(LDSCRIPTS), -T$d)
