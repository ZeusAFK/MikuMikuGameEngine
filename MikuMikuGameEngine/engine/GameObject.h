#pragma once

#include "core/graphics/ModelRenderer.h"
#include "core/graphics/PMDModelRenderer.h"
#include "core/graphics/VMDAnimationClip.h"
#include "core/graphics/RenderTexture.h"

#include "core/util/TreeNode.h"

#include "Camera.h"

struct sRenderInfo
{
	Camera camera;

	D3DXVECTOR3 lightDir;
	D3DXCOLOR lightColor;

	bool shadowMap;

	RenderTexturePtr shadowMapTexture;

	D3DXMATRIX matShadow;
};

class GameObject : public TreeNode<GameObject>
{
private:
	tstring m_name;

	D3DXMATRIX m_matWorld;

	bool m_changedLocal;

	D3DXMATRIX m_matLocal;
	D3DXVECTOR3 m_localPosition;
	D3DXQUATERNION m_localRotation;
	D3DXVECTOR3 m_localScale;

	ModelRenderer* m_pModelRenderer;
	PMDModelRenderer* m_pPMDModelRenderer;

	VMDAnimationClipPtr m_pAnimationClip;
	PMDBone** m_ppAnimationBones;
	int* m_pAnimationSkins;
	float m_animationTime;
	float m_animationLastFrame;

public:
	GameObject();
	virtual ~GameObject();

public:
	void SetName( const tstring& name );
	const tstring& GetName();

	void SetModelRenderer( ModelRenderer* pModelRenderer );
	ModelRenderer* GetModelRenderer();

	void SetPMDModelRenderer( PMDModelRenderer* pPMDModelRenderer );
	PMDModelRenderer* GetPMDModelRenderer();

	void SetVMDAnimationClip( VMDAnimationClipPtr& animationClip );

	D3DXMATRIX GetWorldMatrix();

	void SetLocalPosition( const D3DXVECTOR3& position );
	void SetLocalRotation( const D3DXQUATERNION& rotation );
	void SetLocalScale( const D3DXVECTOR3& scale );

	void ChangeParent( GameObject* parent );

	void UpdateTransform( const D3DXMATRIX& matParent );

	void UpdateAnimation( float elapsedTime );

public:
	void UpdateMesh( const sRenderInfo& renderInfo );

	void RenderZPlot( const sRenderInfo& renderInfo );
	void Render( const sRenderInfo& renderInfo );

	void RenderNonShader();
};