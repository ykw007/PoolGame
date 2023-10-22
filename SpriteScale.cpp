#include "poolgame.h"

void CSpriteScale::Clear()
{
	m_fEffectTime = 0.0001f;
	m_fTotalEffectTime = 0.0f;
	m_bEndEffect = true;
}

void CSpriteScale::Destroy()
{
	Clear();
}

CSpriteScale::CSpriteScale(	CSprite* pTargetSprite,
							float fTotalEffectTime)
{
	Clear();
	m_pSprite = pTargetSprite;
	m_fTotalEffectTime = fTotalEffectTime;
	m_pd3dDevice = m_pSprite->GetDevice();
	m_bEndEffect = false;
}

bool CSpriteScale::Output(float fFadeTime)
{
	m_pSprite->OnceWhenRender();

	static float fScale = 1.0f;
	D3DXMATRIX matWorld,matScale,matTrans;

	if(fScale > 0.5f)
	{
		fScale-=fFadeTime;
	}
	D3DXMatrixScaling(&matScale,fScale,fScale,0.0f);
	//D3DXMatrixTranslation(&matTrans,200.0f+(fScale/4.0f),200.0f-(fScale*2.0f),0.0f);

	matWorld = matScale;// * matTrans;

	m_pd3dDevice->SetTransform( D3DTS_WORLD,&matWorld);

	m_pSprite->ApplyColorKey();

	m_pSprite->Output();

	return m_bEndEffect;
}
/*
	static float z=1.0f;
	D3DXMATRIX usermatrix, matW,matT;
	z-=fFadeTime;
	D3DXMatrixScaling(&matW, z,z,0.0f);
	D3DXMatrixTranslation(&matT,200.0f+(z/2.0f),200.0f+(z/2.0f),0.0f);
	usermatrix = matW * matT;
	m_pd3dDevice->SetTransform( D3DTS_WORLD,&usermatrix);
*/