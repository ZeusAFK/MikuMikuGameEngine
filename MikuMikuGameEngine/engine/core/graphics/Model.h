#pragma once

#include "ModelFrame.h"

#include "../ResourceManager.h"

class Model : public IResource
{
private:
	ModelFrame* m_pModelFrame;

public:
	Model( ModelFrame* pModelFrame );
	virtual ~Model();

public:
	ModelFrame* GetFrame();
};

typedef std::tr1::shared_ptr< Model > ModelPtr;
typedef std::tr1::weak_ptr< Model > ModelWeakPtr;

void SetModelRef( ModelPtr pModel,ModelFrame* pModelFrame );