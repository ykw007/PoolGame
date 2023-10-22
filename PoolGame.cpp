#include "poolgame.h"

#define MAINMENU_SPRITE_NUM 3
#define MAINMENU_SPRITE_BUTTON_NUM 2



void CPoolGame::Clear()
{
	m_pd3dDevice				= g_sGameEnv->GetDevice();
	m_nGameFlow					= GAMELOGO;
	m_ppSprite					= 0;
	m_ppSpriteButton			= 0;
	m_pSpriteEffect				= 0;
}

void CPoolGame::Destroy()
{
	if(m_ppSprite!=NULL)
	{
		for(int i =0;i<MAINMENU_SPRITE_NUM;i++)
			FREE_MEM( m_ppSprite[i] );
	}
	FREE_ARRAY_MEM( m_ppSprite );

	if(m_ppSpriteButton!=NULL)
	{
		for(int i =0;i<MAINMENU_SPRITE_BUTTON_NUM;i++)
			FREE_MEM( m_ppSpriteButton[i] );
	}
	FREE_ARRAY_MEM( m_ppSpriteButton );

	FREE_MEM(m_pSpriteEffect);
	Clear();

}

bool CPoolGame::Create()
{
	//######################## 스프라이트 초기화
	m_ppSprite = new CSprite*[MAINMENU_SPRITE_NUM];
	for(int i =0;i<MAINMENU_SPRITE_NUM;i++)
		m_ppSprite[i] = new CSprite(m_pd3dDevice,640,480);

	m_ppSprite[0]->CreateVertex(0,0,640,480);
	m_ppSprite[0]->LoadTexture(".\\Sprite\\WGA2LOGO.TGA");

	m_ppSprite[1]->CreateVertex(0,0,640,480);
	m_ppSprite[1]->LoadTexture(".\\Sprite\\MAINMENU_BG.jpg");

	m_ppSprite[2]->CreateVertex(0,0,640,480);
	m_ppSprite[2]->LoadTexture(".\\Sprite\\loading.tga");

	// BUTTON sprite
	m_ppSpriteButton = new CSpriteButton*[MAINMENU_SPRITE_BUTTON_NUM];
	for(int i =0;i<MAINMENU_SPRITE_BUTTON_NUM;i++)
		m_ppSpriteButton[i] = new CSpriteButton(m_pd3dDevice,640,480);

	// PLAYER1 sprite
	m_ppSpriteButton[0]->CreateVertex(200.0f,200.0f,200.0f,72.0f);
	m_ppSpriteButton[0]->LoadTexture(".\\Sprite\\playnow.TGA",0Xff000000,false,false,
		2,200.0f,36.0f,200.0f,72.0f);

	// PLAYER2 sprite
	m_ppSpriteButton[1]->CreateVertex(200.0f,250.0f,200.0f,72.0f);
	m_ppSpriteButton[1]->LoadTexture(".\\Sprite\\exit.TGA",0XFF000000,false,false,
		2,200.0f,36.0f,200.0f,72.0f);

    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );

	g_sGameEnv->SetupProjMatrices();

	return true;
}

static float g_sFps = 0.0f;
void CPoolGame::Update()
{
	// 디버그용 fps 표시
//	char str[255];

	// 초당 프레임 속도 측정
	g_sGameEnv->DetermineFPS();
	g_sFps = g_sGameEnv->GetFPS();

	// GAME FLOW
	switch( m_nGameFlow )
	{

	case GAMELOGO:
		OutputGameLogo();
		break;

	case GAMEMAINMENU:		// 메인 메뉴 화면 출력
		MainStage();
		break;

	case INITPLAY:
		SetCursor(NULL);
		ShowCursor(false);
		// 엔진 초기화
		if( m_Engine.Create() )
		{
            m_nGameFlow = PLAYGAME;			// 초기화 완료 되었으면 게임으로 들어간다.

		}
		break;

	case PLAYGAME:
		Play();
		break;

	case EXITGAME:
		break;
	}

	//sprintf(str,"FPS: %.02f",g_sFps);
	//g_sGameEnv->m_Font.Print(str,10,10);

	g_sGameEnv->Flip();
}



void CPoolGame::Play()
{
	g_sGameEnv->SetupLights(true);
	m_Engine.Update();
}


void CPoolGame::OutputGameLogo()
{
	g_sGameEnv->RenderBegin();
	g_sGameEnv->SetupLights(false);

	m_ppSprite[0]->OnceWhenRender();
	m_ppSprite[0]->Output();

	if( GetAsyncKeyState( VK_LBUTTON ) != 0 )
	{
		POINT	pt;
		GetCursorPos( &pt );
		RECT	rc={0,0,0,};
		GetClientRect( g_sGameEnv->GetHwnd(), &rc );
		pt.x = rc.left;
		pt.y = rc.top;
		ClientToScreen( g_sGameEnv->GetHwnd(), &pt );
		SetCursorPos( pt.x, pt.y );

		m_nGameFlow = GAMEMAINMENU;
	}

	// 렌더링 종료
	g_sGameEnv->RenderEnd();
}

void CPoolGame::MainStage()
{
	// 커서가 항상 보이게 영역안에 가두어 둔다.
	RECT Clip={0,0,630,460};
	ClientToScreen( g_sGameEnv->GetHwnd(), (LPPOINT)&Clip );
	ClipCursor(&Clip);
	ShowCursor(TRUE);

	g_sGameEnv->RenderBegin();
	g_sGameEnv->SetupLights(false);

	m_ppSprite[1]->OnceWhenRender();
	m_ppSprite[1]->Output();

	m_ppSpriteButton[0]->OnceWhenRender();
	if(m_ppSpriteButton[0]->Output())
	{
		m_nGameFlow = INITPLAY;
		InitPlay();
		return;
	}

	m_ppSpriteButton[1]->OnceWhenRender();
	if(m_ppSpriteButton[1]->Output())
		SendMessage(g_sGameEnv->GetHwnd(),WM_DESTROY,0,0);


	// 렌더링 종료
	g_sGameEnv->RenderEnd();

}

void CPoolGame::InitPlay()
{
	g_sGameEnv->RenderBegin();
	//g_sGameEnv->SetupLights(false);

	m_ppSprite[2]->OnceWhenRender();
	m_ppSprite[2]->Output();

	// 렌더링 종료
	g_sGameEnv->RenderEnd();
}