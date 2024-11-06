#load RUCKUS environment and library
source -quiet $::env(RUCKUS_DIR)/vivado_proc.tcl

# Load the common source code
loadRuckusTcl $::env(TOP_DIR)/submodules/surf
loadRuckusTcl $::env(TOP_DIR)/common/ts
loadRuckusTcl $::env(TOP_DIR)/common/tdaq
loadRuckusTcl $::env(TOP_DIR)/submodules/axi-soc-ultra-plus-core/shared

# Load shared source code for axi-soc
loadSource -lib axi_soc_ultra_plus_core -dir "$::env(TOP_DIR)/submodules/axi-soc-ultra-plus-core/hardware/XilinxKriaLdmxZccm/rtl"

# Set the board part
set_property board_part xilinx.com:k26c:part0:1.4 [current_project]

# Load the block design
if  { $::env(VIVADO_VERSION) >= 2023.1 } {
   set bdVer "2023.2"
} else {
   set bdVer "2022.2"
}
loadBlockDesign -path $::env(TOP_DIR)/submodules/axi-soc-ultra-plus-core/hardware/XilinxKriaLdmxZccm/bd/${bdVer}/AxiSocUltraPlusCpuCore.bd

loadSource -lib ldmx_ts -dir "$::env(TOP_DIR)/common/ts/rtl/"

# SURF
loadRuckusTcl $::env(TOP_DIR)/submodules/surf
loadRuckusTcl $::env(TOP_DIR)/submodules/surf/protocols/pgp/pgp2fc/gthUltraScale+/

# LDMX Common
loadRuckusTcl $::env(TOP_DIR)/common/tdaq
loadRuckusTcl $::env(TOP_DIR)/common/tracker

# Target tops
loadSource -lib work -path "$::env(TOP_DIR)/targets/zCCM_kria/rtl/zCCM_kria.vhd"

# Load target's source code and constraints
loadSource -lib work -sim_only -dir "$::env(PROJ_DIR)/tb"

# Set the top level for sim_1
set_property top "zCCM_kria"   [get_filesets {sources_1}]
set_property top "zCCMfullTb"  [get_filesets sim_1]
