#pragma once

#include "VMDAnimationClip.h"

class VMDFileLoader
{
public:
	VMDAnimationClipPtr Open( const tstring& filePath );
};