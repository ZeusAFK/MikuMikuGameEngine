#include "stdafx.h"

#include "BoundingCone.h"

#include "BoundingBox.h"
#include "BoundingSphere.h"

BoundingCone::BoundingCone(const BoundingBox& box, const D3DXMATRIX& projection, const D3DXVECTOR3& apex)
{
	const D3DXVECTOR3 yAxis(0.f, 1.f, 0.f);
    const D3DXVECTOR3 zAxis(0.f, 0.f, 1.f);
    const D3DXVECTOR3 negZAxis(0.f, 0.f, -1.f);

    //  compute a tight bounding sphere for the vertices of the bounding boxes.
    //  the vector from the apex to the center of the sphere is the optimized view direction
    //  start by xforming all points to post-projective space
    D3DXVECTOR3 ppPts[ box.CornerCount ];
	box.GetCorners( ppPts );

	for( int index=0; index<box.CornerCount; index++)
    {
        D3DXVec3TransformCoord( &ppPts[index], &ppPts[index], &projection );
    }

    //  get minimum bounding sphere
	BoundingSphere bSphere = BoundingSphere::CreateFromPoints( ppPts,box.CornerCount );

    float min_cosTheta = 1.f;
    
    m_direction = bSphere.GetCenter() - apex;
    D3DXVec3Normalize(&m_direction, &m_direction);

    D3DXVECTOR3 axis = yAxis;

    if( fabsf(D3DXVec3Dot(&yAxis, &m_direction)) > 0.99f )
	{
        axis = zAxis;
	}

    D3DXMatrixLookAtLH(&m_lookAt, &apex, &(apex+m_direction), &axis);

    m_near = 1e32f;
    m_far = 0.f;

    float maxx=0.f, maxy=0.f;
    for( int index=0; index<box.CornerCount; index++ )
    {
        D3DXVECTOR3 tmp;
        D3DXVec3TransformCoord(&tmp, &ppPts[index], &m_lookAt);
        maxx = max(maxx, fabsf(tmp.x / tmp.z));
        maxy = max(maxy, fabsf(tmp.y / tmp.z));
        m_near = min(m_near, tmp.z);
        m_far  = max(m_far, tmp.z);
    }

    m_fovx = atanf(maxx);
	m_fovy = atanf(maxy);
}

BoundingCone::BoundingCone(const BoundingBox& box, const D3DXMATRIX& projection, const D3DXVECTOR3& apex, const D3DXVECTOR3& direction )
{
	m_direction = direction;

	const D3DXVECTOR3 yAxis(0.f, 1.f, 0.f);
    const D3DXVECTOR3 zAxis(0.f, 0.f, 1.f);
    D3DXVec3Normalize(&m_direction, &m_direction);
    
    D3DXVECTOR3 axis = yAxis;

    if ( fabsf(D3DXVec3Dot(&yAxis, &m_direction))>0.99f )
	{
        axis = zAxis;
	}
    
    D3DXMatrixLookAtLH( &m_lookAt, &apex, &(apex+m_direction), &axis );
    
    float maxx = 0.f, maxy = 0.f;
    m_near = 1e32f;
    m_far =  0.f;

    D3DXMATRIX concatMatrix;
    D3DXMatrixMultiply( &concatMatrix, &projection, &m_lookAt );

	D3DXVECTOR3 ppPts[ box.CornerCount ];
	box.GetCorners( ppPts );

	for(int index=0; index<box.CornerCount; index++)
    {
		D3DXVECTOR3 vec = ppPts[index];
        D3DXVec3TransformCoord(&vec, &vec, &concatMatrix);
        maxx = max(maxx, fabsf(vec.x / vec.z));
        maxy = max(maxy, fabsf(vec.y / vec.z));
        m_near = min(m_near, vec.z);
        m_far  = max(m_far, vec.z);
    }

    m_fovx = atanf(maxx);
    m_fovy = atanf(maxy);
}

const D3DXVECTOR3& BoundingCone::GetDirection() const
{
	return m_direction;
}

const D3DXMATRIX& BoundingCone::GetLookAt() const
{
	return m_lookAt;
}

float BoundingCone::GetFovX() const
{
	return m_fovx;
}
float BoundingCone::GetFovY() const
{
	return m_fovy;
}

float BoundingCone::GetNear() const
{
	return m_near;
}

float BoundingCone::GetFar() const
{
	return m_far;
}