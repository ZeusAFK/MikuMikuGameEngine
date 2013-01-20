#include "stdafx.h"

#include "BoundingBox.h"

#include "BoundingSphere.h"

#include "Ray.h"

// static Member

BoundingBox BoundingBox::CreateFromPoints ( const D3DXVECTOR3* points,int pointsCount )
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

	return BoundingBox(vMin,vMax);
}

BoundingBox BoundingBox::CreateFromSphere( const BoundingSphere& sphere )
{
	const D3DXVECTOR3& center = sphere.GetCenter();
	float radius = sphere.GetRadius();

	D3DXVECTOR3 vMin = center - D3DXVECTOR3( radius,radius,radius );
	D3DXVECTOR3 vMax = center + D3DXVECTOR3( radius,radius,radius );

	return BoundingBox(vMin,vMax);
}

void BoundingBox::CreateFromSphere( const BoundingSphere& sphere, BoundingBox* result )
{
	const D3DXVECTOR3& center = sphere.GetCenter();
	float radius = sphere.GetRadius();

	D3DXVECTOR3 vMin = center - D3DXVECTOR3( radius,radius,radius );
	D3DXVECTOR3 vMax = center + D3DXVECTOR3( radius,radius,radius );

	result->m_min = vMin;
	result->m_max = vMax;

	result->m_corners[0] = D3DXVECTOR3(vMin.x, vMax.y, vMin.z); // left,top,near
	result->m_corners[1] = D3DXVECTOR3(vMax.x, vMax.y, vMin.z); // right,top,near
	result->m_corners[2] = D3DXVECTOR3(vMin.x, vMin.y, vMin.z); // left,bottom,near
	result->m_corners[3] = D3DXVECTOR3(vMax.x, vMin.y, vMin.z); // right,bottom,near
	result->m_corners[4] = D3DXVECTOR3(vMin.x, vMax.y, vMax.z); // left,top,far
	result->m_corners[5] = D3DXVECTOR3(vMax.x, vMax.y, vMax.z); // right,top,far
	result->m_corners[6] = D3DXVECTOR3(vMin.x, vMin.y, vMax.z); // left,bottom,far
	result->m_corners[7] = D3DXVECTOR3(vMax.x, vMin.y, vMax.z); // right,bottom,far
}

BoundingBox BoundingBox::CreateMerged( const BoundingBox& original,const BoundingBox& additional )
{
	D3DXVECTOR3 vMin(FLT_MAX,FLT_MAX,FLT_MAX);
	D3DXVECTOR3 vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX);

	const D3DXVECTOR3& vMin2 = additional.GetMin();
	const D3DXVECTOR3& vMax2 = additional.GetMax();

	vMin.x = std::min<float>(vMin2.x,vMin.x);
	vMin.y = std::min<float>(vMin2.y,vMin.y);
	vMin.z = std::min<float>(vMin2.z,vMin.z);

	vMax.x = std::max<float>(vMax2.x,vMax.x);
	vMax.y = std::max<float>(vMax2.y,vMax.y);
	vMax.z = std::max<float>(vMax2.z,vMax.z);

	return BoundingBox(vMin,vMax);
}

void BoundingBox::CreateMerged( const BoundingBox& original,const BoundingBox& additional,BoundingBox* result )
{
	D3DXVECTOR3 vMin = original.GetMin();
	D3DXVECTOR3 vMax = original.GetMax();

	const D3DXVECTOR3& vMin2 = additional.GetMin();
	const D3DXVECTOR3& vMax2 = additional.GetMax();

	vMin.x = std::min<float>(vMin2.x,vMin.x);
	vMin.y = std::min<float>(vMin2.y,vMin.y);
	vMin.z = std::min<float>(vMin2.z,vMin.z);

	vMax.x = std::max<float>(vMax2.x,vMax.x);
	vMax.y = std::max<float>(vMax2.y,vMax.y);
	vMax.z = std::max<float>(vMax2.z,vMax.z);

	result->m_min = vMin;
	result->m_max = vMax;

	result->m_corners[0] = D3DXVECTOR3(vMin.x, vMax.y, vMin.z); // left,top,near
	result->m_corners[1] = D3DXVECTOR3(vMax.x, vMax.y, vMin.z); // right,top,near
	result->m_corners[2] = D3DXVECTOR3(vMin.x, vMin.y, vMin.z); // left,bottom,near
	result->m_corners[3] = D3DXVECTOR3(vMax.x, vMin.y, vMin.z); // right,bottom,near
	result->m_corners[4] = D3DXVECTOR3(vMin.x, vMax.y, vMax.z); // left,top,far
	result->m_corners[5] = D3DXVECTOR3(vMax.x, vMax.y, vMax.z); // right,top,far
	result->m_corners[6] = D3DXVECTOR3(vMin.x, vMin.y, vMax.z); // left,bottom,far
	result->m_corners[7] = D3DXVECTOR3(vMax.x, vMin.y, vMax.z); // right,bottom,far
}

// BoundingBox

BoundingBox::BoundingBox( const D3DXVECTOR3& vMin,const D3DXVECTOR3& vMax )
{
	m_min = vMin;
	m_max = vMax;

	m_corners[0] = D3DXVECTOR3(vMin.x, vMax.y, vMin.z); // left,top,near
	m_corners[1] = D3DXVECTOR3(vMax.x, vMax.y, vMin.z); // right,top,near
	m_corners[2] = D3DXVECTOR3(vMin.x, vMin.y, vMin.z); // left,bottom,near
	m_corners[3] = D3DXVECTOR3(vMax.x, vMin.y, vMin.z); // right,bottom,near
	m_corners[4] = D3DXVECTOR3(vMin.x, vMax.y, vMax.z); // left,top,far
	m_corners[5] = D3DXVECTOR3(vMax.x, vMax.y, vMax.z); // right,top,far
	m_corners[6] = D3DXVECTOR3(vMin.x, vMin.y, vMax.z); // left,bottom,far
	m_corners[7] = D3DXVECTOR3(vMax.x, vMin.y, vMax.z); // right,bottom,far
}
BoundingBox::~BoundingBox()
{
}

