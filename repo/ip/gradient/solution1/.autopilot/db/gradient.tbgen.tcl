set moduleName gradient
set isTopModule 1
set isTaskLevelControl 1
set isCombinational 0
set isDatapathOnly 0
set isFreeRunPipelineModule 0
set isPipelined 0
set pipeline_type none
set FunctionProtocol ap_ctrl_hs
set isOneStateSeq 0
set ProfileFlag 0
set StallSigGenFlag 0
set isEnableWaveformDebug 1
set C_modelName {gradient}
set C_modelType { void 0 }
set C_modelArgList {
	{ input_f int 32 regular {bram 4096 { 1 3 } 1 1 }  }
	{ kernel int 32 regular {bram 4096 { 1 3 } 1 1 }  }
	{ output_f int 32 regular {bram 4096 { 0 3 } 0 1 }  }
	{ status int 32 unused {axi_slave 0}  }
}
set C_modelArgMapList {[ 
	{ "Name" : "input_f", "interface" : "bram", "bitwidth" : 32, "direction" : "READONLY", "bitSlice":[{"low":0,"up":31,"cElement": [{"cName": "input_f","cData": "unsigned int","bit_use": { "low": 0,"up": 31},"cArray": [{"low" : 0,"up" : 4095,"step" : 1}]}]}]} , 
 	{ "Name" : "kernel", "interface" : "bram", "bitwidth" : 32, "direction" : "READONLY", "bitSlice":[{"low":0,"up":31,"cElement": [{"cName": "kernel","cData": "unsigned int","bit_use": { "low": 0,"up": 31},"cArray": [{"low" : 0,"up" : 4095,"step" : 1}]}]}]} , 
 	{ "Name" : "output_f", "interface" : "bram", "bitwidth" : 32, "direction" : "WRITEONLY", "bitSlice":[{"low":0,"up":31,"cElement": [{"cName": "output_f","cData": "unsigned int","bit_use": { "low": 0,"up": 31},"cArray": [{"low" : 0,"up" : 4095,"step" : 1}]}]}]} , 
 	{ "Name" : "status", "interface" : "axi_slave", "bundle":"CRTL_BUS","type":"ap_none","bitwidth" : 32, "direction" : "READONLY", "bitSlice":[{"low":0,"up":31,"cElement": [{"cName": "status","cData": "unsigned int","bit_use": { "low": 0,"up": 31},"cArray": [{"low" : 0,"up" : 0,"step" : 0}]}]}], "offset" : {"in":16}, "offset_end" : {"in":23}} ]}
