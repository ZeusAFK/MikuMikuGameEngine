#pragma once

class BoundingBox;
class BoundingFrustum;

class BoundingSphere
{
public:
	static BoundingSphere CreateFromBoundingBox( const BoundingBox& box );
	static void CreateFromBoundingBox( const BoundingBox& box,BoundingSphere* result );

	static BoundingSphere CreateFromFrustum( const BoundingFrustum& frustum );
	static BoundingSphere CreateFromPoints( const D3DXVECTOR3* points,int pointsCount );

	static BoundingSphere CreateMerged( const BoundingSphere& original,const BoundingSphere& additional );
	static void CreateMerged( const BoundingSphere& original,const BoundingSphere& additional,BoundingSphere* result );

public:
	BoundingSphere( const D3DXVECTOR3& center,float radius );
	virtual ~BoundingSphere();

	BoundingSphere( const BoundingSphere& sphere );
public:
	const D3DXVECTOR3& GetCenter() const;
	float GetRadius() const;

public:
	BoundingSphere& operator = ( const BoundingSphere& sphere );

	bool operator==(const BoundingSphere& sphere) const;
    bool operator!=(const BoundingSphere& sphere) const;

private:
	D3DXVECTOR3 m_center;
	float m_radius;
};