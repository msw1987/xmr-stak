#include <stdio.h>
#include <tchar.h>

///
///	 Based on AMD ADL sample code OverdriveN.cpp
///  Copyright (c) 2018 Mariusz Zoolek 
///  Copyright (c) 2008 - 2016 Advanced Micro Devices, Inc.
 
///  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
///  EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
///  WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.

/// \file Source.cpp

#include <windows.h>
#include "adl\adl_sdk.h"
#include "adl\adl_structures.h"

// Definitions of the used function pointers. Add more if you use other ADL APIs
typedef int ( *ADL_MAIN_CONTROL_CREATE )(ADL_MAIN_MALLOC_CALLBACK, int );
typedef int ( *ADL_MAIN_CONTROL_DESTROY )();
typedef int ( *ADL_FLUSH_DRIVER_DATA)(int);
typedef int (*ADL2_ADAPTER_ACTIVE_GET ) (ADL_CONTEXT_HANDLE, int, int* );

typedef int ( *ADL_ADAPTER_NUMBEROFADAPTERS_GET ) ( int* );
typedef int ( *ADL_ADAPTER_ADAPTERINFO_GET ) ( LPAdapterInfo, int );
typedef int ( *ADL_ADAPTERX2_CAPS) (int, int*);
typedef int ( *ADL2_OVERDRIVE_CAPS) (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int * iSupported, int * iEnabled, int * iVersion);
typedef int ( *ADL2_OVERDRIVEN_CAPABILITIES_GET)	(ADL_CONTEXT_HANDLE, int, ADLODNCapabilities*);
typedef int ( *ADL2_OVERDRIVEN_SYSTEMCLOCKS_GET)	(ADL_CONTEXT_HANDLE, int, ADLODNPerformanceLevels*);
typedef int ( *ADL2_OVERDRIVEN_SYSTEMCLOCKS_SET)	(ADL_CONTEXT_HANDLE, int, ADLODNPerformanceLevels*);
typedef int ( *ADL2_OVERDRIVEN_MEMORYCLOCKS_GET)	(ADL_CONTEXT_HANDLE, int, ADLODNPerformanceLevels*);
typedef int ( *ADL2_OVERDRIVEN_MEMORYCLOCKS_SET)	(ADL_CONTEXT_HANDLE, int, ADLODNPerformanceLevels*);
typedef int ( *ADL2_OVERDRIVEN_PERFORMANCESTATUS_GET) (ADL_CONTEXT_HANDLE, int, ADLODNPerformanceStatus*);
typedef int ( *ADL2_OVERDRIVEN_FANCONTROL_GET) (ADL_CONTEXT_HANDLE, int, ADLODNFanControl*);
typedef int ( *ADL2_OVERDRIVEN_FANCONTROL_SET) (ADL_CONTEXT_HANDLE, int, ADLODNFanControl*);
typedef int ( *ADL2_OVERDRIVEN_POWERLIMIT_GET) (ADL_CONTEXT_HANDLE, int, ADLODNPowerLimitSetting*);
typedef int ( *ADL2_OVERDRIVEN_POWERLIMIT_SET) (ADL_CONTEXT_HANDLE, int, ADLODNPowerLimitSetting*);
typedef int ( *ADL2_OVERDRIVEN_TEMPERATURE_GET) (ADL_CONTEXT_HANDLE, int, int, int*);
HINSTANCE hDLL;

ADL_MAIN_CONTROL_CREATE          ADL_Main_Control_Create = NULL;
ADL_MAIN_CONTROL_DESTROY         ADL_Main_Control_Destroy = NULL;
ADL_ADAPTER_NUMBEROFADAPTERS_GET ADL_Adapter_NumberOfAdapters_Get = NULL;
ADL_ADAPTER_ADAPTERINFO_GET      ADL_Adapter_AdapterInfo_Get = NULL;
ADL_ADAPTERX2_CAPS ADL_AdapterX2_Caps = NULL;
ADL2_ADAPTER_ACTIVE_GET				ADL2_Adapter_Active_Get=NULL;
ADL2_OVERDRIVEN_CAPABILITIES_GET ADL2_OverdriveN_Capabilities_Get = NULL;
ADL2_OVERDRIVEN_SYSTEMCLOCKS_GET ADL2_OverdriveN_SystemClocks_Get = NULL;
ADL2_OVERDRIVEN_SYSTEMCLOCKS_SET ADL2_OverdriveN_SystemClocks_Set = NULL;
ADL2_OVERDRIVEN_PERFORMANCESTATUS_GET ADL2_OverdriveN_PerformanceStatus_Get = NULL;
ADL2_OVERDRIVEN_FANCONTROL_GET ADL2_OverdriveN_FanControl_Get =NULL;
ADL2_OVERDRIVEN_FANCONTROL_SET ADL2_OverdriveN_FanControl_Set=NULL;
ADL2_OVERDRIVEN_POWERLIMIT_GET ADL2_OverdriveN_PowerLimit_Get =NULL;
ADL2_OVERDRIVEN_POWERLIMIT_SET ADL2_OverdriveN_PowerLimit_Set=NULL;
ADL2_OVERDRIVEN_MEMORYCLOCKS_GET ADL2_OverdriveN_MemoryClocks_Get = NULL;
ADL2_OVERDRIVEN_MEMORYCLOCKS_GET ADL2_OverdriveN_MemoryClocks_Set = NULL;
ADL2_OVERDRIVE_CAPS ADL2_Overdrive_Caps = NULL;
ADL2_OVERDRIVEN_TEMPERATURE_GET ADL2_OverdriveN_Temperature_Get = NULL;
// Memory allocation function
void* __stdcall ADL_Main_Memory_Alloc ( int iSize )
{
    void* lpBuffer = malloc ( iSize );
    return lpBuffer;
}

