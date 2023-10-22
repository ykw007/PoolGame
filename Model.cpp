#include "poolgame.h"

void CModel::Clear()
{
	m_p3dData = 0;
	m_pd3dDevice = CGameEnv::GetObj()->GetDevice();

	m_vPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXMatrixIdentity(&m_matWorld);
}

void CModel::Destroy()
{
	FREE_MEM(m_p3dData)
	Clear();
}

bool CModel::Create(MODEL* p3dData, D3DXVECTOR3& vPos)
{
	if( p3dData != 0 )
	{
		Set3dData( p3dData );		// 3d 데이타 매핑
		SetPosition(vPos);			// 월드에 위치
		SetMatrixByPos();
		return true;
	}
	else
        return false;
}

void CModel::Primitive()
{
	OBJECT* pObj=0;
	MODEL* p3dData = m_p3dData;
	D3DXMATRIXA16 matWorld;
	bool bUseMatTm = false;

	if(p3dData->pNext != 0 || p3dData->pObjList->pNext !=0)
		bUseMatTm = true;

	while(p3dData!=NULL)
	{
		m_pd3dDevice->SetMaterial( &(p3dData->Mtrl) );

		if(p3dData->pTexture!=0)
			m_pd3dDevice->SetTexture( 0, p3dData->pTexture );
		else
			m_pd3dDevice->SetTexture( 0, NULL );

		pObj = p3dData->pObjList;

		while(pObj!=NULL)
		{
			if(bUseMatTm)
			{
                matWorld = pObj->matTm * m_matWorld;
				m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
			}
			else
                m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

			// 프리미티브
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST,
											pObj->nNumFace,
											pObj->pVertex,
											sizeof(UVERTEX) );
			pObj = pObj->pNext;
		}
		p3dData = p3dData->pNext;
	}
}