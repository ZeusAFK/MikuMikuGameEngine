#include "stdafx.h"

#include "Ray.h"

Ray::Ray( const D3DXVECTOR3& position,const D3DXVECTOR3& direction )
	: m_position( position )
	, m_direction( direction )
{
}

Ray::~Ray()
{
}

Ray::Ray( const Ray& ray )
{
}

const D3DXVECTOR3& Ray::GetPosition() const
{
	return m_position;
}

const D3DXVECTOR3& Ray::GetDirection() const
{
	return m_direction;
}

Ray& Ray::operator = ( const Ray& ray )
{
	m_position = ray.m_position;
	m_direction = ray.m_direction;

	return *this;
}

bool Ray::operator==(const Ray& ray) const
{
	return (m_position == ray.m_position)!=FALSE && (m_direction == m_direction)!=FALSE;
}

bool Ray::operator!=(const Ray& ray) const
{
	return (m_position != ray.m_position)!=FALSE || (m_direction != m_direction)!=FALSE;
}