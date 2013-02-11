#pragma once

#pragma once

#include "core/util/CharSet.h"
#include "core/util/TreeNode.h"

class AssetNode : public TreeNode<AssetNode>
{
public:
	enum Type
	{
		Directory,
		PMDFile,
		VMDFile,
		XFile,
		EffectFile,
		NutFile,
		UnknownFile,
	};

private:
	Type m_type;

	tstring m_name;

	tstring_symbol m_uuid;

	tstring m_path;

public:
	AssetNode(Type type);
	virtual ~AssetNode();

	Type GetType() const;

	const tstring_symbol& GetUUID() const;

	AssetNode* SearchAssetFromUUID( const tstring_symbol& uuidSymbol );

	void SetName( const tstring& name );
	const tstring& GetName() const;

	void SetPath( const tstring& path );
	const tstring& GetPath() const;
};







	