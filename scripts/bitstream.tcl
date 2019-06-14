#!/usr/bin/tclsh
# Set the reference directory for source file relative paths (by default the value is script directory path)
set origin_dir ".."
# Set the directory path for the original project from where this script was exported
set bin_dir "[file normalize "$origin_dir/bin"]"
reset_run synth_1
launch_runs impl_1 -to_step write_bitstream -jobs 6 
wait_on_run impl_1
open_run impl_1
write_bitstream $bin_dir/System.bit