#include "stdafx.h"

#include "GameObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GameObject::GameObject()
	: m_pModelRenderer(NULL)
	, m_pPMDModelRenderer(NULL)
	, m_name(_T("GameObject"))
	, m_pAnimation(NULL)
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
	std::vector<ScriptBehavior*>::iterator it = m_scriptBehaviors.begin();
	while( it!=m_scriptBehaviors.end() )
	{
		delete *it;

		++it;
	}
	m_scriptBehaviors.clear();

	if( m_pModelRenderer )
	{
		delete m_pModelRenderer;
	}
	if( m_pPMDModelRenderer )
	{
		delete m_pPMDModelRenderer;
	}

	delete m_pAnimation;
	m_pAnimation = NULL;
}

void GameObject::SetName( const tstring& name )
{
	m_name = name;
}

const tstring& GameObject::GetName()
{
	return m_name;
}

GameObject::BehaviorList::iterator GameObject::GetBehaviorBegin()
{
	return m_behaviors.begin();
}

GameObject::BehaviorList::iterator GameObject::GetBehaviorEnd()
{
	return m_behaviors.end();
}

void GameObject::SetModelRenderer( ModelRenderer* pModelRenderer )
{
	delete m_pPMDModelRenderer;
	m_pPMDModelRenderer = NULL;

	delete m_pModelRenderer;
	m_pModelRenderer = pModelRenderer;

	if( m_pAnimation )
	{
		m_pAnimation->SetPMDModelRenderer( m_pPMDModelRenderer );
	}
}

ModelRenderer* GameObject::GetModelRenderer()
{
	return m_pModelRenderer;
}

void GameObject::SetPMDModelRenderer( PMDModelRenderer* pPMDModelRenderer )
{
	delete m_pModelRenderer;
	m_pModelRenderer = NULL;

	delete m_pPMDModelRenderer;
	m_pPMDModelRenderer = pPMDModelRenderer;
	if( m_pAnimation )
	{
		m_pAnimation->SetPMDModelRenderer( m_pPMDModelRenderer );
	}
}

PMDModelRenderer* GameObject::GetPMDModelRenderer()
{
	return m_pPMDModelRenderer;
}

void GameObject::SetAnimation( Animation* pAnimation )
{
	delete m_pAnimation;

	m_pAnimation = pAnimation;
	if( m_pAnimation )
	{
		m_pAnimation->SetPMDModelRenderer( m_pPMDModelRenderer );
	}
}

Animation* GameObject::GetAnimation()
{
	return m_pAnimation;
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

const D3DXVECTOR3& GameObject::GetLocalPosition() const
{
	return m_localPosition;
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

const D3DXQUATERNION& GameObject::GetLocalRotation() const
{
	return m_localRotation;
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

const D3DXVECTOR3& GameObject::GetLocalScale() const
{
	return m_localScale;
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
	if( m_pAnimation )
	{
		m_pAnimation->UpdateAnimation( elapsedTime );
	}

	GameObject* child = GetChild();
	while( child )
	{
		child->UpdateAnimation( elapsedTime );

		child = child->GetSiblingNext();
	}
}

void GameObject::AttachScript( const tstring_symbol& behaviorName )
{
	ScriptManager* scriptManager = ScriptManager::GetInstance();

	ScriptClassObject* classObject = scriptManager->GetClassObject( behaviorName );
	if( classObject == NULL )
	{
		return;
	}

	ScriptBehavior* behavior = classObject->CreateBehavior();

	behavior->Awake();

	m_scriptBehaviors.push_back( behavior );
}

void GameObject::UpdateScript( float deltaTime )
{
	std::vector<ScriptBehavior*>::iterator it = m_scriptBehaviors.begin();
	while( it!=m_scriptBehaviors.end() )
	{
		(*it)->Update( deltaTime );

		++it;
	}

	GameObject* child = GetChild();
	while( child )
	{
		child->UpdateScript( deltaTime );

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