// Optional Memory de-allocation function
void __stdcall ADL_Main_Memory_Free ( void** lpBuffer )
{
    if ( NULL != *lpBuffer )
    {
        free ( *lpBuffer );
        *lpBuffer = NULL;
    }
}

ADL_CONTEXT_HANDLE context = NULL;

LPAdapterInfo   lpAdapterInfo = NULL;
int  iNumberAdapters;


int initializeADL()
{
	hDLL = LoadLibrary(TEXT("atiadlxx.dll"));
	if (hDLL == NULL)
	{
		// A 32 bit calling application on 64 bit OS will fail to LoadLibrary.
		// Try to load the 32 bit library (atiadlxy.dll) instead
		hDLL = LoadLibrary(TEXT("atiadlxy.dll"));
	}
	
	if (NULL == hDLL)
	{
		return FALSE;
	}
	
	ADL_Main_Control_Create = (ADL_MAIN_CONTROL_CREATE) GetProcAddress(hDLL,"ADL_Main_Control_Create");
	ADL_Main_Control_Destroy = (ADL_MAIN_CONTROL_DESTROY) GetProcAddress(hDLL,"ADL_Main_Control_Destroy");
	ADL_Adapter_NumberOfAdapters_Get = (ADL_ADAPTER_NUMBEROFADAPTERS_GET) GetProcAddress(hDLL,"ADL_Adapter_NumberOfAdapters_Get");
	ADL_Adapter_AdapterInfo_Get = (ADL_ADAPTER_ADAPTERINFO_GET) GetProcAddress(hDLL,"ADL_Adapter_AdapterInfo_Get");
	ADL_AdapterX2_Caps = (ADL_ADAPTERX2_CAPS) GetProcAddress( hDLL, "ADL_AdapterX2_Caps");
	ADL2_Adapter_Active_Get = (ADL2_ADAPTER_ACTIVE_GET)GetProcAddress(hDLL, "ADL2_Adapter_Active_Get");
	ADL2_OverdriveN_Capabilities_Get = (ADL2_OVERDRIVEN_CAPABILITIES_GET) GetProcAddress (hDLL, "ADL2_OverdriveN_Capabilities_Get");
	ADL2_OverdriveN_SystemClocks_Get = (ADL2_OVERDRIVEN_SYSTEMCLOCKS_GET) GetProcAddress (hDLL, "ADL2_OverdriveN_SystemClocks_Get");
	ADL2_OverdriveN_SystemClocks_Set = (ADL2_OVERDRIVEN_SYSTEMCLOCKS_SET) GetProcAddress (hDLL, "ADL2_OverdriveN_SystemClocks_Set");
	ADL2_OverdriveN_MemoryClocks_Get = (ADL2_OVERDRIVEN_MEMORYCLOCKS_GET) GetProcAddress (hDLL, "ADL2_OverdriveN_MemoryClocks_Get");
	ADL2_OverdriveN_MemoryClocks_Set = (ADL2_OVERDRIVEN_MEMORYCLOCKS_SET) GetProcAddress (hDLL, "ADL2_OverdriveN_MemoryClocks_Set");
	ADL2_OverdriveN_PerformanceStatus_Get = (ADL2_OVERDRIVEN_PERFORMANCESTATUS_GET) GetProcAddress (hDLL,"ADL2_OverdriveN_PerformanceStatus_Get");
	ADL2_OverdriveN_FanControl_Get = (ADL2_OVERDRIVEN_FANCONTROL_GET) GetProcAddress (hDLL, "ADL2_OverdriveN_FanControl_Get");
	ADL2_OverdriveN_FanControl_Set = (ADL2_OVERDRIVEN_FANCONTROL_SET) GetProcAddress (hDLL, "ADL2_OverdriveN_FanControl_Set");
	ADL2_OverdriveN_PowerLimit_Get = (ADL2_OVERDRIVEN_POWERLIMIT_GET) GetProcAddress (hDLL, "ADL2_OverdriveN_PowerLimit_Get");
	ADL2_OverdriveN_PowerLimit_Set = (ADL2_OVERDRIVEN_POWERLIMIT_SET) GetProcAddress (hDLL, "ADL2_OverdriveN_PowerLimit_Set");
	ADL2_OverdriveN_Temperature_Get = (ADL2_OVERDRIVEN_TEMPERATURE_GET) GetProcAddress (hDLL, "ADL2_OverdriveN_Temperature_Get");
	ADL2_Overdrive_Caps = (ADL2_OVERDRIVE_CAPS) GetProcAddress (hDLL, "ADL2_Overdrive_Caps");
	if ( NULL == ADL_Main_Control_Create ||
		 NULL == ADL_Main_Control_Destroy ||
		 NULL == ADL_Adapter_NumberOfAdapters_Get||
		 NULL == ADL_Adapter_AdapterInfo_Get ||
		 NULL == ADL_AdapterX2_Caps ||
		NULL == ADL2_Adapter_Active_Get ||
		NULL == ADL2_OverdriveN_Capabilities_Get || 
		NULL == ADL2_OverdriveN_SystemClocks_Get ||
		NULL == ADL2_OverdriveN_SystemClocks_Set ||
		NULL == ADL2_OverdriveN_MemoryClocks_Get ||
		NULL == ADL2_OverdriveN_MemoryClocks_Set ||
		NULL == ADL2_OverdriveN_PerformanceStatus_Get ||
		NULL == ADL2_OverdriveN_FanControl_Get ||
		NULL == ADL2_OverdriveN_FanControl_Set ||
		NULL == ADL2_Overdrive_Caps
		)
	{
		return FALSE;
	}

	if (ADL_OK != ADL_Main_Control_Create(ADL_Main_Memory_Alloc, 1))
	{
		return ADL_ERR;
	}
	
	return TRUE;
}

