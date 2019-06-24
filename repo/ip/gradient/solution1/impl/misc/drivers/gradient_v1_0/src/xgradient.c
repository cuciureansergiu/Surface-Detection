// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.1 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xgradient.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XGradient_CfgInitialize(XGradient *InstancePtr, XGradient_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Crtl_bus_BaseAddress = ConfigPtr->Crtl_bus_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XGradient_Start(XGradient *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGradient_ReadReg(InstancePtr->Crtl_bus_BaseAddress, XGRADIENT_CRTL_BUS_ADDR_AP_CTRL) & 0x80;
    XGradient_WriteReg(InstancePtr->Crtl_bus_BaseAddress, XGRADIENT_CRTL_BUS_ADDR_AP_CTRL, Data | 0x01);
}

u32 XGradient_IsDone(XGradient *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGradient_ReadReg(InstancePtr->Crtl_bus_BaseAddress, XGRADIENT_CRTL_BUS_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XGradient_IsIdle(XGradient *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGradient_ReadReg(InstancePtr->Crtl_bus_BaseAddress, XGRADIENT_CRTL_BUS_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XGradient_IsReady(XGradient *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGradient_ReadReg(InstancePtr->Crtl_bus_BaseAddress, XGRADIENT_CRTL_BUS_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XGradient_EnableAutoRestart(XGradient *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGradient_WriteReg(InstancePtr->Crtl_bus_BaseAddress, XGRADIENT_CRTL_BUS_ADDR_AP_CTRL, 0x80);
}

void XGradient_DisableAutoRestart(XGradient *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGradient_WriteReg(InstancePtr->Crtl_bus_BaseAddress, XGRADIENT_CRTL_BUS_ADDR_AP_CTRL, 0);
}

void XGradient_Set_status(XGradient *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGradient_WriteReg(InstancePtr->Crtl_bus_BaseAddress, XGRADIENT_CRTL_BUS_ADDR_STATUS_DATA, Data);
}

u32 XGradient_Get_status(XGradient *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGradient_ReadReg(InstancePtr->Crtl_bus_BaseAddress, XGRADIENT_CRTL_BUS_ADDR_STATUS_DATA);
    return Data;
}

void XGradient_InterruptGlobalEnable(XGradient *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGradient_WriteReg(InstancePtr->Crtl_bus_BaseAddress, XGRADIENT_CRTL_BUS_ADDR_GIE, 1);
}

void XGradient_InterruptGlobalDisable(XGradient *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGradient_WriteReg(InstancePtr->Crtl_bus_BaseAddress, XGRADIENT_CRTL_BUS_ADDR_GIE, 0);
}

void XGradient_InterruptEnable(XGradient *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XGradient_ReadReg(InstancePtr->Crtl_bus_BaseAddress, XGRADIENT_CRTL_BUS_ADDR_IER);
    XGradient_WriteReg(InstancePtr->Crtl_bus_BaseAddress, XGRADIENT_CRTL_BUS_ADDR_IER, Register | Mask);
}

void XGradient_InterruptDisable(XGradient *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XGradient_ReadReg(InstancePtr->Crtl_bus_BaseAddress, XGRADIENT_CRTL_BUS_ADDR_IER);
    XGradient_WriteReg(InstancePtr->Crtl_bus_BaseAddress, XGRADIENT_CRTL_BUS_ADDR_IER, Register & (~Mask));
}

void XGradient_InterruptClear(XGradient *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGradient_WriteReg(InstancePtr->Crtl_bus_BaseAddress, XGRADIENT_CRTL_BUS_ADDR_ISR, Mask);
}

u32 XGradient_InterruptGetEnabled(XGradient *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XGradient_ReadReg(InstancePtr->Crtl_bus_BaseAddress, XGRADIENT_CRTL_BUS_ADDR_IER);
}

u32 XGradient_InterruptGetStatus(XGradient *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XGradient_ReadReg(InstancePtr->Crtl_bus_BaseAddress, XGRADIENT_CRTL_BUS_ADDR_ISR);
}

