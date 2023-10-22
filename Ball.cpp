#include "poolgame.h"

// 바닥의 운동 마찰 계수
#define KINETIC_FRICTION	1.5f

// 볼의 질량
#define BALL_MASS			5.0f
#define BALL_DIAMETER		8.0f

const DWORD BALLVERTEX::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2;

void CBall::Clear()
{
    m_pBallTexture     = NULL;
    m_pEnvMapTexture    = NULL;

	m_vOldPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vOldVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 볼의 속도
	m_vAccel = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// 볼의 가속도
	m_vForce = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 볼이 받는 마찰력
	m_vAngVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vAngAccel = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_qR = D3DXQUATERNION( 0.0f, 0.0f, 0.0f, 0.0f );
	D3DXMatrixIdentity(&m_matR);
	D3DXMatrixIdentity(&m_matOldR);

	m_fMass = BALL_MASS;				// 볼의 질량
	m_fRadius = 0.0f;
	m_bActivity = false;

}

void CBall::Destroy()
{
    RELEASE( m_pBallTexture );
    RELEASE( m_pEnvMapTexture );

	//CModel::Destroy();
	m_BallVerts.clear();
	Clear();
}

bool CBall::Create(char* pMapFile, D3DXVECTOR3& vPos)
{
	SetPosition(vPos);
	InitEnvMap(pMapFile);
	SetMatrixByPos();
	m_vOldPos = vPos;
	return true;
}

void CBall::SetForce(D3DXVECTOR3* pForce,bool bIsCollision)
{
	// 힘 = 질량 * 가속도
	// 최초 볼이 힘을 받으면 볼이 받는 가속도는 받은 힘/볼의 질량
	// 따라서 볼의 속도에 가속도를 더해준다.
	if(bIsCollision)
		m_vVelocity = (*pForce);
	else
		m_vVelocity = (*pForce)/m_fMass;

	m_bActivity = true;
}

void CBall::Update(float fDeltaTime)
{
	// 현재 속도 크기
	float fVelocityLength;
	
	// 볼이 움직이고 있는지?
	if(!m_bActivity) return;

	fVelocityLength = D3DXVec3Length(&m_vVelocity);

	// 속도가 어느정도 작아지면 속도를 0으로 하여 정지시킨다.
	// 공이 멈추었으면 다시 칠수 있게 한다.
	if(  fVelocityLength < 1.0f)
	{
		m_vForce = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_bActivity = false;
		return;
	}

	Move(fDeltaTime);							// 볼 이동 계산
}

void CBall::Move(float fDeltaTime)
{
	D3DXVECTOR3 vNv;

	// 계산하기전에 이전 볼의 속도, 위치, 회전행렬 저장
	m_vOldVelocity = m_vVelocity;
	m_vOldPos = m_vPos;
	m_matOldR = m_matR;

	D3DXVec3Normalize(&vNv,&m_vVelocity);

	// 볼마찰력
	m_vForce.x = -(vNv.x * KINETIC_FRICTION);
	m_vForce.z = -(vNv.z * KINETIC_FRICTION);

	// 볼의 속도에서 운동 마찰력을 적용하여 계산
	m_vVelocity.x += m_vForce.x * fDeltaTime;
	m_vVelocity.z += m_vForce.z * fDeltaTime;

	// 다음 시간 볼의 속도가 계산되었으면 가속도를 구한다.
	m_vAccel.x = (m_vVelocity.x - m_vOldVelocity.x)/fDeltaTime;
	m_vAccel.z = (m_vVelocity.z - m_vOldVelocity.z)/fDeltaTime;

	// 속도를 적용 - 볼의 다음 위치를 구한다.
	m_vPos.x += m_vVelocity.x * fDeltaTime;
	m_vPos.z += m_vVelocity.z * fDeltaTime;

	// 각속도 v = r * w
	m_vAngVelocity.x = m_vVelocity.x / m_fRadius;
	m_vAngVelocity.z = m_vVelocity.z / m_fRadius;

	float fYawAngle = m_vAngVelocity.y * fDeltaTime;
	float fPitchAngle = m_vAngVelocity.z * fDeltaTime;
	float fRollAngle = m_vAngVelocity.x * fDeltaTime;

	Roll(fYawAngle,fPitchAngle,fRollAngle);
}

void CBall::Roll(float fYawAngle,float fPitchAngle,float fRollAngle)
{
	D3DXQUATERNION qR;
	D3DXMATRIXA16 matQr,matWorld,matInv;

	D3DXQuaternionRotationYawPitchRoll(&qR,fYawAngle,fPitchAngle,-fRollAngle);

	//######################## 쿼터니언 구면 선형 보간
	D3DXQuaternionSlerp(&m_qR, &m_qR, &qR, 1.0f);

	//######################## 쿼터니언 -> 행렬로 변환
	D3DXMatrixRotationQuaternion(&matQr, &m_qR);

	m_matR *= matQr;

	// 지정된 위치의 이동행렬 계산
	D3DXMatrixTranslation(&matWorld, m_vPos.x, m_vPos.y, m_vPos.z);

	// 회전 -> 이동
	m_matWorld = m_matR * matWorld;
}

void CBall::ClearThisTime()
{
	m_vPos = m_vOldPos;
	m_vVelocity = m_vOldVelocity;
	m_matR = m_matOldR;
}

