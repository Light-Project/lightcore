obj-y += info.o
obj-y += check.o
obj-y += mem.o
obj-y += memtest.o
obj-y += string.o
obj-y += vsprintf.o

piggy := $(addprefix _piggy_,$(wildcard $(obj)piggy.o))
$(piggy):
	$(RM) $(patsubst _piggy_%,%,$@)
$(obj)piggy.o: $(piggy)
obj-y += piggy.o

ifndef CONFIG_PACK_NONE
obj-y += decompress.o
endif
