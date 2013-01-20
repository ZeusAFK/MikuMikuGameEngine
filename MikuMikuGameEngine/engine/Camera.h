#pragma once

class Camera
{
public:
	Camera();
	virtual ~Camera();

public:
	void GetFrustumCorners( float shadowDist, D3DXVECTOR3* frustumCorners );

	void UpdateView( const D3DXVECTOR3& lookPosition,const D3DXQUATERNION& rotation,float radius );
	void UpdateProj( float fov,float aspect,float nearClip,float farClip );

	const D3DXMATRIX& GetViewMatrix() const;
	const D3DXMATRIX& GetProjMatrix() const;

	const D3DXVECTOR3& GetLookPosition() const;
	const D3DXQUATERNION& GetRotation() const;
	float GetRadius() const;

	const D3DXVECTOR3& GetLookDir() const;
	const D3DXVECTOR3& GetPosition() const;

	float GetAspect() const;
	float GetFieldOfView() const;
	float GetNearClip() const;
	float GetFarClip() const;

private:
	D3DXMATRIX m_matView;
	D3DXMATRIX m_matProj;

	D3DXVECTOR3 m_lookPos;
	D3DXVECTOR3 m_lookDir;
	D3DXVECTOR3 m_cameraPos;
	D3DXQUATERNION m_cameraRotation;
	float m_cameraRadius;

	float m_aspect;
	float m_fieldOfView;
	float m_nearClip;
	float m_farClip;
};

