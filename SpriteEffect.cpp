#include "poolgame.h"

CSpriteEffect::CSpriteEffect()
{
	m_pd3dDevice			= NULL;
	m_pSprite				= 0;
	m_fEffectTime			= 0.0f;
	m_fTotalEffectTime		= 0.0f;
	m_bEndEffect			= true;
}