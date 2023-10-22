/**
*
* CameraControl.cpp
* 카메라 컨트롤 클래스 구현
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

	// 카메라 초기화
	m_pCamera->Init(vEyePt,vLookatPt,vUpVec);

	m_pCamera->Update();
}

void CCameraControl::FixLookatPt(D3DXVECTOR3& vLookatPt)
{
	m_pCamera->SetLookatPt(vLookatPt);
}

/******************************************************************************
* MoveCamera(AXIS Axis, float fDst, bool bFixedLookat)
* 기준 축에 따라 카메라 이동
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
* 카메라좌표축 기준 회전
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

	// 회전 행렬 구함.
	D3DXMatrixRotationAxis( &matRotate, &vAxis, fAngle );

	// 방향 벡터 회전
	D3DXVec3TransformCoord( &vRotDirection, &vUpdate, &matRotate );

	// (노멀 방향 벡터 * eyept 벡터 길이) + eyept 벡터
	m_pCamera->m_vLookatPt = ( vRotDirection * D3DXVec3Length(&(m_pCamera->m_vEyePt)) ) + m_pCamera->m_vEyePt;

	// drection 벡터와 cross 벡터 방향이 변했으므로 다시 계산
	m_pCamera->Update();
}

/*****************************************************************
* void CCamera::CircleAxis(D3DVECTOR3& vAxis, float fAngle)
* 지정된 위치의 지정된 축 주위로 카메라를 이동시킨다.
*****************************************************************/
void CCameraControl::CircleAxis(D3DXVECTOR3& vPoint,
								D3DXVECTOR3& vAxis,
								float fAngle,
								float fDeltaTime)
{
	D3DXQUATERNION qR;
	D3DXMATRIXA16 matQr,matWorld,matInv;

	//######################## 쿼터니언 작성
	qR.x = (float)sinf(fAngle/2.0f) * vAxis.x;
	qR.y = (float)sinf(fAngle/2.0f) * vAxis.y;
	qR.z = (float)sinf(fAngle/2.0f) * vAxis.z;
	qR.w = (float)cosf(fAngle/2.0f);

	//######################## 쿼터니언 구면 선형 보간
	D3DXQuaternionSlerp(&m_qRotate, &m_qRotate, &qR, fDeltaTime);

	//######################## 쿼터니언 -> 행렬로 변환
	D3DXMatrixRotationQuaternion(&matQr, &m_qRotate);

	// 지정된 위치의 이동행렬 계산
	D3DXMatrixTranslation(&matWorld, vPoint.x, vPoint.y, vPoint.z);
	D3DXMatrixInverse( &matInv,0,&matWorld );

	// 이동 -> 회전 -> 이동
	m_matWorld = matInv * matQr * matWorld;

	//######################## 카메라 위치 벡터 * 회전 행렬
	D3DXVec3TransformCoord( &(m_pCamera->m_vEyePt), &(m_pCamera->m_vEyePt), &m_matWorld );

	m_pCamera->SetLookatPt(vPoint);

	m_pCamera->Update();
}