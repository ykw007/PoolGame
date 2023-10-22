#include "poolgame.h"

void CSprite::Clear()
{
	m_pd3dDevice	= NULL;
	m_pTexture		= NULL;
	m_pVB			= NULL;

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_fWidth = 0.0f;
	m_fHeight = 0.0f;

	m_nScreenWidth	= 0;
	m_nScreenHeight	= 0;

	m_bColorKey		= false;
	m_bAnimation	= false;
	m_bCutWidth		= true;
	m_bEndAnimation = true;
	m_nMaxAnimationFrame = 0;
	m_nAnimationFramecounter = 0;
	m_fCellWidth = 0.0f;
	m_fCellHeight = 0.0f;
	m_fCellWidth_InTexture = 0.0f;
	m_fCellHeight_InTexture = 0.0f;
	m_fTotalWidth = 0.0f;
	m_fTotalHeight = 0.0f;
	m_fDelayTime = 0.0f;
	m_fCurDelayTime = 0.0f;
	m_fFps = 0.0f;
}

void CSprite::Destroy()
{
	RELEASE(m_pTexture);
	RELEASE(m_pVB);
	Clear();
}

CSprite::CSprite(LPDIRECT3DDEVICE9 pd3dDevice,
					  int nScreenWidth,
					  int nScreenHeight)
{
	Clear();
	m_pd3dDevice = pd3dDevice;
	m_nScreenWidth = nScreenWidth;
	m_nScreenHeight = nScreenHeight;
}

void CSprite::LoadTexture(const char *pImagePath,
						  D3DCOLOR ColorKey,
						  bool bAnimation,
						  bool bCutWidth,
						  int nMaxAnimationFrame,
						  float fCellWidth,
						  float fCellHeight,
						  float fTotalWidth,
						  float fTotalHeight,
						  float fDelayTime)
{
	RELEASE(m_pTexture);

	if( ColorKey!=0 )
	{
		ColorKey = ColorKey | 0xff000000;
		m_bColorKey	= true;
	}

	//################### 텍스쳐 로딩
	if( FAILED( D3DXCreateTextureFromFileEx( m_pd3dDevice,
		pImagePath,
        D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
		D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
		ColorKey,
		NULL,
		NULL,
		&m_pTexture ) ) )
	{
		m_pTexture = NULL;
	}

	m_bAnimation=bAnimation;
	m_bCutWidth = bCutWidth;
	m_nMaxAnimationFrame = nMaxAnimationFrame;
	m_fCellWidth = fCellWidth;
	m_fCellHeight = fCellHeight;
	m_fTotalWidth = fTotalWidth;
	m_fTotalHeight = fTotalHeight;
	if(m_bAnimation) m_bEndAnimation = false;
	m_fCurDelayTime = m_fDelayTime = fDelayTime;
	if(m_fCellWidth) DirectFrame(1);
}


bool CSprite::CreateVertex(float fX,float fY,float fWidth,float fHeight)
{

	m_fWidth = fWidth;
	m_fHeight = fHeight;

	//################### 정점 생성
	m_SpriteVertex[0].vP = D3DXVECTOR3( fX - 0.5f,
										fY - 0.5f,
										0.0f );
	m_SpriteVertex[0].dwColor = 0XFFFFFFFF;
	m_SpriteVertex[0].fU = 0.0f;
	m_SpriteVertex[0].fV = 0.0f;


	m_SpriteVertex[1].vP = D3DXVECTOR3( m_SpriteVertex[0].vP.x + fWidth,
										m_SpriteVertex[0].vP.y,
										0.0f );
	m_SpriteVertex[1].dwColor = 0XFFFFFFFF;
	m_SpriteVertex[1].fU = 1.0f;
	m_SpriteVertex[1].fV = 0.0f;


	m_SpriteVertex[2].vP = D3DXVECTOR3( m_SpriteVertex[0].vP.x,
										m_SpriteVertex[0].vP.y + fHeight,
										0.0f );
	m_SpriteVertex[2].dwColor = 0XFFFFFFFF;
	m_SpriteVertex[2].fU = 0.0f;
	m_SpriteVertex[2].fV = 1.0f;


	m_SpriteVertex[3].vP = D3DXVECTOR3( m_SpriteVertex[0].vP.x + fWidth,
										m_SpriteVertex[0].vP.y + fHeight,
										0.0f );
	m_SpriteVertex[3].dwColor = 0XFFFFFFFF;
	m_SpriteVertex[3].fU = 1.0f;
	m_SpriteVertex[3].fV = 1.0f;

    if( FAILED( m_pd3dDevice->CreateVertexBuffer( 4*sizeof(SPRITE_VERTEX),
                                                  0, D3DFVF_SPRITE_VERTEX,
                                                  D3DPOOL_DEFAULT, &m_pVB, NULL ) ) )
    {
        return false;
    }

    SPRITE_VERTEX* pVertices;
	if( FAILED( m_pVB->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
        return false;

	memcpy( pVertices, m_SpriteVertex, 4*sizeof(SPRITE_VERTEX) );

    m_pVB->Unlock();

	return true;
}

bool CSprite::SetVertexColor(D3DCOLOR Color)
{
    SPRITE_VERTEX* pVertices;

	if( FAILED( m_pVB->Lock( 0, 4*sizeof(SPRITE_VERTEX), (void**)&pVertices, 0 ) ) )
        return false;

	for(int i =0;i<4;i++)
        pVertices[i].dwColor = Color;
  
    if( FAILED( m_pVB->Unlock() ) )
        return false;

	return true;
}

// 렌더링마다 한번씩만
void CSprite::OnceWhenRender()
{
	m_pd3dDevice->GetTransform( D3DTS_WORLD,&m_matWorld);
	m_pd3dDevice->GetTransform( D3DTS_VIEW,&m_matView );
	m_pd3dDevice->GetTransform( D3DTS_PROJECTION,&m_matProj );

	m_pd3dDevice->SetFVF( D3DFVF_SPRITE_VERTEX );

	D3DXMATRIXA16 matOrtho;
	D3DXMatrixIdentity(&matOrtho);

	m_pd3dDevice->SetTransform( D3DTS_WORLD,&matOrtho);
	m_pd3dDevice->SetTransform( D3DTS_VIEW,&matOrtho );

	D3DXMatrixOrthoOffCenterLH( &matOrtho,
								0.0f,
								(float)m_nScreenWidth,
								(float)m_nScreenHeight,
								0.0f,
								0.0f,
								1.0f);

	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matOrtho );
}

void CSprite::Output()
{
	if(m_bColorKey)
		ApplyColorKey();

	// 애니메이션
	if(m_bAnimation && !m_bEndAnimation && m_fFps>0.0f)
	{
		float fTime = (1.0f/m_fFps)*3.5f;
		if( (m_fDelayTime-m_fCurDelayTime) < 0.1f )
		{
			ApplyAnimation();
			m_fCurDelayTime = 0.0f;
		}
		else
            m_fCurDelayTime += fTime;
	}

	m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(SPRITE_VERTEX) );
  	m_pd3dDevice->SetTexture( 0, m_pTexture );
	m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

	// 다음의 다각형이 반투명이 되지 않게
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	m_pd3dDevice->SetTransform( D3DTS_WORLD,&m_matWorld);
	m_pd3dDevice->SetTransform( D3DTS_VIEW,&m_matView );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );
}

