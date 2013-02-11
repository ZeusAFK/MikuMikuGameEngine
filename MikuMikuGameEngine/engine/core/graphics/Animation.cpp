#include "stdafx.h"

#include "Animation.h"

Animation::Animation()
	: m_ppAnimationBones(NULL)
	, m_pAnimationSkins(NULL)
	, m_animationTime(0.0f)
	, m_pPMDModelRenderer(NULL)
{
}

Animation::~Animation()
{
	if( m_ppAnimationBones )
	{
		delete[] m_ppAnimationBones;
	}

	if( m_pAnimationSkins )
	{
		delete[] m_pAnimationSkins;
	}
}

void Animation::SetAnimationBone()
{
	delete[] m_ppAnimationBones;
	m_ppAnimationBones = NULL;
	m_animationLastFrame = 0;

	delete[] m_pAnimationSkins;
	m_pAnimationSkins = NULL;

	if( m_pPMDModelRenderer && m_pAnimationClip )
	{
		DWORD motionBoneCount = m_pAnimationClip->GetMotionBoneCount();
		m_ppAnimationBones = new PMDBone*[motionBoneCount];
		
		for( DWORD idx=0;idx<motionBoneCount;idx++ )
		{
			const sMotionBone* motionBone = m_pAnimationClip->GetMotionBone( idx );
			m_ppAnimationBones[idx] = m_pPMDModelRenderer->SearchBone( motionBone->boneName );

			if( m_animationLastFrame < motionBone->keyFrames[motionBone->keyFrameNum-1].frameNo )
			{
				m_animationLastFrame = (float)motionBone->keyFrames[motionBone->keyFrameNum-1].frameNo;
			}
		}

		DWORD morphingSkinCount = m_pAnimationClip->GetMorphingSkinCount();
		m_pAnimationSkins = new int[morphingSkinCount];

		for( DWORD idx=0;idx<morphingSkinCount;idx++ )
		{
			const sMorphingSkin* morphingSkin = m_pAnimationClip->GetMorphingSkin( idx );
			m_pAnimationSkins[idx] = m_pPMDModelRenderer->SearchSkin( morphingSkin->skinName );

			if( m_animationLastFrame < morphingSkin->keyFrames[morphingSkin->keyFrameNum-1].frameNo )
			{
				m_animationLastFrame = (float)morphingSkin->keyFrames[morphingSkin->keyFrameNum-1].frameNo;
			}
		}
	}
}

void Animation::SetClip( VMDAnimationClipPtr& animationClip )
{
	m_pAnimationClip = animationClip;
	SetAnimationBone();
}

void Animation::SetPMDModelRenderer( PMDModelRenderer* pPMDModelRenderer )
{
	m_pPMDModelRenderer = pPMDModelRenderer;
	SetAnimationBone();
}

void Animation::SetAssetUUID( const tstring_symbol& assetUUID )
{
	m_assetUUID = assetUUID;
}

const tstring_symbol& Animation::GetAssetUUID() const
{
	return m_assetUUID;
}

