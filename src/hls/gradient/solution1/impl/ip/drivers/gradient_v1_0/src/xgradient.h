// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.1 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XGRADIENT_H
#define XGRADIENT_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xgradient_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
#else
typedef struct {
    u16 DeviceId;
    u32 Crtl_bus_BaseAddress;
} XGradient_Config;
#endif

typedef struct {
    u32 Crtl_bus_BaseAddress;
    u32 IsReady;
} XGradient;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XGradient_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XGradient_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XGradient_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XGradient_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
int XGradient_Initialize(XGradient *InstancePtr, u16 DeviceId);
XGradient_Config* XGradient_LookupConfig(u16 DeviceId);
int XGradient_CfgInitialize(XGradient *InstancePtr, XGradient_Config *ConfigPtr);
#else
int XGradient_Initialize(XGradient *InstancePtr, const char* InstanceName);
int XGradient_Release(XGradient *InstancePtr);
#endif

void XGradient_Start(XGradient *InstancePtr);
u32 XGradient_IsDone(XGradient *InstancePtr);
u32 XGradient_IsIdle(XGradient *InstancePtr);
u32 XGradient_IsReady(XGradient *InstancePtr);
void XGradient_EnableAutoRestart(XGradient *InstancePtr);
void XGradient_DisableAutoRestart(XGradient *InstancePtr);

void XGradient_Set_status(XGradient *InstancePtr, u32 Data);
u32 XGradient_Get_status(XGradient *InstancePtr);

void XGradient_InterruptGlobalEnable(XGradient *InstancePtr);
void XGradient_InterruptGlobalDisable(XGradient *InstancePtr);
void XGradient_InterruptEnable(XGradient *InstancePtr, u32 Mask);
void XGradient_InterruptDisable(XGradient *InstancePtr, u32 Mask);
void XGradient_InterruptClear(XGradient *InstancePtr, u32 Mask);
u32 XGradient_InterruptGetEnabled(XGradient *InstancePtr);
u32 XGradient_InterruptGetStatus(XGradient *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
