/**
*
* Main.cpp
* 게임 메인
* kyung wook yang (ykw007@hotmail.com)
* 
**/

#include "poolgame.h"

/******************************************************************************
* WinMain()
******************************************************************************/
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    // 게임 기본 환경 초기화 - 윈도우 생성, Direct3D 초기화
	if( !g_sGameEnv->Create(hInst,CLASSNAME,WINDOWTITLE,SCREENWIDTH,SCREENHEIGHT) )
		return 0;

	// 게임 인스턴스 생성
	CPoolGame Game;

	// 메시지 루프 시작
	MSG msg; 
	ZeroMemory( &msg, sizeof(msg) );

	// 게임 초기화
	if(!Game.Create())
		return 0;

	// 디버그용 폰트 설정
	g_sGameEnv->m_Font.Init(g_sGameEnv->GetDevice(),10,14);

	while( msg.message != WM_QUIT )
	{
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
			Game.Update();		// 게임 루프
	}

    UnregisterClass( CLASSNAME, hInst );

	// 폰트 해제
	g_sGameEnv->m_Font.Clean();

	// singleton 이므로 이렇게 명시적으로 지워줘야 한다.
	delete g_sGameEnv;
    return 0;
}


/******************************************************************************
* MsgProc()
* 메시지 처리
******************************************************************************/
LRESULT CALLBACK MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
	case WM_KEYDOWN:
		if(LOWORD(wParam)==VK_ESCAPE)
		{
			SendMessage(hWnd,WM_DESTROY,0,0);
		}
		return 0;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}