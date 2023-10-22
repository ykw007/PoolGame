/**
*
* Camera.cpp
* ī�޶� Ŭ���� ����
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
* ī�޶� ���� ���� ����
******************************************************************************/
void CCamera::Update()
{
	//######################## direction ���Ϳ� cross ����
	D3DXVec3Normalize( &m_vDirection, &( m_vLookatPt - m_vEyePt ) );
	D3DXVec3Cross( &m_vCross, &m_vUpVec, &m_vDirection );
	//D3DXVec3Normalize( &m_vCross, &m_vCross);
}