# RTL Port declarations: 
set portNum 41
set portList { 
	{ ap_clk sc_in sc_logic 1 clock -1 } 
	{ ap_rst_n sc_in sc_logic 1 reset -1 active_low_sync } 
	{ input_f_Addr_A sc_out sc_lv 32 signal 0 } 
	{ input_f_EN_A sc_out sc_logic 1 signal 0 } 
	{ input_f_WEN_A sc_out sc_lv 4 signal 0 } 
	{ input_f_Din_A sc_out sc_lv 32 signal 0 } 
	{ input_f_Dout_A sc_in sc_lv 32 signal 0 } 
	{ input_f_Clk_A sc_out sc_logic 1 signal 0 } 
	{ input_f_Rst_A sc_out sc_logic 1 signal 0 } 
	{ kernel_Addr_A sc_out sc_lv 32 signal 1 } 
	{ kernel_EN_A sc_out sc_logic 1 signal 1 } 
	{ kernel_WEN_A sc_out sc_lv 4 signal 1 } 
	{ kernel_Din_A sc_out sc_lv 32 signal 1 } 
	{ kernel_Dout_A sc_in sc_lv 32 signal 1 } 
	{ kernel_Clk_A sc_out sc_logic 1 signal 1 } 
	{ kernel_Rst_A sc_out sc_logic 1 signal 1 } 
	{ output_f_Addr_A sc_out sc_lv 32 signal 2 } 
	{ output_f_EN_A sc_out sc_logic 1 signal 2 } 
	{ output_f_WEN_A sc_out sc_lv 4 signal 2 } 
	{ output_f_Din_A sc_out sc_lv 32 signal 2 } 
	{ output_f_Dout_A sc_in sc_lv 32 signal 2 } 
	{ output_f_Clk_A sc_out sc_logic 1 signal 2 } 
	{ output_f_Rst_A sc_out sc_logic 1 signal 2 } 
	{ s_axi_CRTL_BUS_AWVALID sc_in sc_logic 1 signal -1 } 
	{ s_axi_CRTL_BUS_AWREADY sc_out sc_logic 1 signal -1 } 
	{ s_axi_CRTL_BUS_AWADDR sc_in sc_lv 5 signal -1 } 
	{ s_axi_CRTL_BUS_WVALID sc_in sc_logic 1 signal -1 } 
	{ s_axi_CRTL_BUS_WREADY sc_out sc_logic 1 signal -1 } 
	{ s_axi_CRTL_BUS_WDATA sc_in sc_lv 32 signal -1 } 
	{ s_axi_CRTL_BUS_WSTRB sc_in sc_lv 4 signal -1 } 
	{ s_axi_CRTL_BUS_ARVALID sc_in sc_logic 1 signal -1 } 
	{ s_axi_CRTL_BUS_ARREADY sc_out sc_logic 1 signal -1 } 
	{ s_axi_CRTL_BUS_ARADDR sc_in sc_lv 5 signal -1 } 
	{ s_axi_CRTL_BUS_RVALID sc_out sc_logic 1 signal -1 } 
	{ s_axi_CRTL_BUS_RREADY sc_in sc_logic 1 signal -1 } 
	{ s_axi_CRTL_BUS_RDATA sc_out sc_lv 32 signal -1 } 
	{ s_axi_CRTL_BUS_RRESP sc_out sc_lv 2 signal -1 } 
	{ s_axi_CRTL_BUS_BVALID sc_out sc_logic 1 signal -1 } 
	{ s_axi_CRTL_BUS_BREADY sc_in sc_logic 1 signal -1 } 
	{ s_axi_CRTL_BUS_BRESP sc_out sc_lv 2 signal -1 } 
	{ interrupt sc_out sc_logic 1 signal -1 } 
}
set NewPortList {[ 
	{ "name": "s_axi_CRTL_BUS_AWADDR", "direction": "in", "datatype": "sc_lv", "bitwidth":5, "type": "signal", "bundle":{"name": "CRTL_BUS", "role": "AWADDR" },"address":[{"name":"gradient","role":"start","value":"0","valid_bit":"0"},{"name":"gradient","role":"continue","value":"0","valid_bit":"4"},{"name":"gradient","role":"auto_start","value":"0","valid_bit":"7"},{"name":"status","role":"data","value":"16"}] },
	{ "name": "s_axi_CRTL_BUS_AWVALID", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "CRTL_BUS", "role": "AWVALID" } },
	{ "name": "s_axi_CRTL_BUS_AWREADY", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "CRTL_BUS", "role": "AWREADY" } },
	{ "name": "s_axi_CRTL_BUS_WVALID", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "CRTL_BUS", "role": "WVALID" } },
	{ "name": "s_axi_CRTL_BUS_WREADY", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "CRTL_BUS", "role": "WREADY" } },
	{ "name": "s_axi_CRTL_BUS_WDATA", "direction": "in", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "CRTL_BUS", "role": "WDATA" } },
	{ "name": "s_axi_CRTL_BUS_WSTRB", "direction": "in", "datatype": "sc_lv", "bitwidth":4, "type": "signal", "bundle":{"name": "CRTL_BUS", "role": "WSTRB" } },
	{ "name": "s_axi_CRTL_BUS_ARADDR", "direction": "in", "datatype": "sc_lv", "bitwidth":5, "type": "signal", "bundle":{"name": "CRTL_BUS", "role": "ARADDR" },"address":[{"name":"gradient","role":"start","value":"0","valid_bit":"0"},{"name":"gradient","role":"done","value":"0","valid_bit":"1"},{"name":"gradient","role":"idle","value":"0","valid_bit":"2"},{"name":"gradient","role":"ready","value":"0","valid_bit":"3"},{"name":"gradient","role":"auto_start","value":"0","valid_bit":"7"}] },
	{ "name": "s_axi_CRTL_BUS_ARVALID", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "CRTL_BUS", "role": "ARVALID" } },
	{ "name": "s_axi_CRTL_BUS_ARREADY", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "CRTL_BUS", "role": "ARREADY" } },
	{ "name": "s_axi_CRTL_BUS_RVALID", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "CRTL_BUS", "role": "RVALID" } },
	{ "name": "s_axi_CRTL_BUS_RREADY", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "CRTL_BUS", "role": "RREADY" } },
	{ "name": "s_axi_CRTL_BUS_RDATA", "direction": "out", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "CRTL_BUS", "role": "RDATA" } },
	{ "name": "s_axi_CRTL_BUS_RRESP", "direction": "out", "datatype": "sc_lv", "bitwidth":2, "type": "signal", "bundle":{"name": "CRTL_BUS", "role": "RRESP" } },
	{ "name": "s_axi_CRTL_BUS_BVALID", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "CRTL_BUS", "role": "BVALID" } },
	{ "name": "s_axi_CRTL_BUS_BREADY", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "CRTL_BUS", "role": "BREADY" } },
	{ "name": "s_axi_CRTL_BUS_BRESP", "direction": "out", "datatype": "sc_lv", "bitwidth":2, "type": "signal", "bundle":{"name": "CRTL_BUS", "role": "BRESP" } },
	{ "name": "interrupt", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "CRTL_BUS", "role": "interrupt" } }, 
 	{ "name": "ap_clk", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "clock", "bundle":{"name": "ap_clk", "role": "default" }} , 
 	{ "name": "ap_rst_n", "direction": "in", "datatype": "sc_logic", "bitwidth":1, "type": "reset", "bundle":{"name": "ap_rst_n", "role": "default" }} , 
 	{ "name": "input_f_Addr_A", "direction": "out", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "input_f", "role": "Addr_A" }} , 
 	{ "name": "input_f_EN_A", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "input_f", "role": "EN_A" }} , 
 	{ "name": "input_f_WEN_A", "direction": "out", "datatype": "sc_lv", "bitwidth":4, "type": "signal", "bundle":{"name": "input_f", "role": "WEN_A" }} , 
 	{ "name": "input_f_Din_A", "direction": "out", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "input_f", "role": "Din_A" }} , 
 	{ "name": "input_f_Dout_A", "direction": "in", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "input_f", "role": "Dout_A" }} , 
 	{ "name": "input_f_Clk_A", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "input_f", "role": "Clk_A" }} , 
 	{ "name": "input_f_Rst_A", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "input_f", "role": "Rst_A" }} , 
 	{ "name": "kernel_Addr_A", "direction": "out", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "kernel", "role": "Addr_A" }} , 
 	{ "name": "kernel_EN_A", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "kernel", "role": "EN_A" }} , 
 	{ "name": "kernel_WEN_A", "direction": "out", "datatype": "sc_lv", "bitwidth":4, "type": "signal", "bundle":{"name": "kernel", "role": "WEN_A" }} , 
 	{ "name": "kernel_Din_A", "direction": "out", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "kernel", "role": "Din_A" }} , 
 	{ "name": "kernel_Dout_A", "direction": "in", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "kernel", "role": "Dout_A" }} , 
 	{ "name": "kernel_Clk_A", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "kernel", "role": "Clk_A" }} , 
 	{ "name": "kernel_Rst_A", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "kernel", "role": "Rst_A" }} , 
 	{ "name": "output_f_Addr_A", "direction": "out", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "output_f", "role": "Addr_A" }} , 
 	{ "name": "output_f_EN_A", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "output_f", "role": "EN_A" }} , 
 	{ "name": "output_f_WEN_A", "direction": "out", "datatype": "sc_lv", "bitwidth":4, "type": "signal", "bundle":{"name": "output_f", "role": "WEN_A" }} , 
 	{ "name": "output_f_Din_A", "direction": "out", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "output_f", "role": "Din_A" }} , 
 	{ "name": "output_f_Dout_A", "direction": "in", "datatype": "sc_lv", "bitwidth":32, "type": "signal", "bundle":{"name": "output_f", "role": "Dout_A" }} , 
 	{ "name": "output_f_Clk_A", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "output_f", "role": "Clk_A" }} , 
 	{ "name": "output_f_Rst_A", "direction": "out", "datatype": "sc_logic", "bitwidth":1, "type": "signal", "bundle":{"name": "output_f", "role": "Rst_A" }}  ]}

