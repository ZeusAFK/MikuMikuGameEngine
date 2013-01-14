#pragma once

#include "../util/Charset.h"
#include "../Thread.h"

class Shader;
typedef std::tr1::shared_ptr< Shader > ShaderPtr;

#define WM_USER_LOSTDEVICE (WM_APP+0)
#define WM_USER_RESETDEVICE (WM_APP+1)

class UnmanagedResource
{
public:
	UnmanagedResource(){}
	virtual ~UnmanagedResource(){}

public:
	virtual void OnLostDevice() = 0;
	virtual void OnResetDevice() = 0;
};

class Graphics
{
public:
	static Graphics* GetInstance()
	{
		static Graphics graphics;
		return &graphics;
	}

private:
	Graphics();
	~Graphics();

public:
	HRESULT Initialize( HWND window );
	void ChangeWindowActive( bool active );
	bool IsScreenWindowed();
	void ChangeScreenMode( bool windowed );
	void OnLostDevice();
	void LostDevice();
	void OnResetDevice();
	void ResetDevice();

	HRESULT Clear(DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil);

	HRESULT BeginScene();
	HRESULT EndScene();

	HRESULT SetIndices( IDirect3DIndexBuffer9* pIndexData );
	HRESULT SetVertexDeclaration( IDirect3DVertexDeclaration9* pDecl );

	HRESULT SetFVF( DWORD FVF );
	HRESULT SetRenderState(D3DRENDERSTATETYPE State,DWORD Value);

	HRESULT DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	HRESULT DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);

	void Cleanup();
	void Resize( int width,int height );
	void EnableRender( bool enable );

	IDirect3DDevice9Ptr GetDirect3DDevice()
	{
		return m_pDirect3DDevice;
	}

	CriticalSection* GetCriticalSection()
	{
		return &m_cs;
	}

	unsigned int GetScreenWidth()
	{
		return m_screenWidth;
	}
	unsigned int GetScreenHeight()
	{
		return m_screenHeight;
	}

	UINT GetBackBufferWidth()
	{
		return m_d3dpp.BackBufferWidth;
	}
	UINT GetBackBufferHeight()
	{
		return m_d3dpp.BackBufferHeight;
	}

	bool GetViewPort( D3DVIEWPORT9* vp );

	void AddResource( UnmanagedResource* resource );
	void DelResource( UnmanagedResource* resource );

private:
	bool m_init;
	HWND m_window;
	IDirect3D9Ptr m_pDirect3D;
	IDirect3DDevice9Ptr m_pDirect3DDevice;
	bool m_deviceLost;
	D3DPRESENT_PARAMETERS m_d3dpp;
	CriticalSection m_cs;
	unsigned int m_screenWidth;
	unsigned int m_screenHeight;
	bool m_windowed;
	RECT m_rectWindow;
	RECT m_rectClient;
	bool m_windowActive;
	bool m_enableRender;

	std::set< UnmanagedResource* > m_resourceList;

public:
	ShaderPtr GetDefaultShader();

private:
	ShaderPtr m_defaultShader;
};
