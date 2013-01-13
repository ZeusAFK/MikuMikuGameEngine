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

	sPMDMaterial* pMaterials = new sPMDMaterial[pmd->material_list.material_count];

	for( DWORD i=0;i<pmd->material_list.material_count;i++ )
	{
		sPMD_Material* pmdMat = &pmd->material_list.material[i];
		sPMDMaterial* pMaterial = &pMaterials[i];

		pMaterial->colorDiffuse.r = pmdMat->diffuse_color[0];
		pMaterial->colorDiffuse.g = pmdMat->diffuse_color[1];
		pMaterial->colorDiffuse.b = pmdMat->diffuse_color[2];
		pMaterial->colorDiffuse.a = pmdMat->alpha;

		pMaterial->colorSpecular.r = pmdMat->specular_color[0];
		pMaterial->colorSpecular.g = pmdMat->specular_color[1];
		pMaterial->colorSpecular.b = pmdMat->specular_color[2];
		pMaterial->colorSpecular.a = 0.0f;

		pMaterial->colorAmbient.r = pmdMat->ambient_color[0];
		pMaterial->colorAmbient.g = pmdMat->ambient_color[1];
		pMaterial->colorAmbient.b = pmdMat->ambient_color[2];
		pMaterial->colorAmbient.a = 0.0f;

		pMaterial->specularPower = pmdMat->specularity;

		pMaterial->spheremap = eSPHEREMAP_MUL;

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
				texFileName = _T("");
				sphereFileName = texFileName;
			}
		}

		if( texFileName.empty() )
		{
			texFileName = _T("project\\assets\\white.jpg");
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

		tstring toonTexName = _T("");
		tstring toonTexPath = _T("");

		if( 0<=pmdMat->toon_index && pmdMat->toon_index<10 )
		{
			// TODO:デフォルトのtoonファイルは固定パスか・・・
			toonTexName = to_tstring( pmd->toon_list.toon_file_name[pmdMat->toon_index] );

			PathAppend( path,toonTexName.c_str() );
			toonTexPath = path;
			PathRemoveFileSpec( path );
		}
		else
		{
			toonTexPath = _T("project\\assets\\white.jpg");
		}

		{
			TexturePtr pTex = ResourceManager::GetInstance().GetResource<Texture>( toonTexPath );
			if( !pTex )
			{
				pTex = TexturePtr(new Texture);
				pTex->CreateFromFile( toonTexPath );

				ResourceManager::GetInstance().AddResource( toonTexPath,pTex );
			}

			pMaterial->textureToon = pTex;
		}

		if( !pMaterial->textureToon )
		{
			// dataフォルダ直下から検索
			TCHAR _path[MAX_PATH] = _T("project\\assets\\");

			PathAppend( _path,toonTexName.c_str() );

			{
				tstring textureFileName = _path;
				TexturePtr pTex = ResourceManager::GetInstance().GetResource<Texture>( textureFileName );
				if( !pTex )
				{
					pTex = TexturePtr(new Texture);
					pTex->CreateFromFile( textureFileName );

					ResourceManager::GetInstance().AddResource( textureFileName,pTex );
				}

				pMaterial->textureToon = pTex;
			}

			PathRemoveFileSpec( path );
		}
	}

	return PMDModelPtr( new PMDModel(pmd,pmd->material_list.material_count,pMaterials) );
}