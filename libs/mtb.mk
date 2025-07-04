#
# This file is generated by ModusToolbox during the 'make getlibs' operation
# Any edits to this file will be lost the next time the library manager is run or
# the next time 'make getlibs' is run.
#
# List of local libraries


# Path to the current BSP
SEARCH_TARGET_tsdz8_for_GPIO_TEST=bsps/TARGET_tsdz8_for_GPIO_TEST

# The search paths for the included middleware
SEARCH_retarget-io-cat3=../mtb_shared/retarget-io-cat3/release-v1.1.0
SEARCH_cmsis=../mtb_shared/cmsis/release-v5.8.0
SEARCH_core-lib=../mtb_shared/core-lib/release-v1.4.3
SEARCH_core-make=../mtb_shared/core-make/release-v3.4.1
SEARCH_mtb-xmclib-cat3=../mtb_shared/mtb-xmclib-cat3/release-v4.4.0
SEARCH_recipe-make-cat3=../mtb_shared/recipe-make-cat3/release-v2.3.1

# Search libraries added to build
SEARCH_MTB_MK+=$(SEARCH_retarget-io-cat3)
SEARCH_MTB_MK+=$(SEARCH_cmsis)
SEARCH_MTB_MK+=$(SEARCH_core-lib)
SEARCH_MTB_MK+=$(SEARCH_core-make)
SEARCH_MTB_MK+=$(SEARCH_mtb-xmclib-cat3)
SEARCH_MTB_MK+=$(SEARCH_recipe-make-cat3)

-include $(CY_INTERNAL_APP_PATH)/importedbsp.mk
COMPONENTS += MW_RETARGET_IO_CAT3
COMPONENTS += MW_CMSIS
COMPONENTS += MW_CORE_LIB
COMPONENTS += MW_CORE_MAKE
COMPONENTS += MW_MTB_XMCLIB_CAT3
COMPONENTS += MW_RECIPE_MAKE_CAT3

# Register map file
DEVICE_XMC1302-T038x0064_SVD=$(SEARCH_mtb-xmclib-cat3)/CMSIS/Infineon/SVD/XMC1300.svd


#
# generate make targets for the graphical editors that are specific to this project
#

bsp-assistant:
	$(CY_TOOL_mtblaunch_EXE_ABS) --project . --short-name bsp-assistant
.PHONY: bsp-assistant

CY_HELP_bsp-assistant=Launches the BSP Assistant 1.40 GUI
CY_HELP_bsp-assistant_VERBOSE=Launches the BSP Assistant 1.40 GUI. Check the BSP Assistant 1.40 User Guide for more information.
mtb_help_tool_bsp-assistant:
	@:
	$(info $(MTB__SPACE)bsp-assistant       $(CY_HELP_bsp-assistant))

mtb_help_tools_end: mtb_help_tool_bsp-assistant
mtb_help_tool_bsp-assistant: mtb_help_tools_start
.PHONY: mtb_help_tool_bsp-assistant

config:
	$(CY_TOOL_mtblaunch_EXE_ABS) --project . --short-name device-configurator
.PHONY: config

CY_HELP_config=Launches the Device Configurator 5.30 GUI for the target's modus file
CY_HELP_config_VERBOSE=Launches the Device Configurator 5.30 GUI. Check the Device Configurator 5.30 User Guide for more information.
mtb_help_tool_config:
	@:
	$(info $(MTB__SPACE)config              $(CY_HELP_config))

mtb_help_tools_end: mtb_help_tool_config
mtb_help_tool_config: mtb_help_tools_start
.PHONY: mtb_help_tool_config

device-configurator:
	$(CY_TOOL_mtblaunch_EXE_ABS) --project . --short-name device-configurator
.PHONY: device-configurator

CY_HELP_device-configurator=Launches the Device Configurator 5.30 GUI for the target's modus file
CY_HELP_device-configurator_VERBOSE=Launches the Device Configurator 5.30 GUI. Check the Device Configurator 5.30 User Guide for more information.
mtb_help_tool_device-configurator:
	@:
	$(info $(MTB__SPACE)device-configurator $(CY_HELP_device-configurator))

mtb_help_tools_end: mtb_help_tool_device-configurator
mtb_help_tool_device-configurator: mtb_help_tools_start
.PHONY: mtb_help_tool_device-configurator

modlibs:
	$(CY_TOOL_mtblaunch_EXE_ABS) --project . --short-name library-manager
.PHONY: modlibs

CY_HELP_modlibs=Launches the Library Manager 2.40 GUI
CY_HELP_modlibs_VERBOSE=Launches the Library Manager 2.40 GUI. Check the Library Manager 2.40 User Guide for more information.
mtb_help_tool_modlibs:
	@:
	$(info $(MTB__SPACE)modlibs             $(CY_HELP_modlibs))

mtb_help_tools_end: mtb_help_tool_modlibs
mtb_help_tool_modlibs: mtb_help_tools_start
.PHONY: mtb_help_tool_modlibs

library-manager:
	$(CY_TOOL_mtblaunch_EXE_ABS) --project . --short-name library-manager
.PHONY: library-manager

CY_HELP_library-manager=Launches the Library Manager 2.40 GUI
CY_HELP_library-manager_VERBOSE=Launches the Library Manager 2.40 GUI. Check the Library Manager 2.40 User Guide for more information.
mtb_help_tool_library-manager:
	@:
	$(info $(MTB__SPACE)library-manager     $(CY_HELP_library-manager))

mtb_help_tools_end: mtb_help_tool_library-manager
mtb_help_tool_library-manager: mtb_help_tools_start
.PHONY: mtb_help_tool_library-manager

.PHONY: mtb_help_tools_start mtb_help_tools_end
