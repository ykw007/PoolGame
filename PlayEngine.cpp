#include "poolgame.h"

// ī�޶� �ΰ��� - ����Ÿ �ε����� ����
#define MOVDELTA			2.5f
#define ROTDELTA			0.01f			// ���콺 �ΰ��� �׸����� ����� �Է� ���� �� �ְ�

//#define CAMERA_LIMIT_NEAR	20.0f
//#define CAMERA_LIMIT_FAR	500.0f

#define BALL_NUM			4			// �� ����
#define CUSHION_RESTITUTION	0.6f		// ��� ź�� ���
#define BALL_RESTITUTION	0.2f		// ���� ź�� ���
#define BALL_EPSILON		0.001f		// �� ��� EPSILON

#define SPRITE_NUM			4
#define PLAYER_NUM			2
#define BEAD_NUM			5			// ä���� ���� ������ ����

// ���� �ʱ�ȭ
void CPlayEngine::Clear()
{
	m_pd3dDevice = 0;
	m_pPlayer = 0;
	m_nNumPlayer = 0;
	m_nShotOrder = 0;
	m_pBall = 0;
	m_pQue = 0;
	m_ppSprite = 0;
	m_pSpriteEffect = 0;
	m_pCameraMng = 0;
	m_nNumBall = 0;
	m_fDeltaTime = 0.0f;
	m_dwMouseY = m_dwMouseX = 0;
	m_PlayFlow = SELECT_ORDER;
}

void CPlayEngine::Destroy()
{
	FREE_ARRAY_MEM(m_pCameraMng)

	// �޸� �����ϱ����� �����ϰ� �ִ� ù��° ���� �����ϰ�
	// ������ ���� 3d data �����͸� 0���� �����.
	if(m_nNumBall==1){
		FREE_MEM(m_pBall)
	}
	else if(m_nNumBall > 1)
	{
		// ù��° �ν��Ͻ��� �����ڿ��� �����ϰ� �������� �����͸� �η�
		for(int i=1;i<m_nNumBall;i++)
			(m_pBall+i)->Set3dData(0);
		FREE_ARRAY_MEM(m_pBall)
	}

	FREE_MEM(m_pQue)

	if(m_ppSprite!=NULL)
	{
		for(int i =0;i<SPRITE_NUM;i++)
			FREE_MEM( m_ppSprite[i] );
	}

	FREE_ARRAY_MEM( m_ppSprite );
	FREE_MEM( m_pSpriteEffect );

	FREE_ARRAY_MEM(m_pPlayer);
}

