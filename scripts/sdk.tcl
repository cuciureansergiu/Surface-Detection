#!/usr/bin/tclsh
if {[info exists ::create_path]} {
	set dest_dir $::create_path
} else {
	set dest_dir [file normalize [file dirname [info script]]]
}
cd $dest_dir

# Set the reference directory for source file relative paths (by default the value is script directory path)
set proj_name "Surface-Detection"
set bd_name "System"

set origin_dir ".."
set bin_dir "[file normalize "$origin_dir/bin"]"
set sdk_dir "[file normalize "$origin_dir/sdk"]"

setws $sdk_dir
# Create hwspec
sdk createhw -name hw -hwspec $bin_dir/$bd_name.hdf
# Create FSBL bsp
#sdk createbsp -name fsbl_bsp_0 -hwproject hw_0 -proc ps7_cortexa9_0 -os standalone
# Create FSBL
sdk createapp -name fsbl -hwproject hw -proc ps7_cortexa9_0 -os standalone -lang C -app {Zynq FSBL} -bsp fsbl_bsp
# Create device-tree project
sdk createbsp -name device_tree -hwproject hw -proc ps7_cortexa9_0 -os device_tree

clean -type all
build -type all
exit