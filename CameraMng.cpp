/**
*
* CameraMng.cpp
* ī�޶� ������ Ŭ���� ����
* kyung wook yang (ykw007@hotmail.com)
*
**/

#include "poolgame.h"

#define ZY_MIN 0.5f			// z�Ÿ��� y�Ÿ� ����
#define ZDST_MAX 750.0f		// ī�޶� �ִ� z �Ÿ�
#define ZDST_MIN 300.0f		// ī�޶� �ּ� z �Ÿ�

void CCameraMng::Clear()
{
}

void CCameraMng::Destroy()
{
}

/*****************************************************************
* void CCameraMng::MoveCameraX(float fD)
* x�� �������� �̵�
*****************************************************************/
void CCameraMng::MoveCameraX(float fD)
{
	// lookatpt ������ ä�� x�� ���� �̵�
	MoveCamera(X, fD, true);
}

/*****************************************************************
* void CCameraMng::MoveCameraY(float fD)
* Y�� �������� �̵�
*****************************************************************/
void CCameraMng::MoveCameraY(float fD)
{
	MoveCamera(Y, fD, false);
}

/*****************************************************************
* void CCameraMng::MoveCameraZ(float fD)
* LoolatPt �� EyePt ���̸� �̵�
*****************************************************************/
void CCameraMng::MoveCameraZ(float fD)
{
	//z�� �̵��� �׻� ī�޶� Z���� �޼���� �̵������� ���� �ʴ´�.
	float fUpdateDst = AdjustCameraZ(fD);
	MoveCamera(Z, fUpdateDst, true);
}

/*****************************************************************
* float CCameraMng::AdjustCameraZ(float fDst)
* ���� ī�޶� �Ÿ� ����
*****************************************************************/
float CCameraMng::AdjustCameraZ(float fDst)
{
	D3DXVECTOR3 vEyePt,vLookatPt, vDirection;
	float fCurDst;

	vLookatPt = m_pCamera->GetLookatPt();
	vEyePt = m_pCamera->GetEyePt();
	vDirection = m_pCamera->GetDirection();

	// ���콺 �̵������� �̸� ī�޶� ��ǥ�� ���
	vEyePt = vEyePt + (vDirection * fDst);
	fCurDst = D3DXVec3Length(&vEyePt);

	// lookatpt �� �Ѿ���ȴ��� üũ
	if( vEyePt.y < vLookatPt.y)
	{
		fDst = fDst - (fCurDst+ZDST_MIN);
		return false;
	}
	else
	{
		// ���� �Ÿ��� ����� ����
		if( ZDST_MAX < fCurDst )
			fDst = fDst + ( fCurDst - ZDST_MAX );
		else if ( ZDST_MIN > fCurDst )
			fDst = ((fDst +  fCurDst) - ZDST_MIN);
	}

	return fDst;
}

/*****************************************************************
* void CCameraMng::AdjustCameraY()
* ī�޶��� ���� ����
*****************************************************************/
void CCameraMng::AdjustCameraY()
{
	float fZdst, fYdst, fDst;
	D3DXVECTOR3 vDst;
	D3DXVECTOR3 vEyePt = m_pCamera->GetEyePt();
	D3DXVECTOR3 vLookatPt = m_pCamera->GetLookatPt();

	// z�Ÿ�
	fZdst = (float)fabs(vLookatPt.z - vEyePt.z);

	// y �Ÿ�
	fYdst = (float)fabs(vEyePt.y - vLookatPt.y);

	if( fZdst*ZY_MIN > fYdst )
	{
		// �Ÿ���
		fDst = (fZdst*ZY_MIN) - fYdst;
		m_pCamera->SetEyePt( D3DXVECTOR3(vEyePt.x, vEyePt.y + fDst, vEyePt.z) );
	}
}

/*****************************************************************
* bool CCameraMng::TakeBasePos()
* ���� ī�޶� ��ġ���� ���� ĥ �������� �̵�
*****************************************************************/
bool CCameraMng::TakeBasePos(D3DXVECTOR3& vLookatPt)
{
	float fUpdateValue, fBaseDst;
	float fDiffLookatX,fDiffLookatZ, fDiffY, fDiffZ;
	D3DXVECTOR3 vEyePt, vOldLookatPt, vUpdateLookatPt;
	float fAnimationTime = 7.0f;				// �ִϸ��̼� Ÿ��

	vEyePt = m_pCamera->GetEyePt();				// ���� ī�޶� ��ġ
	vOldLookatPt = m_pCamera->GetLookatPt();
	fBaseDst = 400.0f;

	//####################### ī�޶� lookatpt ����
	vUpdateLookatPt.x = vOldLookatPt.x + ((vLookatPt.x-vOldLookatPt.x)/fAnimationTime);
	vUpdateLookatPt.z = vOldLookatPt.z + ((vLookatPt.z-vOldLookatPt.z)/fAnimationTime);
	vUpdateLookatPt.y = vOldLookatPt.y;

	fDiffLookatX = (float)fabs(vLookatPt.x - vUpdateLookatPt.x);
	if( fDiffLookatX > 3.0f )
	{
		m_pCamera->SetLookatPt(vUpdateLookatPt);
	}
	fDiffLookatZ = (float)fabs(vLookatPt.z - vUpdateLookatPt.z);
	if( fDiffLookatZ > 3.0f )
	{
		m_pCamera->SetLookatPt(vUpdateLookatPt);
	}
	//####################### ī�޶� y�� �������� �̵�
	// ī�޶� ��ġ�� ������ ã�´�.
	// �⺻ ī�޶� ������ 30�� sin(M_PI/6)
	// ī�޶� y�� = ����*sin(M_PI/6)
	float fTargetY = vUpdateLookatPt.y+(fBaseDst*sinf(M_PI/4));
	fDiffY = (float)fabs(vEyePt.y - fTargetY);
	if( fDiffY > 3.0f )
	{
		fUpdateValue = (fTargetY-vEyePt.y)/fAnimationTime;
		vEyePt.y+=fUpdateValue;
		m_pCamera->SetEyePt( D3DXVECTOR3(vEyePt.x,
						vEyePt.y,
						vEyePt.z) );
	}

	//####################### ī�޶� z�Ÿ� ���� �̵�
	// ���� z�Ÿ��� ���Ѵ�.
	D3DXVECTOR3 vDst = vUpdateLookatPt - vEyePt;
	float fCurDst = D3DXVec3Length(&vDst);
	fDiffZ = (float)fabs(fBaseDst - fCurDst);

	if( fDiffZ > 3.0f )
	{
		fUpdateValue = (fBaseDst - fCurDst)/fAnimationTime;
		MoveCamera(Z, -fUpdateValue, true);
	}

	if( fDiffZ < 3.0f && fDiffZ < 3.0f && fDiffLookatX < 3.0f && fDiffLookatZ < 3.0f )
        return true;
	else
		return false;
}