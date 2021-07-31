# It is forbidden to place files except logo here
# Because he will clean up the C file


$(obj)/%.bmp: $(src)/%.svg
	$(Q)$(BASH) $(src)/logogen.sh

$(obj)/logo.c: $(obj)/logo.bmp
	$(Q)$(MAKE) $(build)=tools/logo
	$(Q)$(srctree)/tools/logo/bmp2c $^ -o $@

obj-y += logo.o terminal-logo.o

clean-y += logo.c *.bmp
