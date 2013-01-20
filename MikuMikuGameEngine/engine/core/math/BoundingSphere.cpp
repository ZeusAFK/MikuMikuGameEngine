#include "stdafx.h"

#include "BoundingSphere.h"

#include "BoundingFrustum.h"
#include "BoundingBox.h"

// static Member

BoundingSphere BoundingSphere::CreateFromBoundingBox( const BoundingBox& box )
{
	const D3DXVECTOR3& vMin = box.GetMin();
	const D3DXVECTOR3& vMax = box.GetMax();

	D3DXVECTOR3 center = (vMax+vMin)*0.5f;

	float radius = 0.0f;
	const D3DXVECTOR3* corners = box.GetCorners();
	for( int index = 0;index<box.CornerCount;index++ )
	{
		radius = std::max<float>( D3DXVec3Length( &(corners[index]-center) ),radius );
	}

	return BoundingSphere( center,radius );
}

void BoundingSphere::CreateFromBoundingBox( const BoundingBox& box,BoundingSphere* result )
{
	const D3DXVECTOR3& vMin = box.GetMin();
	const D3DXVECTOR3& vMax = box.GetMax();

	result->m_center = (vMax+vMin)*0.5f;

	result->m_radius = 0.0f;
	const D3DXVECTOR3* corners = box.GetCorners();
	for( int index = 0;index<box.CornerCount;index++ )
	{
		result->m_radius = std::max<float>( D3DXVec3Length( &(corners[index]-result->m_center) ),result->m_radius );
	}
}

BoundingSphere BoundingSphere::CreateFromFrustum( const BoundingFrustum& frustum )
{
	return CreateFromPoints( frustum.GetCorners(),frustum.CornerCount );
}

BoundingSphere BoundingSphere::CreateFromPoints( const D3DXVECTOR3* points,int pointsCount )
{
	D3DXVECTOR3 vMin(FLT_MAX,FLT_MAX,FLT_MAX);
	D3DXVECTOR3 vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX);

	for( int index=0;index<pointsCount;index++ )
	{
		const D3DXVECTOR3* point = &points[index];

		vMin.x = std::min<float>(point->x,vMin.x);
		vMin.y = std::min<float>(point->y,vMin.y);
		vMin.z = std::min<float>(point->z,vMin.z);

		vMax.x = std::max<float>(point->x,vMax.x);
		vMax.y = std::max<float>(point->y,vMax.y);
		vMax.z = std::max<float>(point->z,vMax.z);
	}

	D3DXVECTOR3 center = (vMax+vMin)*0.5f;

	float radius = 0.0f;
	for( int index = 0;index<pointsCount;index++ )
	{
		radius = std::max<float>( D3DXVec3Length( &(points[index]-center) ),radius );
	}

	return BoundingSphere( center,radius );
}

BoundingSphere BoundingSphere::CreateMerged( const BoundingSphere& original,const BoundingSphere& additional )
{
	D3DXVECTOR3 v1 = original.GetCenter();
	D3DXVECTOR3 v2 = additional.GetCenter();
	D3DXVECTOR3 v = v1-v2;
	D3DXVec3Normalize( &v,&v );
	v1 += v * original.GetRadius();
	v2 += -v * additional.GetRadius();

	D3DXVECTOR3 center = (v1+v2)*0.5f;
	float radius = D3DXVec3Length( &(v1-v2) )*0.5f;

	return BoundingSphere( center,radius );
}

void BoundingSphere::CreateMerged( const BoundingSphere& original,const BoundingSphere& additional,BoundingSphere* result )
{
	D3DXVECTOR3 v1 = original.GetCenter();
	D3DXVECTOR3 v2 = additional.GetCenter();
	D3DXVECTOR3 v = v1-v2;
	D3DXVec3Normalize( &v,&v );
	v1 += v * original.GetRadius();
	v2 += -v * additional.GetRadius();

	result->m_center = (v1+v2)*0.5f;
	result->m_radius = D3DXVec3Length( &(v1-v2) )*0.5f;
}

// BoundingSphere

BoundingSphere::BoundingSphere( const D3DXVECTOR3& center,float radius )
	: m_center(center)
	, m_radius(radius)
{
}

BoundingSphere::~BoundingSphere()
{
}

BoundingSphere::BoundingSphere( const BoundingSphere& sphere )
	: m_center(sphere.m_center)
	, m_radius(sphere.m_radius)
{
}

const D3DXVECTOR3& BoundingSphere::GetCenter() const
{
	return m_center;
}
float BoundingSphere::GetRadius() const
{
	return m_radius;
}

BoundingSphere& BoundingSphere::operator = ( const BoundingSphere& sphere )
{
	m_center = sphere.m_center;
	m_radius = sphere.m_radius;

	return *this;
}

bool BoundingSphere::operator==(const BoundingSphere& sphere) const
{
	return (m_center == sphere.m_center)!=FALSE && m_radius == sphere.m_radius;
}

bool BoundingSphere::operator!=(const BoundingSphere& sphere) const
{
	return (m_center != sphere.m_center)!=FALSE || m_radius != sphere.m_radius;
}