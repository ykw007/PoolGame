/**
*
* Camera.cpp
* 카메라 클래스 구현
* kyung wook yang (ykw007@hotmail.com)
* 2003.10.10
*
**/

#include "poolgame.h"

void CCamera::Clear()
{
	m_vDirection = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vCross = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	m_vEyePt = D3DXVECTOR3( 0.0f, 300.0f, -300.0f );
	m_vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vUpVec = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	D3DXMatrixIdentity(&m_matView);
}

void CCamera::Destroy()
{
}

void CCamera::Init(D3DXVECTOR3& vEyePt, D3DXVECTOR3& vLookatPt, D3DXVECTOR3& vUpVec)
{
	m_vEyePt = vEyePt;
	m_vLookatPt = vLookatPt;
	m_vUpVec = vUpVec;
}

/******************************************************************************
* void CCamera::Update()
* 카메라 변경 정보 적용
******************************************************************************/
void CCamera::Update()
{
	//######################## direction 벡터와 cross 벡터
	D3DXVec3Normalize( &m_vDirection, &( m_vLookatPt - m_vEyePt ) );
	D3DXVec3Cross( &m_vCross, &m_vUpVec, &m_vDirection );
	//D3DXVec3Normalize( &m_vCross, &m_vCross);
}
