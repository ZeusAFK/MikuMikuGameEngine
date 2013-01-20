#pragma once

class BoundingSphere;
class Ray;

class BoundingBox
{
public:
	static BoundingBox CreateFromPoints( const D3DXVECTOR3* points,int pointsCount );
	static BoundingBox CreateFromSphere( const BoundingSphere& sphere );
	static void CreateFromSphere( const BoundingSphere& sphere, BoundingBox* result );
	static BoundingBox CreateMerged( const BoundingBox& original,const BoundingBox& additional );
	static void CreateMerged( const BoundingBox& original,const BoundingBox& additional,BoundingBox* result );

public:
	BoundingBox( const D3DXVECTOR3& vMin,const D3DXVECTOR3& vMax );
	virtual ~BoundingBox();

	BoundingBox( const BoundingBox& box );

public:
	static const int CornerCount = 8;

public:
	bool Intersect( const Ray& ray,float* hitDist ) const;
	void Transform( const D3DXMATRIX& matrix );

public:
	const D3DXVECTOR3* GetCorners() const;
	void GetCorners( D3DXVECTOR3* corners ) const;

	const D3DXVECTOR3& GetMin() const;
	const D3DXVECTOR3& GetMax() const;

public:
	BoundingBox& operator = ( const BoundingBox& box );

	bool operator==(const BoundingBox& box) const;
    bool operator!=(const BoundingBox& box) const;

private:
	D3DXVECTOR3 m_min;
	D3DXVECTOR3 m_max;

	D3DXVECTOR3 m_corners[CornerCount];
};