BoundingBox::BoundingBox( const BoundingBox& box )
{
	m_min = box.m_min;
	m_max = box.m_max;

	box.GetCorners( m_corners );
}

#define FLT_AS_DW(F) (*(DWORD*)&(F))
#define FLT_SIGN(F) ((FLT_AS_DW(F) & 0x80000000L))
#define ALMOST_ZERO(F) ((FLT_AS_DW(F) & 0x7f800000L)==0)
#define IS_SPECIAL(F)  ((FLT_AS_DW(F) & 0x7f800000L)==0x7f800000L)

bool BoundingBox::Intersect( const Ray& ray,float* hitDist ) const
{
	D3DXPLANE sides[6] = { D3DXPLANE( 1, 0, 0,-m_min.x), D3DXPLANE(-1, 0, 0, m_max.x),
                           D3DXPLANE( 0, 1, 0,-m_min.y), D3DXPLANE( 0,-1, 0, m_max.y),
                           D3DXPLANE( 0, 0, 1,-m_min.z), D3DXPLANE( 0, 0,-1, m_max.z) };

	const D3DXVECTOR3& rayPos = ray.GetPosition();
	const D3DXVECTOR3& rayDir = ray.GetDirection();

    *hitDist = 0.f;  // safe initial value
    D3DXVECTOR3 hitPt = rayPos;

    bool inside = false;

    for ( int i=0; (i<6) && !inside; i++ )
    {
        float cosTheta = D3DXPlaneDotNormal( &sides[i], &rayDir );
        float dist = D3DXPlaneDotCoord ( &sides[i], &rayPos );
        
        //  if we're nearly intersecting, just punt and call it an intersection
        if ( ALMOST_ZERO(dist) ) return true;
        //  skip nearly (&actually) parallel rays
        if ( ALMOST_ZERO(cosTheta) ) continue;
        //  only interested in intersections along the ray, not before it.
        *hitDist = -dist / cosTheta;
        if ( *hitDist < 0.f ) continue;

        hitPt = (*hitDist)*rayDir + rayPos;

        inside = true;
        
        for ( int j=0; (j<6) && inside; j++ )
        {
            if ( j==i )
                continue;
            float d = D3DXPlaneDotCoord( &sides[j], &hitPt );
            
            inside = ((d + 0.00015) >= 0.f);
        }
    }

    return inside;        
}

void BoundingBox::Transform( const D3DXMATRIX& matrix )
{
	D3DXVECTOR3 corners[CornerCount];

	GetCorners( corners );

	D3DXVECTOR3 vMin(FLT_MAX,FLT_MAX,FLT_MAX);
	D3DXVECTOR3 vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX);

	for( int index=0;index<CornerCount;index++ )
	{
		const D3DXVECTOR3* point = &corners[index];
		D3DXVec3TransformCoord(&corners[index], &corners[index], &matrix );

		vMin.x = std::min<float>(point->x,vMin.x);
		vMin.y = std::min<float>(point->y,vMin.y);
		vMin.z = std::min<float>(point->z,vMin.z);

		vMax.x = std::max<float>(point->x,vMax.x);
		vMax.y = std::max<float>(point->y,vMax.y);
		vMax.z = std::max<float>(point->z,vMax.z);
	}

	m_corners[0] = D3DXVECTOR3(vMin.x, vMax.y, vMin.z); // left,top,near
	m_corners[1] = D3DXVECTOR3(vMax.x, vMax.y, vMin.z); // right,top,near
	m_corners[2] = D3DXVECTOR3(vMin.x, vMin.y, vMin.z); // left,bottom,near
	m_corners[3] = D3DXVECTOR3(vMax.x, vMin.y, vMin.z); // right,bottom,near
	m_corners[4] = D3DXVECTOR3(vMin.x, vMax.y, vMax.z); // left,top,far
	m_corners[5] = D3DXVECTOR3(vMax.x, vMax.y, vMax.z); // right,top,far
	m_corners[6] = D3DXVECTOR3(vMin.x, vMin.y, vMax.z); // left,bottom,far
	m_corners[7] = D3DXVECTOR3(vMax.x, vMin.y, vMax.z); // right,bottom,far
}

const D3DXVECTOR3* BoundingBox::GetCorners() const
{
	return m_corners;
}

void BoundingBox::GetCorners( D3DXVECTOR3* corners ) const
{
	memcpy( corners,m_corners,sizeof(D3DXVECTOR3)*CornerCount );
}

const D3DXVECTOR3& BoundingBox::GetMin() const
{
	return m_min;
}

const D3DXVECTOR3& BoundingBox::GetMax() const
{
	return m_max;
}

BoundingBox& BoundingBox::operator = ( const BoundingBox& box )
{
	m_min = box.m_min;
	m_max = box.m_max;

	box.GetCorners( m_corners );

	return *this;
}

bool BoundingBox::operator==(const BoundingBox& box) const
{
	return (m_min == box.m_min)!=FALSE && (m_max == box.m_max)!=FALSE;
}

bool BoundingBox::operator!=(const BoundingBox& box) const
{
	return (m_min != box.m_min)!=FALSE || (m_max != box.m_max)!=FALSE;
}