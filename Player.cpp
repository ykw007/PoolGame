#include "poolgame.h"

void CPlayer::Clear()
{
	m_pMyBall = 0;
	m_nRestBead = 0;
}

void CPlayer::Destroy()
{
}

void CPlayer::Init(CBall *pBall,int nRestBead)
{
	m_pMyBall = pBall;
	m_nRestBead = nRestBead;
}