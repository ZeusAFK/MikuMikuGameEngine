#include "stdafx.h"

#include "Quaternion.h"

D3DXVECTOR3* D3DXQuaternionToEularZXY( D3DXVECTOR3* eular,const D3DXQUATERNION* quaternion )
{
	D3DXMATRIX matRotation;
	D3DXMatrixRotationQuaternion( &matRotation,quaternion );

	eular->z = atan2f(matRotation._12, matRotation._22);
	eular->x = asinf(-matRotation._32);
	eular->y = atan2f(matRotation._31, matRotation._33);

	if( fabsf(cosf(eular->x)) < 1.0e-6f )
	{
		eular->z += matRotation._12 > 0.0f ? D3DX_PI : -D3DX_PI;
		eular->y += matRotation._31 > 0.0f ? D3DX_PI : -D3DX_PI;
	}

	return eular;
}

D3DXVECTOR3* D3DXVec3Rotate( D3DXVECTOR3 *pOut,CONST D3DXVECTOR3 *pV,const D3DXQUATERNION* quaternion )
{
	// 回転軸と角度からクォータニオンを作成
	D3DXQUATERNION R, Qv, Out;
	D3DXQuaternionConjugate(&R, quaternion);

	// ベクトル回転
	D3DXVECTOR3 V;
	FLOAT fLength = D3DXVec3Length(pV);   // 長さ
	D3DXVec3Normalize(&V, pV);   // 回転させるベクトルの標準化
	Qv.x = V.x;
	Qv.y = V.y;
	Qv.z = V.z;
	Qv.w = 0.0f;
	D3DXQuaternionMultiply(&Qv, &R, &Qv);
	D3DXQuaternionMultiply(&Qv, &Qv, quaternion);

	pOut->x = Qv.x * fLength;
	pOut->y = Qv.y * fLength;
	pOut->z = Qv.z * fLength;

	return pOut;
}