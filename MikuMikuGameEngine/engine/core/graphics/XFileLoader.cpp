#include "stdafx.h"

#include "XFileLoader.h"

#include "Graphics.h"

#include <d3dx9.h>
#include <dxfile.h>
#include <rmxftmpl.h>
#include <rmxfguid.h>
#include <d3dx9mesh.h>
unsigned char SKINEXP_XTEMPLATES[] = XSKINEXP_TEMPLATES;
unsigned char XEXTENSIONS_XTEMPLATES[] = XEXTENSIONS_TEMPLATES;

#pragma warning( disable : 4996 )
#include <strsafe.h>
#pragma warning( default : 4996 )

#include <initguid.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HRESULT GetXFileDataName( tstring& name,LPD3DXFILEDATA pXofData )
{
	HRESULT hr = S_OK;

	DWORD cchName;
	hr = pXofData->GetName(NULL,&cchName);
	if( FAILED(hr) )
	{
		return hr;
	}

	if( cchName > 0 )
	{
		char* szName = NULL;
		if( NULL == (szName = new char[cchName]) )
		{
			hr = E_OUTOFMEMORY;
			return hr;
		}
		hr = pXofData->GetName( szName,&cchName );
		if( FAILED(hr) )
		{
			delete[] szName;
			return hr;
		}

		name = to_tstring(szName);
		delete[] szName;
	}

	return hr;
}

XFileLoader::XFileLoader()
	: m_scale(1.0f)
{
}

XFileLoader::~XFileLoader()
{
}