void Animation::UpdateAnimation( float elapsedTime )
{
	if( m_pPMDModelRenderer && m_pAnimationClip )
	{
		m_animationTime += elapsedTime;

		float frame = m_animationTime*VMDFPS;

		DWORD motionBoneCount = m_pAnimationClip->GetMotionBoneCount();
		for( DWORD idx=0;idx<motionBoneCount;idx++ )
		{
			PMDBone* pmdBone = m_ppAnimationBones[idx];
			if( !pmdBone )
			{
				continue;
			}

			ePMD_BoneType boneType = pmdBone->GetBoneType();

			if( boneType== ePMD_BoneType_InfluenceIK )
			{
				pmdBone->SetMotionPosition( D3DXVECTOR3(0.0f,0.0f,0.0f) );
				pmdBone->SetMotionRotation( D3DXQUATERNION(0.0f,0.0f,0.0f,1.0f) );
				continue;
			}

			const sMotionBone* motionBone = m_pAnimationClip->GetMotionBone( idx );
			
			sMotionKeyFrame* prevKeyFrame = &motionBone->keyFrames[0];

			if( boneType==ePMD_BoneType_TransRotation || boneType==ePMD_BoneType_IK )
			{
				pmdBone->SetMotionPosition( prevKeyFrame->pos );
			}
			pmdBone->SetMotionRotation( prevKeyFrame->rotate );

			for( DWORD keyIdx=1;keyIdx<motionBone->keyFrameNum;keyIdx++ )
			{
				sMotionKeyFrame* nextKeyFrame = &motionBone->keyFrames[keyIdx];
				if( nextKeyFrame->frameNo > frame )
				{
					float t = (frame-prevKeyFrame->frameNo)/(nextKeyFrame->frameNo-prevKeyFrame->frameNo);

					if( boneType==ePMD_BoneType_TransRotation || boneType==ePMD_BoneType_IK )
					{
						D3DXVECTOR3 localPosition;
						float wx = nextKeyFrame->GetInterpolationWeightX( t );
						float wy = nextKeyFrame->GetInterpolationWeightY( t );
						float wz = nextKeyFrame->GetInterpolationWeightZ( t );

						localPosition.x = prevKeyFrame->pos.x + wx * (nextKeyFrame->pos.x - prevKeyFrame->pos.x);
						localPosition.y = prevKeyFrame->pos.y + wy * (nextKeyFrame->pos.y - prevKeyFrame->pos.y);
						localPosition.z = prevKeyFrame->pos.z + wz * (nextKeyFrame->pos.z - prevKeyFrame->pos.z);

						pmdBone->SetMotionPosition( localPosition );
					}

					float wr = nextKeyFrame->GetInterpolationWeightR( t );

					D3DXQUATERNION localRotation;
					D3DXQuaternionSlerp( &localRotation,&prevKeyFrame->rotate,&nextKeyFrame->rotate,wr );
					D3DXQuaternionNormalize( &localRotation,&localRotation );
					pmdBone->SetMotionRotation( localRotation );

					break;
				}

				if( keyIdx == motionBone->keyFrameNum-1 )
				{
					if( boneType==ePMD_BoneType_TransRotation || boneType==ePMD_BoneType_IK )
					{
						pmdBone->SetMotionPosition( nextKeyFrame->pos );
					}
					pmdBone->SetMotionRotation( nextKeyFrame->rotate );

					break;
				}

				prevKeyFrame = nextKeyFrame;
			}
		}

		m_pPMDModelRenderer->UpdateBone();

		DWORD morphingSkinCount = m_pAnimationClip->GetMorphingSkinCount();
		for( DWORD idx=0;idx<morphingSkinCount;idx++ )
		{
			int skinIndex = m_pAnimationSkins[idx];
			if( skinIndex < 0 )
			{
				continue;
			}

			const sMorphingSkin* morphingSkin = m_pAnimationClip->GetMorphingSkin( idx );
			
			sMorphingKeyFrame* prevKeyFrame = &morphingSkin->keyFrames[0];

			m_pPMDModelRenderer->SetSkinWeight( skinIndex,prevKeyFrame->weight );

			for( DWORD keyIdx=1;keyIdx<morphingSkin->keyFrameNum;keyIdx++ )
			{
				sMorphingKeyFrame* nextKeyFrame = &morphingSkin->keyFrames[keyIdx];
				if( nextKeyFrame->frameNo > frame )
				{
					float t = (frame-prevKeyFrame->frameNo)/(nextKeyFrame->frameNo-prevKeyFrame->frameNo);

					float weight = prevKeyFrame->weight + t * (nextKeyFrame->weight - prevKeyFrame->weight);
					
					m_pPMDModelRenderer->SetSkinWeight( skinIndex,weight );
					
					break;
				}

				if( keyIdx == morphingSkin->keyFrameNum-1 )
				{
					m_pPMDModelRenderer->SetSkinWeight( skinIndex,nextKeyFrame->weight );

					break;
				}

				prevKeyFrame = nextKeyFrame;
			}
		}
	}
}