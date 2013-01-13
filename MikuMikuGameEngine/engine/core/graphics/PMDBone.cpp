#include "stdafx.h"

#include "PMDBone.h"
#include "../util/util.h"

PMDBone::PMDBone()
	: m_name(_T("Bone"))
{
	m_localPosition = D3DXVECTOR3(0.0f,0.0f,0.0f);
	D3DXQuaternionIdentity( &m_localRotation );

	m_motionPosition = D3DXVECTOR3(0.0f,0.0f,0.0f);
	D3DXQuaternionIdentity( &m_motionRotation );

	D3DXQuaternionIdentity( &m_ikRotation );

	D3DXMATRIX matParent;
	D3DXMatrixIdentity( &matParent );

	m_changedLocal = true;
	m_changedMotion = true;
	m_changedIK = true;
	UpdateTransform( matParent,false );

	m_influenceBoneIndex = -1;
	m_influenceWeight = 0.0f;
}

PMDBone::~PMDBone()
{
}

void PMDBone::SetName( const tstring_symbol& name )
{
	m_name = name;
}

const tstring_symbol& PMDBone::GetName()
{
	return m_name;
}

void PMDBone::SetBoneType( ePMD_BoneType boneType )
{
	m_boneType = boneType;
}

ePMD_BoneType PMDBone::GetBoneType()
{
	return m_boneType;
}

void PMDBone::SetInfluenceBoneIndex( int boneIndex )
{
	m_influenceBoneIndex = boneIndex;
}

int PMDBone::GetInfluenceBoneIndex()
{
	return m_influenceBoneIndex;
}
void PMDBone::SetInfluenceWeight( float weight )
{
	m_influenceWeight = weight;
}

float PMDBone::GetInfluenceWeight()
{
	return m_influenceWeight;
}

const D3DXMATRIX& PMDBone::GetWorldMatrix()
{
	return m_matWorld;
}

const D3DXMATRIX& PMDBone::GetBoneLocalMatrix()
{
	return m_matBoneLocal;
}

void PMDBone::SetLocalPosition( const D3DXVECTOR3& position )
{
	m_localPosition  = position;
	m_changedLocal = true;
}

void PMDBone::SetLocalRotation( const D3DXQUATERNION& rotation )
{
	m_localRotation = rotation;
	m_changedLocal = true;
}

void PMDBone::SetMotionPosition( const D3DXVECTOR3& position )
{
	m_motionPosition  = position;
	m_changedMotion = true;
}

void PMDBone::SetMotionRotation( const D3DXQUATERNION& rotation )
{
	m_motionRotation = rotation;
	m_changedMotion = true;
}

const D3DXQUATERNION& PMDBone::GetMotionRotation()
{
	return m_motionRotation;
}

void PMDBone::SetIKRotation( const D3DXQUATERNION& rotation )
{
	m_ikRotation = rotation;
	m_changedIK = true;
}

const D3DXQUATERNION& PMDBone::GetIKRotation()
{
	return m_ikRotation;
}

void PMDBone::UpdateTransform( const D3DXMATRIX& matParent,bool ik )
{
	if( m_changedMotion )
	{
		D3DXMATRIX matTrans;
		D3DXMatrixTranslation( &matTrans,m_motionPosition.x,m_motionPosition.y,m_motionPosition.z );

		D3DXMATRIX matRotation;
		D3DXMatrixRotationQuaternion( &matRotation,&m_motionRotation );

		D3DXMatrixMultiply( &m_matMotion,&matRotation,&matTrans );

		m_changedMotion = false;
	}

	if( m_changedLocal )
	{
		D3DXMATRIX matTrans;
		D3DXMatrixTranslation( &matTrans,m_localPosition.x,m_localPosition.y,m_localPosition.z );

		D3DXMATRIX matRotation;
		D3DXMatrixRotationQuaternion( &matRotation,&m_localRotation );

		D3DXMatrixMultiply( &m_matLocal,&matRotation,&matTrans );

		m_changedLocal = false; 
	}

	if( m_changedIK )
	{
		D3DXMatrixRotationQuaternion( &m_matIK,&m_ikRotation );

		m_changedIK = false; 
	}

	if( ik )
	{
		D3DXMatrixMultiply( &m_matBoneLocal,&m_matIK,&m_matMotion );
	}
	else
	{
		m_matBoneLocal = m_matMotion;
	}

	D3DXMatrixMultiply( &m_matBoneLocal,&m_matBoneLocal,&m_matLocal );
	
	D3DXMatrixMultiply( &m_matWorld,&m_matBoneLocal,&matParent );

	static tstring_symbol dummyName = _T("ダミーボーン1");

	if( m_name == dummyName )
	{
		D3DXVECTOR3 pos(0.0f,0.0f,0.0f);
		D3DXVec3TransformCoord( &pos,&pos,&m_matWorld );

		OutputDebugStringFormat( _T("ダミーボーン (%f,%f,%f)\n"),pos.x,pos.y,pos.z );
	}

	PMDBone* child = GetChild();
	while( child )
	{
		child->UpdateTransform( m_matWorld,ik );

		child = child->GetSiblingNext();
	}
}