#include "stdafx.h"

#include "PMDModelRenderer.h"

#include "../../GameObject.h"
#include "../util/util.h"

PMDModelRenderer::PMDModelRenderer()
	: m_gameObject(NULL)
	, m_pMesh(NULL)
	, m_ppShaders(NULL)
	, m_ppBoneList(NULL)
	, m_pOffsetMatrices(NULL)
	, m_rootBoneCount(0)
	, m_ppRootBoneList(NULL)
	, m_skinWeights(NULL)
{
}

PMDModelRenderer::~PMDModelRenderer()
{
	if( m_pMesh )
	{
		delete m_pMesh;
		m_pMesh = NULL;
	}

	if( m_ppShaders )
	{
		delete[] m_ppShaders;
		m_ppShaders = NULL;
	}

	if( m_ppRootBoneList )
	{
		for( DWORD idx=0;idx<m_rootBoneCount;idx++ )
		{
			delete m_ppRootBoneList[idx];
		}
		delete[] m_ppRootBoneList;
		m_ppRootBoneList = NULL;
	}

	if( m_ppBoneList )
	{
		delete[] m_ppBoneList;
		m_ppBoneList = NULL;
	}

	if( m_pOffsetMatrices )
	{
		delete[] m_pOffsetMatrices;
		m_pOffsetMatrices = NULL;
	}

	if( m_skinWeights )
	{
		delete[] m_skinWeights;
		m_skinWeights = NULL;
	}
}

void PMDModelRenderer::SetGameObject( GameObject* gameObject )
{
	m_gameObject = gameObject;
}

