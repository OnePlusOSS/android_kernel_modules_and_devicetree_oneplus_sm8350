vendor := $(srctree)/$(src)

#ifndef VENDOR_EDIT
#ifneq "$(wildcard $(vendor)/qcom)" ""
#	subdir-y += qcom
#endif
#else
#	subdir-y += 20031
	subdir-y += qcom
ifeq ($(strip $(CONFIG_CHG_FOR_OP9_PRO)), y)
        subdir-y += oplus/lemonadev
else ifeq ($(strip $(CONFIG_OPLUS_CHG_OP9RT_PMIC_VOOCPHY)), y)
        subdir-y += oplus/martini
endif
#endif
