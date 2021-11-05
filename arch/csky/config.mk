ifdef CONFIG_CPU_HAS_FPU
FPUEXT = f
endif

ifdef CONFIG_CPU_HAS_VDSP
VDSPEXT = v
endif

ifdef CONFIG_CPU_HAS_TEE
TEEEXT = t
endif


ifdef CONFIG_CPU_CK810
CPUTYPE = ck810
CSKYABI	= abiv2
endif

ifdef CONFIG_CPU_CK807
CPUTYPE = ck807
CSKYABI	= abiv2
endif

ifdef CONFIG_CPU_CK860
CPUTYPE = ck860
CSKYABI	= abiv2
endif

ifneq ($(CSKYABI),)
MCPU_STR = $(CPUTYPE)$(FPUEXT)$(VDSPEXT)$(TEEEXT)
acflags-y += -mcpu=$(CPUTYPE) -Wa,-mcpu=$(MCPU_STR)
acflags-y += -DCSKYCPU_DEF_NAME=\"$(MCPU_STR)\"
acflags-y += -msoft-float -mdiv
acflags-y += -fno-tree-vectorize -pipe
endif

acflags-y += -pipe
ldflags-sub-y += -EL

ifeq ($(CSKYABI),abiv2)
acflags-y += -mno-stack-size
endif