void deinitializeADL()
{
	ADL_Main_Control_Destroy();
	
	FreeLibrary(hDLL);
}

int adl_adapterInfoInit()
{ 	
	if ( ADL_OK != ADL_Adapter_NumberOfAdapters_Get ( &iNumberAdapters ) )
	{
		   return FALSE;
	}
	
	if ( 0 < iNumberAdapters )
    {
        lpAdapterInfo = (LPAdapterInfo)malloc ( sizeof (AdapterInfo) * iNumberAdapters );
        memset ( lpAdapterInfo,'\0', sizeof (AdapterInfo) * iNumberAdapters );

        ADL_Adapter_AdapterInfo_Get (lpAdapterInfo, sizeof (AdapterInfo) * iNumberAdapters);
    }
	
	return TRUE;
}

int adl_getOvedriveNidx(int bus, int device, int function)
{
	int i;
	int iSupported,iEnabled,iVersion;
	
	for ( i = 0; i < iNumberAdapters; i++ )
    {
		if (lpAdapterInfo[ i ].iBusNumber > -1)
		{
			ADL2_Overdrive_Caps(context,lpAdapterInfo[ i ].iAdapterIndex, &iSupported, &iEnabled, &iVersion);
			
			if (iVersion == 7)
			{
				//PRINTF("bus %d device %d function %d\n", lpAdapterInfo[i].iBusNumber, lpAdapterInfo[i].iDeviceNumber, lpAdapterInfo[i].iFunctionNumber);
				if (bus == lpAdapterInfo[i].iBusNumber && device == lpAdapterInfo[i].iDeviceNumber && function == lpAdapterInfo[i].iFunctionNumber)
					return lpAdapterInfo[i].iAdapterIndex;
			}
		}
	}
	
	return -1;
}

int adl_getTemperature(int adapter_index)
{
	initializeADL();
	int temp;
	if (ADL_OK == ADL2_OverdriveN_Temperature_Get(context, adapter_index, 1, &temp))
	{
		deinitializeADL();
		return temp;
	}
	deinitializeADL();

	return -1;
}

int adl_getFanInfo(int adapter_index, int *currentFanSpeed, int *targetFanSpeed)
{
	ADLODNFanControl odNFanControl;
	memset(&odNFanControl, 0, sizeof(ADLODNFanControl));
	
	initializeADL();
	
	if (ADL_OK == ADL2_OverdriveN_FanControl_Get(context, adapter_index, &odNFanControl))
	{
		*currentFanSpeed = odNFanControl.iCurrentFanSpeed;
		*targetFanSpeed = odNFanControl.iTargetFanSpeed;
		deinitializeADL();
		return 0;
	}
	deinitializeADL();

	return -1;
}

int adl_getPerformanceStatus(int adapter_index, int *coreClock, int *memClock)
{
	ADLODNPerformanceStatus odNPerformanceStatus;
	memset(&odNPerformanceStatus, 0, sizeof(ADLODNPerformanceStatus));

	initializeADL();

	if (ADL_OK == ADL2_OverdriveN_PerformanceStatus_Get(context, adapter_index, &odNPerformanceStatus))
	{
		*coreClock = odNPerformanceStatus.iCoreClock;
		*memClock = odNPerformanceStatus.iMemoryClock;
		deinitializeADL();
		return 0;
	}
	deinitializeADL();

	return -1;
}