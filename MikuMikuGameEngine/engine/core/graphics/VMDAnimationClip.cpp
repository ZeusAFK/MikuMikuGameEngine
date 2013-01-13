#include "stdafx.h"

#include "VMDAnimationClip.h"

VMDAnimationClip::VMDAnimationClip( DWORD motionBoneCount,sMotionBone* motionBones,DWORD morphingSkinCount,sMorphingSkin* morphingSkins )
	: m_motionBoneCount(motionBoneCount)
	, m_motionBones(motionBones)
	, m_morphingSkinCount(morphingSkinCount)
	, m_morphingSkins(morphingSkins)
{
}

VMDAnimationClip::~VMDAnimationClip()
{
	if( m_motionBones )
	{
		delete[] m_motionBones;
	}

	if( m_morphingSkins )
	{
		delete[] m_morphingSkins;
	}
}

DWORD VMDAnimationClip::GetMotionBoneCount()
{
	return m_motionBoneCount;
}

const sMotionBone* VMDAnimationClip::GetMotionBone( DWORD index )
{
	if( m_motionBoneCount <= index )
	{
		return NULL;
	}

	return &m_motionBones[index];
}

DWORD VMDAnimationClip::GetMorphingSkinCount()
{
	return m_morphingSkinCount;
}

const sMorphingSkin* VMDAnimationClip::GetMorphingSkin( DWORD index )
{
	if( m_morphingSkinCount <= index )
	{
		return NULL;
	}

	return &m_morphingSkins[index];
}