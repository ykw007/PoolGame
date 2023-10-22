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

	CBall* pBallA = &pBall[nShotOrder]; // ����
	CBall* pBallB = &pBall[nOther]; // ���溼
	CBall* pBallC = &pBall[2]; // Ÿ�ٺ�1
	CBall* pBallD = &pBall[3]; // Ÿ�ٺ�2

	list<COLLISION*>::iterator pi;
	for( pi=(*pCollisionList).begin(); pi!=(*pCollisionList).end(); ++pi)
	{
		pCollision = (COLLISION*)(*pi);

		if( pCollision->Type == WITH_CUSHION )	// ��ǿ� �浹
		{
			pPlane = pCollision->Cc.BallNcushion.pPlane;
			pBallOne = pCollision->Cc.BallNcushion.pBall;

			// ������ ������ ���� ���¿���
			// ������ ��ǿ� �浹�Ǿ����� m_nCollisionCushion++;
			// pBallOne �� �����ΰ�
			if(pBallOne == pBallA)
				m_nCollisionCushion++;
		}
		else if( pCollision->Type == WITH_BALL )	// ���� �浹
		{
			pBallOne = pCollision->Cc.BallNball.pBall;
			pBallTwo = pCollision->Cc.BallNball.pBall2;

			// �Ŀ��̸� ���� ��
			if( (pBallOne == pBallA && pBallTwo == pBallB) ||
				(pBallOne == pBallB && pBallTwo == pBallA) )
			{
				m_Result = FOUL;
				m_bEnd = true;
				return;
			}

			// ������ Ÿ�ٺ� 1�� �浹������ m_bAC = true;
			// �׸��� m_bAD �� true���� üũ - true �� ���� ��
			if( (pBallOne == pBallA && pBallTwo == pBallC) ||
				(pBallOne == pBallC && pBallTwo == pBallA) )
			{
				m_bAC = true;
				if(!m_bAD) m_Result = ONESHOT;
			}

			// ������ Ÿ�ٺ� 2�� �浹������ m_bAD = true;
			// �׸��� m_bAC �� true���� üũ - true �� ���� ��
			if( (pBallOne == pBallA && pBallTwo == pBallD) ||
				(pBallOne == pBallD && pBallTwo == pBallA) )
			{
				m_bAD = true;
				if(!m_bAC) m_Result = ONESHOT;
			}

			if(m_bAC && m_bAD)
			{
				// �� Ÿ�ٺ��� ���߱��� 3���̻� ��ǰ� �浹�� �־����� NICESHOT
				m_Result = (m_nCollisionCushion>3)? NICESHOT : SHOT ;
				//m_bEnd = true;
				//return;
			}
		}
	}
}