set RtlHierarchyInfo {[
	{"ID" : "0", "Level" : "0", "Path" : "`AUTOTB_DUT_INST", "Parent" : "", "Child" : ["1", "2", "3"],
		"CDFG" : "gradient",
		"Protocol" : "ap_ctrl_hs",
		"ControlExist" : "1", "ap_start" : "1", "ap_ready" : "1", "ap_done" : "1", "ap_continue" : "0", "ap_idle" : "1",
		"Pipeline" : "None", "UnalignedPipeline" : "0", "RewindPipeline" : "0", "ProcessNetwork" : "0",
		"II" : "0",
		"VariableLatency" : "1", "ExactLatency" : "-1", "EstimateLatencyMin" : "20481", "EstimateLatencyMax" : "20481",
		"Combinational" : "0",
		"Datapath" : "0",
		"ClockEnable" : "0",
		"HasSubDataflow" : "0",
		"InDataflowNetwork" : "0",
		"HasNonBlockingOperation" : "0",
		"Port" : [
			{"Name" : "input_f", "Type" : "Bram", "Direction" : "I"},
			{"Name" : "kernel", "Type" : "Bram", "Direction" : "I"},
			{"Name" : "output_f", "Type" : "Bram", "Direction" : "O"},
			{"Name" : "status", "Type" : "None", "Direction" : "I"}]},
	{"ID" : "1", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.gradient_CRTL_BUS_s_axi_U", "Parent" : "0"},
	{"ID" : "2", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.gradient_mul_mul_8ns_16ns_24_1_1_U1", "Parent" : "0"},
	{"ID" : "3", "Level" : "1", "Path" : "`AUTOTB_DUT_INST.gradient_mul_mul_8ns_24ns_32_1_1_U2", "Parent" : "0"}]}


set ArgLastReadFirstWriteLatency {
	gradient {
		input_f {Type I LastRead 1 FirstWrite -1}
		kernel {Type I LastRead 1 FirstWrite -1}
		output_f {Type O LastRead -1 FirstWrite 5}
		status {Type I LastRead -1 FirstWrite -1}}}

set hasDtUnsupportedChannel 0

set PerformanceInfo {[
	{"Name" : "Latency", "Min" : "20481", "Max" : "20481"}
	, {"Name" : "Interval", "Min" : "20482", "Max" : "20482"}
]}

set PipelineEnableSignalInfo {[
]}

set Spec2ImplPortList { 
	input_f { bram {  { input_f_Addr_A MemPortADDR2 1 32 }  { input_f_EN_A MemPortCE2 1 1 }  { input_f_WEN_A MemPortWE2 1 4 }  { input_f_Din_A MemPortDIN2 1 32 }  { input_f_Dout_A MemPortDOUT2 0 32 }  { input_f_Clk_A mem_clk 1 1 }  { input_f_Rst_A mem_rst 1 1 } } }
	kernel { bram {  { kernel_Addr_A MemPortADDR2 1 32 }  { kernel_EN_A MemPortCE2 1 1 }  { kernel_WEN_A MemPortWE2 1 4 }  { kernel_Din_A MemPortDIN2 1 32 }  { kernel_Dout_A MemPortDOUT2 0 32 }  { kernel_Clk_A mem_clk 1 1 }  { kernel_Rst_A mem_rst 1 1 } } }
	output_f { bram {  { output_f_Addr_A MemPortADDR2 1 32 }  { output_f_EN_A MemPortCE2 1 1 }  { output_f_WEN_A MemPortWE2 1 4 }  { output_f_Din_A MemPortDIN2 1 32 }  { output_f_Dout_A MemPortDOUT2 0 32 }  { output_f_Clk_A mem_clk 1 1 }  { output_f_Rst_A mem_rst 1 1 } } }
}

set busDeadlockParameterList { 
}

# RTL port scheduling information:
set fifoSchedulingInfoList { 
}

# RTL bus port read request latency information:
set busReadReqLatencyList { 
}

# RTL bus port write response latency information:
set busWriteResLatencyList { 
}

# RTL array port load latency information:
set memoryLoadLatencyList { 
}