/******************************************************************************
* bool CPlayEngine::Create()
* ���� �ʱ�ȭ
******************************************************************************/
bool CPlayEngine::Create()
{

	m_pd3dDevice = g_sGameEnv->GetDevice();

	m_nNumBall = BALL_NUM;

	//######################## �� �� �ʱ�ȭ
	D3DXVECTOR3	vRoomPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3	vTablePos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
//	D3DXVECTOR3	vQuePos = D3DXVECTOR3( 100.0f, 50.0f, -50.0f );
	D3DXVECTOR3	vQuePos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	D3DXVECTOR3	vBallPos[BALL_NUM];
	
	vBallPos[0] = D3DXVECTOR3( -240.0f, 142.0f, 70.0f );
	vBallPos[1] = D3DXVECTOR3( -240.0f, 142.0f, 110.0f );
	vBallPos[2] = D3DXVECTOR3( 81.5f, 142.0f, 80.0f );
	vBallPos[3] = D3DXVECTOR3( 40.0f, 142.0f, 60.0f );


	char* paMap[] = 
	{
		".\\Data\\ball_black.bmp",
		".\\Data\\ball_yellow_solid.bmp",
		".\\Data\\ball_red_solid.bmp",
		".\\Data\\ball_red_solid.bmp"
	};

	//######################## ROOM �ʱ�ȭ
	if( !m_Room.Create( m_Ase.ReadAse(ROOM_FILE), vRoomPos) )
		return false;

	//######################## �籸 ���̺� �ʱ�ȭ
	if( !m_Table.Create( m_Ase.ReadAse(TABLE_FILE),vTablePos,4) )
		return false;

	//######################## �� �ʱ�ȭ
	m_pBall = new CBall[m_nNumBall];
	for(int i=0;i<m_nNumBall;i++)
	{
		if( !(m_pBall+i)->Create( paMap[i], vBallPos[i] ) )
			return false;
	}

	//######################## ī�޶� ������ �غ�
	D3DXVECTOR3 vEyePt = D3DXVECTOR3( 0.0f, 400.0f, -300.0f );
	D3DXVECTOR3 vEyePt2 = D3DXVECTOR3( 0.0f, 550.0f, -400.0f );
	D3DXVECTOR3 vLookatPt = m_pBall[0].GetPosition();  //D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vLookatPt2 = D3DXVECTOR3( 0.0f, 142.0f, 0.0f );
	D3DXVECTOR3 vUpVec = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	m_pCameraMng = new CCameraMng[2];
	m_pCameraMng->Init(vEyePt, vLookatPt, vUpVec);				// ť ī�޶� �غ�
	(m_pCameraMng+1)->Init(vEyePt2, vLookatPt2, vUpVec);

	//######################## ť �ʱ�ȭ
	D3DXVECTOR3 vQueDir = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_pQue = new CQue;
	if( !m_pQue->Create( m_Ase.ReadAse(QUE_FILE), vQueDir) )
		return false;
	m_pQue->Hide();	// ���� ť �����

	// ī�޶� ������ ť �ʱ�ȭ - �𷺼� ���� ����
	m_pQue->Init( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ),
					m_pCameraMng->GetUp(),
					m_pCameraMng->GetCross() );

	//######################## player �ʱ�ȭ
	m_nNumPlayer = PLAYER_NUM;
	m_nShotOrder = m_nNumPlayer - 1;

	m_pPlayer = new CPlayer[m_nNumPlayer];
	for(int i=0;i<m_nNumPlayer;i++)
        m_pPlayer[i].Init(m_pBall+i,BEAD_NUM);

	//######################## ��������Ʈ �ʱ�ȭ
	m_ppSprite = new CSprite*[SPRITE_NUM];
	for(int i =0;i<SPRITE_NUM;i++)
		m_ppSprite[i] = new CSprite(m_pd3dDevice,640,480);

	// PLAYER1 sprite
	m_ppSprite[0]->CreateVertex(0.0f,400.0f,200.0f,72.0f);
	m_ppSprite[0]->LoadTexture(".\\Sprite\\player1.TGA",0Xff000000,false,false,
		2,200.0f,36.0f,200.0f,72.0f);

	// PLAYER2 sprite
	m_ppSprite[1]->CreateVertex(0.0f,436.0f,200.0f,72.0f);
	m_ppSprite[1]->LoadTexture(".\\Sprite\\player2.TGA",0XFF000000,false,false,
		2,200.0f,36.0f,200.0f,72.0f);

	// ���ھ� ���� ���
	m_ppSprite[2]->CreateVertex(200.0f,400.0f,440.0f,72.0f);
	m_ppSprite[2]->LoadTexture(".\\Sprite\\scoreboard.TGA",0XFF000000);

	// ä���� ����
	m_ppSprite[3]->CreateVertex(207.0f,401.0f,17.0f,34.0f);
	m_ppSprite[3]->LoadTexture(".\\Sprite\\bead_white.TGA",0XFF000000);

	return true;
}

