#pragma once

#include "PMDModelRenderer.h"
#include "VMDAnimationClip.h"

class Animation
{
private:
	tstring_symbol m_assetUUID;

	VMDAnimationClipPtr m_pAnimationClip;
	PMDModelRenderer* m_pPMDModelRenderer;

	PMDBone** m_ppAnimationBones;
	int* m_pAnimationSkins;
	float m_animationTime;
	float m_animationLastFrame;

public:
	Animation();
	virtual ~Animation();

protected:
	void SetAnimationBone();

public:
	void SetClip( VMDAnimationClipPtr& animationClip );
	void SetPMDModelRenderer( PMDModelRenderer* pPMDModelRenderer );

	void SetAssetUUID( const tstring_symbol& assetUUID );
	const tstring_symbol& GetAssetUUID() const;

	void UpdateAnimation( float elapsedTime );
};
