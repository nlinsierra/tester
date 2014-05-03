#ifndef LIMIT_SETTINGS_H
#define LIMIT_SETTINGS_H

#include "Global.h"

bool SetLimits(HANDLE job, int MaxTime, int MaxMem, int CurrentThreadIdx);

#endif