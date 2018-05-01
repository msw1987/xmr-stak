#pragma once

int initializeADL();
void deinitializeADL();
int adl_adapterInfoInit();
int adl_getOvedriveNidx(int bus, int device, int function);
int adl_getTemperature(int adapter_index);
int adl_getFanInfo(int adapter_index, int *currentFanSpeed, int *targetFanSpeed);
int adl_getPerformanceStatus(int adapter_index, int *coreClock, int *memClock);