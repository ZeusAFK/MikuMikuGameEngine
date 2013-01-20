#pragma once

class Ray
{
public:
	Ray( const D3DXVECTOR3& position,const D3DXVECTOR3& direction );
	virtual ~Ray();

	Ray( const Ray& ray );

public:
	const D3DXVECTOR3& GetPosition() const;
	const D3DXVECTOR3& GetDirection() const;

public:
	Ray& operator = ( const Ray& ray );

	bool operator==(const Ray& ray) const;
    bool operator!=(const Ray& ray) const;

private:
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_direction;
};