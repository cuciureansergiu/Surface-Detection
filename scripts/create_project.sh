#!/bin/sh
#source /tools/Xilinx/Vivado/2018.3/settings64.sh
./cleanup.sh
vivado -mode batch -nojournal -nolog -notrace -source create.tcl