HRESULT XFileLoader::LoadMeshContainer( LPD3DXFILEDATA pXofData,MeshContainer* meshContainer )
{
	HRESULT hr = S_OK;

	Graphics* graphics = Graphics::GetInstance();
	IDirect3DDevice9Ptr pD3DDevice = graphics->GetDirect3DDevice();

	LPD3DXBUFFER pAdjacencyBuf = NULL;
	LPD3DXBUFFER pMaterialBuf = NULL;
	LPD3DXBUFFER pEffectInstancesBuf = NULL;
	DWORD Materials = 0;
	LPD3DXMESH pD3DMesh = NULL;

	hr = D3DXLoadMeshFromXof( pXofData,
		D3DXMESH_SYSTEMMEM,
		pD3DDevice,
		&pAdjacencyBuf,
		&pMaterialBuf,
		&pEffectInstancesBuf,
		&Materials,
		&pD3DMesh );
	if( FAILED(hr) )
	{
		goto exit;
	}

	if( !(pD3DMesh->GetFVF() & D3DFVF_NORMAL) )
    {
		LPD3DXMESH tmpMesh = NULL;

        // 柔軟な頂点フォーマット (FVF) コードを使ってメッシュのコピーを作成する
        hr = pD3DMesh->CloneMeshFVF(
            pD3DMesh->GetOptions(),
            pD3DMesh->GetFVF() | D3DFVF_NORMAL,
            pD3DDevice,
            &tmpMesh); // ←ここにコピー
        if(FAILED(hr))
        {
            goto exit;
        }
        // メッシュに含まれる各頂点の法線を計算して、設定する
        //D3DXComputeNormals( tmpMesh, reinterpret_cast<DWORD*>(pAdjacencyBuf->GetBufferPointer()) );
		D3DXComputeNormals( tmpMesh, NULL );

		pD3DMesh->Release();
		pD3DMesh = tmpMesh;
	}

	D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
	hr = pD3DMesh->GetDeclaration(pDecl);
	if( FAILED(hr) )
	{
		goto exit;
	}

	DWORD vertexNum = pD3DMesh->GetNumVertices();
	DWORD faceNum = pD3DMesh->GetNumFaces();

	DWORD attrNum = 0;
	pD3DMesh->GetAttributeTable(NULL, &attrNum);
	
	DWORD size = pD3DMesh->GetNumBytesPerVertex();

	BYTE* pD3DVertice = NULL;
	pD3DMesh->LockVertexBuffer( 0,(LPVOID*)&pD3DVertice );

	sVertex* vertices = new sVertex[vertexNum];

	for( DWORD vertIdx = 0;vertIdx<vertexNum;vertIdx++ )
	{
		sVertex* vertex = &vertices[vertIdx];
		vertex->uv = D3DXVECTOR2(0.0f,0.0f);
		vertex->color = 0xFFFFFFFF;

		for( DWORD i=0;i<MAX_FVF_DECL_SIZE;i++ )
		{
			D3DVERTEXELEMENT9& decl = pDecl[i];

			if( decl.Stream==0xFF )
			{
				break;
			}

			switch( decl.Usage )
			{
			case D3DDECLUSAGE_POSITION:
				vertex->position = *(D3DXVECTOR3*)(pD3DVertice+vertIdx*size+decl.Offset);
				vertex->position = vertex->position * m_scale;
				break;
			case D3DDECLUSAGE_NORMAL:
				vertex->normal = *(D3DXVECTOR3*)(pD3DVertice+vertIdx*size+decl.Offset);
				break;
			case D3DDECLUSAGE_TEXCOORD:
				vertex->uv = *(D3DXVECTOR2*)(pD3DVertice+vertIdx*size+decl.Offset);
				break;
			case D3DDECLUSAGE_COLOR:
				vertex->color = *(DWORD*)(pD3DVertice+vertIdx*size+decl.Offset);
				break;
			}
		}
	}

	pD3DMesh->UnlockVertexBuffer();

	LPDIRECT3DINDEXBUFFER9 pIndexBuffer = NULL;
	pD3DMesh->GetIndexBuffer( &pIndexBuffer );
	D3DINDEXBUFFER_DESC desc;
	pIndexBuffer->GetDesc( &desc );
	pIndexBuffer->Release();

	DWORD* indices = new DWORD[faceNum*3];

	if( desc.Format==D3DFMT_INDEX16 )
	{
		WORD* pD3DIndices = NULL;
		pD3DMesh->LockIndexBuffer(0,(LPVOID*)&pD3DIndices );
		for( DWORD i=0;i<faceNum*3;i++ )
		{
			indices[i] = pD3DIndices[i];
		}
		pD3DMesh->UnlockIndexBuffer();
	}
	else
	{
		DWORD* pD3DIndices =NULL;
		pD3DMesh->LockIndexBuffer(0,(LPVOID*)&pD3DIndices );
		memcpy( indices,pD3DIndices,sizeof(DWORD)*faceNum*3 );
		pD3DMesh->UnlockIndexBuffer();
	}

	D3DXATTRIBUTERANGE *attrList = new D3DXATTRIBUTERANGE[attrNum];
	pD3DMesh->GetAttributeTable(attrList, &attrNum);

	meshContainer->pMesh = new Mesh;
	meshContainer->pMesh->Create( vertexNum,faceNum,attrNum );

	meshContainer->pMesh->SetVertices( vertices );

	meshContainer->pMesh->SetIndices( indices );

	meshContainer->pMesh->SetAttributeRanges( attrList );

	delete[] vertices;
	delete[] indices;
	delete[] attrList;

	meshContainer->materialNum = Materials;
	meshContainer->pMaterials = new sMaterial[Materials];

	D3DXMATERIAL* pD3DMaterials = (D3DXMATERIAL*)pMaterialBuf->GetBufferPointer();

	for( DWORD i=0;i<Materials;i++ )
	{
		sMaterial* material = &meshContainer->pMaterials[i];

		D3DXMATERIAL* pD3DMaterial = &pD3DMaterials[i];

		tstring textureFileName;

		material->colorDiffuse = pD3DMaterial->MatD3D.Diffuse;
		if( pD3DMaterial->pTextureFilename && strlen(pD3DMaterial->pTextureFilename)>0 )
		{
			textureFileName = m_path+to_tstring(pD3DMaterial->pTextureFilename);
		}

		TexturePtr pTex = ResourceManager::GetInstance().GetResource<Texture>( textureFileName );
		if( !pTex )
		{
			pTex = TexturePtr(new Texture);
			if( !pTex->CreateFromFile( textureFileName ) )
			{
				pTex.reset();
			}
		}

		if( !pTex )
		{
			textureFileName = _T("<FFFFFFFF>");

			pTex = ResourceManager::GetInstance().GetResource<Texture>( textureFileName );
			if( !pTex )
			{
				pTex = TexturePtr(new Texture);
				if( !pTex->CreateDotColor( 0xFFFFFFFF ) )
				{
					pTex.reset();
				}
			}
		}

		if( pTex )
		{
			ResourceManager::GetInstance().AddResource( textureFileName,pTex );
			material->textureDiffuse = pTex;
		}
	}

exit:
	if( pMaterialBuf )
	{
		pMaterialBuf->Release();
		pMaterialBuf = NULL;
	}
	if( pEffectInstancesBuf )
	{
		pEffectInstancesBuf->Release();
		pEffectInstancesBuf = NULL;
	}
	if( pAdjacencyBuf )
	{
		pAdjacencyBuf->Release();
		pAdjacencyBuf = NULL;
	}
	if( pD3DMesh )
	{
		pD3DMesh->Release();
		pD3DMesh = NULL;
	}

	return hr;
}

