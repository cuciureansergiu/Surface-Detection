# This script segment is generated automatically by AutoPilot

set axilite_register_dict [dict create]
set port_CRTL_BUS {
ap_start { }
ap_done { }
ap_ready { }
ap_idle { }
status { 
	dir I
	width 32
	depth 1
	mode ap_none
	offset 16
	offset_end 23
}
}
dict set axilite_register_dict CRTL_BUS $port_CRTL_BUS