void PMDModelRenderer::SetModel( PMDModelPtr pModel )
{
	if( m_pModel==pModel )
	{
		return;
	}

	m_pModel = pModel;

	if( !m_pMesh )
	{
		m_pMesh = new Mesh;
	}

	sPMD* pPmd = m_pModel->GetData();

	m_pMesh->Create( pPmd->vertex_list.vert_count,pPmd->index_list.face_vert_count,pPmd->material_list.material_count );

	sVertex* vertices = m_pMesh->GetVertices();

	for( DWORD i=0;i<pPmd->vertex_list.vert_count;i++ )
	{
		sPMD_Vertex* pmdVertex = &pPmd->vertex_list.vertex[i];
		sVertex* vertex = &vertices[i];

		vertex->position = D3DXVECTOR3(pmdVertex->pos);
		vertex->normal = D3DXVECTOR3(pmdVertex->normal_vec);
		vertex->color = 0xFFFFFFFF;
		vertex->uv = D3DXVECTOR2(pmdVertex->uv);
	}

	DWORD* indices = m_pMesh->GetIndices();
	for( DWORD i=0;i<pPmd->index_list.face_vert_count;i++ )
	{
		indices[i] = pPmd->index_list.face_vert_index[i];
	}

	DWORD faceStart = 0;

	D3DXATTRIBUTERANGE* pAttrs = m_pMesh->GetAttributeRanges();
	for( DWORD i=0;i<pPmd->material_list.material_count;i++ )
	{
		sPMD_Material* pmdMaterial = &pPmd->material_list.material[i];
		D3DXATTRIBUTERANGE* pAttr = &pAttrs[i];

		pAttr->AttribId = i;
		pAttr->FaceStart = faceStart/3;
		pAttr->FaceCount = pmdMaterial->face_vert_count/3;
		pAttr->VertexStart = 0;
		pAttr->VertexCount = pPmd->vertex_list.vert_count;

		faceStart += pmdMaterial->face_vert_count;
	}

	m_skinWeights = new SkinWeight[ pPmd->skin_list.skin_count ];
	ZeroMemory( m_skinWeights,sizeof(float)*pPmd->skin_list.skin_count );
	
	m_ppShaders = new ShaderPtr[ pPmd->material_list.material_count ];

	m_ppBoneList = new PMDBone*[ pPmd->bone_list.bone_count ];

	for( DWORD idx=0;idx<pPmd->bone_list.bone_count;idx++ )
	{
		m_ppBoneList[idx] = new PMDBone;
	}

	m_rootBoneCount = 0;

	for( DWORD idx=0;idx<pPmd->bone_list.bone_count;idx++ )
	{
		PMDBone* pmdBone = m_ppBoneList[idx];
		sPMD_Bone* pPmdBone = &pPmd->bone_list.bone[idx];

		pmdBone->SetName( to_tstring( pPmdBone->bone_name ) );

		ePMD_BoneType boneType = (ePMD_BoneType)pPmdBone->bone_type;

		pmdBone->SetBoneType( boneType );

		if( boneType == ePMD_BoneType_Gyration )
		{
			pmdBone->SetInfluenceBoneIndex( pPmdBone->tail_pos_bone_index );
			short weight = pPmdBone->ik_parent_bone_index;
			pmdBone->SetInfluenceWeight( ((float)weight)/100.0f );
		}

		D3DXVECTOR3 pos(pPmdBone->bone_head_pos);
		
		PMDBone* parent = NULL;

		if( pPmdBone->parent_bone_index != (unsigned short)-1 )
		{
			sPMD_Bone* pPmdBoneParent = &pPmd->bone_list.bone[pPmdBone->parent_bone_index];
			parent = m_ppBoneList[pPmdBone->parent_bone_index];

			pos -= D3DXVECTOR3(pPmdBoneParent->bone_head_pos);
		}
		else
		{
			m_rootBoneCount++;
		}

		pmdBone->SetLocalPosition( pos );

		if( parent )
		{
			m_ppBoneList[idx]->SetParent( parent );
		}
	}

	m_ppRootBoneList = new PMDBone*[ m_rootBoneCount ];
	DWORD rootBoneIndex = 0;

	D3DXMATRIX matIdent;
	D3DXMatrixIdentity( &matIdent );

	for( DWORD idx=0;idx<pPmd->bone_list.bone_count;idx++ )
	{
		if( m_ppBoneList[idx]->GetParent()==NULL )
		{
			m_ppBoneList[idx]->UpdateTransform( matIdent,false );

			m_ppRootBoneList[rootBoneIndex] = m_ppBoneList[idx];
			rootBoneIndex++;
		}
	}

	m_pOffsetMatrices = new D3DXMATRIX[ pPmd->bone_list.bone_count ];
	for( DWORD idx=0;idx<pPmd->bone_list.bone_count;idx++ )
	{
		D3DXMatrixInverse( &m_pOffsetMatrices[idx],NULL,&m_ppBoneList[idx]->GetWorldMatrix() );
	}
}
	
DWORD PMDModelRenderer::GetMaterialNum()
{
	sPMD* pPmd = m_pModel->GetData();

	return pPmd->material_list.material_count;
}

void PMDModelRenderer::SetShader( DWORD materialIndex,ShaderPtr shader )
{
	if( GetMaterialNum() > materialIndex )
	{
		m_ppShaders[materialIndex] = shader;
	}
}

DWORD PMDModelRenderer::GetBoneNum()
{
	sPMD* pPmd = m_pModel->GetData();

	return pPmd->bone_list.bone_count;
}

PMDBone* PMDModelRenderer::GetBone( DWORD index )
{
	if( GetBoneNum() < index )
	{
		return NULL;
	}

	return m_ppBoneList[index];
}

PMDBone* PMDModelRenderer::SearchBone( const tstring_symbol& name )
{
	sPMD* pPmd = m_pModel->GetData();

	for( DWORD idx=0;idx<pPmd->bone_list.bone_count;idx++ )
	{
		if( m_ppBoneList[idx]->GetName()==name )
		{
			return m_ppBoneList[idx];
		}
	}

	return NULL;
}

