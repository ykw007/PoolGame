#include "poolgame.h"

void CSpriteButton::Clear()
{
}

void CSpriteButton::Destroy()
{
	Clear();
}

bool CSpriteButton::Output()
{
	CSprite::Output();

	if( IsMouseUp() )
	{
		this->DirectFrame(2);
		if( IsMouseClick() )
			return true;
	}
	else
	{
		this->DirectFrame(1);
	}

	return false;
}

bool CSpriteButton::IsMouseUp()
{
	POINT	pt;
	GetCursorPos( &pt );

	float fWidth = (m_fCellWidth>0) ? m_fCellWidth : m_fWidth;
	float fHeight = (m_fCellHeight>0) ? m_fCellHeight : m_fHeight;

	if( ( ((m_SpriteVertex[0].vP.x+0.5f) < pt.x) && (((m_SpriteVertex[0].vP.x+0.5f)+fWidth) > pt.x) ) &&
		( ((m_SpriteVertex[0].vP.y+0.5f) < pt.y) && (((m_SpriteVertex[0].vP.y+0.5f)+fHeight) > pt.y) ) )
	{
		return true;
	}
	else
		return false;
}

bool CSpriteButton::IsMouseClick()
{
	if( GetAsyncKeyState( VK_LBUTTON ) != 0 )
			return true;
	return false;
}