void CSprite::ApplyColorKey()
{

	// texture의 blending 방법 정의
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);// 인수의 성분을 곱셈한다
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	// 알파 합성 설정
	// 내삽：색 유리같이, 뒤가 비쳐 보이는 표현
	// 반투명의 다각형의 알파치를 바꾸는 일에 의해
	// 진함을 조정할 수 있다
	// (0:투명~255:불투명). 
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
}

void CSprite::ApplyAnimation()
{
	float fW,fH,fTx,fTy,fTw,fTh;

	m_nAnimationFramecounter	= (m_nAnimationFramecounter+1)%m_nMaxAnimationFrame;
	m_fCellWidth_InTexture		= m_fCellWidth / m_fTotalWidth;
	m_fCellHeight_InTexture		= m_fCellHeight / m_fTotalHeight;

	fW = m_fCellWidth;
	fH = m_fCellHeight;
	
	if(m_bCutWidth)
	{
		//가로 애니메이션일때
		fTx = m_nAnimationFramecounter * m_fCellWidth_InTexture;
		fTy = 0.0f;
	}
	else
	{
		//세로 애니메이션일때
		fTx = 0.0f;
		fTy = m_nAnimationFramecounter * m_fCellHeight_InTexture;
	}

	fTw = m_fCellWidth_InTexture;
	fTh = m_fCellHeight_InTexture;

	SPRITE_VERTEX* v;

	m_pVB->Lock( 0, 4*sizeof(SPRITE_VERTEX), (void**)&v, 0 );

	v[1].vP.x = v[0].vP.x + fW;
	v[1].vP.y = v[0].vP.y;
	v[2].vP.x = v[0].vP.x;
	v[2].vP.y = v[0].vP.y + fH;
	v[3].vP.x = v[0].vP.x + fW;
	v[3].vP.y = v[0].vP.y + fH;

	v[0].fU = fTx;
	v[0].fV = fTy;
	v[1].fU = v[0].fU + fTw;
	v[1].fV = v[0].fV;
	v[2].fU = v[0].fU;
	v[2].fV = v[0].fV + fTh;
	v[3].fU = v[0].fU + fTw;
	v[3].fV = v[0].fV + fTh;

	m_pVB->Unlock();

	// 애니메이션을 반복하지 않을때..
	//if(m_nAnimationFramecounter==0)
	//	m_bEndAnimation = true;
}

void CSprite::SetPosition(float fX,float fY)
{
	SPRITE_VERTEX* v;

	m_pVB->Lock( 0, 4*sizeof(SPRITE_VERTEX), (void**)&v, 0 );

	v[0].vP = D3DXVECTOR3( fX - 0.5f,fY - 0.5f,	0.0f );
	v[1].vP = D3DXVECTOR3( v[0].vP.x + m_fWidth,v[0].vP.y, 0.0f );
	v[2].vP = D3DXVECTOR3( v[0].vP.x, v[0].vP.y + m_fHeight, 0.0f );
	v[3].vP = D3DXVECTOR3( v[0].vP.x + m_fWidth, v[0].vP.y + m_fHeight, 0.0f );

	m_pVB->Unlock();
}

void CSprite::DirectFrame(int nFrame)
{
	m_nAnimationFramecounter = nFrame;
	ApplyAnimation();
}