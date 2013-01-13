#pragma once

#include "PMDModel.h"

class PMDFileLoader
{
private:
	tstring m_fileName;
	tstring m_path;

public:
	PMDFileLoader();
	virtual ~PMDFileLoader();

public:
	PMDModelPtr Open( const tstring& filePath );
};