HRESULT XFileLoader::LoadFrameNode( LPD3DXFILEDATA pXofData,ModelFrame* frame )
{
	HRESULT hr = S_OK;

	GUID type;
	hr = pXofData->GetType(&type);
	if( FAILED(hr) )
	{
		return hr;
	}

	tstring szName;
	hr = GetXFileDataName( szName,pXofData );
	if( FAILED(hr) )
	{
		return hr;
	}

	if( type == TID_D3DRMFrame )
	{
		ModelFrame* childFrame = new ModelFrame;
		childFrame->SetParent( frame );

		childFrame->SetName( szName );

		SIZE_T counts;
		pXofData->GetChildren( &counts );
		for( SIZE_T i = 0; i < counts; i++ )
		{
			LPD3DXFILEDATA pXofDataChild = NULL;
			pXofData->GetChild(i, &pXofDataChild);

			hr = LoadFrameNode( pXofDataChild,childFrame );

			pXofDataChild->Release();
		}
	}
	else if( type == TID_D3DRMFrameTransformMatrix )
	{
		SIZE_T cbSize;
		const D3DXMATRIX* transform;
		hr = pXofData->Lock(&cbSize, (const void **)&transform);
		if( SUCCEEDED(hr) )
		{
			D3DXVECTOR3 position;
			D3DXQUATERNION rotation;
			D3DXVECTOR3 scale;

			D3DXMatrixDecompose( &scale,&rotation,&position,transform );
			position = position * m_scale;

			D3DXMATRIX matTrans;
			D3DXMatrixTranslation( &matTrans,position.x,position.y,position.z );

			D3DXMATRIX matRotation;
			D3DXMatrixRotationQuaternion( &matRotation,&rotation );

			D3DXMATRIX matScale;
			D3DXMatrixScaling( &matScale,scale.x,scale.y,scale.z );

			frame->SetTransform( matScale * matRotation * matTrans );
			pXofData->Unlock();
		}
	}
	else if( type == TID_D3DRMMesh )
	{
		MeshContainer* meshContainer = new MeshContainer;
		
		hr = LoadMeshContainer( pXofData,meshContainer );
		if( FAILED(hr) )
		{
			delete meshContainer;
			return hr;
		}

		frame->SetMeshContainer( meshContainer );
	}

	return hr;
}

ModelPtr XFileLoader::Open( const tstring& filePath,float scale )
{
	ModelPtr pModel;

	m_fileName = filePath;

	TCHAR path[MAX_PATH];
	_tcscpy_s( path,MAX_PATH,filePath.c_str() );
	PathRemoveFileSpec( path );
	PathAddBackslash( path );
	m_path = path;

	m_scale = scale;

	LPD3DXFILE pXof = NULL;
	LPD3DXFILEENUMOBJECT pXofEnum = NULL;

	HRESULT hr = D3DXFileCreate( &pXof );

	if( FAILED(hr) )
	{
		goto exit;
	}

	hr = pXof->RegisterTemplates((void *)D3DRM_XTEMPLATES, D3DRM_XTEMPLATE_BYTES);
	if( FAILED(hr) )
	{
		goto exit;
	}
	hr = pXof->RegisterTemplates((void *)SKINEXP_XTEMPLATES, sizeof(SKINEXP_XTEMPLATES) - 1);
	if( FAILED(hr) )
	{
		goto exit;
	}
	hr = pXof->RegisterTemplates((void *)XEXTENSIONS_XTEMPLATES, sizeof(XEXTENSIONS_XTEMPLATES) - 1);
	if( FAILED(hr) )
	{
		goto exit;
	}

	hr = pXof->CreateEnumObject( (void*)to_string(m_fileName).c_str(),DXFILELOAD_FROMFILE,&pXofEnum );
	if( FAILED(hr) )
	{
		goto exit;
	}

	TCHAR filename[MAX_PATH];
	_tcscpy_s( filename,PathFindFileName( filePath.c_str() ) );
	PathRemoveExtension( filename );

	ModelFrame* pFrame = new ModelFrame;
	pFrame->SetName( filename );

	SIZE_T counts = 0;
	pXofEnum->GetChildren(&counts);
	for( SIZE_T i = 0; i < counts; i++ )
	{
		LPD3DXFILEDATA pXofData = NULL;
		pXofEnum->GetChild(i, &pXofData);

		LoadFrameNode( pXofData,pFrame );

		pXofData->Release();
	}

	pModel = ModelPtr( new Model(pFrame) );

	SetModelRef( pModel,pFrame );

exit:

	if( pXofEnum )
	{
		pXofEnum->Release();
		pXofEnum = NULL;
	}
	if( pXof )
	{
		pXof->Release();
		pXof = NULL;
	}

	return pModel;
}
