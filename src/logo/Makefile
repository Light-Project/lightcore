# It is forbidden to place files except logo here
# Because he will clean up the C file

obj-y += logo.o

$(obj)/%.bmp: $(src)/%.svg
	$(Q)$(BASH) $(src)/logogen.sh

$(obj)/logo.c: $(obj)/logo.bmp
	$(Q)$(MAKE) $(build)=tools/logo
	$(Q)$(srctree)/tools/logo/bmp2c $^ -o $@

clean-y += *.c *.bmp