void PMDModelRenderer::UpdateBone()
{
	sPMD* pPmd = m_pModel->GetData();

	for( DWORD boneIdx=0;boneIdx<pPmd->bone_list.bone_count;boneIdx++ )
	{
		PMDBone* bone = m_ppBoneList[boneIdx];

		ePMD_BoneType boneType = bone->GetBoneType();

		if( boneType == ePMD_BoneType_InfluenceIK )
		{
			bone->SetIKRotation( D3DXQUATERNION(0.0f,0.0f,0.0f,1.0f) );
		}
		else if( boneType == ePMD_BoneType_Gyration )
		{
			int influenceBoneIndex = bone->GetInfluenceBoneIndex();
			if( 0 <= influenceBoneIndex && influenceBoneIndex < pPmd->bone_list.bone_count )
			{
				float influenceWeight = bone->GetInfluenceWeight();

				PMDBone* influenceBone = m_ppBoneList[influenceBoneIndex];
				if( influenceBone )
				{
					D3DXQUATERNION rotation;
					D3DXQUATERNION motionRotation = influenceBone->GetMotionRotation();
					D3DXQuaternionIdentity( &rotation );
					D3DXQuaternionSlerp( &rotation,&rotation,&motionRotation,influenceWeight );
					D3DXQuaternionNormalize( &rotation,&rotation );

					bone->SetMotionRotation( rotation );
				}
			}
		}
	}

	D3DXMATRIX matIdent;
	D3DXMatrixIdentity( &matIdent );

	for( DWORD idx=0;idx<m_rootBoneCount;idx++ )
	{
		if( m_ppRootBoneList[idx]!=NULL )
		{
			m_ppRootBoneList[idx]->UpdateTransform( matIdent,false );
		}
	}

	static tstring_symbol rightKneeName = _T("右ひざ");
	static tstring_symbol leftKneeName = _T("左ひざ");

	for( DWORD ikIdx=0;ikIdx<pPmd->ik_data_list.ik_data_count;ikIdx++ )
	{
		sPMD_IKData* ikData = &pPmd->ik_data_list.ik_data[ikIdx];

		PMDBone* targetBone = m_ppBoneList[ikData->ik_target_bone_index];
		PMDBone* ikBone = m_ppBoneList[ikData->ik_bone_index];

		//targetBone->SetIKRotation(targetBone->GetMotionRotation());
		//targetBone->SetMotionRotation(D3DXQUATERNION(0.0f,0.0f,0.0f,1.0f));

		//// ターゲットボーンの位置を初期化するとミクさんがおかしくなる。
		//// 初期化しないと、MMDエンジンかしもでる.pmdが動かない・・・
		//D3DXMATRIX targetMatrix = m_pOffsetMatrices[ikData->ik_target_bone_index];
		//D3DXMatrixInverse( &targetMatrix,NULL,&targetMatrix );

		//D3DXVECTOR3 oldTargetPos(0.0f,0.0f,0.0f);
		//D3DXVec3TransformCoord( &oldTargetPos,&oldTargetPos,&targetMatrix );

		//D3DXVECTOR3 targetPos = oldTargetPos;
		//PMDBone* parentTargetBone = targetBone->GetParent();
		//if( parentTargetBone )
		//{
		//	D3DXMATRIX matWorld = parentTargetBone->GetWorldMatrix();
		//	D3DXMatrixInverse( &matWorld,NULL,&matWorld );
		//	D3DXVec3TransformCoord( &targetPos,&targetPos,&matWorld );
		//}

		//targetBone->SetLocalPosition(targetPos);

		//if( parentTargetBone )
		//{
		//	targetBone->UpdateTransform( parentTargetBone->GetWorldMatrix() );
		//}
		//else
		//{
		//	targetBone->UpdateTransform( matIdent );
		//}

		D3DXVECTOR3 localEffectorPos, localTargetPos;

		float	total_x		=	0.0f;

		D3DXVECTOR3 ikBonePos(0.0f,0.0f,0.0f);
		D3DXVec3TransformCoord( &ikBonePos,&ikBonePos,&ikBone->GetWorldMatrix() );

		for (unsigned int iteration = 0; iteration < ikData->iterations; ++iteration)
		{
			for (unsigned int ikChildIdx = 0; ikChildIdx < ikData->ik_chain_length; ++ikChildIdx)
			{
				PMDBone* childBone = m_ppBoneList[ ikData->ik_child_bone_index[ikChildIdx] ];

				// TODO:IK影響ボーンに適用

				D3DXVECTOR3 effectorPos(0.0f,0.0f,0.0f);
				D3DXVec3TransformCoord( &effectorPos,&effectorPos,&targetBone->GetWorldMatrix() );

				D3DXMATRIX invCoord;
				D3DXMatrixInverse(&invCoord, 0, &childBone->GetWorldMatrix() );

				D3DXVec3TransformCoord(&localEffectorPos, &effectorPos, &invCoord);
				D3DXVec3TransformCoord(&localTargetPos, &ikBonePos, &invCoord);
				D3DXVECTOR3 localEffectorDir;															// エフェクタのローカル方向（注目ボーンの位置基準）
				D3DXVec3Normalize(&localEffectorDir, &localEffectorPos);
				D3DXVECTOR3 localTargetDir;																// ターゲットのローカル方向（注目ボーンの位置基準）
				D3DXVec3Normalize(&localTargetDir, &localTargetPos);

				D3DXVECTOR3 v = localTargetDir-localEffectorDir;
				float s=D3DXVec3LengthSq( &v );

				if(s<0.0000001f)
				{
					break;
				}

				bool limitBone = (childBone->GetName().key().find( _T("右ひざ") ) == 0 || childBone->GetName().key().find( _T("左ひざ") ) == 0 );

				if( iteration == 0 )
				{
					if( limitBone )
					{
						D3DXQUATERNION q;
						D3DXQuaternionRotationMatrix( &q,&childBone->GetBoneLocalMatrix() );
						float sp = -2.0f * (q.x * q.z + q.w * q.y);

						if( fabs(sp) > 0.9999f ) {
							total_x = atan2f( ( -q.y * q.z + q.w * q.x ), ( 0.5f - q.x * q.x - q.z * q.z ) );
						} else {
							total_x = atan2f( ( q.y * q.z + q.w * q.x ), ( 0.5f - q.x * q.x - q.y * q.y ) );
						}
					}
				}

				float nais = D3DXVec3Dot(&localEffectorDir, &localTargetDir);
				if( nais>1.0f ) nais=1.0f;
				if( nais<-1.0f ) nais=-1.0f;

				float IKrot = acos(nais);

				float limit_arg	= ikData->control_weight*(ikChildIdx+1)*4.0f;
				if( IKrot > limit_arg )
				{
					IKrot = limit_arg;
				}

				//OutputDebugStringFormat( _T("IKrot(%d) : %f\n"),ikIdx,D3DXToDegree(IKrot) );

				D3DXVECTOR3 axis;
				D3DXVec3Cross(&axis, &localEffectorDir, &localTargetDir);
				D3DXVec3Normalize( &axis,&axis );
				D3DXQUATERNION rotation;
				D3DXQuaternionRotationAxis(&rotation, &axis, IKrot);
				D3DXQuaternionNormalize( &rotation,&rotation );

				if( limitBone )
				{
					float x_rad;
					float sp = -2.0f * (rotation.x * rotation.z + rotation.w * rotation.y);

					if( fabs(sp) > 0.9999f ) 
					{
						x_rad = atan2f( -rotation.y * rotation.z + rotation.w * rotation.x, 0.5f - rotation.x * rotation.x - rotation.z * rotation.z );
					}
					else
					{
						x_rad = atan2f( rotation.y * rotation.z + rotation.w * rotation.x, 0.5f - rotation.x * rotation.x - rotation.y * rotation.y );
					}

					// 膝が伸びてるとき
					if( total_x >= 0.0f ) 
					{
						x_rad = -IKrot;	// どっち方向もX方向に曲げてしまう
					}

					/* 膝の曲がる方向を限定 */
					if( total_x + x_rad > 0.0f ) 
					{
						x_rad = 0.0f - total_x;
						total_x = 0.0f;
					}
					else
					{
						/* 曲げた量を足す */
						total_x += x_rad;
					}

					/* 角度制限したものから回転行列を作る */
					D3DXQuaternionRotationYawPitchRoll( &rotation, 0.0f, x_rad, 0.0f );
					D3DXQuaternionNormalize( &rotation,&rotation );
				}

				rotation = rotation * childBone->GetIKRotation();
				D3DXQuaternionNormalize( &rotation,&rotation );
				if( iteration == 0 )
				{
					rotation = rotation * childBone->GetMotionRotation();
					D3DXQuaternionNormalize( &rotation,&rotation );
					childBone->SetMotionRotation( D3DXQUATERNION(0.0f,0.0f,0.0f,1.0f) );
				}
				
				childBone->SetIKRotation( rotation );

				PMDBone* parentBone = childBone->GetParent();

				if( parentBone )
				{
					childBone->UpdateTransform( parentBone->GetWorldMatrix(),true );
				}
				else
				{
					childBone->UpdateTransform( matIdent,true );
				}
			}
		}
	}
}

