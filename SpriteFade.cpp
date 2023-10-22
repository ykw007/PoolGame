#include "poolgame.h"

void CSpriteFade::Clear()
{
	m_FadeType = FADEIN;
	m_FadeColor = D3DXCOLOR(0,0,0,0);
	m_fEffectTime = 0.0001f;
	m_fTotalEffectTime = 0.0f;
	m_fAlphaRate = 0.0f;
	m_bEndEffect = true;
}

void CSpriteFade::Destroy()
{
	Clear();
}

CSpriteFade::CSpriteFade(	FADETYPE FadeType,
							CSprite* pTargetSprite,
							D3DXCOLOR FadeColor,
							float fTotalEffectTime,
							float fAlphaRate)
{
	Clear();
	m_FadeType = FadeType;
	m_pSprite = pTargetSprite;
	m_FadeColor = FadeColor;
	m_fTotalEffectTime = fTotalEffectTime;
	m_fAlphaRate = fAlphaRate;
	m_pd3dDevice = m_pSprite->GetDevice();
	m_bEndEffect = false;
}

void CSpriteFade::ComputeToFade(float fFadeTime)
{
	D3DXCOLOR FadeColor = m_FadeColor;

	m_fEffectTime += fFadeTime;

	if(m_FadeType == FADEOUT)
	{
		if(m_FadeColor.a==0.0f)
			FadeColor.a = 1.0f-(m_FadeColor.a+(m_fEffectTime / m_fTotalEffectTime));
		else
			FadeColor.a = 1.0f-(m_FadeColor.a*(m_fEffectTime / m_fTotalEffectTime));

		if (FadeColor.a < (1.0f-m_fAlphaRate) )
		{
			FadeColor.a = (1.0f-m_fAlphaRate);
			m_bEndEffect = true;
		}
	}
	else if(m_FadeType == FADEIN)
	{
		if(m_FadeColor.a==0.0f)
			FadeColor.a = (1.0f-m_fAlphaRate)+(m_FadeColor.a+(m_fEffectTime / m_fTotalEffectTime));
		else
			FadeColor.a = (1.0f-m_fAlphaRate)+(m_FadeColor.a*(m_fEffectTime / m_fTotalEffectTime));

		if (FadeColor.a > 1.0f )
		{
			FadeColor.a = 1.0f;
			m_bEndEffect = true;
		}
	}

	m_pSprite->SetVertexColor((DWORD)FadeColor);
}

bool CSpriteFade::Output(float fFadeTime)
{
	m_pSprite->OnceWhenRender();

    m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
  
    //m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    //m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    //m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    //m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

	m_pSprite->Output();

    if (m_fEffectTime > 0.0f) {

		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );

		// 시간에 따른 정점 색 변환
		ComputeToFade(fFadeTime);
    
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		//m_pSprite->Output();

		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    }

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

	return m_bEndEffect;
}
