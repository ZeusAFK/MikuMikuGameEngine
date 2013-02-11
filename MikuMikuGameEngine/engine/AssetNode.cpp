#include "stdafx.h"

#include "AssetNode.h"

#include "core/util/util.h"

AssetNode::AssetNode(Type type)
	: m_type(type)
{
	CreateUUIDSymbol( m_uuid );
}

AssetNode::~AssetNode()
{
}

void AssetNode::SetName( const tstring& name )
{
	m_name = name;
}

const tstring_symbol& AssetNode::GetUUID() const
{
	return m_uuid;
}

AssetNode* AssetNode::SearchAssetFromUUID( const tstring_symbol& uuidSymbol )
{
	if( m_uuid == uuidSymbol )
	{
		return this;
	}

	AssetNode* child = GetChild();
	while( child )
	{
		AssetNode* findAsset = child->SearchAssetFromUUID(uuidSymbol);
		if( findAsset )
		{
			return findAsset;
		}

		child = child->GetSiblingNext();
	}

	return NULL;
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