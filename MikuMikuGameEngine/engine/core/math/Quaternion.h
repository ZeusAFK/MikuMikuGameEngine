#pragma once

D3DXVECTOR3* D3DXQuaternionToEularZXY( D3DXVECTOR3* eular,const D3DXQUATERNION* quaternion );

D3DXVECTOR3* D3DXVec3Rotate( D3DXVECTOR3 *pOut,CONST D3DXVECTOR3 *pV,const D3DXQUATERNION* quaternion );
