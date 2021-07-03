obj-y += libc/
obj-y += malloc/
obj-y += math/
obj-y += pthread/
obj-y += sys/

klib-flags-y += -fPIC -shared
quiet_cmd_link_klib = $(ECHO_ELF)  $@
      cmd_link_klib = $(RANLIB) $^ ; \
                      $(LD) -r -o $(obj)/built-in.o --whole-archive $^; \
                      $(MKELF) $(klib-flags-y) -o $@ $(obj)/built-in.o -lgcc
$(obj)/klib.so: $(obj)/built-in.a
	$(call if_changed,link_klib)

klib: $(obj)/klib.so
	$(Q)$(INSTALL) $^ $(rootfs)/lib

always-y += klib
clean-y += klib.so built-in.o