/******************************************************************************
* void CPlayEngine::Update()
* ���� ����
******************************************************************************/
static bool s_bFixCamera = false;
static int s_nCounter = 0;
void CPlayEngine::Update()
{
	CSpriteFade* pSpriteFade=0;
	CIntersectionTest IntersectionTest;

	// �� �����Ӵ� �ɸ��� �ð�
	float fFps = g_sGameEnv->GetFPS();
	//m_fDeltaTime = (1.0f/fFps)*4.5f;

	//if(m_fDeltaTime<0.054f)
	m_fDeltaTime = 0.054f;

	//char str[255];

	// �����̴� ���� ����
	int nBallActiveCount = 0;

	SetCursor(NULL);
	ShowCursor(false);
	if( m_PlayFlow!=READY_QUE && m_PlayFlow != SELECT_ORDER )
		ProcessMouse( g_sGameEnv->GetHwnd() );

	ProcessKey();

	//######################## ���� �÷��� FLOW
	switch(m_PlayFlow)
	{
	case SELECT_ORDER:
		if( m_Referee.GetResult() != m_Referee.NICESHOT && m_Referee.GetResult() != m_Referee.SHOT )
		{
			// ���� ��� ��
			m_ppSprite[m_nShotOrder]->DirectFrame(1);
			m_ppSprite[m_nShotOrder]->SetVertexColor( (DWORD)D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f) );

			// ���� ��� ��
			m_nShotOrder = (m_nShotOrder+1)%m_nNumPlayer;
			m_ppSprite[m_nShotOrder]->DirectFrame(2);

			FREE_MEM( m_pSpriteEffect );

			// �÷��̾� ��������Ʈ ȿ�� - FADE OUT ���� ����
			pSpriteFade = new CSpriteFade(CSpriteFade::FADEOUT,
										m_ppSprite[m_nShotOrder],
										D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f),
										2.0f,0.5f);// 50% ���� ���ĺ�
			m_pSpriteEffect = pSpriteFade;
		}

		// ���� �ʱ�ȭ
		m_Referee.Clear();

		m_PlayFlow = READY_QUE;
		break;

	case READY_QUE:
		if(m_pCameraMng->TakeBasePos(m_pPlayer[m_nShotOrder].GetMyBall()->GetPosition()))
		{
			m_PlayFlow = READY_SHOT;
			s_bFixCamera = true;
		}
		break;
	case READY_SHOT:		// ť�� �غ��Ѵ�. - ť�� ��ġ��Ų��.
		if(s_nCounter==1)
			m_pQue->Ready( m_pPlayer[m_nShotOrder].GetMyBall()->GetPosition() );
		if(s_bFixCamera)
		{
            if(++s_nCounter > 25) 
			{
				s_nCounter = 0;
				s_bFixCamera = false;
			}
		}
		break;
	case WAIT:		// ���� �̵��ϴ� ���� ��ٸ���.
		// �� �̵��� ī�޶� �̵�
		//m_pCameraMng->MoveCameraX(m_pPlayer[m_nShotOrder].GetMyBall()->GetAccel().x);
		//m_pCameraMng->MoveCameraZ(-m_pPlayer[m_nShotOrder].GetMyBall()->GetAccel().z);
		// �浹üũ - ������, ���� ���̺�
		for(int i=0;i<m_nNumBall;i++)
		{
			(m_pBall+i)->Update(m_fDeltaTime);

			if( (m_pBall+i)->GetActivity() )
				nBallActiveCount++;
		}
		// �浹üũ - �浹�� �浹 ����ü �ۼ�
		DetectCollision();

		// ��� ����
		if(!m_Referee.GetEndReferee())
			m_Referee.Update(m_nShotOrder,m_pBall,&m_CollisionList);

		// �浹 ����Ʈ�� �浹 ����
		ResolveCollision();

		// �����̴� ���� ������ ���� ����
		if(nBallActiveCount==0)	m_PlayFlow = WAIT_REFEREE;
		if(s_bFixCamera)
		{
            if(++s_nCounter > 35) 
			{
				s_nCounter = 0;
				s_bFixCamera = false;
			}
		}
		break;
	case WAIT_REFEREE:
		// ���� ó��
		if( m_Referee.GetResult() == m_Referee.FOUL || m_Referee.GetResult() == m_Referee.NOSHOT )
		{
			m_pPlayer[m_nShotOrder].AddBead();
		}
		else if( m_Referee.GetResult() == m_Referee.NICESHOT )
		{
			m_pPlayer[m_nShotOrder].SubtractBead();
		}
		else if( m_Referee.GetResult() == m_Referee.SHOT )
		{
			m_pPlayer[m_nShotOrder].SubtractBead();
		}

		m_PlayFlow = SELECT_ORDER;
		break;
	}

	// ������Ʈ ������
	RenderScene();

	//sprintf(str,"1 Tick : %.05f\n"	,m_fDeltaTime	);
	//g_sGameEnv->m_Font.Print(str,10,20);

}

