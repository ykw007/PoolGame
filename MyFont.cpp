#include "poolgame.h"

void CMyFont::Init(LPDIRECT3DDEVICE9 lpD3DDEV, int w, int h)
{
	LPD3DXFONT pFont = NULL;
	HDC		hTextDC = NULL;
	HFONT	hFont = NULL, hOldFont = NULL;
	
	hTextDC = CreateCompatibleDC(NULL);
    hFont = CreateFont( h, w, 0, 0,
                      FW_REGULAR,
                      FALSE, FALSE, FALSE,
                      SHIFTJIS_CHARSET,
                      OUT_DEFAULT_PRECIS,
                      CLIP_DEFAULT_PRECIS,
                      DEFAULT_QUALITY,
                      DEFAULT_PITCH,
                      "MS °íµñ"
                      );

	if(!hFont) return;
	hOldFont = (HFONT)SelectObject(hTextDC, hFont);
	//D3DXCreateFont(lpD3DDEV, h, w, FW_DONTCARE, 1, FALSE, DEFAULT_CHARSET,
		//OUT_DEFAULT_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		//"MS °íµñ", &m_lpFont);
	if( FAILED( D3DXCreateFont(lpD3DDEV, h, w, FW_DONTCARE, 1, FALSE, DEFAULT_CHARSET,
	OUT_DEFAULT_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		"MS °íµñ", & m_lpFont) ) ) {
		MessageBox( 0, "D3DXCreateFontIndirect FALSE", "ok", MB_OK);
		return;
	}
	SelectObject(hTextDC, hOldFont);
	DeleteObject(hFont);
	DeleteDC(hTextDC);
}

void CMyFont::Print( char *str, int x, int y, D3DCOLOR color )
{
	RECT rect;
	rect.left = x; 
	rect.right = SCREENWIDTH;
	rect.top = y;
	rect.bottom = SCREENHEIGHT;

	//m_lpFont->DrawText(str, -1, &rect,DT_LEFT | DT_EXPANDTABS,color);

	m_lpFont->DrawText(NULL, str,
		-1, &rect, DT_LEFT | DT_EXPANDTABS, color);
}

void CMyFont::Clean()
{
	RELEASE(m_lpFont);
}

