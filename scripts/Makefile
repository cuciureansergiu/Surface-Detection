#Extract the project name from the project.tcl script
vivado=@sh /tools/Xilinx/Vivado/2018.3/bin/vivado
proj_name=$(shell ./get_project.sh)
proj_dir=~/$(proj_name)/proj
devicetree=system-top.dts
bd_name=System

all: compile bitstream save sdk-hw device-tree
	@clear
	@echo -e '\e[32mDone' '\e[39m'
compile:
	@echo -e '\e[32mCompiling project :' '\e[39m' $(proj_name) 
	@$(vivado) -mode batch -nojournal -nolog -notrace -source project.tcl
clean:
	@clear
	@echo '\e[31mCleaning project :' '\e[39m' $(proj_name) 'in' '\e[33m' $(proj_dir) '\e[39m'
	@rm -rf $(proj_dir)/$(proj_name)*
	@rm -rf ~/$(proj_name)/$(bd_name)*
	@rm -rf ~/$(proj_name)/bin/*
bitstream:
	@echo -e '\e[32mGenerating bitstream''\e[39m'
	$(vivado) $(proj_dir)/$(proj_name).xpr -mode batch -nojournal -nolog -notrace -source bitstream.tcl
open-gui:
	@echo -e '\e[31mOpening project :' '\e[39m' $(proj_name) '\e[39m'
	@$(vivado) $(proj_dir)/$(proj_name).xpr
save:
	@cp $(proj_dir)/$(proj_name).srcs/sources_1/bd/$(bd_name)/hw_handoff/$(bd_name)_bd.tcl ~/$(proj_name)/src/bd/$(bd_name).tcl
	@cp $(proj_dir)/$(proj_name).runs/impl_1/$(bd_name)_wrapper.sysdef ~/$(proj_name)/bin/$(bd_name).hdf 
	@mv ~/$(proj_name)/scripts/$(bd_name).bit ~/$(proj_name)/bin/
sdk-hw:
	@echo -e '\e[31mCleaning workspace ' '\e[39m'
	@rm -rf ~/$(proj_name)/sdk/*
	@rm -rf ~/$(proj_name)/sdk/\.Xil
	@rm -rf ~/$(proj_name)/sdk/\.metadata
	@xsdk -batch -source sdk.tcl
	@cp ~/$(proj_name)/sdk/fsbl/Debug/fsbl.elf ~/$(proj_name)/bin/
device-tree: 
	@echo -e '\e[32mCompiling device tree as :' '\e[39m' $(devicetree) '\e[39m'
	@awk '{gsub(/#include/, "/include/")};{print}' ~/$(proj_name)/sdk/device_tree/$(devicetree) > ~/$(proj_name)/sdk/device_tree/new-$(devicetree)
	@dtc -I dts -O dtb -o ~/$(proj_name)/bin/devicetree.dtb ~/$(proj_name)/sdk/device_tree/new-$(devicetree)
re: clean all