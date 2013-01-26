#include "stdafx.h"

#include "PMDFileLoader.h"

PMDFileLoader::PMDFileLoader()
{
}

PMDFileLoader::~PMDFileLoader()
{
}

PMDModelPtr PMDFileLoader::Open( const tstring& filePath )
{
	m_fileName = filePath;

	TCHAR path[MAX_PATH];
	_tcscpy_s( path,MAX_PATH,filePath.c_str() );
	PathRemoveFileSpec( path );
	PathAddBackslash( path );
	m_path = path;

	FILE* fp=NULL;
	if( _tfopen_s(&fp,m_fileName.c_str(),_T("rb"))!=0 )
	{
		return PMDModelPtr();
	}

	fpos_t fsize = 0;
	fseek(fp,0,SEEK_END);
	fgetpos(fp,&fsize);

	fseek(fp,0,SEEK_SET); 

	size_t sz=(size_t)fsize;

	unsigned char* buffer=new unsigned char[sz];
	fread(buffer,1,sz,fp);

	fclose(fp);

	sPMD* pmd = new sPMD;
	bool ret = PMDLoad(buffer,sz,pmd);
	
	delete[] buffer;

	if( !ret )
	{
		delete pmd;
		return PMDModelPtr();
	}

	sPMD_Skin* skinBase = NULL;

	for( DWORD skinIdx=0;skinIdx<pmd->skin_list.skin_count;skinIdx++ )
	{
		sPMD_Skin* skin = &pmd->skin_list.skin[skinIdx];

		if( skin->skin_type == ePMD_SkinType_Base )
		{
			skinBase = skin;

			for( DWORD vertIdx = 0; vertIdx < skin->skin_vert_count ; vertIdx++ )
			{
				DWORD targetIndex = skin->skin_vert[vertIdx].index;
				memcpy( pmd->vertex_list.vertex[ targetIndex ].pos, skin->skin_vert[vertIdx].pos,sizeof(float)*3 );
			}
			
			break;
		}
	}

	for( DWORD skinIdx=0;skinIdx<pmd->skin_list.skin_count;skinIdx++ )
	{
		sPMD_Skin* skin = &pmd->skin_list.skin[skinIdx];

		if( skin->skin_type != ePMD_SkinType_Base )
		{
			for( DWORD vertIdx = 0; vertIdx < skin->skin_vert_count ; vertIdx++ )
			{
				DWORD targetIndex = skin->skin_vert[vertIdx].index;
				skin->skin_vert[vertIdx].index = skinBase->skin_vert[ targetIndex ].index;
			}
		}
	}

	Graphics* graphics = Graphics::GetInstance();

	sMaterial* pMaterials = new sMaterial[pmd->material_list.material_count];

	for( DWORD i=0;i<pmd->material_list.material_count;i++ )
	{
		sPMD_Material* pmdMat = &pmd->material_list.material[i];
		sMaterial* pMaterial = &pMaterials[i];

		pMaterial->colorDiffuse.r = 0.0f;
		pMaterial->colorDiffuse.g = 0.0f;
		pMaterial->colorDiffuse.b = 0.0f;
		pMaterial->colorDiffuse.a = pmdMat->alpha;

		pMaterial->colorAmbient.r = pmdMat->diffuse_color[0];
		pMaterial->colorAmbient.g = pmdMat->diffuse_color[1];
		pMaterial->colorAmbient.b = pmdMat->diffuse_color[2];
		pMaterial->colorAmbient.a = 0.0f;

		pMaterial->colorSpecular.r = pmdMat->specular_color[0];
		pMaterial->colorSpecular.g = pmdMat->specular_color[1];
		pMaterial->colorSpecular.b = pmdMat->specular_color[2];
		pMaterial->colorSpecular.a = 0.0f;

		pMaterial->colorEmissive.r = pmdMat->ambient_color[0];
		pMaterial->colorEmissive.g = pmdMat->ambient_color[1];
		pMaterial->colorEmissive.b = pmdMat->ambient_color[2];
		pMaterial->colorEmissive.a = 0.0f;

		pMaterial->specularPower = pmdMat->specularity;

		pMaterial->spheremap = eSPHEREMAP_MUL;

		pMaterial->edge = pmdMat->edge_flag!=0;

		TCHAR path[MAX_PATH];
		_tcscpy_s( path,m_path.c_str() );

		tstring texFileName;
		tstring sphereFileName;

		if( strlen(pmdMat->texture_file_name)>0 )
		{
			tstring filename = to_tstring(pmdMat->texture_file_name);

			tstring::size_type index = filename.find( _T("*") );
			if( index != tstring::npos )
			{
				sphereFileName = filename.substr( index+1 );
				PathAppend( path,sphereFileName.c_str() );
				sphereFileName = path;
				PathRemoveFileSpec( path );

				texFileName = filename.erase( index );
				PathAppend( path,texFileName.c_str() );
				texFileName = path;
				PathRemoveFileSpec( path );
			}
			else
			{
				texFileName = filename;
				PathAppend( path,texFileName.c_str() );
				texFileName = path;
				PathRemoveFileSpec( path );
			}

			tstring ext = PathFindExtension( texFileName.c_str() );

			if( ext == _T(".sph" ) || ext == _T(".spa") )
			{
				sphereFileName = texFileName;
				texFileName = _T("");
			}
		}

		if( !texFileName.empty() )
		{
			TexturePtr pTex = ResourceManager::GetInstance().GetResource<Texture>( texFileName );
			if( !pTex )
			{
				pTex = TexturePtr(new Texture);
				pTex->CreateFromFile( texFileName );

				ResourceManager::GetInstance().AddResource( texFileName,pTex );
			}

			pMaterial->textureDiffuse = pTex;
		}

		if( !sphereFileName.empty() )
		{
			TexturePtr pTex = ResourceManager::GetInstance().GetResource<Texture>( sphereFileName );
			if( !pTex )
			{
				pTex = TexturePtr(new Texture);
				pTex->CreateFromFile( sphereFileName );

				ResourceManager::GetInstance().AddResource( sphereFileName,pTex );
			}

			pMaterial->textureSphere = pTex;

			tstring ext = PathFindExtension( sphereFileName.c_str() );
			if( ext == _T(".sph" ) )
			{
				pMaterial->spheremap = eSPHEREMAP_MUL;
			}
			else if( ext == _T(".spa") )
			{
				pMaterial->spheremap = eSPHEREMAP_ADD;
			}
		}

		pMaterial->colorToon = D3DXCOLOR( 1.0f,1.0f,1.0f,1.0f );

		tstring toonTexName = _T("");
		tstring toonTexPath = _T("");

		if( 0<=pmdMat->toon_index && pmdMat->toon_index<10 )
		{
			// TODO:デフォルトのtoonファイルは固定パスか・・・
			toonTexName = to_tstring( pmd->toon_list.toon_file_name[pmdMat->toon_index] );
		}

		TexturePtr pTex;
		
		if( !toonTexName.empty() )
		{
			PathAppend( path,toonTexName.c_str() );
			toonTexPath = path;
			PathRemoveFileSpec( path );

			pTex = ResourceManager::GetInstance().GetResource<Texture>( toonTexPath );
			if( !pTex )
			{
				pTex = TexturePtr(new Texture);
				if( !pTex->CreateFromFile( toonTexPath ) )
				{
					pTex.reset();
				}
			}
		}

		if( !pTex )
		{
			pTex = graphics->GetDefaultToonTexture( pmdMat->toon_index );
			if( !pTex )
			{
				toonTexPath = _T("<FFFFFFFF>");

				pTex = ResourceManager::GetInstance().GetResource<Texture>( toonTexPath );
				if( !pTex )
				{
					pTex = TexturePtr(new Texture);
					if( !pTex->CreateDotColor( 0xFFFFFFFF ) )
					{
						pTex.reset();
					}
				}
			}
		}
		
		if( pTex )
		{
			pMaterial->textureToon = pTex;

			ResourceManager::GetInstance().AddResource( toonTexPath,pTex );

			IDirect3DTexture9Ptr pD3DTexture = pTex->GetTexture();

			D3DSURFACE_DESC desc;
			pD3DTexture->GetLevelDesc( 0,&desc );

			D3DLOCKED_RECT lockRect;
			pD3DTexture->LockRect(0, &lockRect, NULL, 0);

			int x = 0;
			int y = desc.Height-1;

			DWORD color;

			memcpy(&color,(BYTE*)lockRect.pBits + lockRect.Pitch*y + 4*x, sizeof(DWORD) );

			pD3DTexture->UnlockRect(0);

			pMaterial->colorToon = D3DXCOLOR( color );
		}
	}

	return PMDModelPtr( new PMDModel(pmd,pmd->material_list.material_count,pMaterials) );
}