/******************************************************************************
* void CPlayEngine::RenderScene()
* ��� ������
******************************************************************************/
int g_nCamera = 0;
void CPlayEngine::RenderScene()
{
	D3DXMATRIXA16 matView,matViewCamera2;

	// �� ���
	if( g_nCamera == 0 )
	{
		matView = m_pCameraMng->Update();
		m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	}
	if( g_nCamera == 1 )
	{
		matViewCamera2 = (m_pCameraMng+1)->Update();
		m_pd3dDevice->SetTransform( D3DTS_VIEW, &matViewCamera2 );
	}

	// ��� ������
	m_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );

	// ������ ����
	g_sGameEnv->RenderBegin();

	//####################### skybox
	m_Room.Primitive();

	//####################### table
	m_Table.Primitive();

	//####################### que
	if( m_PlayFlow == READY_SHOT )
       m_pQue->Primitive();

	//####################### ball
	for(int i=0;i<m_nNumBall;i++)
		(m_pBall+i)->Primitive();

	//####################### sprites

	// ä���� ��׶��� ǥ��
	m_ppSprite[2]->OnceWhenRender();
	m_ppSprite[2]->Output();

	for(int i=0;i<PLAYER_NUM;i++)
	{
		if(m_nShotOrder!=i)
		{
			m_ppSprite[i]->OnceWhenRender();
			m_ppSprite[i]->Output();
		}

		// �÷��̾��� ���� ���� ǥ��
		for(int j=0;j<m_pPlayer[i].GetMyRestBead();j++)
		{
			// ��������Ʈ ��ġ ����
			m_ppSprite[3]->SetPosition( 207.0f+(13.0f*j), 401.0f+(i*36.0f) );
			// ���
			m_ppSprite[3]->OnceWhenRender();
			m_ppSprite[3]->Output();
		}

	}

	// �÷��̾� ĥ ���� �˸� EFFECT
	if(m_pSpriteEffect->Output(0.05f))
	{
		CSpriteFade* pSpriteFade = (CSpriteFade*)m_pSpriteEffect;

		if(pSpriteFade->GetFadeType()==CSpriteFade::FADEIN)
			pSpriteFade->SetToCotinue(CSpriteFade::FADEOUT);
		else if(pSpriteFade->GetFadeType()==CSpriteFade::FADEOUT)
			pSpriteFade->SetToCotinue(CSpriteFade::FADEIN);
	}

	// ������ ����
	g_sGameEnv->RenderEnd();
}

/******************************************************************************
* void CPlayEngine::ProcessMouse(HWND hWnd)
* ���콺 ó��
******************************************************************************/
void CPlayEngine::ProcessMouse(HWND hWnd)
{
	POINT	pt;
	GetCursorPos( &pt );
	int dx = pt.x - m_dwMouseX;	// ���콺�� ��ȭ��
	int dy = pt.y - m_dwMouseY;	// ���콺�� ��ȭ��

	// ���콺�� �������� �߾����� �ʱ�ȭ
	RECT	rc={0,0,0,};
	GetClientRect( hWnd, &rc );
	pt.x = (rc.right - rc.left) / 2;
	pt.y = (rc.bottom - rc.top) / 2;
	ClientToScreen( hWnd, &pt );
	SetCursorPos( pt.x, pt.y );
	m_dwMouseX = pt.x;
	m_dwMouseY = pt.y;


	float fForce = 0.0f;
	D3DXVECTOR3 vYaw = D3DXVECTOR3(0.0f,1.0f,0.0f);
	D3DXVECTOR3 vForce = D3DXVECTOR3(0.0f,0.0f,0.0f);

	if( s_bFixCamera )
	{
		dx = 0;
		dy = 0;
	}
	
	if( GetAsyncKeyState( VK_CONTROL ) != 0 && m_PlayFlow == READY_SHOT )
	{
		if( (fForce = m_pQue->Move((float)dy)) > 0.0f )
		{
			// ť ��
			vForce = m_pQue->Shot((fForce));
			vForce.y = 0.0f;
			// ���� ��ݷ�
			m_pPlayer[m_nShotOrder].GetMyBall()->SetForce(&vForce);
			m_PlayFlow = WAIT;
			s_bFixCamera = true;
			// ť ����
			m_pQue->Hide();
		}
	}
	else
	{
		m_pQue->SetMovPos();

		// ���콺 x ��ǥ�� ���� ȸ�� ����
		float fAngle = dx * ROTDELTA;

		// ī�޶� move
		m_pCameraMng->MoveCameraZ(-dy * MOVDELTA);

		// ī�޶� �ٶ󺸴� ������ ť�� ����
		m_pCameraMng->CircleAxis( m_pPlayer[m_nShotOrder].GetMyBall()->GetPosition(), vYaw, fAngle, 0.3f );		// ī�޶� ȸ��
		m_pCameraMng->AdjustCameraY();
		m_pQue->TurnOnYaxis( m_pCameraMng->GetDirection(),m_pCameraMng->GetUp(),m_pCameraMng->GetCross() );
	}



}

