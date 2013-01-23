#include "stdafx.h"

#include "AssetNode.h"

AssetNode::AssetNode(Type type)
	: m_type(type)
{
}

AssetNode::~AssetNode()
{
}

void AssetNode::SetName( const tstring& name )
{
	m_name = name;
}

AssetNode::Type AssetNode::GetType() const
{
	return m_type;
}

const tstring& AssetNode::GetName() const
{
	return m_name;
}

void AssetNode::SetPath( const tstring& path )
{
	m_path = path;
}

const tstring& AssetNode::GetPath() const
{
	return m_path;
}