void CBall::Primitive()
{
	// 현재 광원에서 볼의 재질 설정
	D3DMATERIAL9 mtrl;
	ZeroMemory( &mtrl, sizeof(mtrl) );
	/*
	mtrl.Diffuse.r = mtrl.Ambient.r = 0.8f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 0.8f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 0.8f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 0.0f;
	*/
	mtrl.Diffuse.r = mtrl.Ambient.r = 0.27f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 0.27f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 0.27f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 0.0f;
	m_pd3dDevice->SetMaterial( &mtrl );

	m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

	m_pd3dDevice->SetFVF(BALLVERTEX::FVF); 
	DisplayBaseMap();
	DisplayEnvMap();
}

void CBall::InitEnvMap(char* pMapFile)
{
	D3DXCreateTextureFromFile( m_pd3dDevice,
								pMapFile,
                                 &m_pBallTexture);

	D3DXCreateTextureFromFile( m_pd3dDevice,
								".\\Data\\envmap.bmp",
                                 &m_pEnvMapTexture);

	m_fRadius = BALL_DIAMETER;
	float scales[2] = {1.0f, 2.0f};

	CreateSphereVertices( m_fRadius, &m_BallVerts, &scales[0] );
}

//수정필-모델 관리자에서 관리하게...같은 정점 정보는 공유한다.
void CBall::CreateSphereVertices( float radius,
								 vector<BALLVERTEX>* pVec,
								 float* scales )
{
    int numSphereRings    = 30;
    int numSphereSegments = 30;

	BALLVERTEX curr;

    // Establish constants used in sphere generation
    float fDeltaRingAngle = ( M_PI / numSphereRings );
    float fDeltaSegAngle  = ( 2.0f * M_PI / numSphereSegments );

    // Generate the group of rings for the sphere
    for( int ring = 0; ring < numSphereRings; ring++ )
    {    
        float r0 = (float)sin( (ring+0) * fDeltaRingAngle );
        float r1 = (float)sin( (ring+1) * fDeltaRingAngle );
        float y0 = (float)cos( (ring+0) * fDeltaRingAngle );
        float y1 = (float)cos( (ring+1) * fDeltaRingAngle );

        // Generate the group of segments for the current ring
        for( int seg = 0; seg < (numSphereSegments+1); seg++ )
        {
            float x0 =  r0 * (float)sin( seg * fDeltaSegAngle );
            float z0 =  r0 * (float)cos( seg * fDeltaSegAngle );
            float x1 =  r1 * (float)sin( seg * fDeltaSegAngle );
            float z1 =  r1 * (float)cos( seg * fDeltaSegAngle );

			curr.p = radius * D3DXVECTOR3(x0,y0,z0);
			D3DXVec3Normalize( &curr.n, &curr.p );
			curr.tu1 = scales[0] * -((FLOAT)seg)/numSphereSegments;
			curr.tv1 = scales[0] * ((ring+0)/(FLOAT)numSphereRings);
			curr.tu2 = scales[1] * -((FLOAT)seg)/numSphereSegments;
			curr.tv2 = scales[1] * ((ring+0)/(FLOAT)numSphereRings);

			pVec->push_back( curr );

			curr.p = radius * D3DXVECTOR3(x1,y1,z1);
			D3DXVec3Normalize( &curr.n, &curr.p );
			curr.tu1 = scales[0] * -((FLOAT)seg)/numSphereSegments;
			curr.tv1 = scales[0] * ((ring+1)/(FLOAT)numSphereRings);
			curr.tu2 = scales[1] * -((FLOAT)seg)/numSphereSegments;
			curr.tv2 = scales[1] * ((ring+1)/(FLOAT)numSphereRings);

			pVec->push_back( curr );
        }
    }
}

void CBall::DisplayBaseMap()
{
	m_pd3dDevice->SetTexture( 0, m_pBallTexture );

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );

	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

    // 프리미티브
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP,
		m_BallVerts.size() - 2,
		&m_BallVerts[0],
		sizeof( BALLVERTEX ) );

    // 텍스쳐 설정 해제
	ReleaseRenderState();
}

void CBall::DisplayEnvMap()
{
	m_pd3dDevice->SetTexture( 0, m_pBallTexture );

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE | D3DTA_COMPLEMENT );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );


	m_pd3dDevice->SetTexture( 1, m_pEnvMapTexture );

	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE );

	m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL  );
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	// 환경맵 변환 행렬 작성
	// u=n_x/2 + 0.5, v=-n_y/2 + 0.5 - 이부분이 핵심
	D3DXMATRIXA16 matTexture;
	D3DXMatrixIdentity(&matTexture);
	matTexture._11 = 0.5f;
	matTexture._41 = 0.5f;
	matTexture._22 = -0.5f;
	matTexture._42 = 0.5f;
	m_pd3dDevice->SetTransform( D3DTS_TEXTURE1, (D3DMATRIX*)&matTexture );

	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );


    // 프리미티브
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP,
		m_BallVerts.size() - 2,
		&m_BallVerts[0],
		(sizeof( BALLVERTEX )) );

	m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP ,D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0 );

	// 알파 브렌딩 해제
    m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    // 텍스쳐 설정 해제
	ReleaseRenderState();
}

void CBall::ReleaseRenderState()
{
    m_pd3dDevice->SetTexture( 0, NULL );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
}