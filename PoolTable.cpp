#include "poolgame.h"

// ���Ϸ� ���� �� �� �ְ� ����...
// ��� ��� - ������ ���迡 ���� ������ ��... 
// ax + by + cz + d = 0
// 4���� �浹 ������ ���� ���Ͱ� �������� ���ϹǷ� ��鿡�� ���������� �Ÿ� d�� �� ���...
#define UP_PLANE			132.0f
#define DOWN_PLANE			135.5f
#define LEFT_PLANE			242.0f
#define RIGHT_PLANE			278.0f

void CPoolTable::Clear()
{
	m_ppPlane = 0;
}

void CPoolTable::Destroy()
{
	CModel::Destroy();

	if(m_ppPlane!=NULL)
	{
		for(int i =0;i<m_nNumPlane;i++)
			FREE_MEM( m_ppPlane[i] );
	}
	FREE_ARRAY_MEM(m_ppPlane);
	Clear();
}

bool CPoolTable::Create(MODEL* p3dData, D3DXVECTOR3& vPos, int nNumPlane)
{
	if( CModel::Create(p3dData,vPos) )
	{
		m_nNumPlane = nNumPlane;
        SetCushion(nNumPlane);
		return true;
	}
	else
		return false;
}

void CPoolTable::SetCushion(int nNumPlane)
{
	m_ppPlane = new CPlane*[nNumPlane];

	//for(int i=0;i<nNumPlane;i++)
	{
		m_ppPlane[0] = new CPlane(D3DXVECTOR3(-257.9688f, 132.9914f, 131.9368f),
			D3DXVECTOR3(1.0f,0.0f,0.0f));	// left
		m_ppPlane[1] = new CPlane(D3DXVECTOR3(263.5962f, 142.5428f, -136.1693f),
			D3DXVECTOR3(-1.0f,0.0f,0.0f));	// right
		m_ppPlane[2] = new CPlane(D3DXVECTOR3(263.5962f, 142.5428f, 131.9368f),
			D3DXVECTOR3(0.0f,0.0f,-1.0f));	// up
		m_ppPlane[3] = new CPlane(D3DXVECTOR3(263.5962f, 132.9914f, -136.1693f),
			D3DXVECTOR3(0.0f,0.0f,1.0f));	// down
	}
/*
	m_ppPlane[0]->m_fD = UP_PLANE;
	m_ppPlane[0]->m_vN = D3DXVECTOR3(0.0f,0.0f,-1.0f);

	m_ppPlane[1]->m_fD = DOWN_PLANE;
	m_ppPlane[1]->m_vN = D3DXVECTOR3(0.0f,0.0f,1.0f);

	m_ppPlane[2]->m_fD = LEFT_PLANE;
	m_ppPlane[2]->m_vN = D3DXVECTOR3(1.0f,0.0f,0.0f);

	m_ppPlane[3]->m_fD = RIGHT_PLANE;
	m_ppPlane[3]->m_vN = D3DXVECTOR3(-1.0f,0.0f,0.0f);
*/
}