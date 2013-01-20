#pragma once

class BoundingBox;

class BoundingCone
{
public:
	BoundingCone(const BoundingBox& box, const D3DXMATRIX& projection, const D3DXVECTOR3& apex);
    BoundingCone(const BoundingBox& box, const D3DXMATRIX& projection, const D3DXVECTOR3& apex, const D3DXVECTOR3& direction );

public:
	const D3DXVECTOR3& GetDirection() const;
	const D3DXMATRIX& GetLookAt() const;

	float GetFovX() const;
	float GetFovY() const;

	float GetNear() const;
	float GetFar() const;

private:
	D3DXVECTOR3 m_direction;
	D3DXMATRIX m_lookAt;

	float m_near;
	float m_far;

	float m_fovx;
	float m_fovy;
};