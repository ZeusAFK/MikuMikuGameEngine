#pragma once

#include "../ResourceManager.h"

struct sMotionKeyFrame
{
	DWORD frameNo;
	D3DXVECTOR3 pos;
	D3DXQUATERNION rotate;
	D3DXVECTOR2 interpolationX1;
	D3DXVECTOR2 interpolationX2;
	D3DXVECTOR2 interpolationY1;
	D3DXVECTOR2 interpolationY2;
	D3DXVECTOR2 interpolationZ1;
	D3DXVECTOR2 interpolationZ2;
	D3DXVECTOR2 interpolationR1;
	D3DXVECTOR2 interpolationR2;

	static float GetInterpolationWeight(const D3DXVECTOR2& p1, const D3DXVECTOR2& p2,float x)
	{
		float t = 0.5f, s = 0.5f;
		for(int i = 0; i < 15; i++)
		{
			float ft = (3.0f * s * s * t * p1.x) + (3.0f * s * t * t * p2.x) + (t * t * t) - x;
			if( fabs(ft) < 1e-6)
			{
				break;
			}
			if( ft > 0 )
			{
				t -= 1.0f / (float)(4 << i);
			}
			else
			{
				t += 1.0f / (float)(4 << i);
			}
			s = 1.0f - t;
		}
		return (3.0f * s * s * t * p1.y) + (3.0f * s * t * t * p2.y) + (t * t * t);
	}

	void SetInterpolation( char Interpolation[4][16] )
	{
		interpolationX1 = D3DXVECTOR2( Interpolation[0][0]/127.0f, Interpolation[0][4]/127.0f );
		interpolationX2 = D3DXVECTOR2( Interpolation[0][8]/127.0f, Interpolation[0][12]/127.0f );

		interpolationY1 = D3DXVECTOR2( Interpolation[0][1]/127.0f, Interpolation[0][5]/127.0f );
		interpolationY2 = D3DXVECTOR2( Interpolation[0][9]/127.0f, Interpolation[0][13]/127.0f );

		interpolationZ1 = D3DXVECTOR2( Interpolation[0][2]/127.0f, Interpolation[0][6]/127.0f );
		interpolationZ2 = D3DXVECTOR2( Interpolation[0][10]/127.0f, Interpolation[0][14]/127.0f );

		interpolationR1 = D3DXVECTOR2( Interpolation[0][3]/127.0f, Interpolation[0][7]/127.0f );
		interpolationR2 = D3DXVECTOR2( Interpolation[0][11]/127.0f, Interpolation[0][15]/127.0f );
	}

	float GetInterpolationWeightX( float t )
	{
		return GetInterpolationWeight( interpolationX1, interpolationX2,t );
	}
	float GetInterpolationWeightY( float t )
	{
		return GetInterpolationWeight( interpolationY1, interpolationY2,t );
	}
	float GetInterpolationWeightZ( float t )
	{
		return GetInterpolationWeight( interpolationZ1, interpolationZ2,t );
	}
	float GetInterpolationWeightR( float t )
	{
		return GetInterpolationWeight( interpolationR1, interpolationR2,t );
	}
};

struct sMotionBone
{
	tstring_symbol boneName;
	DWORD keyFrameNum;
	sMotionKeyFrame* keyFrames;
	
	sMotionBone()
	{
		keyFrameNum = 0;
		keyFrames = NULL;
	}

	~sMotionBone()
	{
		if( keyFrames )
		{
			delete[] keyFrames;
		}
	}
};

struct sMorphingKeyFrame
{
	DWORD frameNo;
	float weight;
};

struct sMorphingSkin
{
	tstring_symbol skinName;
	DWORD keyFrameNum;
	sMorphingKeyFrame* keyFrames;

	sMorphingSkin()
	{
		keyFrameNum = 0;
		keyFrames = NULL;
	}

	~sMorphingSkin()
	{
		if( keyFrames )
		{
			delete[] keyFrames;
		}
	}
};

class VMDAnimationClip : public IResource
{
private:
	DWORD m_motionBoneCount;
	sMotionBone* m_motionBones;

	DWORD m_morphingSkinCount;
	sMorphingSkin* m_morphingSkins;

public:
	VMDAnimationClip( DWORD motionBoneCount,sMotionBone* motionBones,DWORD morphingSkinCount,sMorphingSkin* morphingSkins );
	virtual ~VMDAnimationClip();

public:
	DWORD GetMotionBoneCount();
	const sMotionBone* GetMotionBone( DWORD index );

	DWORD GetMorphingSkinCount();
	const sMorphingSkin* GetMorphingSkin( DWORD index );
};

typedef std::tr1::shared_ptr< VMDAnimationClip > VMDAnimationClipPtr;
typedef std::tr1::weak_ptr< VMDAnimationClip > VMDAnimationClipWeakPtr;

#define VMDFPS 30.0f