/******************************************************************************
* void CPlayEngine::ProcessKey()
* Ű ó��
******************************************************************************/
void CPlayEngine::ProcessKey()
{

	// ī�޶� ����
	if( GetAsyncKeyState( VK_F2 ) != 0 )
	{
		g_nCamera = 0;
	}
	if( GetAsyncKeyState( VK_F3 ) != 0 )
	{
		g_nCamera = 1;
	}

	if( GetAsyncKeyState( VK_LEFT ) != 0 )
		m_pCameraMng->MoveCamera(CCameraControl::X,MOVDELTA,false);
	if( GetAsyncKeyState( VK_RIGHT ) != 0 )
		m_pCameraMng->MoveCamera(CCameraControl::X,-MOVDELTA,false);

	if( GetAsyncKeyState( VK_UP ) != 0 )
		m_pCameraMng->MoveCamera(CCameraControl::Z,-MOVDELTA,false);
	if( GetAsyncKeyState( VK_DOWN ) != 0 )
		m_pCameraMng->MoveCamera(CCameraControl::Z,MOVDELTA,false);

}


/******************************************************************************
* void CPlayEngine::DetectCollision()
* �浹 üũ
******************************************************************************/
static int s_Count=0;
void CPlayEngine::DetectCollision()
{
	//################# ���� ����
	float fRadius, fCollisionTime, fDst, fGap;
	D3DXVECTOR3 vVelocity,vCollisionPos, vOldPos, vPos;
	COLLISION* pCollision;
	CBall *pBall,*pBall2;
	CPlane** ppPlane;
	int nNumPlane;
	CIntersectionTest CollisionTest;

	//################# ������ �浹 üũ�� �ʿ��� ���� ����
	D3DXVECTOR3 vBall2Pos,vBall2OldPos,vDst;
	float fBall2Radius;
	CBall *pSaveBall,*pSaveBall2;
	bool bCheckFlag;

	// ��, ��� ������
	ppPlane = m_Table.GetPlane();
	nNumPlane = m_Table.GetNumPlane();

	//################## �� ���� ��ŭ ����
	for(int bi = 0; bi < m_nNumBall; bi++)
	{
		pBall = (m_pBall+bi);
		vVelocity = pBall->GetVelocity();
		vOldPos = pBall->GetOldPosition();
		vPos = pBall->GetPosition();
		fRadius = pBall->GetRadius();

		//#################### ��ǿ� ���� �浹 üũ - ����
		for( int i=0 ; i < nNumPlane ; i++ )
		{
			// ��� �浹�� - �浹 ����ü ����
			if(CollisionTest.SpherePlaneSweep(fRadius,vOldPos,vPos,*(ppPlane[i]),
				vCollisionPos,fCollisionTime))
			{
				// �浹�� ����ü�� ����
				pCollision = new COLLISION;
				pCollision->Type = WITH_CUSHION;
				pCollision->Cc.BallNcushion.pBall = pBall;
				pCollision->Cc.BallNcushion.pPlane = ppPlane[i];
				pCollision->fCollisionTime = fCollisionTime;

				m_CollisionList.push_back(pCollision);
			}
		}
		//#################### ��ǿ� ���� �浹 üũ - ��

		//#################### �ٸ� ������ �浹 üũ - ����
		for( int bj=0 ; bj < m_nNumBall ; bj++ )
		{
			pBall2 = (m_pBall+bj);

			// ���� ���� �����ϰ�
			if( pBall2 != pBall )
			{
				bCheckFlag = true;

				// �ߺ� üũ���� �˻� - ����
				list<COLLISION*>::iterator pi;
				for( pi=m_CollisionList.begin(); pi!=m_CollisionList.end(); ++pi)
				{
					pCollision = (COLLISION*)(*pi);
					
					if( pCollision->Type == WITH_BALL )
					{
						pSaveBall = pCollision->Cc.BallNball.pBall;
						pSaveBall2 = pCollision->Cc.BallNball.pBall2;

						if( (pSaveBall == pBall && pSaveBall2 == pBall2) || 
							(pSaveBall == pBall2 && pSaveBall2 == pBall) )
						{
							bCheckFlag = false;
							break;
						}
					}
				}
				// �ߺ� üũ���� �˻� - ��

				if( bCheckFlag )
				{
					D3DXVECTOR3 vBall2Velocity = pBall2->GetVelocity();
					CBall *pB1,*pB2;
					float fCollisionTime1,fCollisionTime2;

					vBall2OldPos = pBall2->GetOldPosition();
					vBall2Pos = pBall2->GetPosition();
					fBall2Radius = pBall2->GetRadius();

					// ���� �浹 �Ÿ��� ���Ѵ�.
					D3DXVec3Subtract(&vDst,&vBall2OldPos,&vOldPos);
					fDst = D3DXVec3Length(&vDst);
					float fOldDst = D3DXVec3Length(&vDst) - (fRadius + fBall2Radius);

					if( (fOldDst<0.0f) && ( fDst > 0.0f ))
						printf("test");

					// �̿��ϴ� ������ �Ÿ��� ���Ѵ�.
					D3DXVec3Subtract(&vDst,&vBall2Pos,&vPos);
					fDst = D3DXVec3Length(&vDst);
					fGap = fDst - (fRadius + fBall2Radius);

					// �浹����
					if( ( fGap < BALL_EPSILON ) && ( fDst > 0.0f ) &&
						CollisionTest.SphereSphereSweep( fRadius,
                             vOldPos,
                             vPos,
                             fBall2Radius,
                             vBall2OldPos,
                             vBall2Pos,
                             fCollisionTime1,
                             fCollisionTime2
                             ) )
					{
						s_Count++;

						if(s_Count==2)
							printf("test");

						// �ӵ��� ���� ���� �� 1
						if( D3DXVec3Length(&vBall2Velocity) > D3DXVec3Length(&vVelocity) )
						{
							pB1 = pBall2;
							pB2 = pBall;
						}
						else
						{
							pB1 = pBall;
							pB2 = pBall2;
						}
						
						// �浹�� ����ü�� ����
						pCollision = new COLLISION;
						pCollision->Type = WITH_BALL;
						pCollision->Cc.BallNball.pBall = pB1;
						pCollision->Cc.BallNball.pBall2 = pB2;
						pCollision->fCollisionTime = fCollisionTime1;

						m_CollisionList.push_back(pCollision);
					}
				}
			}
		}
		//#################### �ٸ� ������ �浹 üũ - ��
	}
}

