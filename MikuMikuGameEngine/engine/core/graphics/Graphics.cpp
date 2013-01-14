#include "stdafx.h"

#include "../Thread.h"
#include "Graphics.h"
#include "Shader.h"

#include "../util/Charset.h"

#include "effect/DefaultEffect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Graphics::Graphics()
	: m_init(false)
	, m_window(NULL)
	, m_pDirect3D(NULL)
	, m_pDirect3DDevice(NULL)
	, m_deviceLost(false)
	, m_screenWidth(800)
	, m_screenHeight(600)
	, m_windowed(true)
	, m_windowActive(false)
	, m_enableRender(true)
{
	ZeroMemory( &m_d3dpp,sizeof(D3DPRESENT_PARAMETERS) ); 
}

Graphics::~Graphics()
{
}

//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT Graphics::Initialize( HWND window )
{
	if( window==NULL )
	{
		return E_FAIL;
	}

	HWND hWnd = window;

    // Create the D3D object, which is needed to create the D3DDevice.
	m_pDirect3D.Attach( Direct3DCreate9( D3D_SDK_VERSION ) );
    if( NULL == m_pDirect3D )
	{
        return E_FAIL;
	}

	m_windowed = true;

	HRESULT hr;

	//// TEST:アダプター情報の列挙
	//DWORD adapterNum=m_pDirect3D->GetAdapterCount();
	//for( DWORD i=0;i<adapterNum;i++ )
	//{
	//	D3DADAPTER_IDENTIFIER9 identifier;
	//	hr = m_pDirect3D->GetAdapterIdentifier( i,0,&identifier );
	//	
	//	D3DDISPLAYMODE displayMode;
	//	hr = m_pDirect3D->GetAdapterDisplayMode( i,&displayMode );
	//
	//	HMONITOR monitor = m_pDirect3D->GetAdapterMonitor( i );
	//
	//	D3DFORMAT checkFormat[] = {
	//		D3DFMT_X8R8G8B8,
	//		D3DFMT_A8R8G8B8,
	//		D3DFMT_A2R10G10B10,
	//		D3DFMT_X1R5G5B5,
	//		D3DFMT_A1R5G5B5,
	//		D3DFMT_R5G6B5,
	//	};
	//
	//	DWORD checkFormatNum = sizeof(checkFormat)/sizeof(D3DFORMAT);
	//
	//	for( DWORD j=0;j<checkFormatNum;j++ )
	//	{
	//		D3DFORMAT fmt = checkFormat[j];
	//		DWORD modeCount = m_pDirect3D->GetAdapterModeCount( i,fmt );
	//		for( DWORD k=0;k<modeCount;k++ )
	//		{
	//			D3DDISPLAYMODE displayMode;
	//			m_pDirect3D->EnumAdapterModes( i,fmt,k,&displayMode );
	//
	//			::OutputDebugString( _T("a") );
	//		}
	//	}
	//}

    // Set up the structure used to create the D3DDevice. Most parameters are
    // zeroed out. We set Windowed to TRUE, since we want to do D3D in a
    // window, and then set the SwapEffect to "discard", which is the most
    // efficient method of presenting the back buffer to the display.  And 
    // we request a back buffer format that matches the current desktop display 
    // format.
    ZeroMemory( &m_d3dpp, sizeof( m_d3dpp ) );
    m_d3dpp.Windowed = TRUE;
    m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    m_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	m_d3dpp.BackBufferCount = 1;
	m_d3dpp.EnableAutoDepthStencil = TRUE;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	//m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	//// TEST:性能評価
	//D3DCAPS9 caps;
	//hr = m_pDirect3D->GetDeviceCaps( D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps );

    // Create the Direct3D device. Here we are using the default adapter (most
    // systems only have one, unless they have multiple graphics hardware cards
    // installed) and requesting the HAL (which is saying we want the hardware
    // device rather than a software one). Software vertex processing is 
    // specified since we know it will work on all cards. On cards that support 
    // hardware vertex processing, though, we would see a big performance gain 
    // by specifying hardware vertex processing.
	hr = m_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
                                      &m_d3dpp, &m_pDirect3DDevice );
    if( FAILED( hr ) )
    {
		hr = m_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
			D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
										  &m_d3dpp, &m_pDirect3DDevice );
		if( FAILED( hr ) )
		{
			hr = m_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
											  &m_d3dpp, &m_pDirect3DDevice );
			if( FAILED( hr ) )
			{
				hr = m_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
												  &m_d3dpp, &m_pDirect3DDevice );
				if( FAILED( hr ) )
				{
					return hr;
				}
			}
		}
    }

	// Device state would normally be set here

	m_window = window;

	::GetWindowRect(hWnd, &m_rectWindow);
	::GetClientRect(hWnd, &m_rectClient);
	m_windowActive = true;

	m_defaultShader = ShaderPtr(new Shader);
	if( !m_defaultShader->CreateFromMemory( g_DefaultEffect,sizeof(g_DefaultEffect)/sizeof(BYTE) ) )
	{
		m_defaultShader = ShaderPtr();
	}

	m_init = true;

	return S_OK;
}

