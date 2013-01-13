#include "stdafx.h"

#include "PMDModel.h"

bool PMDLoad(unsigned char* buffer,size_t size,sPMD* pmd)
{
	if( !pmd )
	{
		return false;
	}

	memset(pmd,0,sizeof(sPMD));

	strcpy_s( pmd->toon_list.toon_file_name[0],"toon01.bmp" );
	strcpy_s( pmd->toon_list.toon_file_name[1],"toon02.bmp" );
	strcpy_s( pmd->toon_list.toon_file_name[2],"toon03.bmp" );
	strcpy_s( pmd->toon_list.toon_file_name[3],"toon04.bmp" );
	strcpy_s( pmd->toon_list.toon_file_name[4],"toon05.bmp" );
	strcpy_s( pmd->toon_list.toon_file_name[5],"toon06.bmp" );
	strcpy_s( pmd->toon_list.toon_file_name[6],"toon07.bmp" );
	strcpy_s( pmd->toon_list.toon_file_name[7],"toon08.bmp" );
	strcpy_s( pmd->toon_list.toon_file_name[8],"toon09.bmp" );
	strcpy_s( pmd->toon_list.toon_file_name[9],"toon10.bmp" );

	size_t sz=0;

	{
		sz=sizeof(sPMD_Header);
		if( size<sz )
		{
			PMDRelease(pmd);
			return false;
		}
		memcpy(&pmd->header,buffer,sz);
		size-=sz;
		buffer+=sz;
	}

	if( strncmp(pmd->header.magic,"Pmd",3)!=0 )
	{
		PMDRelease(pmd);
		return false;
	}

	{
		sz=sizeof(unsigned long);
		if( size<sz )
		{
			PMDRelease(pmd);
			return false;
		}
		memcpy(&pmd->vertex_list.vert_count,buffer,sz);
		size-=sz;
		buffer+=sz;

		sz=sizeof(sPMD_Vertex)*pmd->vertex_list.vert_count;
		if( size<sz )
		{
			PMDRelease(pmd);
			return false;
		}
		pmd->vertex_list.vertex=new sPMD_Vertex[pmd->vertex_list.vert_count];
		memcpy(pmd->vertex_list.vertex,buffer,sz);
		size-=sz;
		buffer+=sz;
	}

	{
		sz=sizeof(unsigned long);
		if( size<sz )
		{
			PMDRelease(pmd);
			return false;
		}
		memcpy(&pmd->index_list.face_vert_count,buffer,sz);
		size-=sz;
		buffer+=sz;
		
		sz=sizeof(unsigned short)*pmd->index_list.face_vert_count;
		if( size<sz )
		{
			PMDRelease(pmd);
			return false;
		}
		pmd->index_list.face_vert_index=new unsigned short[pmd->index_list.face_vert_count];
		memcpy(pmd->index_list.face_vert_index,buffer,sz);
		size-=sz;
		buffer+=sz;
	}

	{
		sz=sizeof(unsigned long);
		if( size<sz )
		{
			PMDRelease(pmd);
			return false;
		}
		memcpy(&pmd->material_list.material_count,buffer,sz);
		size-=sz;
		buffer+=sz;

		sz=sizeof(sPMD_Material)*pmd->material_list.material_count;
		if( size<sz )
		{
			PMDRelease(pmd);
			return false;
		}
		pmd->material_list.material=new sPMD_Material[pmd->material_list.material_count];
		memcpy(pmd->material_list.material,buffer,sz);
		size-=sz;
		buffer+=sz;
	}

	{
		sz=sizeof(unsigned short);
		if( size<sz )
		{
			PMDRelease(pmd);
			return false;
		}
		memcpy(&pmd->bone_list.bone_count,buffer,sz);
		size-=sz;
		buffer+=sz;

		sz=sizeof(sPMD_Bone)*pmd->bone_list.bone_count;
		if( size<sz )
		{
			PMDRelease(pmd);
			return false;
		}
		pmd->bone_list.bone=new sPMD_Bone[pmd->bone_list.bone_count];
		memcpy(pmd->bone_list.bone,buffer,sz);
		size-=sz;
		buffer+=sz;
	}

	{
		sz=sizeof(unsigned short);
		if( size<sz )
		{
			PMDRelease(pmd);
			return false;
		}
		memcpy(&pmd->ik_data_list.ik_data_count,buffer,sz);
		size-=sz;
		buffer+=sz;

		pmd->ik_data_list.ik_data=new sPMD_IKData[pmd->ik_data_list.ik_data_count];
		memset(pmd->ik_data_list.ik_data,0,sizeof(sPMD_IKData)*pmd->ik_data_list.ik_data_count);

		for( unsigned short idx=0;idx<pmd->ik_data_list.ik_data_count;idx++ )
		{
			sPMD_IKData* ikData=&pmd->ik_data_list.ik_data[idx];

			sz=sizeof(unsigned short);
			if( size<sz )
			{
				PMDRelease(pmd);
				return false;
			}
			memcpy(&ikData->ik_bone_index,buffer,sz);
			size-=sz;
			buffer+=sz;
			
			sz=sizeof(unsigned short);
			if( size<sz )
			{
				PMDRelease(pmd);
				return false;
			}
			memcpy(&ikData->ik_target_bone_index,buffer,sz);
			size-=sz;
			buffer+=sz;
			
			sz=sizeof(unsigned char);
			if( size<sz )
			{
				PMDRelease(pmd);
				return false;
			}
			memcpy(&ikData->ik_chain_length,buffer,sz);
			size-=sz;
			buffer+=sz;
			
			sz=sizeof(unsigned short);
			if( size<sz )
			{
				PMDRelease(pmd);
				return false;
			}
			memcpy(&ikData->iterations,buffer,sz);
			size-=sz;
			buffer+=sz;
			
			sz=sizeof(float);
			if( size<sz )
			{
				PMDRelease(pmd);
				return false;
			}
			memcpy(&ikData->control_weight,buffer,sz);
			size-=sz;
			buffer+=sz;
			
			sz=sizeof(unsigned short)*ikData->ik_chain_length;
			if( size<sz )
			{
				PMDRelease(pmd);
				return false;
			}
			ikData->ik_child_bone_index = new unsigned short[ikData->ik_chain_length];
			memcpy(ikData->ik_child_bone_index,buffer,sz);
			size-=sz;
			buffer+=sz;
		}
	}

	{
		sz=sizeof(unsigned short);
		if( size<sz )
		{
			PMDRelease(pmd);
			return false;
		}
		memcpy(&pmd->skin_list.skin_count,buffer,sz);
		size-=sz;
		buffer+=sz;

		pmd->skin_list.skin=new sPMD_Skin[pmd->skin_list.skin_count];
		memset(pmd->skin_list.skin,0,sizeof(sPMD_Skin)*pmd->skin_list.skin_count);

		for( unsigned short idx=0;idx<pmd->skin_list.skin_count;idx++ )
		{
			sPMD_Skin* skin=&pmd->skin_list.skin[idx];

			sz=sizeof(char)*20;
			if( size<sz )
			{
				PMDRelease(pmd);
				return false;
			}
			memcpy(skin->skin_name,buffer,sz);
			size-=sz;
			buffer+=sz;

			sz=sizeof(unsigned long);
			if( size<sz )
			{
				PMDRelease(pmd);
				return false;
			}
			memcpy(&skin->skin_vert_count,buffer,sz);
			size-=sz;
			buffer+=sz;

			sz=sizeof(unsigned char);
			if( size<sz )
			{
				PMDRelease(pmd);
				return false;
			}
			memcpy(&skin->skin_type,buffer,sz);
			size-=sz;
			buffer+=sz;

			sz=sizeof(sPMD_SkinVertex)*skin->skin_vert_count;
			if( size<sz )
			{
				PMDRelease(pmd);
				return false;
			}
			skin->skin_vert = new sPMD_SkinVertex[skin->skin_vert_count];
			memcpy(skin->skin_vert,buffer,sz);
			size-=sz;
			buffer+=sz;
		}
	}

	{
		sz=sizeof(unsigned char);
		if( size<sz )
		{
			PMDRelease(pmd);
			return false;
		}
		memcpy(&pmd->skin_disp.skin_disp_count,buffer,sz);
		size-=sz;
		buffer+=sz;

		sz=sizeof(unsigned short)*pmd->skin_disp.skin_disp_count;
		if( size<sz )
		{
			PMDRelease(pmd);
			return false;
		}
		pmd->skin_disp.skin_index=new unsigned short[pmd->skin_disp.skin_disp_count];
		memcpy(pmd->skin_disp.skin_index,buffer,sz);
		size-=sz;
		buffer+=sz;
	}

	{
		sz=sizeof(unsigned char);
		if( size<sz )
		{
			PMDRelease(pmd);
			return false;
		}
		memcpy(&pmd->bone_disp_name_list.bone_disp_name_count,buffer,sz);
		size-=sz;
		buffer+=sz;

		sz=sizeof(sPMD_BoneDispName)*pmd->bone_disp_name_list.bone_disp_name_count;
		if( size<sz )
		{
			PMDRelease(pmd);
			return false;
		}
		pmd->bone_disp_name_list.dips_name=new sPMD_BoneDispName[pmd->bone_disp_name_list.bone_disp_name_count];
		memcpy(pmd->bone_disp_name_list.dips_name,buffer,sz);
		size-=sz;
		buffer+=sz;
	}

	{
		sz=sizeof(unsigned long);
		if( size<sz )
		{
			PMDRelease(pmd);
			return false;
		}
		memcpy(&pmd->bone_disp_list.bone_disp_count,buffer,sz);
		size-=sz;
		buffer+=sz;

		sz=sizeof(sPMD_BoneDisp)*pmd->bone_disp_list.bone_disp_count;
		if( size<sz )
		{
			PMDRelease(pmd);
			return false;
		}
		pmd->bone_disp_list.bone_disp=new sPMD_BoneDisp[pmd->bone_disp_list.bone_disp_count];
		memcpy(pmd->bone_disp_list.bone_disp,buffer,sz);
		size-=sz;
		buffer+=sz;
	}

	if( size==0 )
	{
		return true;
	}

	// âpñºëŒâû
	{
		unsigned char english_name_compatibility;

		sz=sizeof(unsigned char);
		if( size<sz )
		{
			PMDRelease(pmd);
			return false;
		}
		memcpy(&english_name_compatibility,buffer,sz);
		size-=sz;
		buffer+=sz;

		if( english_name_compatibility==1 )
		{
			// âpåÍëŒâûÇîÚÇŒÇ∑
			//sPMD_EnglishHeader englishHeader;
			sz = sizeof(sPMD_EnglishHeader);
			if( size<sz )
			{
				PMDRelease(pmd);
				return false;
			}
			//memcpy(&englishHeader,buffer,sz );
			size-=sz;
			buffer+=sz;

			// É{Å[Éìñº(âpåÍ)
			sz = sizeof(char)*20*pmd->bone_list.bone_count;
			if( size<sz )
			{
				PMDRelease(pmd);
				return false;
			}
			size-=sz;
			buffer+=sz;

			// ï\èÓ(âpåÍ)
			sz = sizeof(char)*20*(pmd->skin_list.skin_count-1);
			if( size<sz )
			{
				PMDRelease(pmd);
				return false;
			}
			size-=sz;
			buffer+=sz;

			// É{Å[ÉìògópògñºÉäÉXÉg(âpåÍ)
			sz = sizeof(char)*50*pmd->bone_disp_name_list.bone_disp_name_count;
			if( size<sz )
			{
				PMDRelease(pmd);
				return false;
			}
			size-=sz;
			buffer+=sz;
		}
	}

	// ToonéwíË
	{
		// ToonéwíËÇîÚÇŒÇ∑
		sz = sizeof(char)*100*10;
		if( size<sz )
		{
			PMDRelease(pmd);
			return false;
		}
		memcpy( pmd->toon_list.toon_file_name,buffer,sz );
		size-=sz;
		buffer+=sz;
	}

	// ï®óùââéZ
	{
		sz = sizeof(unsigned long);
		if( size<sz )
		{
			PMDRelease(pmd);
			return false;
		}
		memcpy(&pmd->rigidbody_list.rigidbody_count,buffer,sz );
		size-=sz;
		buffer+=sz;

		sz=sizeof(sPMD_RigidBody)*pmd->rigidbody_list.rigidbody_count;
		if( size<sz )
		{
			PMDRelease(pmd);
			return false;
		}
		pmd->rigidbody_list.rigidbody=new sPMD_RigidBody[pmd->rigidbody_list.rigidbody_count];
		memcpy(pmd->rigidbody_list.rigidbody,buffer,sz);
		size-=sz;
		buffer+=sz;

		sz = sizeof(unsigned long);
		if( size<sz )
		{
			PMDRelease(pmd);
			return false;
		}
		memcpy(&pmd->joint_list.joint_count,buffer,sz );
		size-=sz;
		buffer+=sz;

		sz=sizeof(sPMD_Joint)*pmd->joint_list.joint_count;
		if( size<sz )
		{
			PMDRelease(pmd);
			return false;
		}
		pmd->joint_list.joint=new sPMD_Joint[pmd->joint_list.joint_count];
		memcpy(pmd->joint_list.joint,buffer,sz);
		size-=sz;
		buffer+=sz;
	}

	return true;
}