/******************************************************************************
* void CPlayEngine::ResolveCollision()
* �浹 ó��
******************************************************************************/
void CPlayEngine::ResolveCollision()
{
	float fCollisionTime;
	CPlane* pPlane;
	CBall* pBall,*pBall2;
	COLLISION* pCollision;

	list<COLLISION*>::iterator pi;
	for( pi=m_CollisionList.begin(); pi!=m_CollisionList.end(); ++pi)
	{
		pCollision = (COLLISION*)(*pi);

		if( pCollision->Type == WITH_CUSHION )	// ��ǿ� �浹
		{
			pPlane = pCollision->Cc.BallNcushion.pPlane;
			pBall = pCollision->Cc.BallNcushion.pBall;
			fCollisionTime = pCollision->fCollisionTime;

			if(fCollisionTime>0.0f)
			{
				pBall->ClearThisTime();

				// �浹 �ð��� �� ��ġ ����
				pBall->Update(fCollisionTime*m_fDeltaTime);
			}

			// �� ��ġ���� ���ݻ� ����
			ResolveReflexion(pCollision->Cc.BallNcushion);
		}
		else if( pCollision->Type == WITH_BALL )	// ���� �浹
		{
			int nCount = 0;

			pBall = pCollision->Cc.BallNball.pBall;
			pBall2 = pCollision->Cc.BallNball.pBall2;
			fCollisionTime = pCollision->fCollisionTime;
			
			float fR1,fR2;
			float fDst,fUpdateTime,fLastTime,fStartTime;
			D3DXVECTOR3 vDst;

			fR1 = pBall->GetRadius();
			fR2 = pBall2->GetRadius();

			fStartTime = 0.0f;
			fLastTime = fUpdateTime = fCollisionTime*m_fDeltaTime;

			// �浹 �Ÿ��� ���Ѵ�.
			D3DXVec3Subtract(&vDst,&(pBall2->GetPosition()),&(pBall->GetPosition()));
			fDst = D3DXVec3Length(&vDst) - (fR1 + fR2);

			if( fDst < 0.0f )
			{
				do
				{
					if( fDst < 0.0f )
					{
						fLastTime = fUpdateTime;
						fUpdateTime -= (fLastTime - fStartTime)/2.0f;
					}
					else if( fDst > BALL_EPSILON )
					{
						fStartTime = fUpdateTime;
						fUpdateTime += (fLastTime - fStartTime)/2.0f;
					}

					pBall->ClearThisTime();
					if(pBall2->GetActivity())
						pBall2->ClearThisTime();

					// �浹 �ð��� �� ��ġ ����
					pBall->Update(fUpdateTime);
					if(pBall2->GetActivity())
						pBall2->Update(fUpdateTime);

					// �浹 �Ÿ��� ���Ѵ�.
					D3DXVec3Subtract(&vDst,&(pBall2->GetPosition()),&(pBall->GetPosition()));
					fDst = D3DXVec3Length(&vDst) - (fR1 + fR2);

					nCount++;
				}
				while( !( (fDst < BALL_EPSILON) && (fDst > 0.0f) )  && (nCount<200) );
			}

			ResolveReflexion(pCollision->Cc.BallNball);

			pBall->Update(m_fDeltaTime-fUpdateTime);
			if(pBall2->GetActivity())
				pBall2->Update(m_fDeltaTime-fUpdateTime);
		}
	}

	stl_wipe(m_CollisionList);
}

