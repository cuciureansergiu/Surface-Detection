#Extract the project name from the project.tcl script
vivado=@sh /tools/Xilinx/Vivado/2018.3/bin/vivado
proj_name=$(shell ./get_project.sh)
proj_dir=~/$(proj_name)/proj
sdk_dir=~/$(proj_name)/sdk
src_dir=~/$(proj_name)/src
bin_dir=~/$(proj_name)/bin
devicetree=system-top.dts
bd_name=System

all: compile bitstream save sdk-hw device-tree boot
	@clear && echo '\e[32mDone' '\e[39m'
compile:
	@clear && echo '\e[32mCompiling project :' '\e[39m' $(proj_name) 
	@vivado -mode batch -nojournal -nolog -notrace -source project.tcl
clean:
	@clear && echo '\e[31mCleaning project :' '\e[39m' $(proj_name) 'in' '\e[33m' $(proj_dir) '\e[39m'
	@rm -rf $(proj_dir)/$(proj_name)*
	@rm -rf ~/$(proj_name)/$(bd_name)*
	@rm -rf $(bin_dir)/*
bitstream:
	@clear && echo '\e[32mGenerating bitstream''\e[39m'
	@vivado $(proj_dir)/$(proj_name).xpr -mode batch -nojournal -nolog -notrace -source bitstream.tcl
open-vivado:
	@clear && echo '\e[31mOpening project :' '\e[39m' $(proj_name) '\e[39m'
	@vivado $(proj_dir)/$(proj_name).xpr
open-sdk:
	@clear && echo '\e[31mOpening project :' '\e[39m' $(proj_name) '\e[39m'
	@xsdk -workspace $(sdk_dir)
save:
	@cp $(proj_dir)/$(proj_name).srcs/sources_1/bd/$(bd_name)/hw_handoff/$(bd_name)_bd.tcl ~/$(proj_name)/src/bd/$(bd_name).tcl
	@cp $(proj_dir)/$(proj_name).runs/impl_1/$(bd_name)_wrapper.sysdef ~/$(proj_name)/bin/$(bd_name).hdf
sdk-hw:
	@clear && echo '\e[31mCleaning workspace ' '\e[39m'
	@rm -rf $(sdk_dir)/*
	@rm -rf $(sdk_dir)/\.Xil
	@rm -rf $(sdk_dir)/\.metadata
	@xsdk -batch -source sdk.tcl
	@cp ~/$(proj_name)/sdk/fsbl/Debug/fsbl.elf $(bin_dir)
device-tree: 
	@clear && echo '\e[32mCompiling device tree as :' '\e[39m' $(devicetree) '\e[39m'
	@awk '{gsub(/#include/, "/include/")};{print}' $(sdk_dir)/device_tree/$(devicetree) > $(sdk_dir)/device_tree/new-$(devicetree)
	@rm $(sdk_dir)/device_tree/$(devicetree)
	@awk '{gsub(/memory {/, "memory@0 {")};{print}' $(sdk_dir)/device_tree/new-$(devicetree) > $(sdk_dir)/device_tree/$(devicetree)
	@sed -i '10s/^/\/include\/\ "dma_proxy.dtsi"\n/' $(sdk_dir)/device_tree/$(devicetree)
	@sed -i '10s/^/\ \tphy-handle = <&ethernet_phy>;\n\tethernet_phy: ethernet-phy@0 {\n\t\t\treg = <0>;\n\t\t};\n/' $(sdk_dir)/device_tree/system.dts
	@cp $(src_dir)/dtc/dma_proxy.dtsi $(sdk_dir)/device_tree
	@dtc -I dts -O dtb -o $(bin_dir)/devicetree.dtb $(sdk_dir)/device_tree/$(devicetree)
boot:
	@clear && echo '\e[32mCreating boot image' '\e[39m'
	@rm ~/$(proj_name)/boot/BOOT.bin
	@bootgen -arch zynq -image ~/$(proj_name)/scripts/boot.bif -o i ~/$(proj_name)/boot/BOOT.bin
sd-card:
	@cp ~/$(proj_name)/boot/BOOT.bin /media/$(shell whoami)/BOOT
	@cp ~/$(proj_name)/boot/devicetree.dtb /media/$(shell whoami)/BOOT 
	@cp $(src_dir)/boot-src/uImage /media/$(shell whoami)/BOOT 
	@umount /media/$(shell whoami)/BOOT
	@umount /media/$(shell whoami)/rootfs
re: clean all