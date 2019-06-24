// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.1 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xgradient.h"

extern XGradient_Config XGradient_ConfigTable[];

XGradient_Config *XGradient_LookupConfig(u16 DeviceId) {
	XGradient_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XGRADIENT_NUM_INSTANCES; Index++) {
		if (XGradient_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XGradient_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XGradient_Initialize(XGradient *InstancePtr, u16 DeviceId) {
	XGradient_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XGradient_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XGradient_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

