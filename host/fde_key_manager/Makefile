# Normally this makefile shouldn't be called directly and we expect the output
# path to be on a certain location to fit together with the other OP-TEE
# gits and helper scripts.

include ../../scripts/common.mk
out-dir := $(call strip-trailing-slashes-and-dots,$(O))
ifeq ($(out-dir),)
$(error invalid output directory (O=$(O)))
endif


CC		?= $(CROSS_COMPILE)gcc
CPP		?= $(CROSS_COMPILE)cpp
LD		?= $(CROSS_COMPILE)ld
AR		?= $(CROSS_COMPILE)ar
NM		?= $(CROSS_COMPILE)nm
OBJCOPY		?= $(CROSS_COMPILE)objcopy
OBJDUMP		?= $(CROSS_COMPILE)objdump
READELF		?= $(CROSS_COMPILE)readelf

srcs := fde_key_manager.c base64.c

objs 	:= $(patsubst %.c,$(out-dir)/fde_key_manager/%.o, $(srcs))

CFLAGS += -I./

CFLAGS += -g3

LDFLAGS += -L$(OPTEE_CLIENT_EXPORT)/lib -ljson-c
LDFLAGS += -lm

.PHONY: all
all: fde-reveal-key fde-setup fde-key-manager

fde-reveal-key: $(objs)
	@echo "  LD      $(out-dir)/fde_key_manager/$@"
	$(q)@$(CC) -o $(out-dir)/fde_key_manager/$@ $+ $(LDFLAGS)

fde-setup: $(objs)
	@echo "  LD      $(out-dir)/fde_key_manager/$@"
	$(q)@$(CC) -o $(out-dir)/fde_key_manager/$@ $+ $(LDFLAGS)

fde-key-manager: $(objs)
	@echo "  LD      $(out-dir)/fde_key_manager/$@"
	$(q)@$(CC) -o $(out-dir)/fde_key_manager/$@ $+ $(LDFLAGS)

$(out-dir)/fde_key_manager/%.o: $(CURDIR)/%.c
	$(q)mkdir -p $(out-dir)/fde_key_manager
	@echo '  CC      $<'
	$(q)$(CC) $(CFLAGS) -c $< -o $@

RMDIR := rmdir --ignore-fail-on-non-empty
define rm-build-dirs
	$(q)for d in $1; do $(RMDIR) $(out-dir)/fde_key_manager/$$d 2> /dev/null; true; done
	$(q)$(RMDIR) $(out-dir)/fde_key_manager 2> /dev/null; true
	$(q)$(RMDIR) $(out-dir) 2> /dev/null; true
endef

.PHONY: clean
clean:
	@echo '  CLEAN $(out-dir)'
	$(q)rm -f $(out-dir)/fde_key_manager
	$(q)$(foreach obj,$(objs), rm -f $(obj))
