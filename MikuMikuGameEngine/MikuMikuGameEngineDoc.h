
// MikuMikuGameEngineDoc.h : CMikuMikuGameEngineDoc クラスのインターフェイス
//


#pragma once

#include "engine/GameObject.h"


class CMikuMikuGameEngineDoc : public CDocument
{
protected: // シリアル化からのみ作成します。
	CMikuMikuGameEngineDoc();
	DECLARE_DYNCREATE(CMikuMikuGameEngineDoc)

// 属性
private:
	GameObject* m_pRoot;

	GameObject* m_pSelectObject;

// 操作
public:
	GameObject* GetRootGameObject();

	GameObject* GetSelectGameObject();

	void AddGameObject( GameObject* obj,GameObject* parent,bool select );

	void SetObjectName( GameObject* obj, const tstring& name );

	void DeleteObject( GameObject* obj );

	void SetObjectParent( GameObject* obj,GameObject* parent );

	void SetSelectObject( GameObject* obj );

// オーバーライド
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 実装
public:
	virtual ~CMikuMikuGameEngineDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成された、メッセージ割り当て関数
protected:
	DECLARE_MESSAGE_MAP()
};


