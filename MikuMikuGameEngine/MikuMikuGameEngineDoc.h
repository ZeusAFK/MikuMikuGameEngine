
// MikuMikuGameEngineDoc.h : CMikuMikuGameEngineDoc �N���X�̃C���^�[�t�F�C�X
//


#pragma once

#include "engine/GameObject.h"
#include "engine/AssetNode.h"


class CMikuMikuGameEngineDoc : public CDocument
{
protected: // �V���A��������̂ݍ쐬���܂��B
	CMikuMikuGameEngineDoc();
	DECLARE_DYNCREATE(CMikuMikuGameEngineDoc)

// ����
private:
	GameObject* m_pRoot;
	GameObject* m_pSelectObject;

	AssetNode* m_pAssetRoot;
	AssetNode* m_pSelectAsset;

// ����
public:
	GameObject* GetRootGameObject();
	GameObject* GetSelectGameObject();
	void AddGameObject( GameObject* obj,GameObject* parent,bool select );
	void SetObjectName( GameObject* obj, const tstring& name );
	void DeleteObject( GameObject* obj );
	void SetObjectParent( GameObject* obj,GameObject* parent );
	void SetSelectObject( GameObject* obj );

	AssetNode* GetRootAsset();
	AssetNode* GetSelectAsset();
	void AddAsset( AssetNode* asset,AssetNode* parent,bool select );
	void AddAssetFiles( const std::vector<tstring>& filePaths,AssetNode* parent,bool select );
	void AddAssetFolder( const tstring& folderName,AssetNode* parent,bool select );
	void SetAssetName( AssetNode* asset, const tstring& name );
	void DeleteAsset( AssetNode* asset );
	void SetAssetParent( AssetNode* asset,AssetNode* parent );
	void SetSelectAsset( AssetNode* asset );
	void DropAsset( AssetNode* asset,CWnd* pDropWnd,const POINT& pt );

// �I�[�o�[���C�h
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ����
public:
	virtual ~CMikuMikuGameEngineDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
protected:
	DECLARE_MESSAGE_MAP()
};