DWORD PMDModelRenderer::GetSkinNum()
{
	sPMD* pPmd = m_pModel->GetData();

	return pPmd->skin_list.skin_count;
}

int PMDModelRenderer::SearchSkin( const tstring_symbol& name )
{
	sPMD* pPmd = m_pModel->GetData();

	for( DWORD idx=0;idx<pPmd->skin_list.skin_count;idx++ )
	{
		sPMD_Skin* skin = &pPmd->skin_list.skin[idx];

		tstring_symbol skinName = to_tstring( skin->skin_name );

		if( skinName==name )
		{
			return idx;
		}
	}

	return -1;
}

void PMDModelRenderer::SetSkinWeight( int index,float weight )
{
	sPMD* pPmd = m_pModel->GetData();

	if( index >= pPmd->skin_list.skin_count )
	{
		return;
	}

	m_skinWeights[index].weight = weight;
	m_skinWeights[index].changed = true;
}

void PMDModelRenderer::UpdateSkinMesh()
{
	if( !m_pMesh || !m_ppBoneList )
	{
		return;
	}

	DWORD vertexNum = m_pMesh->GetVertexNum();
	sVertex* vertices = m_pMesh->GetVertices();

	sPMD* pmd = m_pModel->GetData();

	D3DXMATRIX* pBoneMatrix = new D3DXMATRIX[pmd->bone_list.bone_count];

	for( DWORD idx=0;idx<pmd->bone_list.bone_count;idx++ )
	{
		PMDBone* boneObj = m_ppBoneList[idx];

		if( boneObj )
		{
			pBoneMatrix[idx] = m_pOffsetMatrices[idx] * boneObj->GetWorldMatrix();
		}
		else
		{
			D3DXMatrixIdentity( &pBoneMatrix[idx] );
		}
	}

	D3DXVECTOR3* skinOffsets = new D3DXVECTOR3[vertexNum];
	ZeroMemory( skinOffsets,sizeof(D3DXVECTOR3)*vertexNum );

	SkinWeight* skinWeight;

	for( DWORD skinIdx=0;skinIdx<pmd->skin_list.skin_count;skinIdx++ )
	{
		sPMD_Skin* skin = &pmd->skin_list.skin[skinIdx];

		skinWeight = &m_skinWeights[skinIdx];

		if( skinWeight->changed )
		{
			if( skin->skin_type != ePMD_SkinType_Base )
			{
				for( DWORD vertIdx = 0; vertIdx < skin->skin_vert_count ; vertIdx++ )
				{
					sPMD_SkinVertex* skinVert = &skin->skin_vert[vertIdx];

					skinOffsets[ skinVert->index ] += D3DXVECTOR3( skinVert->pos ) * skinWeight->weight;
				}
			}

			skinWeight->changed = false;
		}
	}

	DWORD boneIdx1;
	DWORD boneIdx2;

	sVertex* vertex;
	sPMD_Vertex* pmdVertex;

	D3DXVECTOR3 pos1,pos2;
	D3DXVECTOR3 norm1,norm2;

	float weight;

	for( DWORD idx=0;idx<vertexNum;idx++ )
	{
		vertex = &vertices[idx];

		pmdVertex = &pmd->vertex_list.vertex[idx];

		boneIdx1 = pmdVertex->bone_num[0];
		boneIdx2 = pmdVertex->bone_num[1];

		weight = (float)pmdVertex->bone_weight/100.0f;
		
		vertex->position.x = pmdVertex->pos[0];
		vertex->position.y = pmdVertex->pos[1];
		vertex->position.z = pmdVertex->pos[2];

		vertex->position += skinOffsets[ idx ];

		D3DXVec3TransformCoord( &pos1,&vertex->position,&pBoneMatrix[boneIdx1] );
		D3DXVec3TransformCoord( &pos2,&vertex->position,&pBoneMatrix[boneIdx2] );

		D3DXVec3Lerp( &vertex->position,&pos2,&pos1,weight );

		vertex->normal.x = pmdVertex->normal_vec[0];
		vertex->normal.y = pmdVertex->normal_vec[1];
		vertex->normal.z = pmdVertex->normal_vec[2];

		D3DXVec3TransformNormal( &norm1,&vertex->normal,&pBoneMatrix[boneIdx1] );
		D3DXVec3TransformNormal( &norm2,&vertex->normal,&pBoneMatrix[boneIdx2] );

		D3DXVec3Lerp( &vertex->normal,&norm2,&norm1,weight );
	}

	delete[] skinOffsets;
	delete[] pBoneMatrix;
}