void Graphics::ChangeWindowActive( bool active )
{
	if( m_windowActive==active )
	{
		return;
	}
	m_windowActive = active;

	if( m_windowActive && m_deviceLost )
	{
		LostDevice();
	}
}

bool Graphics::IsScreenWindowed()
{
	return m_windowed;
}

void Graphics::ChangeScreenMode( bool windowed )
{
	if( windowed==m_windowed )
	{
		return;
	}
	m_windowed = windowed;

	if( m_windowed )
	{
		ZeroMemory( &m_d3dpp, sizeof( m_d3dpp ) );
		m_d3dpp.Windowed = TRUE;
		m_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		m_d3dpp.BackBufferWidth = m_rectClient.right-m_rectClient.left;
		m_d3dpp.BackBufferHeight = m_rectClient.bottom-m_rectClient.top;
		m_d3dpp.BackBufferCount = 1;
		m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_d3dpp.EnableAutoDepthStencil = TRUE;
		m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	}
	else
	{
		D3DDISPLAYMODE d3ddisp;
		m_pDirect3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT,&d3ddisp );

		ZeroMemory( &m_d3dpp, sizeof( m_d3dpp ) );
		m_d3dpp.Windowed = FALSE;
		m_d3dpp.BackBufferFormat = d3ddisp.Format;
		m_d3dpp.BackBufferWidth = m_screenWidth;
		m_d3dpp.BackBufferHeight = m_screenHeight;
		m_d3dpp.BackBufferCount = 1;
		m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_d3dpp.EnableAutoDepthStencil = TRUE;
		m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

		::GetWindowRect(m_window, &m_rectWindow);
		::GetClientRect(m_window, &m_rectClient);
	}

	Locker lock( &m_cs );

	OnLostDevice();

	m_pDirect3DDevice->Reset( &m_d3dpp );

	OnResetDevice();

	D3DVIEWPORT9 vp;
	vp.X  = 0;
	vp.Y  = 0;
	vp.Width = m_d3dpp.BackBufferWidth;
	vp.Height = m_d3dpp.BackBufferHeight;
	vp.MinZ  = 0.0f;
	vp.MaxZ  = 1.0f;
	m_pDirect3DDevice->SetViewport(&vp);

	if( m_windowed )
	{
		// フルスクリーンだった場合、ウィンドウスタイルや位置が自動的に変わるため、
		// 元に戻す必要がある。
		// フルスクリーン用のウィンドウを作っている場合は、この問題が発生しない(ウィンドウの表示、非表示は切り替える必要がある)
		::SetWindowLong(m_window, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE );
		::SetWindowPos(m_window, HWND_NOTOPMOST,
			m_rectWindow.left, m_rectWindow.top,
			m_rectWindow.right - m_rectWindow.left,
			m_rectWindow.bottom - m_rectWindow.top,
			SWP_SHOWWINDOW );
	}
	else
	{
		// フルスクリーンにした段階で、自動的に変更される。
		// だが、わかりにくいため念のため明記しておく。
		::SetWindowLong(m_window, GWL_STYLE, WS_POPUP | WS_VISIBLE);
	}
}

void Graphics::Resize( int width,int height )
{
	if( m_windowed )
	{
		m_d3dpp.BackBufferWidth = width;
		m_d3dpp.BackBufferHeight = height;

		Locker lock( &m_cs );

		OnLostDevice();

		m_pDirect3DDevice->Reset( &m_d3dpp );

		OnResetDevice();

		D3DVIEWPORT9 vp;
		vp.X  = 0;
		vp.Y  = 0;
		vp.Width = m_d3dpp.BackBufferWidth;
		vp.Height = m_d3dpp.BackBufferHeight;
		vp.MinZ  = 0.0f;
		vp.MaxZ  = 1.0f;
		m_pDirect3DDevice->SetViewport(&vp);
	}
}

bool Graphics::GetViewPort( D3DVIEWPORT9* vp )
{
	if( m_pDirect3DDevice )
	{
		m_pDirect3DDevice->GetViewport(vp);
		return true;
	}
	return false;
}

void Graphics::AddResource( UnmanagedResource* resource )
{
	m_resourceList.insert( resource );
}

void Graphics::DelResource( UnmanagedResource* resource )
{
	m_resourceList.erase( resource );
}

void Graphics::OnLostDevice()
{
	// TODO:リソースを解放してください。

	std::set< UnmanagedResource* >::iterator it = m_resourceList.begin();
	while( it!=m_resourceList.end() )
	{
		(*it)->OnLostDevice();
		++it;
	}
}

void Graphics::LostDevice()
{
	if( !m_init )
	{
		return;
	}

	m_deviceLost = true;
	if( !m_windowActive )
	{
		return;
	}

	OnLostDevice();	

	ResetDevice();
}

void Graphics::OnResetDevice()
{
	// TODO:リソースを復帰してください。

	std::set< UnmanagedResource* >::iterator it = m_resourceList.begin();
	while( it!=m_resourceList.end() )
	{
		(*it)->OnResetDevice();
		++it;
	}
}

