obj-y += boot/
obj-y += lib/
obj-y += variants/
obj-y += power.o
obj-y += proc.o
obj-y += setup.o
obj-y += vectors.o


# In the Xtensa architecture, assembly generates literals which must always
# precede the L32R instruction with a relative offset less than 256 kB.
# Therefore, the .text and .literal section must be combined in parenthesis
# in the linker script, such as: *(.literal .text).
#
# We need to post-process the generated vmlinux.lds scripts to convert
# *(xxx.text) to  *(xxx.literal xxx.text) for the following text sections:
#   .text .ref.text .*init.text .*exit.text .text.*
#
# Replicate rules in scripts/Makefile.build

sed-y = -e ':a; s/\*(\([^)]*\)\.text\.unlikely/*(\1.literal.unlikely .{text}.unlikely/; ta; '   \
        -e ':b; s/\*(\([^)]*\)\.text\(\.[a-z]*\)/*(\1.{text}\2.literal .{text}\2/; tb; '        \
        -e ':c; s/\*(\([^)]*\)\(\.[a-z]*it\|\.ref\)\.text/*(\1\2.literal \2.{text}/; tc; '      \
        -e ':d; s/\*(\([^)]\+ \|\)\.text/*(\1.literal .{text}/; td; '                           \
        -e 's/\.{text}/.text/g'

quiet_cmd__cpp_lds_S = LDS     $@
cmd__cpp_lds_S = $(CPP) $(cpp_flags) -P -C -Uxtensa -D__ASSEMBLY__ \
                -DLINKER_SCRIPT $< | sed $(sed-y) >$@
$(obj)/kernel.lds: $(src)/kernel.lds.S FORCE
	$(call if_changed_dep,_cpp_lds_S)
extra-y += kernel.lds

clean-y += include/generated/