void PMDModelRenderer::Render( const D3DXMATRIX& matWorldViewProj )
{
	if( !m_pMesh )
	{
		return;
	}

	UpdateSkinMesh();

	DWORD attrNum = m_pMesh->GetAttributeRangeNum();

	sPMDMaterial* pMaterials = m_pModel->GetMaterials();

	for( DWORD attrIdx = 0 ; attrIdx < attrNum ; attrIdx++ )
	{
		DWORD attrID = m_pMesh->GetAttributeID( attrIdx );

		sPMDMaterial* pMaterial = &pMaterials[attrID];
		ShaderPtr pShader = m_ppShaders[attrID];

		if( pShader )
		{
			ID3DXEffectPtr pEffect = pShader->GetEffect();
		
			pEffect->SetMatrix( "matWorldViewProj", &matWorldViewProj );
			
			pEffect->SetValue( "colorDiffuse",&pMaterial->colorDiffuse,sizeof(D3DXCOLOR) );
			if( pMaterial->textureDiffuse )
			{
				pEffect->SetTexture( "textureDiffuse",pMaterial->textureDiffuse->GetTexture() );
			}
			else
			{
				pEffect->SetTexture( "textureDiffuse",NULL );
			}
			
			pEffect->SetTechnique( "Tec_Default" );
			UINT numPass;
			pEffect->Begin( &numPass, 0 );
			pEffect->BeginPass(0);

			m_pMesh->Draw( attrIdx );

			pEffect->EndPass();
			pEffect->End();
		}
	}
}

void PMDModelRenderer::RenderNonShader()
{
	if( !m_pMesh )
	{
		return;
	}

	DWORD attrNum = m_pMesh->GetAttributeRangeNum();

	sPMDMaterial* pMaterials = m_pModel->GetMaterials();

	for( DWORD attrIdx = 0 ; attrIdx < attrNum ; attrIdx++ )
	{
		DWORD attrID = m_pMesh->GetAttributeID( attrIdx );

		m_pMesh->Draw( attrIdx );
	}
}