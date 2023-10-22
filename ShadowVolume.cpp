#include "poolgame.h"

//-----------------------------------------------------------------------------
// Name: render
// Desc:
//-----------------------------------------------------------------------------
HRESULT CShadowVolume::render( LPDIRECT3DDEVICE9 pd3dDevice )
{
    pd3dDevice->SetFVF( D3DFVF_XYZ );

    return pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, m_dwNumVertices/3,
                                        m_pVertices, sizeof(D3DXVECTOR3) );
}

//-----------------------------------------------------------------------------
// Name: buildShadowVolume()
// Desc: Takes a mesh as input, and uses it to build a shadow volume. The
//       technique used considers each triangle of the mesh, and adds it's
//       edges to a temporary list. The edge list is maintained, such that
//       only silohuette edges are kept. Finally, the silohuette edges are
//       extruded to make the shadow volume vertex list.
//-----------------------------------------------------------------------------
HRESULT CShadowVolume::buildShadowVolume( CBall* pBall, D3DXVECTOR3 vLight )
{
	vector<BALLVERTEX> BallVerts = pBall->GetBallVerts();
	for( int i=0; i < BallVerts.size(); i++)
	{
//		BallVer = (BALLVERTEX)i;	
	}

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: addEdge()
// Desc: Adds an edge to a list of silohuette edges of a shadow volume.
//-----------------------------------------------------------------------------
void CShadowVolume::addEdge( WORD* pEdges, DWORD& dwNumEdges, WORD v0, WORD v1 )
{
    // Remove interior edges (which appear in the list twice)
    for( DWORD i = 0; i < dwNumEdges; ++i )
    {
        if( ( pEdges[2*i+0] == v0 && pEdges[2*i+1] == v1 ) ||
            ( pEdges[2*i+0] == v1 && pEdges[2*i+1] == v0 ) )
        {
            if( dwNumEdges > 1 )
            {
                pEdges[2*i+0] = pEdges[2*(dwNumEdges-1)+0];
                pEdges[2*i+1] = pEdges[2*(dwNumEdges-1)+1];
            }

            --dwNumEdges;
            return;
        }
    }

    pEdges[2*dwNumEdges+0] = v0;
    pEdges[2*dwNumEdges+1] = v1;
    dwNumEdges++;
}