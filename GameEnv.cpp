#include "poolgame.h"

// 정적 멤버 변수는 구현부에 초기화
CGameEnv* CGameEnv::m_pObj = 0;

CGameEnv* CGameEnv::GetObj()
{
	if( m_pObj == 0 )
		m_pObj = new CGameEnv;

	return m_pObj;
}

/******************************************************************************
* void Clear()
* 클래스 초기화
******************************************************************************/
void CGameEnv::Clear()
{
	m_pD3D = 0;
	m_pd3dDevice = 0;

	m_bFullScreen = false;
	m_hInst = 0;
	m_hWnd = 0;
	m_lpszClassName = 0;
	m_lpszWindowTitle = 0;
	m_nScreenWidth = 100;
	m_nScreenHeight = 100;

	D3DXMatrixIdentity(&m_matProj);
	m_fFps = 0.0f;
}

/******************************************************************************
* void Destroy()
* 클래스에서 생성한 자원 해제
******************************************************************************/
void CGameEnv::Destroy()
{
	RELEASE(m_pd3dDevice)
	RELEASE(m_pD3D)

	Clear();
}

/******************************************************************************
*
* bool CGameEnv::InitWindow()
*
* 윈도우 초기화 및 생성
*
******************************************************************************/
bool CGameEnv::InitWindow()
{
	DWORD		dwExStyle;
	DWORD		dwStyle;
	RECT		WindowRect;

	if ( m_bFullScreen )
	{
		dwExStyle	= WS_EX_APPWINDOW | WS_EX_TOPMOST;
		dwStyle		= WS_POPUP;
		ShowCursor(false);
	}
	else
	{
		dwExStyle	= WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle		= WS_OVERLAPPEDWINDOW;
	}

	SetRect(&WindowRect, 0, 0, m_nScreenWidth, m_nScreenHeight);
	// 윈도우 클라이언트 영역 조정
	AdjustWindowRectEx ( &WindowRect, dwStyle, false, dwExStyle );
	int nWidth  = WindowRect.right-WindowRect.left;
	int nHeight = WindowRect.bottom-WindowRect.top;

	WNDCLASSEX		wc;
	//g_hInstance			= GetModuleHandle(NULL);
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC) MsgProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= m_hInst;
	wc.hIcon			= LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_ICON1));
    wc.hIconSm			= LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor			= LoadCursor ( NULL, IDC_ARROW );
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= m_lpszClassName;

	// 윈도우 클래스 등록
	if ( !RegisterClassEx ( &wc ) )
	{
		MessageBox ( 0, "윈도우 클래스 등록 실패!!.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;
	}

	// 윈도우 생성
	if ( !( m_hWnd = CreateWindowEx(dwExStyle,
									m_lpszClassName,
									m_lpszWindowTitle,
									dwStyle |
									WS_CLIPSIBLINGS |
									WS_CLIPCHILDREN,
									0, 0,
									nWidth,
									nHeight,
									NULL,
									NULL,
									m_hInst,
									NULL ) ) )
	{
		MessageBox ( 0,"윈도우 생성 에러!", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;
	}

	// 윈도우 출력
	ShowWindow(m_hWnd,SW_SHOW);
	SetForegroundWindow(m_hWnd);
	UpdateWindow(m_hWnd);

    GetWindowRect( m_hWnd, &m_rcWindowBounds );
    GetClientRect( m_hWnd, &m_rcWindowClient );

	// 풀스크린
	if ( m_bFullScreen )
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset ( &dmScreenSettings, 0, sizeof ( dmScreenSettings ) );
		dmScreenSettings.dmSize			= sizeof ( dmScreenSettings );
		dmScreenSettings.dmPelsWidth	= m_nScreenWidth;
		dmScreenSettings.dmPelsHeight	= m_nScreenHeight;
		dmScreenSettings.dmBitsPerPel	= 16;
		dmScreenSettings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;


		if ( ChangeDisplaySettings ( &dmScreenSettings, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
		{
			if ( MessageBox ( NULL,
							  "비디오카드가 풀스크린 지원하지 않음. 윈도우 모드로 시작 ?",
							  "RbGame",
							  MB_YESNO | MB_ICONEXCLAMATION ) == IDYES)
			{
				m_bFullScreen = false;
			}
			else
			{
				MessageBox ( 0, "프로그램 종료.", "ERROR", MB_OK | MB_ICONSTOP );
				return false;
			}
		}
	}

	return true;
}

/******************************************************************************
* bool Create(HINSTANCE hInst,LPSTR lpszClassName, LPSTR lpszWindowName,
					int nScreenWidth, int nScreenHeight)
* 게임 기본 환경 초기화 - 윈도우 생성, Direct3D 초기화
******************************************************************************/
bool CGameEnv::Create(HINSTANCE hInst,LPSTR lpszClassName, LPSTR lpszWindowName,
					int nScreenWidth, int nScreenHeight)
{
	m_hInst = hInst;
	m_nScreenWidth = nScreenWidth;
	m_nScreenHeight = nScreenHeight;
	m_lpszClassName = lpszClassName;
	m_lpszWindowTitle = lpszWindowName;

	// 풀스크린 모드 질문
	if ( MessageBox ( 0, "전체 화면으로 볼까요?", "알림", MB_YESNO | MB_ICONQUESTION ) == IDYES )
		m_bFullScreen = true;
	else
		m_bFullScreen = false;

	// 윈도우 출력
	if( !InitWindow() )
		return false;

    /// 디바이스를 생성하기 위한 D3D 객체 생성
    if( ( m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) == 0 )
        return false;

    /// 디바이스를 생성할 구조체
    ZeroMemory ( &m_d3dpp, sizeof ( m_d3dpp ) );
    m_d3dpp.Windowed				= true;
	m_d3dpp.BackBufferWidth			= m_rcWindowClient.right - m_rcWindowClient.left;
	m_d3dpp.BackBufferHeight		= m_rcWindowClient.bottom - m_rcWindowClient.top;
    m_d3dpp.SwapEffect				= D3DSWAPEFFECT_DISCARD;
    m_d3dpp.BackBufferFormat		= D3DFMT_UNKNOWN;
    m_d3dpp.EnableAutoDepthStencil	= true;
    m_d3dpp.AutoDepthStencilFormat	= D3DFMT_D16;
	m_d3dpp.PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;
	m_d3dpp.Flags					= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

    // 디바이스 생성
    if( FAILED( m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &m_d3dpp, &m_pd3dDevice ) ) )
    {
		MessageBox(0,"Direct3D 실행할 수 없습니다.",m_lpszWindowTitle,MB_OK | MB_ICONSTOP);
        return false;
    }

    return true;
}

void CGameEnv::DetermineFPS()
{
	static DWORD dwLast = timeGetTime();
	//static float fFps;
	float fDt=0.0f;
	static int nFrames = 0;

	DWORD dwCurrent = timeGetTime();

	nFrames++;

	// 0.5초마다 갱신
	if( dwCurrent - dwLast >= 500 )
	{
		fDt = (dwCurrent - dwLast)/1000.0f;
		m_fFps = (float)nFrames/fDt;
		dwLast = dwCurrent;
		nFrames = 0;
	}
	//return fFps;
}

/******************************************************************************
* SetupLights()
******************************************************************************/
void CGameEnv::SetupLights(bool Switch)
{
	if(Switch)
	{

		/// 광원 설정
		D3DXVECTOR3 vecDir1 = D3DXVECTOR3( 1.0f, -1.0f, -1.0f );
		D3DXVECTOR3 vecDir2 = D3DXVECTOR3( -1.0f, -1.0f, 1.0f );
		D3DLIGHT9 light1;									/// 광원 구조체
		ZeroMemory( &light1, sizeof(D3DLIGHT9) );			/// 구조체를 0으로 지운다.
		light1.Type       = D3DLIGHT_DIRECTIONAL;			/// 광원의 종류(점 광원,방향성 광원,스포트라이트)
		light1.Diffuse.r  = 1.0f;							/// 광원의 색깔과 밝기
		light1.Diffuse.g  = 1.0f;
		light1.Diffuse.b  = 1.0f;

		light1.Specular.r  = 1.0f;							/// 광원의 색깔과 밝기
		light1.Specular.g  = 1.0f;
		light1.Specular.b  = 1.0f;

		light1.Ambient.r  = 1.0f;							/// 광원의 색깔과 밝기
		light1.Ambient.g  = 1.0f;
		light1.Ambient.b  = 1.0f;

		D3DXVec3Normalize( (D3DXVECTOR3*)&light1.Direction, &vecDir1 );	/// 광원의 방향을 단위벡터로 만든다.

		m_pd3dDevice->SetLight( 0, &light1 );	/// 디바이스에 0번 광원 설치
		m_pd3dDevice->LightEnable( 0, TRUE );	

		D3DLIGHT9 light2;									/// 광원 구조체
		ZeroMemory( &light2, sizeof(D3DLIGHT9) );			/// 구조체를 0으로 지운다.
		light2.Type       = D3DLIGHT_DIRECTIONAL;			/// 광원의 종류(점 광원,방향성 광원,스포트라이트)
		light2.Diffuse.r  = 1.0f;							/// 광원의 색깔과 밝기
		light2.Diffuse.g  = 1.0f;
		light2.Diffuse.b  = 1.0f;

		light2.Specular.r  = 1.0f;							/// 광원의 색깔과 밝기
		light2.Specular.g  = 1.0f;
		light2.Specular.b  = 1.0f;

		light2.Ambient.r  = 1.0f;							/// 광원의 색깔과 밝기
		light2.Ambient.g  = 1.0f;
		light2.Ambient.b  = 1.0f;

		D3DXVec3Normalize( (D3DXVECTOR3*)&light2.Direction, &vecDir2 );	/// 광원의 방향을 단위벡터로 만든다.

		m_pd3dDevice->SetLight( 1, &light2 );
		m_pd3dDevice->LightEnable( 1, TRUE );

		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );			/// 광원설정을 켠다
		m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00202020 );

	//
	// Set up a point light source...
	//
		/*
		ZeroMemory( &m_Light, sizeof(D3DLIGHT9) );
		m_Light.Type         = D3DLIGHT_POINT;
		m_Light.Position     = D3DXVECTOR3( 2.0f, 2.0f, 0.0f ); // World-space position
		m_Light.Diffuse.r    = 1.0f;
		m_Light.Diffuse.g    = 1.0f;
		m_Light.Diffuse.b    = 1.0f;
		m_Light.Range        = 100.0f;
		m_Light.Attenuation0 = 1.0f;
		m_Light.Attenuation1 = 0.0f;

		m_pd3dDevice->SetLight( 0, &m_Light );
		m_pd3dDevice->LightEnable( 0, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );
		*/
	}
	else
	{
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, false );
	}
}

/******************************************************************************
* SetupProjMatrices()
******************************************************************************/
void CGameEnv::SetupProjMatrices()
{
	float fAspect = ((float)m_nScreenWidth) / m_nScreenHeight;
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, fAspect, 1.0f, 5000.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );
}

void CGameEnv::RenderBegin()
{
	m_pd3dDevice->BeginScene();
	//m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    //m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
						D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );
}

void CGameEnv::RenderEnd()
{
	m_pd3dDevice->EndScene();
}

void CGameEnv::Flip()
{
	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}
