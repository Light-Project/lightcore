# SPDX-License-Identifier: GPL-2.0
# ===========================================================================
# Kernel configuration targets
# These targets are used from top-level makefile

PHONY += xconfig gconfig menuconfig config localmodconfig localyesconfig \
	build_menuconfig build_nconfig build_gconfig build_xconfig

ifdef KBUILD_KCONFIG
Kconfig := $(KBUILD_KCONFIG)
else
Kconfig := Kconfig
endif

ifeq ($(quiet),silent_)
silent := -s
endif

# We need this, in case the user has it in its environment
unexport CONFIG_

config-prog	    := conf
menuconfig-prog	:= mconf
nconfig-prog	:= nconf
gconfig-prog	:= gconf
xconfig-prog	:= qconf

define config_rule
PHONY += $(1)
$(1): $(obj)/$($(1)-prog)
	$(Q)$$< $(silent) $(Kconfig)

PHONY += build_$(1)
build_$(1): $(obj)/$($(1)-prog)
endef

$(foreach c, config menuconfig nconfig gconfig xconfig, $(eval $(call config_rule,$(c))))

localyesconfig localmodconfig: $(obj)/conf
	$(Q)$(PERL) $(srctree)/$(obj)streamline_config.pl --$@ $(srctree) $(Kconfig) > .tmp.config
	$(Q)if [ -f .config ]; then 				\
		cmp -s .tmp.config .config ||			\
		(mv -f .config .config.old.1;			\
		 mv -f .tmp.config .config;			\
		 $< $(silent) --oldconfig $(Kconfig);		\
		 mv -f .config.old.1 .config.old)		\
	else							\
		mv -f .tmp.config .config;			\
		$< $(silent) --oldconfig $(Kconfig);		\
	fi
	$(Q)rm -f .tmp.config

# These targets map 1:1 to the commandline options of 'conf'
#
# Note:
#  syncconfig has become an internal implementation detail and is now
#  deprecated for external use
simple-targets := oldconfig allnoconfig allyesconfig allmodconfig \
		alldefconfig randconfig listnewconfig olddefconfig syncconfig \
		helpnewconfig yes2modconfig mod2yesconfig

PHONY += $(simple-targets)

$(simple-targets): $(obj)/conf
	$(Q)$< $(silent) --$@ $(Kconfig)

PHONY += savedefconfig defconfig

savedefconfig: $(obj)/conf
	$(Q)$< $(silent) --$@=defconfig $(Kconfig)

defconfig: $(obj)/conf
ifneq ($(wildcard $(srctree)/arch/$(SRCARCH)/configs/$(KBUILD_DEFCONFIG)),)
	@$(kecho) "*** Default configuration is based on '$(KBUILD_DEFCONFIG)'"
	$(Q)$< $(silent) --defconfig=arch/$(SRCARCH)/configs/$(KBUILD_DEFCONFIG) $(Kconfig)
else
	@$(kecho) "*** Default configuration is based on target '$(KBUILD_DEFCONFIG)'"
	$(Q)$(MAKE) -f Makefile $(KBUILD_DEFCONFIG)
endif

%_defconfig: $(obj)/conf
	$(Q)$< $(silent) --defconfig=configs/$@ $(Kconfig)

configfiles=$(wildcard $(srctree)/kernel/configs/$@ $(srctree)/arch/$(SRCARCH)/configs/$@)

%.config: $(obj)/conf
	$(if $(call configfiles),, $(error No configuration exists for this target on this architecture))
	$(Q)$(CONFIG_SHELL) $(srctree)/scripts/kconfig/merge_config.sh -m .config $(configfiles)
	$(Q)$(MAKE) -f $(srctree)/Makefile olddefconfig

PHONY += kvmconfig
kvmconfig: kvm_guest.config
	@echo >&2 "WARNING: 'make $@' will be removed after Linux 5.10"
	@echo >&2 "         Please use 'make $<' instead."

PHONY += xenconfig
xenconfig: xen.config
	@echo >&2 "WARNING: 'make $@' will be removed after Linux 5.10"
	@echo >&2 "         Please use 'make $<' instead."

PHONY += tinyconfig
tinyconfig:
	$(Q)$(MAKE) -f $(srctree)/Makefile allnoconfig tiny.config

# CHECK: -o cache_dir=<path> working?
PHONY += testconfig
testconfig: $(obj)/conf
	$(Q)$(PYTHON3) -B -m pytest $(srctree)/$(obj)tests \
	-o cache_dir=$(abspath $(obj)tests/.cache) \
	$(if $(findstring 1,$(KBUILD_VERBOSE)),--capture=no)
clean-files += tests/.cache

# ===========================================================================
# object files used by all kconfig flavours
common-objs	:= confdata.o expr.o lexer.lex.o menu.o \
			   parser.tab.o preprocess.o symbol.o util.o

$(obj)/lexer.lex.o: $(obj)/parser.tab.h
HOSTCFLAGS_lexer.lex.o	:= -I $(srctree)/$(src)
HOSTCFLAGS_parser.tab.o	:= -I $(srctree)/$(src)

# conf: Used for defconfig, oldconfig and related targets
hostprogs	+= conf
conf-objs	:= conf.o $(common-objs)

