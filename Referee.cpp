#include "poolgame.h"

void CReferee::Clear()
{
	m_Result = NOSHOT;
	m_bAB = false;
	m_bAC = false;
	m_bAD = false;

	m_nCollisionCushion = 0;
	m_bEnd = false;
}

void CReferee::Destroy()
{
	Clear();
}

void CReferee::Update(int nShotOrder,CBall* pBall,list<COLLISION*>* pCollisionList)
{
	CBall* pBallOne;
	CBall* pBallTwo;
	CPlane* pPlane;
	COLLISION* pCollision;

	int nOther = (nShotOrder+1)%2;

	CBall* pBallA = &pBall[nShotOrder]; // 수구
	CBall* pBallB = &pBall[nOther]; // 상대방볼
	CBall* pBallC = &pBall[2]; // 타겟볼1
	CBall* pBallD = &pBall[3]; // 타겟볼2

	list<COLLISION*>::iterator pi;
	for( pi=(*pCollisionList).begin(); pi!=(*pCollisionList).end(); ++pi)
	{
		pCollision = (COLLISION*)(*pi);

		if( pCollision->Type == WITH_CUSHION )	// 쿠션에 충돌
		{
			pPlane = pCollision->Cc.BallNcushion.pPlane;
			pBallOne = pCollision->Cc.BallNcushion.pBall;

			// 판정이 끝나지 않은 상태에서
			// 수구가 쿠션에 충돌되었으면 m_nCollisionCushion++;
			// pBallOne 이 수구인가
			if(pBallOne == pBallA)
				m_nCollisionCushion++;
		}
		else if( pCollision->Type == WITH_BALL )	// 볼에 충돌
		{
			pBallOne = pCollision->Cc.BallNball.pBall;
			pBallTwo = pCollision->Cc.BallNball.pBall2;

			// 파울이면 판정 끝
			if( (pBallOne == pBallA && pBallTwo == pBallB) ||
				(pBallOne == pBallB && pBallTwo == pBallA) )
			{
				m_Result = FOUL;
				m_bEnd = true;
				return;
			}

			// 수구가 타겟볼 1과 충돌했으면 m_bAC = true;
			// 그리고 m_bAD 가 true인지 체크 - true 면 판정 끝
			if( (pBallOne == pBallA && pBallTwo == pBallC) ||
				(pBallOne == pBallC && pBallTwo == pBallA) )
			{
				m_bAC = true;
				if(!m_bAD) m_Result = ONESHOT;
			}

			// 수구가 타겟볼 2와 충돌했으면 m_bAD = true;
			// 그리고 m_bAC 가 true인지 체크 - true 면 판정 끝
			if( (pBallOne == pBallA && pBallTwo == pBallD) ||
				(pBallOne == pBallD && pBallTwo == pBallA) )
			{
				m_bAD = true;
				if(!m_bAC) m_Result = ONESHOT;
			}

			if(m_bAC && m_bAD)
			{
				// 두 타겟볼을 맞추기전 3번이상 쿠션과 충돌이 있었으면 NICESHOT
				m_Result = (m_nCollisionCushion>3)? NICESHOT : SHOT ;
				//m_bEnd = true;
				//return;
			}
		}
	}
}