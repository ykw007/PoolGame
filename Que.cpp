#include "poolgame.h"

#define QUE_MASS 5.0f
#define DST_MIN 20.0f
#define DST_MAX -200.0f

void CQue::Clear()
{
	m_fMass = QUE_MASS;

	m_vDirection = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vMovPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	D3DXMatrixIdentity(&m_matRotate);
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matAdjust);
	D3DXMatrixIdentity(&m_matInvDir);

	// ť ����
	//D3DXMatrixTranslation(&m_matAdjust, 0.0f, 0.0f, 0.0f);
}

void CQue::Destroy()
{
	CModel::Destroy();
}

void CQue::Init(D3DXVECTOR3& vDir,D3DXVECTOR3& vUp,D3DXVECTOR3& vCross)
{
	// ���� ��ȯ�� ���� ����� �ۼ�
	D3DXMATRIXA16 matBaseDir;

	D3DXMatrixIdentity(&matBaseDir);

	m_vDirection = vDir;

	matBaseDir._11 = vCross.x;
	//matBaseDir._12 = vCross.y;
	matBaseDir._13 = vCross.z;

	matBaseDir._21 = vUp.x;
	//matBaseDir._22 = vUp.y;
	matBaseDir._23 = vUp.z;

	matBaseDir._31 = vDir.x;
	//matBaseDir._32 = vDir.y;
	matBaseDir._33 = vDir.z; 

	// �����
	D3DXMatrixInverse( &m_matInvDir, 0, &matBaseDir );
}

void CQue::SetQueMatrix()
{
	D3DXMATRIXA16 matR;
	D3DXMATRIXA16 matPos;

	D3DXMatrixTranslation(&matPos, m_vPos.x, m_vPos.y, m_vPos.z);

	D3DXMatrixRotationY( &matR,0.0f );		// ť�� �������� �����.

	m_matRotate *= matR;

	// ȸ��->�̵�
	m_matWorld = m_matRotate * matPos;
}

// �� �߽��� �������� ť�� �غ��Ѵ�.
bool CQue::Ready(D3DXVECTOR3& vPos)
{
	// ť ��ġ
	m_vPos = vPos;
	SetQueMatrix();
	return true;
}

/******************************************************************************
* void CQue::TurnOnYaxis(D3DXVECTOR3& vDir,D3DXVECTOR3& vUp,D3DXVECTOR3& vCross)
* ť ȸ��
******************************************************************************/
void CQue::TurnOnYaxis(D3DXVECTOR3& vDir,D3DXVECTOR3& vUp,D3DXVECTOR3& vCross)
{
	D3DXMATRIXA16 matCurDir;

	m_vDirection = vDir;

	// ť ī�޶� ���� �������� ȸ��
	D3DXMatrixIdentity(&matCurDir);

	matCurDir._11 = vCross.x;
	//matCurDir._12 = vCross.y;
	matCurDir._13 = vCross.z;

	matCurDir._21 = vUp.x;
	//matCurDir._22 = vUp.y;
	matCurDir._23 = vUp.z;

	matCurDir._31 = vDir.x;
	//matCurDir._32 = vDir.y;
	matCurDir._33 = vDir.z; 

	// ȸ�� ��� ����
	m_matRotate = m_matInvDir * matCurDir;

	SetQueMatrix();
}

/******************************************************************************
* const D3DXVECTOR3 CQue::Shot(float fForce)
* ť ��
******************************************************************************/
const D3DXVECTOR3 CQue::Shot(float fForce)
{
	D3DXVECTOR3 vForce = D3DXVECTOR3( 0.0f, 0.0f, 0.0f);

	// ������ ���ϰ� - �� ��� = ���ӵ� * ����
	// �ִ� ���� �� �ִ� �� ����
	//if(	fForce > 100.0f)
	//	fForce = 100.0f;

	D3DXVec3Scale( &vForce, &m_vDirection, fForce * m_fMass * 2.5f );

	return vForce;
}

float CQue::Move(float fDst)
{
	bool bIsShot = false;
	float fDeltaMove=fDst;
	float fStart = (float)fabs(m_vMovPos.z);

	m_vMovPos.z += -fDst;

	if(m_vMovPos.z < DST_MAX)
	{
		fDst = fDst +(m_vMovPos.z - DST_MAX);
		m_vMovPos.z = DST_MAX;
	}
	else if(m_vMovPos.z >= 5.0f)
	{
		fDst = -(-fDst +(-m_vMovPos.z + DST_MIN));
		m_vMovPos.z = DST_MIN;
		//fDeltaMove = fDst;//(fStart - DST_MIN);
		bIsShot = true;
	}

	D3DXMATRIXA16 matPos;
	D3DXMatrixTranslation(&matPos, 0.0f, 0.0f, -fDst);
	m_matWorld = matPos * m_matWorld;

	if(bIsShot) return (float)fabs(fDeltaMove);
	else return 0.0f;
}