/**
*
* CameraControl.cpp
* ī�޶� ��Ʈ�� Ŭ���� ����
* kyung wook yang (ykw007@hotmail.com)
*
**/

#include "poolgame.h"

void CCameraControl::Clear()
{
	D3DXMatrixIdentity(&m_matWorld);
//	D3DXMatrixIdentity(&m_matRotate);
	m_qRotate = D3DXQUATERNION( 0.0f, 0.0f, 0.0f, 0.0f );
}

void CCameraControl::Destroy()
{
	FREE_MEM(m_pCamera);
}

void CCameraControl::Init(D3DXVECTOR3& vEyePt, D3DXVECTOR3& vLookatPt, D3DXVECTOR3& vUpVec)
{
	m_pCamera = new CCamera;

	// ī�޶� �ʱ�ȭ
	m_pCamera->Init(vEyePt,vLookatPt,vUpVec);

	m_pCamera->Update();
}

void CCameraControl::FixLookatPt(D3DXVECTOR3& vLookatPt)
{
	m_pCamera->SetLookatPt(vLookatPt);
}

/******************************************************************************
* MoveCamera(AXIS Axis, float fDst, bool bFixedLookat)
* ���� �࿡ ���� ī�޶� �̵�
******************************************************************************/
void CCameraControl::MoveCamera(AXIS Axis, float fDst, bool bFixedLookat)
{
	D3DXVECTOR3 vMoveAxis;

	switch(Axis)
	{
	case X:
		vMoveAxis = m_pCamera->m_vCross;
		break;
	case Y:
		vMoveAxis = m_pCamera->m_vUpVec;
		break;
	case Z:
		vMoveAxis = m_pCamera->m_vDirection;
		break;
	}

	m_pCamera->m_vEyePt += (vMoveAxis * fDst);

	if( !bFixedLookat )
		m_pCamera->m_vLookatPt += (vMoveAxis * fDst);

	m_pCamera->Update();
}

/******************************************************************************
* RotateCamera(float fAngle)
* ī�޶���ǥ�� ���� ȸ��
******************************************************************************/
void CCameraControl::RotateCamera(AXIS Axis, float fAngle)
{
	D3DXVECTOR3 vRotDirection, vAxis, vUpdate;
	D3DXMATRIXA16 matRotate;

	switch(Axis)
	{
	case X:
		vAxis = m_pCamera->m_vCross;
		vUpdate = m_pCamera->m_vDirection;
		break;
	case Y:
		vAxis = m_pCamera->m_vUpVec;
		vUpdate = m_pCamera->m_vDirection;
		break;
	case Z:
		vAxis = m_pCamera->m_vDirection;
		vUpdate = m_pCamera->m_vUpVec;
		break;
	}

	// ȸ�� ��� ����.
	D3DXMatrixRotationAxis( &matRotate, &vAxis, fAngle );

	// ���� ���� ȸ��
	D3DXVec3TransformCoord( &vRotDirection, &vUpdate, &matRotate );

	// (��� ���� ���� * eyept ���� ����) + eyept ����
	m_pCamera->m_vLookatPt = ( vRotDirection * D3DXVec3Length(&(m_pCamera->m_vEyePt)) ) + m_pCamera->m_vEyePt;

	// drection ���Ϳ� cross ���� ������ �������Ƿ� �ٽ� ���
	m_pCamera->Update();
}

/*****************************************************************
* void CCamera::CircleAxis(D3DVECTOR3& vAxis, float fAngle)
* ������ ��ġ�� ������ �� ������ ī�޶� �̵���Ų��.
*****************************************************************/
void CCameraControl::CircleAxis(D3DXVECTOR3& vPoint,
								D3DXVECTOR3& vAxis,
								float fAngle,
								float fDeltaTime)
{
	D3DXQUATERNION qR;
	D3DXMATRIXA16 matQr,matWorld,matInv;

	//######################## ���ʹϾ� �ۼ�
	qR.x = (float)sinf(fAngle/2.0f) * vAxis.x;
	qR.y = (float)sinf(fAngle/2.0f) * vAxis.y;
	qR.z = (float)sinf(fAngle/2.0f) * vAxis.z;
	qR.w = (float)cosf(fAngle/2.0f);

	//######################## ���ʹϾ� ���� ���� ����
	D3DXQuaternionSlerp(&m_qRotate, &m_qRotate, &qR, fDeltaTime);

	//######################## ���ʹϾ� -> ��ķ� ��ȯ
	D3DXMatrixRotationQuaternion(&matQr, &m_qRotate);

	// ������ ��ġ�� �̵���� ���
	D3DXMatrixTranslation(&matWorld, vPoint.x, vPoint.y, vPoint.z);
	D3DXMatrixInverse( &matInv,0,&matWorld );

	// �̵� -> ȸ�� -> �̵�
	m_matWorld = matInv * matQr * matWorld;

	//######################## ī�޶� ��ġ ���� * ȸ�� ���
	D3DXVec3TransformCoord( &(m_pCamera->m_vEyePt), &(m_pCamera->m_vEyePt), &m_matWorld );

	m_pCamera->SetLookatPt(vPoint);

	m_pCamera->Update();
}