void PMDRelease(sPMD* pmd)
{
	if( !pmd )
	{
		return;
	}

	delete[] pmd->vertex_list.vertex;
	delete[] pmd->index_list.face_vert_index;
	delete[] pmd->material_list.material;
	delete[] pmd->bone_list.bone;
	for( unsigned short idx=0;idx<pmd->ik_data_list.ik_data_count;idx++ )
	{
		delete[] pmd->ik_data_list.ik_data[idx].ik_child_bone_index;
	}
	delete[] pmd->ik_data_list.ik_data;
	for( unsigned short idx=0;idx<pmd->skin_list.skin_count;idx++ )
	{
		delete[] pmd->skin_list.skin[idx].skin_vert;
	}
	delete[] pmd->skin_list.skin;
	delete[] pmd->skin_disp.skin_index;
	delete[] pmd->bone_disp_name_list.dips_name;
	delete[] pmd->bone_disp_list.bone_disp;
	delete[] pmd->rigidbody_list.rigidbody;
	delete[] pmd->joint_list.joint;

	memset(pmd,0,sizeof(sPMD));
}

PMDModel::PMDModel( sPMD* pPmd,DWORD materialNum,sPMDMaterial* pMaterials )
	: m_pPmd(pPmd)
	, m_materialNum(materialNum)
	, m_pMaterials(pMaterials)
{
}

PMDModel::~PMDModel()
{
	if( m_pMaterials )
	{
		delete[] m_pMaterials;
		m_pMaterials = NULL;
	}

	if( m_pPmd )
	{
		PMDRelease( m_pPmd );
		delete m_pPmd;
		m_pPmd = NULL;
	}
}

sPMD* PMDModel::GetData()
{
	return m_pPmd;
}

DWORD PMDModel::GetMaterialNum()
{
	return m_materialNum;
}

sPMDMaterial* PMDModel::GetMaterials()
{
	return m_pMaterials;
}