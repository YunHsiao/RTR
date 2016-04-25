#include "Utility.h"
#include "Direct3D.h"
#include "BillBoards.h"

CBillBoards::CBillBoards()
:m_pTexture(NULL)
,m_pVB(NULL)
{
	D3DXMatrixIdentity(&m_matWorld);
}

CBillBoards::~CBillBoards()
{
	Safe_Release(m_pTexture);
	Safe_Release(m_pVB);
}

void CBillBoards::onInit(LPCTSTR strTex) {
	D3DXCreateTextureFromFile(CDirect3D::getInstance()->GetD3D9Device(), 
		strTex, &m_pTexture);
	if(FAILED(CDirect3D::getInstance()->CreateVertexBuffer(sizeof(SVertexT)*4, 
		D3DUSAGE_WRITEONLY, SVertexT::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL))) 
		return; 
	SVertexT* pVertices;
	m_pVB->Lock(0, 0, (void**) &pVertices, 0);
	pVertices[0] = SVertexT(-.5f, -.5f, 0.f, 0.f, 1.f);
	pVertices[1] = SVertexT(-.5f,  .5f, 0.f, 0.f, 0.f);
	pVertices[2] = SVertexT( .5f, -.5f, 0.f, 1.f, 1.f);
	pVertices[3] = SVertexT( .5f,  .5f, 0.f, 1.f, 0.f);
	m_pVB->Unlock();
}

void CBillBoards::onRender(const D3DXMATRIX& mView, const D3DXVECTOR3& vPos) {
	CDirect3D::getInstance()->AlphaTestEnable(TRUE);
	CDirect3D::getInstance()->AlphaFunction(D3DCMP_GREATER);
	CDirect3D::getInstance()->AlphaReference(0x80);
	CDirect3D::getInstance()->SetD3DFVF(SVertexT::FVF);
	CDirect3D::getInstance()->SetStreamSource(0, m_pVB, 0, sizeof(SVertexT));
	CDirect3D::getInstance()->SetTexture(0, m_pTexture);
	m_matWorld = mView;
	m_matWorld._12 = m_matWorld._32 = m_matWorld._41 = m_matWorld._42 = m_matWorld._43 = 0.f;
	m_matWorld._22 = 1.f;
	D3DXMatrixTranspose(&m_matWorld, &m_matWorld);
	for (unsigned int i(0); i < m_vPos.size(); i++) {
		m_matWorld._41 = m_vPos[i].x; m_matWorld._42 = m_vPos[i].y; m_matWorld._43 = m_vPos[i].z;
		CDirect3D::getInstance()->SetTransform(D3DTS_WORLD, &m_matWorld);
		CDirect3D::getInstance()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	}
	CDirect3D::getInstance()->SetTexture(0, NULL);
	CDirect3D::getInstance()->AlphaTestEnable(FALSE);
}
