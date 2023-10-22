#include "poolgame.h"

// �ٴ��� � ���� ���
#define KINETIC_FRICTION	1.5f

// ���� ����
#define BALL_MASS			5.0f
#define BALL_DIAMETER		8.0f

const DWORD BALLVERTEX::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2;

void CBall::Clear()
{
    m_pBallTexture     = NULL;
    m_pEnvMapTexture    = NULL;

	m_vOldPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vOldVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// ���� �ӵ�
	m_vAccel = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// ���� ���ӵ�
	m_vForce = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// ���� �޴� ������
	m_vAngVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vAngAccel = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_qR = D3DXQUATERNION( 0.0f, 0.0f, 0.0f, 0.0f );
	D3DXMatrixIdentity(&m_matR);
	D3DXMatrixIdentity(&m_matOldR);

	m_fMass = BALL_MASS;				// ���� ����
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
	// �� = ���� * ���ӵ�
	// ���� ���� ���� ������ ���� �޴� ���ӵ��� ���� ��/���� ����
	// ���� ���� �ӵ��� ���ӵ��� �����ش�.
	if(bIsCollision)
		m_vVelocity = (*pForce);
	else
		m_vVelocity = (*pForce)/m_fMass;

	m_bActivity = true;
}

void CBall::Update(float fDeltaTime)
{
	// ���� �ӵ� ũ��
	float fVelocityLength;
	
	// ���� �����̰� �ִ���?
	if(!m_bActivity) return;

	fVelocityLength = D3DXVec3Length(&m_vVelocity);

	// �ӵ��� ������� �۾����� �ӵ��� 0���� �Ͽ� ������Ų��.
	// ���� ���߾����� �ٽ� ĥ�� �ְ� �Ѵ�.
	if(  fVelocityLength < 1.0f)
	{
		m_vForce = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_bActivity = false;
		return;
	}

	Move(fDeltaTime);							// �� �̵� ���
}

void CBall::Move(float fDeltaTime)
{
	D3DXVECTOR3 vNv;

	// ����ϱ����� ���� ���� �ӵ�, ��ġ, ȸ����� ����
	m_vOldVelocity = m_vVelocity;
	m_vOldPos = m_vPos;
	m_matOldR = m_matR;

	D3DXVec3Normalize(&vNv,&m_vVelocity);

	// ��������
	m_vForce.x = -(vNv.x * KINETIC_FRICTION);
	m_vForce.z = -(vNv.z * KINETIC_FRICTION);

	// ���� �ӵ����� � �������� �����Ͽ� ���
	m_vVelocity.x += m_vForce.x * fDeltaTime;
	m_vVelocity.z += m_vForce.z * fDeltaTime;

	// ���� �ð� ���� �ӵ��� ���Ǿ����� ���ӵ��� ���Ѵ�.
	m_vAccel.x = (m_vVelocity.x - m_vOldVelocity.x)/fDeltaTime;
	m_vAccel.z = (m_vVelocity.z - m_vOldVelocity.z)/fDeltaTime;

	// �ӵ��� ���� - ���� ���� ��ġ�� ���Ѵ�.
	m_vPos.x += m_vVelocity.x * fDeltaTime;
	m_vPos.z += m_vVelocity.z * fDeltaTime;

	// ���ӵ� v = r * w
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

	//######################## ���ʹϾ� ���� ���� ����
	D3DXQuaternionSlerp(&m_qR, &m_qR, &qR, 1.0f);

	//######################## ���ʹϾ� -> ��ķ� ��ȯ
	D3DXMatrixRotationQuaternion(&matQr, &m_qR);

	m_matR *= matQr;

	// ������ ��ġ�� �̵���� ���
	D3DXMatrixTranslation(&matWorld, m_vPos.x, m_vPos.y, m_vPos.z);

	// ȸ�� -> �̵�
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
	// ���� �������� ���� ���� ����
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

//������-�� �����ڿ��� �����ϰ�...���� ���� ������ �����Ѵ�.
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

    // ������Ƽ��
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP,
		m_BallVerts.size() - 2,
		&m_BallVerts[0],
		sizeof( BALLVERTEX ) );

    // �ؽ��� ���� ����
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

	// ȯ��� ��ȯ ��� �ۼ�
	// u=n_x/2 + 0.5, v=-n_y/2 + 0.5 - �̺κ��� �ٽ�
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


    // ������Ƽ��
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP,
		m_BallVerts.size() - 2,
		&m_BallVerts[0],
		(sizeof( BALLVERTEX )) );

	m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP ,D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0 );

	// ���� �귻�� ����
    m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    // �ؽ��� ���� ����
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