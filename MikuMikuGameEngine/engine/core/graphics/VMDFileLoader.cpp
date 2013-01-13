#include "stdafx.h"

#include "VMDFileLoader.h"

#include <algorithm>

#include <string.h>


#pragma pack(push,1)

struct sVMD_Header
{
	char header[30];
	char modelName[20];
};

struct sVMD_Motion
{
	char boneName[15];
	DWORD frameNo;
	float pos[3];
	float rotate[4];
	char Interpolation[4][16];
};

struct sVMD_MotionList
{
	DWORD num;
	sVMD_Motion* motion;
};

struct sVMD_Face
{
	char faceName[15];
	DWORD frameNo;
	float weight;
};

struct sVMD_FaceList
{
	DWORD num;
	sVMD_Face* face;
};

#pragma pack(pop)

struct sVMD
{
	sVMD_Header header;
	sVMD_MotionList motionList;
	sVMD_FaceList faceList;
};

void VMDRelease(sVMD* vmd)
{
	if( !vmd )
	{
		return;
	}

	delete[] vmd->motionList.motion;
	delete[] vmd->faceList.face;

	memset(vmd,0,sizeof(sVMD));
}

bool VMDLoad( unsigned char* buffer,size_t size,sVMD* vmd )
{
	if( !vmd )
	{
		return false;
	}

	memset(vmd,0,sizeof(sVMD));

	size_t sz=0;

	{
		sz=sizeof(sVMD_Header);
		if( size<sz )
		{
			VMDRelease(vmd);
			return false;
		}
		memcpy(&vmd->header,buffer,sz);
		size-=sz;
		buffer+=sz;
	}

	if( strcmp( vmd->header.header,"Vocaloid Motion Data 0002")!=0 )
	{
		VMDRelease(vmd);
		return false;
	}

	{
		sz=sizeof(unsigned long);
		if( size<sz )
		{
			VMDRelease(vmd);
			return false;
		}
		memcpy(&vmd->motionList.num,buffer,sz);
		size-=sz;
		buffer+=sz;

		sz=sizeof(sVMD_Motion)*vmd->motionList.num;
		if( size<sz )
		{
			VMDRelease(vmd);
			return false;
		}
		vmd->motionList.motion = new sVMD_Motion[vmd->motionList.num];
		memcpy(vmd->motionList.motion,buffer,sz);
		size-=sz;
		buffer+=sz;
	}

	{
		sz=sizeof(unsigned long);
		if( size<sz )
		{
			VMDRelease(vmd);
			return false;
		}
		memcpy(&vmd->faceList.num,buffer,sz);
		size-=sz;
		buffer+=sz;

		sz=sizeof(sVMD_Face)*vmd->faceList.num;
		if( size<sz )
		{
			VMDRelease(vmd);
			return false;
		}
		vmd->faceList.face = new sVMD_Face[vmd->motionList.num];
		memcpy(vmd->faceList.face,buffer,sz);
		size-=sz;
		buffer+=sz;
	}

	return true;
}

