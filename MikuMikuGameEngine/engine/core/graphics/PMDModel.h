#pragma once

#include "../ResourceManager.h"

#include "Texture.h"

#pragma pack(push,1)

struct sPMD_Header
{
	char magic[3];
	float version;
	char model_name[20];
	char comment[256];
};

struct sPMD_Vertex
{
	float pos[3];
	float normal_vec[3];
	float uv[2];
	unsigned short bone_num[2];
	unsigned char bone_weight;
	unsigned char edge_flag;
};

struct sPMD_VertexList
{
	unsigned long vert_count;
	sPMD_Vertex* vertex; // count vert_count
};

struct sPMD_IndexList
{
	unsigned long face_vert_count;
	unsigned short* face_vert_index; // count face_vert_count
};

struct sPMD_Material
{
	float diffuse_color[3];
	float alpha;
	float specularity;
	float specular_color[3];
	float ambient_color[3];
	unsigned char toon_index;
	unsigned char edge_flag;
	unsigned long face_vert_count; // Start index is total this parameter of former.
	char texture_file_name[20];
};

struct sPMD_MaterialList
{
	unsigned long material_count;
	sPMD_Material* material; // count material_count
};

enum ePMD_BoneType
{
	ePMD_BoneType_Rotation,
	ePMD_BoneType_TransRotation,
	ePMD_BoneType_IK,
	ePMD_BoneType_Unknown,
	ePMD_BoneType_InfluenceIK,
	ePMD_BoneType_InfluenceRotation,
	ePMD_BoneType_JointIK,
	ePMD_BoneType_Invisible,

	// since v4.00
	ePMD_BoneType_Twist,
	ePMD_BoneType_Gyration,
};

struct sPMD_Bone
{
	char bone_name[20];
	unsigned short parent_bone_index;
	unsigned short tail_pos_bone_index;
	unsigned char bone_type; // reference:ePDM_BoneType
	unsigned short ik_parent_bone_index;
	float bone_head_pos[3];
};

struct sPMD_BoneList
{
	unsigned short bone_count;
	sPMD_Bone* bone; // count bone_count
};

struct sPMD_IKData
{
	unsigned short ik_bone_index;
	unsigned short ik_target_bone_index;
	unsigned char ik_chain_length;
	unsigned short iterations;
	float control_weight;
	unsigned short* ik_child_bone_index; // count ik_chain_length
};

struct sPMD_IKDataList
{
	unsigned short ik_data_count;
	sPMD_IKData* ik_data; // count ik_data_count
};

struct sPMD_SkinVertex
{
	unsigned long index;
	float pos[3]; // 
};

enum ePMD_SkinType
{
	ePMD_SkinType_Base,
	ePMD_SkinType_Eyebrow,
	ePMD_Skintype_Eye,
	ePMD_SkinType_Rip,
	ePMD_SkinType_Other,
};

struct sPMD_Skin
{
	char skin_name[20];
	unsigned long skin_vert_count;
	unsigned char skin_type; // reference:ePMD_SkinType
	sPMD_SkinVertex* skin_vert; // count skin_vert_count
};

struct sPMD_SkinList
{
	unsigned short skin_count;
	sPMD_Skin* skin; // count skin_count
};

struct sPMD_SkinDisp
{
	unsigned char skin_disp_count;
	unsigned short* skin_index;// count skin_disp_count;
};

struct sPMD_BoneDispName
{
	char dips_name[50];
};

struct sPMD_BoneDispNameList
{
	unsigned char bone_disp_name_count;
	sPMD_BoneDispName* dips_name;// count bone_disp_name_count
};

struct sPMD_BoneDisp
{
	unsigned short bone_index;
	unsigned char bone_disp_frame_index;
};

struct sPMD_BoneDispList
{
	unsigned long bone_disp_count;
	sPMD_BoneDisp* bone_disp;// count bone_disp_count
};

struct sPMD_EnglishHeader
{
	char model_name_eg[20];
	char comment_eg[256];
};

struct sPMD_ToonList
{
	char  toon_file_name[10][100];
};

struct sPMD_RigidBody
{
	char rigidbody_name[20];
	unsigned short rigidbody_rel_bone_index;
	unsigned char rigidbody_group_index;
	unsigned short rigidbody_group_target;
	unsigned char shape_type;
	float shape_w;
	float shape_h;
	float shape_d;
	float pos_pos[3];
	float pos_rot[3];
	float rigidbody_weight;
	float rigidbody_pos_dim;
	float rigidbody_rot_dim;
	float rigidbody_recoil;
	float rigidbody_friction;
	unsigned char rigidbody_type;
};

struct sPMD_RigidBodyList
{
	unsigned long rigidbody_count;
	sPMD_RigidBody* rigidbody;
};

struct sPMD_Joint
{
	char joint_name[20];
	unsigned long joint_rigidbody_a;
	unsigned long joint_rigidbody_b;
	float joint_pos[3];
	float joint_rot[3];
	float constrain_pos_1[3];
	float constrain_pos_2[3];
	float constrain_rot_1[3];
	float constrain_rot_2[3];
	float spring_pos[3];
	float spring_rot[3];
};

struct sPMD_JointList
{
	unsigned long joint_count;
	sPMD_Joint* joint;
};

#pragma pack(pop)

struct sPMD
{
	sPMD_Header header;
	sPMD_VertexList vertex_list;
	sPMD_IndexList index_list;
	sPMD_MaterialList material_list;
	sPMD_BoneList bone_list;
	sPMD_IKDataList ik_data_list;
	sPMD_SkinList skin_list;
	sPMD_SkinDisp skin_disp;
	sPMD_BoneDispNameList bone_disp_name_list;
	sPMD_BoneDispList bone_disp_list;
	sPMD_ToonList toon_list;
	sPMD_RigidBodyList rigidbody_list;
	sPMD_JointList joint_list;
};

bool PMDLoad(unsigned char* buffer,size_t size,sPMD* pmd);
void PMDRelease(sPMD* pmd);

enum eSPHEREMAP
{
	eSPHEREMAP_MUL,
	eSPHEREMAP_ADD,
};

struct sPMDMaterial
{
	D3DXCOLOR colorDiffuse;
	D3DXCOLOR colorSpecular;
	D3DXCOLOR colorAmbient;

	float     specularPower;

	TexturePtr textureDiffuse;

	TexturePtr textureSphere;

	D3DXCOLOR colorToon;

	eSPHEREMAP spheremap;

	bool edge;

public:
	sPMDMaterial()
		: colorDiffuse( 0xFFFFFFFF )
		, colorSpecular( 0xFF000000 )
		, colorAmbient( 0xFF000000 )
		, specularPower(1.0f)
		, spheremap(eSPHEREMAP_MUL)
		, edge(false)
	{
	}
};

class PMDModel : public IResource
{
private:
	sPMD* m_pPmd;

	DWORD m_materialNum;
	sPMDMaterial* m_pMaterials;

public:
	PMDModel( sPMD* pPmd,DWORD materialNum,sPMDMaterial* pMaterials );
	virtual ~PMDModel();

public:
	sPMD* GetData();

	DWORD GetMaterialNum();
	sPMDMaterial* GetMaterials();
};

typedef std::tr1::shared_ptr< PMDModel > PMDModelPtr;
typedef std::tr1::weak_ptr< PMDModel > PMDModelWeakPtr;