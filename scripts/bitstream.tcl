#!/usr/bin/tclsh
launch_runs impl_1 -to_step write_bitstream -jobs 6 
wait_on_run impl_1
open_run impl_1
write_bitstream System.bit