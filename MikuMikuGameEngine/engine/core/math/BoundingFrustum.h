#pragma once

class BoundingFrustum
{
public:
	BoundingFrustum( const D3DXMATRIX& matrix );
	virtual ~BoundingFrustum();

	BoundingFrustum( const BoundingFrustum& frustum );

public:
	static const int CornerCount = 8;

public:
	const D3DXVECTOR3* GetCorners() const;
	void GetCorners( D3DXVECTOR3* corners ) const;

	const D3DXPLANE& GetTopPlane() const;
	const D3DXPLANE& GetBottomPlane() const;
	const D3DXPLANE& GetNearPlane() const;
	const D3DXPLANE& GetFarPlane() const;
	const D3DXPLANE& GetLeftPlane() const;
	const D3DXPLANE& GetRightPlane() const;

	void SetMatrix( const D3DXMATRIX& matrix );
	const D3DXMATRIX& GetMatrix() const;

public:
	BoundingFrustum& operator = ( const BoundingFrustum& frustum );

	bool operator==(const BoundingFrustum& frustum) const;
    bool operator!=(const BoundingFrustum& frustum) const;

private:
	D3DXMATRIX m_matrix;

	D3DXPLANE m_topPlane;
	D3DXPLANE m_bottomPlane;
	D3DXPLANE m_nearPlane;
	D3DXPLANE m_farPlane;
	D3DXPLANE m_leftPlane;
	D3DXPLANE m_rightPlane;

	D3DXVECTOR3 m_corners[CornerCount];
};