VMDAnimationClipPtr VMDFileLoader::Open( const tstring& filePath )
{
	FILE* fp=NULL;
	if( _tfopen_s(&fp,filePath.c_str(),_T("rb"))!=0 )
	{
		return VMDAnimationClipPtr();
	}

	fpos_t fsize = 0;
	fseek(fp,0,SEEK_END);
	fgetpos(fp,&fsize);

	fseek(fp,0,SEEK_SET); 

	size_t sz=(size_t)fsize;

	unsigned char* buffer=new unsigned char[sz];
	fread(buffer,1,sz,fp);

	fclose(fp);

	sVMD vmd;

	bool ret = VMDLoad( buffer,sz,&vmd);
	
	delete[] buffer;
	
	if( !ret )
	{
		return VMDAnimationClipPtr();
	}

	DWORD motionBoneCount = 0;
	sMotionBone* motionBones = NULL;

	if( vmd.motionList.num>0 )
	{
		struct sSortMotion
		{
			bool operator()( const sVMD_Motion& rhs,const sVMD_Motion& lhs)
			{
				int retCmp = strcmp(rhs.boneName,lhs.boneName);
				if( retCmp==0 )
				{
					return rhs.frameNo<lhs.frameNo;
				}

				return retCmp>0;
			}
		};

		std::sort( vmd.motionList.motion,vmd.motionList.motion+vmd.motionList.num,sSortMotion() );

		tstring boneName=_T("");

		for( DWORD i=0;i<vmd.motionList.num;i++ )
		{
			sVMD_Motion* motion=&vmd.motionList.motion[i];
			tstring motionBoneName = to_tstring( motion->boneName );

			if( boneName != motionBoneName )
			{
				motionBoneCount++;

				boneName = motionBoneName;
			}
		}

		motionBones = new sMotionBone[motionBoneCount];

		DWORD motionBoneIndex = 0;
		DWORD startIndex = 0;

		sVMD_Motion* motion=&vmd.motionList.motion[0];
		tstring motionBoneName = to_tstring( motion->boneName );

		boneName=motionBoneName;

		for( DWORD i=1;i<vmd.motionList.num;i++ )
		{
			motion=&vmd.motionList.motion[i];
			motionBoneName = to_tstring( motion->boneName );

			if( boneName != motionBoneName && startIndex<i )
			{
				sMotionBone* motionBone = &motionBones[motionBoneIndex];
				motionBone->boneName = boneName;
				motionBone->keyFrameNum = i-startIndex;
				motionBone->keyFrames = new sMotionKeyFrame[motionBone->keyFrameNum];

				for( DWORD j=0;j<motionBone->keyFrameNum;j++ )
				{
					sVMD_Motion* vmdMotion=&vmd.motionList.motion[startIndex+j];
					sMotionKeyFrame* keyFrame = &motionBone->keyFrames[j];
					keyFrame->frameNo = vmdMotion->frameNo;
					keyFrame->pos = D3DXVECTOR3(vmdMotion->pos);
					keyFrame->rotate = D3DXQUATERNION(vmdMotion->rotate);
					if( keyFrame->rotate.x==0.0f && keyFrame->rotate.y==0.0f && keyFrame->rotate.z==0.0f && keyFrame->rotate.w==0.0f )
					{
						D3DXQuaternionIdentity( &keyFrame->rotate );
					}

					keyFrame->SetInterpolation( &vmdMotion->Interpolation[0] );
				}

				motionBoneIndex++;

				startIndex = i;
				boneName = motionBoneName;
			}
		}

		if( startIndex<vmd.motionList.num )
		{
			sMotionBone* motionBone = &motionBones[motionBoneIndex];
			motionBone->boneName = boneName;
			motionBone->keyFrameNum = vmd.motionList.num-startIndex;
			motionBone->keyFrames = new sMotionKeyFrame[motionBone->keyFrameNum];

			for( DWORD j=0;j<motionBone->keyFrameNum;j++ )
			{
				sVMD_Motion* vmdMotion=&vmd.motionList.motion[startIndex+j];
				sMotionKeyFrame* keyFrame = &motionBone->keyFrames[j];
				keyFrame->frameNo = vmdMotion->frameNo;
				keyFrame->pos = D3DXVECTOR3(vmdMotion->pos);
				keyFrame->rotate = D3DXQUATERNION(vmdMotion->rotate);

				keyFrame->SetInterpolation( vmdMotion->Interpolation );
			}
		}
	}

	DWORD morphingTargetCount = 0;
	sMorphingSkin* morphingTargets = NULL;

	if( vmd.faceList.num>0 )
	{
		struct sSortMorphing
		{
			bool operator()( const sVMD_Face& rhs,const sVMD_Face& lhs)
			{
				int retCmp = strcmp(rhs.faceName,lhs.faceName);
				if( retCmp==0 )
				{
					return rhs.frameNo<lhs.frameNo;
				}

				return retCmp>0;
			}
		};

		std::sort( vmd.faceList.face,vmd.faceList.face+vmd.faceList.num,sSortMorphing() );

		tstring targetName=_T("");

		for( DWORD i=0;i<vmd.faceList.num;i++ )
		{
			sVMD_Face* face=&vmd.faceList.face[i];
			tstring morphingTargetName = to_tstring( face->faceName );

			if( targetName != morphingTargetName )
			{
				morphingTargetCount++;

				targetName = morphingTargetName;
			}
		}

		morphingTargets = new sMorphingSkin[morphingTargetCount];

		DWORD morphingTargetIndex = 0;
		DWORD startIndex = 0;

		sVMD_Face* face=&vmd.faceList.face[0];
		tstring morphingTargetName = to_tstring( face->faceName );

		targetName=morphingTargetName;

		for( DWORD i=1;i<vmd.faceList.num;i++ )
		{
			face=&vmd.faceList.face[i];
			morphingTargetName = to_tstring( face->faceName );

			if( targetName != morphingTargetName && startIndex<i )
			{
				sMorphingSkin* morphingTarget = &morphingTargets[morphingTargetIndex];
				morphingTarget->skinName = targetName;
				morphingTarget->keyFrameNum = i-startIndex;
				morphingTarget->keyFrames = new sMorphingKeyFrame[morphingTarget->keyFrameNum];

				for( DWORD j=0;j<morphingTarget->keyFrameNum;j++ )
				{
					sVMD_Face* vmdFace=&vmd.faceList.face[startIndex+j];
					sMorphingKeyFrame* keyFrame = &morphingTarget->keyFrames[j];
					keyFrame->frameNo = vmdFace->frameNo;
					keyFrame->weight = vmdFace->weight;
				}

				morphingTargetIndex++;

				startIndex = i;
				targetName = morphingTargetName;
			}
		}

		if( startIndex<vmd.faceList.num )
		{
			sMorphingSkin* morphingTarget = &morphingTargets[morphingTargetIndex];
			morphingTarget->skinName = targetName;
			morphingTarget->keyFrameNum = vmd.faceList.num-startIndex;
			morphingTarget->keyFrames = new sMorphingKeyFrame[morphingTarget->keyFrameNum];

			for( DWORD j=0;j<morphingTarget->keyFrameNum;j++ )
			{
				sVMD_Face* vmdFace=&vmd.faceList.face[startIndex+j];
				sMorphingKeyFrame* keyFrame = &morphingTarget->keyFrames[j];
				keyFrame->frameNo = vmdFace->frameNo;
				keyFrame->weight = vmdFace->weight;
			}
		}
	}

	VMDRelease( &vmd );

	return VMDAnimationClipPtr( new VMDAnimationClip(motionBoneCount,motionBones,morphingTargetCount,morphingTargets) );
}