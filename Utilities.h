#pragma once
#include "StdAfx.h"

Acad::ErrorStatus createLayer(const ACHAR*, AcDbObjectId& layerId);

Acad::ErrorStatus createBlockRecord(const ACHAR* name);