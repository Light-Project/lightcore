obj-y += boot/startup.o
obj-y += driver/
obj-y += main.o


load := $(addprefix _load_,$(wildcard $(obj)/load.o))
$(load):
	$(Q)$(RM) $(patsubst _load_%,%,$@)
$(obj)/load.o: $(load)
obj-y += load.o

extra-y += boot.lds

include-y += $(src)/include/
export include-y