void Graphics::ResetDevice()
{
	if( !m_init )
	{
		return;
	}

	if( !m_windowActive || !m_deviceLost )
	{
		return;
	}

	{
		Locker lock( &m_cs );

		HRESULT hr = m_pDirect3DDevice->TestCooperativeLevel();
		if( hr==D3DERR_DEVICELOST )
		{
			PostMessage( m_window,WM_USER_RESETDEVICE,0,0 );
			return;
		}

		m_pDirect3DDevice->Reset( &m_d3dpp );

		OnResetDevice();

		m_deviceLost = false;
	}
}

HRESULT Graphics::Clear(DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil)
{
	if( !m_init )
	{
		return E_FAIL;
	}

    if( NULL == m_pDirect3DDevice )
	{
        return E_FAIL;
	}

	if( m_deviceLost )
	{
		return E_FAIL;
	}

	if( !m_enableRender )
	{
		return E_FAIL;
	}

	return m_pDirect3DDevice->Clear( Count, pRects, Flags, Color, Z, Stencil );
}

HRESULT Graphics::BeginScene()
{
	if( !m_init )
	{
		return E_FAIL;
	}

    if( NULL == m_pDirect3DDevice )
	{
        return E_FAIL;
	}

	if( m_deviceLost )
	{
		return E_FAIL;
	}

	if( !m_enableRender )
	{
		return E_FAIL;
	}

	return m_pDirect3DDevice->BeginScene();
}

// End the scene
HRESULT Graphics::EndScene()
{
	if( !m_init )
	{
		return E_FAIL;
	}

    if( NULL == m_pDirect3DDevice )
	{
        return E_FAIL;
	}

	if( m_deviceLost )
	{
		return E_FAIL;
	}

	if( !m_enableRender )
	{
		return E_FAIL;
	}

	HRESULT hr = m_pDirect3DDevice->EndScene();
	if( FAILED(hr) )
	{
		return hr;
	}

	try
	{
		// Present the backbuffer contents to the display
		hr = m_pDirect3DDevice->Present( NULL, NULL, NULL, NULL );
		if( hr == D3DERR_DRIVERINTERNALERROR )
		{
			// 致命的なエラー
			// デバイスを作り直した方が良い
		}
		else if( hr == D3DERR_DEVICELOST )
		{
			// デバイスロストで描画できない
			// 描画ステータスをデバイスロストに変更。
			// 解消されるまで処理しないように。

			m_deviceLost = true;
			PostMessage( m_window,WM_USER_LOSTDEVICE,0,0 );
		}
		else if( hr == D3DERR_INVALIDCALL )
		{
			// 不正な呼び出し
			DebugBreak();
		}
	}
	catch(...)
	{
		DebugBreak();
	}

	return hr;
}

HRESULT Graphics::SetIndices( IDirect3DIndexBuffer9* pIndexData )
{
	if( !m_init )
	{
		return E_FAIL;
	}

	if( NULL == m_pDirect3DDevice )
	{
        return E_FAIL;
	}

	return m_pDirect3DDevice->SetIndices( pIndexData );
}

HRESULT Graphics::SetVertexDeclaration( IDirect3DVertexDeclaration9* pDecl )
{
	if( !m_init )
	{
		return E_FAIL;
	}

	if( NULL == m_pDirect3DDevice )
	{
        return E_FAIL;
	}

	return m_pDirect3DDevice->SetVertexDeclaration( pDecl );
}

HRESULT Graphics::SetFVF( DWORD FVF )
{
	if( !m_init )
	{
		return E_FAIL;
	}

	if( NULL == m_pDirect3DDevice )
	{
        return E_FAIL;
	}

	return m_pDirect3DDevice->SetFVF( FVF );
}

HRESULT Graphics::SetRenderState(D3DRENDERSTATETYPE State,DWORD Value)
{
	if( !m_init )
	{
		return E_FAIL;
	}

	if( NULL == m_pDirect3DDevice )
	{
        return E_FAIL;
	}

	return m_pDirect3DDevice->SetRenderState( State,Value );
}

HRESULT Graphics::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	if( !m_init )
	{
		return E_FAIL;
	}

	if( NULL == m_pDirect3DDevice )
	{
        return E_FAIL;
	}

	return m_pDirect3DDevice->DrawPrimitiveUP( PrimitiveType,PrimitiveCount,pVertexStreamZeroData,VertexStreamZeroStride );
}

HRESULT Graphics::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	if( !m_init )
	{
		return E_FAIL;
	}

	if( NULL == m_pDirect3DDevice )
	{
        return E_FAIL;
	}

	return m_pDirect3DDevice->DrawIndexedPrimitiveUP( PrimitiveType,MinVertexIndex,NumVertices,PrimitiveCount,pIndexData,IndexDataFormat,pVertexStreamZeroData,VertexStreamZeroStride );
}

//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
void Graphics::Cleanup()
{
	if( !m_init )
	{
		return;
	}

	m_pDirect3DDevice.Release();
    m_pDirect3D.Release();
	
	m_init = false;
}

void Graphics::EnableRender( bool enable )
{
	m_enableRender = enable;
}

ShaderPtr Graphics::GetDefaultShader()
{
	return m_defaultShader;
}