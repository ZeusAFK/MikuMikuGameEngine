#pragma once

#include "core/graphics/ModelRenderer.h"
#include "core/graphics/PMDModelRenderer.h"
#include "core/graphics/Animation.h"
#include "core/graphics/RenderTexture.h"

#include "core/util/TreeNode.h"

#include "Camera.h"

#include "script/ScriptManager.h"

#include <list>
#include "Behavior.h"

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
public:
	typedef std::list< BehaviorPtr > BehaviorList;
private:
	tstring m_name;

	BehaviorList m_behaviors;

	D3DXMATRIX m_matWorld;

	bool m_changedLocal;

	D3DXMATRIX m_matLocal;
	D3DXVECTOR3 m_localPosition;
	D3DXQUATERNION m_localRotation;
	D3DXVECTOR3 m_localScale;

	ModelRenderer* m_pModelRenderer;
	PMDModelRenderer* m_pPMDModelRenderer;

	Animation* m_pAnimation;

	std::vector<ScriptBehavior*> m_scriptBehaviors;

public:
	GameObject();
	virtual ~GameObject();

public:
	void SetName( const tstring& name );
	const tstring& GetName();

	void AddBehavior( BehaviorPtr behavior );

	BehaviorList::iterator GetBehaviorBegin();
	BehaviorList::iterator GetBehaviorEnd();

	void SetModelRenderer( ModelRenderer* pModelRenderer );
	ModelRenderer* GetModelRenderer();

	void SetPMDModelRenderer( PMDModelRenderer* pPMDModelRenderer );
	PMDModelRenderer* GetPMDModelRenderer();

	void SetAnimation( Animation* pAnimation );
	Animation* GetAnimation();

	D3DXMATRIX GetWorldMatrix();

	void SetLocalPosition( const D3DXVECTOR3& position );
	const D3DXVECTOR3& GetLocalPosition() const;
	void SetLocalRotation( const D3DXQUATERNION& rotation );
	const D3DXQUATERNION& GetLocalRotation() const;
	void SetLocalScale( const D3DXVECTOR3& scale );
	const D3DXVECTOR3& GetLocalScale() const;

	void ChangeParent( GameObject* parent );

	void UpdateTransform( const D3DXMATRIX& matParent );

	void UpdateAnimation( float elapsedTime );

	void AttachScript( const tstring_symbol& behaviorName );

	void UpdateScript( float deltaTime );

public:
	void UpdateMesh( const sRenderInfo& renderInfo );

	void RenderZPlot( const sRenderInfo& renderInfo );
	void Render( const sRenderInfo& renderInfo );
};