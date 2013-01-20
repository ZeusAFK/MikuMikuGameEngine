#include "stdafx.h"

#include "Camera.h"

#include "core/math/BoundingFrustum.h"

Camera::Camera()
{
	D3DXQUATERNION rotation;
	D3DXQuaternionIdentity( &rotation );
	UpdateView( D3DXVECTOR3( 0.0f,10.0f,0.0f ),rotation,35.0f );

	float nearClip = 1.0f;
	float farClip = 1000000.0f;

	UpdateProj( D3DXToRadian(45.0f),1.0f,nearClip,farClip );
}

Camera::~Camera()
{
}

void Camera::GetFrustumCorners( float shadowDist, D3DXVECTOR3* frustumCorners )
{
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH( &matProj,m_fieldOfView,m_aspect,m_nearClip, shadowDist );

	BoundingFrustum frustum( m_matView * matProj );
    //視錐台からコーナーの取得
	frustum.GetCorners(frustumCorners);
}

void Camera::UpdateView( const D3DXVECTOR3& lookPosition,const D3DXQUATERNION& rotation,float radius )
{
	m_lookPos = lookPosition;
	m_cameraRotation = rotation;
	m_cameraRadius = radius;
	
	m_cameraPos = D3DXVECTOR3(0.0f,0.0f,-1.0f);

	m_lookDir = D3DXVECTOR3(0.0f,0.0f,1.0f);

	D3DXVECTOR3 vecLookPt(0.0f,0.0f,1.0f);
	D3DXVECTOR3 vecUpVec(0.0f,1.0f,0.0f);

	D3DXMATRIX matCameraRotate;

	D3DXMatrixRotationQuaternion( &matCameraRotate,&m_cameraRotation );

	D3DXVec3TransformNormal( &vecLookPt,&vecLookPt,&matCameraRotate );
	D3DXVec3TransformNormal( &m_lookDir,&m_lookDir,&matCameraRotate );
	D3DXVec3TransformNormal( &vecUpVec,&vecUpVec,&matCameraRotate );

	D3DXVec3Scale( &m_cameraPos,&m_lookDir,-m_cameraRadius );
	D3DXVec3Add( &m_cameraPos,&m_lookPos,&m_cameraPos );

	D3DXVec3Add( &vecLookPt,&m_cameraPos,&vecLookPt );

	D3DXMatrixLookAtLH( &m_matView, &m_cameraPos, &vecLookPt, &vecUpVec );
}

void Camera::UpdateProj( float fov,float aspect,float nearClip,float farClip )
{
	m_fieldOfView = fov;
	m_aspect = aspect;
	m_nearClip = nearClip;
	m_farClip = farClip;

	D3DXMatrixPerspectiveFovLH( &m_matProj, fov, aspect, nearClip, farClip);
}

const D3DXMATRIX& Camera::GetViewMatrix() const
{
	return m_matView;
}

const D3DXMATRIX& Camera::GetProjMatrix() const
{
	return m_matProj;
}

const D3DXVECTOR3& Camera::GetLookPosition() const
{
	return m_lookPos;
}

const D3DXQUATERNION& Camera::GetRotation() const
{
	return m_cameraRotation;
}

float Camera::GetRadius() const
{
	return m_cameraRadius;
}

const D3DXVECTOR3& Camera::GetLookDir() const
{
	return m_lookDir;
}

const D3DXVECTOR3& Camera::GetPosition() const
{
	return m_cameraPos;
}

float Camera::GetAspect() const
{
	return m_aspect;
}

float Camera::GetFieldOfView() const
{
	return m_fieldOfView;
}

float Camera::GetNearClip() const
{
	return m_nearClip;
}

float Camera::GetFarClip() const
{
	return m_farClip;
}