# nconf: Used for the nconfig target based on ncurses
hostprogs	+= nconf
nconf-objs	:= nconf.o nconf.gui.o $(common-objs)

HOSTLDLIBS_nconf	    = $(shell . $(obj)/nconf-cfg && echo $$libs)
HOSTCFLAGS_nconf.o	    = $(shell . $(obj)/nconf-cfg && echo $$cflags)
HOSTCFLAGS_nconf.gui.o	= $(shell . $(obj)/nconf-cfg && echo $$cflags)

$(obj)/nconf.o $(obj)/nconf.gui.o: $(obj)/nconf-cfg

# mconf: Used for the menuconfig target based on lxdialog
hostprogs	+= mconf
lxdialog	:= $(addprefix lxdialog/, \
		     checklist.o inputbox.o menubox.o textbox.o util.o yesno.o)
mconf-objs	:= mconf.o $(lxdialog) $(common-objs)

HOSTLDLIBS_mconf = $(shell . $(obj)/mconf-cfg && echo $$libs)
$(foreach f, mconf.o $(lxdialog), \
  $(eval HOSTCFLAGS_$f = $$(shell . $(obj)/mconf-cfg && echo $$$$cflags)))

$(addprefix $(obj)/, mconf.o $(lxdialog)): $(obj)/mconf-cfg

# qconf: Used for the xconfig target based on Qt
hostprogs	+= qconf
qconf-cxxobjs	:= qconf.o qconf-moc.o
qconf-objs	:= images.o $(common-objs)

HOSTLDLIBS_qconf            = $(shell . $(obj)/qconf-cfg && echo $$libs)
HOSTCXXFLAGS_qconf.o	    = $(shell . $(obj)/qconf-cfg && echo $$cflags)
HOSTCXXFLAGS_qconf-moc.o    = $(shell . $(obj)/qconf-cfg && echo $$cflags)

$(obj)/qconf.o: $(obj)/qconf-cfg

quiet_cmd_moc = MOC     $@
      cmd_moc = $(shell . $(obj)/qconf-cfg && echo $$moc) $< -o $@
$(obj)/qconf-moc.cc: $(src)/qconf.h $(obj)/qconf-cfg FORCE
	$(call if_changed,moc)

targets += qconf-moc.cc

# gconf: Used for the gconfig target based on GTK+
hostprogs	+= gconf
gconf-objs	:= gconf.o images.o $(common-objs)

HOSTLDLIBS_gconf    = $(shell . $(obj)/gconf-cfg && echo $$libs)
HOSTCFLAGS_gconf.o  = $(shell . $(obj)/gconf-cfg && echo $$cflags)

$(obj)/gconf.o: $(obj)/gconf-cfg

# check if necessary packages are available, and configure build flags
filechk_conf_cfg = $(CONFIG_SHELL) $<

$(obj)/%conf-cfg: $(src)/%conf-cfg.sh FORCE
	$(call filechk,conf_cfg)

# Help text used by make help
help:
	$(Q)$(ECHO) 'config         - Update current config utilising a line-oriented program'
	$(Q)$(ECHO) 'nconfig        - Update current config utilising a ncurses menu based program'
	$(Q)$(ECHO) 'menuconfig	    - Update current config utilising a menu based program'
	$(Q)$(ECHO) 'xconfig	    - Update current config utilising a Qt based front-end'
	$(Q)$(ECHO) 'gconfig	    - Update current config utilising a GTK+ based front-end'
	$(Q)$(ECHO) 'oldconfig	    - Update current config utilising a provided .config as base'
	$(Q)$(ECHO) 'localmodconfig - Update current config disabling modules not loaded'
	$(Q)$(ECHO) '                  except those preserved by LMC_KEEP environment variable'
	$(Q)$(ECHO) 'localyesconfig - Update current config converting local mods to core'
	$(Q)$(ECHO) '                  except those preserved by LMC_KEEP environment variable'
	$(Q)$(ECHO) 'defconfig      - New config with default from ARCH supplied defconfig'
	$(Q)$(ECHO) 'savedefconfig  - Save current config as ./defconfig (minimal config)'
	$(Q)$(ECHO) 'allnoconfig    - New config where all options are answered with no'
	$(Q)$(ECHO) 'allyesconfig   - New config where all options are accepted with yes'
	$(Q)$(ECHO) 'allmodconfig   - New config selecting modules when possible'
	$(Q)$(ECHO) 'alldefconfig   - New config with all symbols set to default'
	$(Q)$(ECHO) 'randconfig     - New config with random answer to all options'
	$(Q)$(ECHO) 'yes2modconfig  - Change answers from yes to mod if possible'
	$(Q)$(ECHO) 'mod2yesconfig  - Change answers from mod to yes if possible'
	$(Q)$(ECHO) 'listnewconfig  - List new options'
	$(Q)$(ECHO) 'helpnewconfig  - List new options and help text'
	$(Q)$(ECHO) 'olddefconfig   - Same as oldconfig but sets new symbols to their'
	$(Q)$(ECHO) '                  default value without prompting'
	$(Q)$(ECHO) 'tinyconfig	    - Configure the tiniest possible kernel'
	$(Q)$(ECHO) 'testconfig	    - Run Kconfig unit tests (requires python3 and pytest)'

clean-y += *conf-cfg
