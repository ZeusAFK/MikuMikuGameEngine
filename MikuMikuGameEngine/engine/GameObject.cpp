#include "stdafx.h"

#include "GameObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GameObject::GameObject()
	: m_pModelRenderer(NULL)
	, m_pPMDModelRenderer(NULL)
	, m_name(_T("GameObject"))
	, m_ppAnimationBones(NULL)
	, m_pAnimationSkins(NULL)
	, m_animationTime(0.0f)
{
	m_localPosition = D3DXVECTOR3(0.0f,0.0f,0.0f);
	D3DXQuaternionIdentity( &m_localRotation );
	m_localScale = D3DXVECTOR3(1.0f,1.0f,1.0f);

	D3DXMATRIX matParent;
	D3DXMatrixIdentity( &matParent );

	m_changedLocal = true;
	UpdateTransform( matParent );
}

GameObject::~GameObject()
{
	if( m_pModelRenderer )
	{
		delete m_pModelRenderer;
	}
	if( m_pPMDModelRenderer )
	{
		delete m_pPMDModelRenderer;
	}

	if( m_ppAnimationBones )
	{
		delete[] m_ppAnimationBones;
	}

	if( m_pAnimationSkins )
	{
		delete[] m_pAnimationSkins;
	}
}

void GameObject::SetName( const tstring& name )
{
	m_name = name;
}

const tstring& GameObject::GetName()
{
	return m_name;
}

void GameObject::SetModelRenderer( ModelRenderer* pModelRenderer )
{
	m_pModelRenderer = pModelRenderer;
}

ModelRenderer* GameObject::GetModelRenderer()
{
	return m_pModelRenderer;
}

void GameObject::SetPMDModelRenderer( PMDModelRenderer* pPMDModelRenderer )
{
	m_pPMDModelRenderer = pPMDModelRenderer;
}

PMDModelRenderer* GameObject::GetPMDModelRenderer()
{
	return m_pPMDModelRenderer;
}

void GameObject::SetVMDAnimationClip( VMDAnimationClipPtr& animationClip )
{
	m_pAnimationClip = animationClip;

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

D3DXMATRIX GameObject::GetWorldMatrix()
{
	return m_matWorld;
}

void GameObject::SetLocalPosition( const D3DXVECTOR3& position )
{
	m_localPosition  = position;

	D3DXMATRIX matParent;
	D3DXMatrixIdentity( &matParent );

	GameObject* parent = GetParent();
	if( parent )
	{
		matParent = parent->m_matWorld;
	}

	m_changedLocal = true;
	UpdateTransform( matParent );
}

void GameObject::SetLocalRotation( const D3DXQUATERNION& rotation )
{
	m_localRotation = rotation;

	D3DXMATRIX matParent;
	D3DXMatrixIdentity( &matParent );

	GameObject* parent = GetParent();
	if( parent )
	{
		matParent = parent->m_matWorld;
	}

	m_changedLocal = true;
	UpdateTransform( matParent );
}

void GameObject::SetLocalScale( const D3DXVECTOR3& scale )
{
	m_localScale = scale;

	D3DXMATRIX matParent;
	D3DXMatrixIdentity( &matParent );

	GameObject* parent = GetParent();
	if( parent )
	{
		matParent = parent->m_matWorld;
	}

	m_changedLocal = true;
	UpdateTransform( matParent );
}

void GameObject::ChangeParent( GameObject* nextParent )
{
	D3DXMATRIX matParent;
	D3DXMatrixIdentity( &matParent );

	GameObject* parent = GetParent();
	if( parent )
	{
		matParent = parent->m_matWorld;
	}

	UpdateTransform( matParent );

	m_matLocal = m_matWorld;

	D3DXMatrixIdentity( &matParent );
	if( nextParent )
	{
		matParent = nextParent->m_matWorld;
	}

	D3DXMATRIX matInv;
	D3DXMatrixInverse( &matInv,NULL,&matParent );
	D3DXMatrixMultiply( &m_matLocal,&matInv,&m_matLocal );

	D3DXMatrixDecompose( &m_localScale,&m_localRotation,&m_localPosition,&m_matLocal );

	m_changedLocal = true;
	UpdateTransform( matParent );
}

void GameObject::UpdateTransform( const D3DXMATRIX& matParent )
{
	if( m_changedLocal )
	{
		D3DXMATRIX matTrans;
		D3DXMatrixTranslation( &matTrans,m_localPosition.x,m_localPosition.y,m_localPosition.z );

		D3DXMATRIX matRotation;
		D3DXMatrixRotationQuaternion( &matRotation,&m_localRotation );

		D3DXMATRIX matScale;
		D3DXMatrixScaling( &matScale,m_localScale.x,m_localScale.y,m_localScale.z );

		D3DXMatrixMultiply( &m_matLocal,&matScale,&matRotation );
		D3DXMatrixMultiply( &m_matLocal,&m_matLocal,&matTrans );

		m_changedLocal = false; 
	}

	D3DXMatrixMultiply( &m_matWorld,&m_matLocal,&matParent );

	GameObject* child = GetChild();
	while( child )
	{
		child->UpdateTransform( m_matWorld );

		child = child->GetSiblingNext();
	}
}

void GameObject::UpdateAnimation( float elapsedTime )
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

	GameObject* child = GetChild();
	while( child )
	{
		child->UpdateAnimation( elapsedTime );

		child = child->GetSiblingNext();
	}
}

void GameObject::UpdateMesh( const sRenderInfo& renderInfo )
{
	//if( m_pModelRenderer )
	//{
	//	m_pModelRenderer->UpdateMesh( m_matWorld,renderInfo );
	//}

	if( m_pPMDModelRenderer )
	{
		m_pPMDModelRenderer->UpdateMesh( m_matWorld,renderInfo );
	}

	GameObject* child = GetChild();
	while( child )
	{
		child->UpdateMesh( renderInfo );

		child = child->GetSiblingNext();
	}
}

void GameObject::RenderZPlot( const sRenderInfo& renderInfo )
{
	if( m_pModelRenderer )
	{
		m_pModelRenderer->RenderZPlot( m_matWorld,renderInfo );
	}

	if( m_pPMDModelRenderer )
	{
		m_pPMDModelRenderer->RenderZPlot( m_matWorld,renderInfo );
	}

	GameObject* child = GetChild();
	while( child )
	{
		child->RenderZPlot( renderInfo );

		child = child->GetSiblingNext();
	}
}

void GameObject::Render( const sRenderInfo& renderInfo )
{
	if( m_pModelRenderer )
	{
		m_pModelRenderer->Render( m_matWorld,renderInfo );
	}

	if( m_pPMDModelRenderer )
	{
		m_pPMDModelRenderer->Render( m_matWorld,renderInfo );
	}

	GameObject* child = GetChild();
	while( child )
	{
		child->Render( renderInfo );

		child = child->GetSiblingNext();
	}
}

void GameObject::RenderNonShader()
{
	if( m_pModelRenderer )
	{
		m_pModelRenderer->RenderNonShader();
	}

	if( m_pPMDModelRenderer )
	{
		m_pPMDModelRenderer->RenderNonShader();
	}
}