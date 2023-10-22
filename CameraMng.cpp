/**
*
* CameraMng.cpp
* 카메라 관리자 클래스 구현
* kyung wook yang (ykw007@hotmail.com)
*
**/

#include "poolgame.h"

#define ZY_MIN 0.5f			// z거리와 y거리 비율
#define ZDST_MAX 750.0f		// 카메라 최대 z 거리
#define ZDST_MIN 300.0f		// 카메라 최소 z 거리

void CCameraMng::Clear()
{
}

void CCameraMng::Destroy()
{
}

/*****************************************************************
* void CCameraMng::MoveCameraX(float fD)
* x축 기준으로 이동
*****************************************************************/
void CCameraMng::MoveCameraX(float fD)
{
	// lookatpt 고정한 채로 x축 기준 이동
	MoveCamera(X, fD, true);
}

/*****************************************************************
* void CCameraMng::MoveCameraY(float fD)
* Y축 기준으로 이동
*****************************************************************/
void CCameraMng::MoveCameraY(float fD)
{
	MoveCamera(Y, fD, false);
}

/*****************************************************************
* void CCameraMng::MoveCameraZ(float fD)
* LoolatPt 와 EyePt 사이를 이동
*****************************************************************/
void CCameraMng::MoveCameraZ(float fD)
{
	//z축 이동시 항상 카메라 Z조정 메서드로 이동범위를 넘지 않는다.
	float fUpdateDst = AdjustCameraZ(fD);
	MoveCamera(Z, fUpdateDst, true);
}

/*****************************************************************
* float CCameraMng::AdjustCameraZ(float fDst)
* 볼과 카메라 거리 조정
*****************************************************************/
float CCameraMng::AdjustCameraZ(float fDst)
{
	D3DXVECTOR3 vEyePt,vLookatPt, vDirection;
	float fCurDst;

	vLookatPt = m_pCamera->GetLookatPt();
	vEyePt = m_pCamera->GetEyePt();
	vDirection = m_pCamera->GetDirection();

	// 마우스 이동값으로 미리 카메라 좌표를 계산
	vEyePt = vEyePt + (vDirection * fDst);
	fCurDst = D3DXVec3Length(&vEyePt);

	// lookatpt 를 넘어버렸는지 체크
	if( vEyePt.y < vLookatPt.y)
	{
		fDst = fDst - (fCurDst+ZDST_MIN);
		return false;
	}
	else
	{
		// 유지 거리를 벗어나면 조정
		if( ZDST_MAX < fCurDst )
			fDst = fDst + ( fCurDst - ZDST_MAX );
		else if ( ZDST_MIN > fCurDst )
			fDst = ((fDst +  fCurDst) - ZDST_MIN);
	}

	return fDst;
}

/*****************************************************************
* void CCameraMng::AdjustCameraY()
* 카메라의 높이 조정
*****************************************************************/
void CCameraMng::AdjustCameraY()
{
	float fZdst, fYdst, fDst;
	D3DXVECTOR3 vDst;
	D3DXVECTOR3 vEyePt = m_pCamera->GetEyePt();
	D3DXVECTOR3 vLookatPt = m_pCamera->GetLookatPt();

	// z거리
	fZdst = (float)fabs(vLookatPt.z - vEyePt.z);

	// y 거리
	fYdst = (float)fabs(vEyePt.y - vLookatPt.y);

	if( fZdst*ZY_MIN > fYdst )
	{
		// 거리차
		fDst = (fZdst*ZY_MIN) - fYdst;
		m_pCamera->SetEyePt( D3DXVECTOR3(vEyePt.x, vEyePt.y + fDst, vEyePt.z) );
	}
}

/*****************************************************************
* bool CCameraMng::TakeBasePos()
* 현재 카메라 위치에서 다음 칠 지점으로 이동
*****************************************************************/
bool CCameraMng::TakeBasePos(D3DXVECTOR3& vLookatPt)
{
	float fUpdateValue, fBaseDst;
	float fDiffLookatX,fDiffLookatZ, fDiffY, fDiffZ;
	D3DXVECTOR3 vEyePt, vOldLookatPt, vUpdateLookatPt;
	float fAnimationTime = 7.0f;				// 애니메이션 타임

	vEyePt = m_pCamera->GetEyePt();				// 현재 카메라 위치
	vOldLookatPt = m_pCamera->GetLookatPt();
	fBaseDst = 400.0f;

	//####################### 카메라 lookatpt 보간
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
	//####################### 카메라 y축 기준으로 이동
	// 카메라가 위치할 지점을 찾는다.
	// 기본 카메라 각도는 30도 sin(M_PI/6)
	// 카메라 y값 = 빗변*sin(M_PI/6)
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

	//####################### 카메라 z거리 기준 이동
	// 현재 z거리를 구한다.
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