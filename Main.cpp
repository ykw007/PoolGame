/**
*
* Main.cpp
* ���� ����
* kyung wook yang (ykw007@hotmail.com)
* 
**/

#include "poolgame.h"

/******************************************************************************
* WinMain()
******************************************************************************/
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    // ���� �⺻ ȯ�� �ʱ�ȭ - ������ ����, Direct3D �ʱ�ȭ
	if( !g_sGameEnv->Create(hInst,CLASSNAME,WINDOWTITLE,SCREENWIDTH,SCREENHEIGHT) )
		return 0;

	// ���� �ν��Ͻ� ����
	CPoolGame Game;

	// �޽��� ���� ����
	MSG msg; 
	ZeroMemory( &msg, sizeof(msg) );

	// ���� �ʱ�ȭ
	if(!Game.Create())
		return 0;

	// ����׿� ��Ʈ ����
	g_sGameEnv->m_Font.Init(g_sGameEnv->GetDevice(),10,14);

	while( msg.message != WM_QUIT )
	{
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
			Game.Update();		// ���� ����
	}

    UnregisterClass( CLASSNAME, hInst );

	// ��Ʈ ����
	g_sGameEnv->m_Font.Clean();

	// singleton �̹Ƿ� �̷��� ��������� ������� �Ѵ�.
	delete g_sGameEnv;
    return 0;
}


/******************************************************************************
* MsgProc()
* �޽��� ó��
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