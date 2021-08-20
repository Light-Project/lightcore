# SPDX-License-Identifier: GPL-2.0
# scripts/dtc makefile

hostprogs-always-y += dtc

dtc-objs	:= dtc.o flattree.o fstree.o data.o livetree.o treesource.o srcpos.o checks.o util.o
dtc-objs	+= dtc-lexer.lex.o dtc-parser.tab.o

hostc-flags-y += -I $(obj)libfdt

ifeq ($(shell pkg-config --exists yaml-0.1 2>/dev/null && echo yes),)
$(error dtc needs libyaml for DT schema validation support. \
	Install the necessary libyaml development package.)
else
dtc-objs	+=  yamltree.o
# To include <yaml.h> installed in a non-default path
HOSTCFLAGS_yamltree.o := $(shell pkg-config --cflags yaml-0.1)
# To link libyaml installed in a non-default path
HOSTLDLIBS_dtc	:= $(shell pkg-config yaml-0.1 --libs)
endif

# Generated files need one more search path to include headers in source tree
HOSTCFLAGS_dtc-lexer.lex.o := -I $(srctree)/$(src)
HOSTCFLAGS_dtc-parser.tab.o := -I $(srctree)/$(src)

# dependencies on generated files need to be listed explicitly
$(obj)dtc-lexer.lex.o: $(obj)dtc-parser.tab.h