/******************************************************************************
* void CPlayEngine::ApplyReflexion(BALL_AND_CUSHION& BnC)
* ��� �浹�� �� ���ݻ� ����
******************************************************************************/
void CPlayEngine::ResolveReflexion(BALL_AND_CUSHION& BnC)
{
	D3DXVECTOR3 vVelocity;

	if( D3DXVec3Dot(&(BnC.pPlane->m_vN),&(BnC.pBall->GetVelocity()) ) < 0.0f ) 
	{
		// ���ݻ� : ((2*(-1)*D3DXVec3Dot(&d,&n)) * n) + d
		vVelocity = REFLEXION_VECTOR(BnC.pBall->GetVelocity(),BnC.pPlane->m_vN);
		vVelocity.x *= CUSHION_RESTITUTION;
		vVelocity.z *= CUSHION_RESTITUTION;

		BnC.pBall->SetForce(&vVelocity,true);
	}
}

/******************************************************************************
* void CPlayEngine::ApplyReflexion(BALL_AND_BALL& BnB)
* �� �浹��
******************************************************************************/
void CPlayEngine::ResolveReflexion(BALL_AND_BALL& BnB)
{
	D3DXVECTOR3 vBall2Pos, vPos, vBall2Velocity, vVelocity;
	D3DXVECTOR3 vN, vT, vN1, vImpulseForce, vDst;
	float fRadius, fBall2Radius;

	// �̿��ϴ� ������ �Ÿ��� ���Ѵ�.
	vPos = BnB.pBall->GetPosition();
	vVelocity = BnB.pBall->GetVelocity();
	fRadius = BnB.pBall->GetRadius();

	vBall2Pos = BnB.pBall2->GetPosition();
	vBall2Velocity = BnB.pBall2->GetVelocity();
	fBall2Radius = BnB.pBall2->GetRadius();

	D3DXVec3Subtract(&vDst,&vPos,&vBall2Pos);
	D3DXVec3Normalize(&vDst,&vDst);

	// Find the length of the component of each of the movement
	// vectors along n. 
	// a1 = v1 . n
	// a2 = v2 . n
	float a1 = D3DXVec3Dot(&vVelocity,&vDst);
	float a2 = D3DXVec3Dot(&vBall2Velocity,&vDst);


	// Using the optimized version, 
	// optimizedP =  2(a1 - a2)
	//              -----------
	//                m1 + m2
	float optimizedP = (2.0f * (a1 - a2)) / ( BnB.pBall->GetMass() + BnB.pBall2->GetMass() );

	// Calculate v1', the new movement vector of circle1
	// v1' = v1 - optimizedP * m2 * n
	D3DXVECTOR3 vUpdateVelocity1 = vVelocity - optimizedP * BnB.pBall2->GetMass() * vDst;

	// Calculate v1', the new movement vector of circle1
	// v2' = v2 + optimizedP * m1 * n
	D3DXVECTOR3 vUpdateVelocity2 = vBall2Velocity + optimizedP * BnB.pBall->GetMass() * vDst;

	BnB.pBall->SetVelocity(vUpdateVelocity1);
	BnB.pBall2->SetForce(&vUpdateVelocity2,true);
}