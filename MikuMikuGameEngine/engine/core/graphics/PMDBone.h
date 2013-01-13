#pragma once

#include "../util/CharSet.h"
#include "../util/TreeNode.h"

#include "PMDModel.h"

class GameObject;

class PMDBone : public TreeNode<PMDBone>
{
private:
	tstring_symbol m_name;

	ePMD_BoneType m_boneType;

	int m_influenceBoneIndex;
	float m_influenceWeight;

	D3DXMATRIX m_matWorld;

	bool m_changedLocal;

	D3DXMATRIX m_matLocal;
	D3DXVECTOR3 m_localPosition;
	D3DXQUATERNION m_localRotation;

	bool m_changedMotion;

	D3DXMATRIX m_matMotion;
	D3DXVECTOR3 m_motionPosition;
	D3DXQUATERNION m_motionRotation;

	bool m_changedIK;

	D3DXMATRIX m_matIK;
	D3DXQUATERNION m_ikRotation;

	D3DXMATRIX m_matBoneLocal;

public:
	PMDBone();
	virtual ~PMDBone();

public:
	void SetName( const tstring_symbol& name );
	const tstring_symbol& GetName();

	void SetBoneType( ePMD_BoneType boneType );
	ePMD_BoneType GetBoneType();

	void SetInfluenceBoneIndex( int boneIndex );
	int GetInfluenceBoneIndex();
	void SetInfluenceWeight( float weight );
	float GetInfluenceWeight();

	const D3DXMATRIX& GetWorldMatrix();
	const D3DXMATRIX& GetBoneLocalMatrix();

	void SetLocalPosition( const D3DXVECTOR3& position );
	void SetLocalRotation( const D3DXQUATERNION& rotation );

	void SetMotionPosition( const D3DXVECTOR3& position );
	void SetMotionRotation( const D3DXQUATERNION& rotation );
	const D3DXQUATERNION& GetMotionRotation();

	void SetIKRotation( const D3DXQUATERNION& rotation );
	const D3DXQUATERNION& GetIKRotation();

	void UpdateTransform( const D3DXMATRIX& matParent,bool ik );
};