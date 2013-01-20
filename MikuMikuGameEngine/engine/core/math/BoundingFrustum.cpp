#include "stdafx.h"

#include "BoundingFrustum.h"

BoundingFrustum::BoundingFrustum( const D3DXMATRIX& matrix )
{
	SetMatrix( matrix );
}

BoundingFrustum::~BoundingFrustum()
{
}

BoundingFrustum::BoundingFrustum( const BoundingFrustum& frustum )
{
	m_matrix = frustum.m_matrix;

	m_topPlane = frustum.m_topPlane;
	m_bottomPlane = frustum.m_bottomPlane;
	m_nearPlane = frustum.m_nearPlane;
	m_farPlane = frustum.m_farPlane;
	m_leftPlane = frustum.m_leftPlane;
	m_rightPlane = frustum.m_rightPlane;

	frustum.GetCorners( m_corners );
}

const D3DXVECTOR3* BoundingFrustum::GetCorners() const
{
	return m_corners;
}
void BoundingFrustum::GetCorners( D3DXVECTOR3* corners ) const
{
	memcpy( corners,m_corners,sizeof(D3DXVECTOR3)*CornerCount );
}

const D3DXPLANE& BoundingFrustum::GetTopPlane() const
{
	return m_topPlane;
}

const D3DXPLANE& BoundingFrustum::GetBottomPlane() const
{
	return m_bottomPlane;
}

const D3DXPLANE& BoundingFrustum::GetNearPlane() const
{
	return m_nearPlane;
}

const D3DXPLANE& BoundingFrustum::GetFarPlane() const
{
	return m_farPlane;
}

const D3DXPLANE& BoundingFrustum::GetLeftPlane() const
{
	return m_leftPlane;
}

const D3DXPLANE& BoundingFrustum::GetRightPlane() const
{
	return m_rightPlane;
}

void BoundingFrustum::SetMatrix( const D3DXMATRIX& matrix )
{
	m_matrix = matrix;

	D3DXMATRIX matInv;
	D3DXMatrixInverse( &matInv,NULL,&m_matrix );

	m_corners[0] = D3DXVECTOR3(-1.0f, 1.0f,0.0f); // left,top,near
	m_corners[1] = D3DXVECTOR3( 1.0f, 1.0f,0.0f); // right,top,near
	m_corners[2] = D3DXVECTOR3(-1.0f,-1.0f,0.0f); // left,bottom,near
	m_corners[3] = D3DXVECTOR3( 1.0f,-1.0f,0.0f); // right,bottom,near
	m_corners[4] = D3DXVECTOR3(-1.0f, 1.0f,1.0f); // left,top,far
	m_corners[5] = D3DXVECTOR3( 1.0f, 1.0f,1.0f); // right,top,far
	m_corners[6] = D3DXVECTOR3(-1.0f,-1.0f,1.0f); // left,bottom,far
	m_corners[7] = D3DXVECTOR3( 1.0f,-1.0f,1.0f); // right,bottom,far

	for( int index = 0; index<CornerCount; index++ )
	{
		D3DXVec3TransformCoord( &m_corners[index],&m_corners[index],&matInv );
	}

	// top : left,near / right,far / right,near
	D3DXPlaneFromPoints( &m_topPlane,&m_corners[0],&m_corners[5],&m_corners[1] );

	// bottom : left,near / right,near / right,far
	D3DXPlaneFromPoints( &m_bottomPlane,&m_corners[2],&m_corners[3],&m_corners[7] );

	// near : left,top / right,top / left,bottom
	D3DXPlaneFromPoints( &m_nearPlane,&m_corners[0],&m_corners[1],&m_corners[2] );

	// far : right,top / left,top / right,bottom
	D3DXPlaneFromPoints( &m_farPlane,&m_corners[5],&m_corners[4],&m_corners[7] );

	// left : top,far / top,near / bottom,far
	D3DXPlaneFromPoints( &m_leftPlane,&m_corners[4],&m_corners[0],&m_corners[2] );

	// right : top,near / top,far / bottom,near
	D3DXPlaneFromPoints( &m_rightPlane,&m_corners[1],&m_corners[5],&m_corners[3] );
}

const D3DXMATRIX& BoundingFrustum::GetMatrix() const
{
	return m_matrix;
}

BoundingFrustum& BoundingFrustum::operator = ( const BoundingFrustum& frustum )
{
	m_matrix = frustum.m_matrix;

	m_topPlane = frustum.m_topPlane;
	m_bottomPlane = frustum.m_bottomPlane;
	m_nearPlane = frustum.m_nearPlane;
	m_farPlane = frustum.m_farPlane;
	m_leftPlane = frustum.m_leftPlane;
	m_rightPlane = frustum.m_rightPlane;

	frustum.GetCorners( m_corners );

	return *this;
}

bool BoundingFrustum::operator==(const BoundingFrustum& frustum) const
{
	return (m_matrix == frustum.m_matrix)!=FALSE;
}

bool BoundingFrustum::operator!=(const BoundingFrustum& frustum) const
{
	return (m_matrix != frustum.m_matrix)!=FALSE;
}