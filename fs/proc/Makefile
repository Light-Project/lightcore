
obj-y += configs.o

$(obj)/configs.o: $(obj)/config_data.gz

# targets += config_data.gz
$(obj)/config_data.gz: $(Kconfig) FORCE
	$(call if_changed,gzip)
