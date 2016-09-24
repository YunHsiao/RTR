#include "Utility.h"
#include "Direct3D.h"
#include "BillBoard.h"

CBillBoard::CBillBoard()
:m_pTexture(NULL)
,m_pVB(NULL)
{
	D3DXMatrixIdentity(&m_matWorld);
}

CBillBoard::~CBillBoard()
{
	Safe_Release(m_pTexture);
	Safe_Release(m_pVB);
}

void CBillBoard::onInit(const D3DXVECTOR3& vPos, LPCTSTR strTex) {
	m_vPos = vPos;
	if (FAILED(D3DXCreateTextureFromFileEx(CDirect3D::getInstance()->GetD3D9Device(), 
		strTex,D3DX_DEFAULT_NONPOW2,D3DX_DEFAULT_NONPOW2,
		1,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR,D3DX_FILTER_NONE,0,NULL,NULL,
		&m_pTexture))) return;
	D3DSURFACE_DESC desc;
	m_pTexture->GetLevelDesc(0, &desc);
	float fRatio = (float) desc.Height / desc.Width;
	if(FAILED(CDirect3D::getInstance()->CreateVertexBuffer(sizeof(SVertexT)*4, 
		D3DUSAGE_WRITEONLY, SVertexT::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL))) 
		return; 
	SVertexT* pVertices;
	m_pVB->Lock(0, 0, (void**) &pVertices, 0);
	pVertices[0] = SVertexT(-.5f, -.5f, 0.f, 0.f, 1.f);
	pVertices[1] = SVertexT(-.5f, fRatio - .5f, 0.f, 0.f, 0.f);
	pVertices[2] = SVertexT( .5f, -.5f, 0.f, 1.f, 1.f);
	pVertices[3] = SVertexT( .5f, fRatio - .5f, 0.f, 1.f, 0.f);
	m_pVB->Unlock();
}


void CBillBoard::onRender(const D3DXMATRIX& mView, const D3DXVECTOR3& vPos) {
	CDirect3D::getInstance()->AlphaTestEnable(TRUE);
	CDirect3D::getInstance()->AlphaFunction(D3DCMP_GREATER);
	CDirect3D::getInstance()->AlphaReference(0x80);
	CDirect3D::getInstance()->SetD3DFVF(SVertexT::FVF);
	CDirect3D::getInstance()->SetStreamSource(0, m_pVB, 0, sizeof(SVertexT));
	CDirect3D::getInstance()->SetTexture(0, m_pTexture);
	D3DXVECTOR3 vUp(mView._12, mView._22, mView._32);
	/* simpler, yet supposed-to-be less efficient *
	D3DXMatrixLookAtRH(&m_matWorld, &m_vPos, &vPos, &vUp); // Backward Direction
	m_matWorld._41 = m_matWorld._42 = m_matWorld._43 = 0.0f;
	D3DXMatrixTranspose(&m_matWorld, &m_matWorld);

	/* optimized by doing manually */
	D3DXVECTOR3 zaxis(m_vPos - vPos); D3DXVec3Normalize(&zaxis, &zaxis);
	D3DXVECTOR3 xaxis; D3DXVec3Normalize(D3DXVec3Cross(&xaxis, &vUp, &zaxis), &xaxis);
	D3DXVECTOR3 yaxis; D3DXVec3Cross(&yaxis, &zaxis, &xaxis);
	m_matWorld._11 = xaxis.x; m_matWorld._12 = xaxis.y; m_matWorld._13 = xaxis.z;
	m_matWorld._21 = yaxis.x; m_matWorld._22 = yaxis.y; m_matWorld._23 = yaxis.z;
	m_matWorld._31 = zaxis.x; m_matWorld._32 = zaxis.y; m_matWorld._33 = zaxis.z;
	/**/
	m_matWorld._41 = m_vPos.x; m_matWorld._42 = m_vPos.y; m_matWorld._43 = m_vPos.z;
	CDirect3D::getInstance()->SetTransform(D3DTS_WORLD, &m_matWorld);
	CDirect3D::getInstance()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	CDirect3D::getInstance()->SetTexture(0, NULL);
	CDirect3D::getInstance()->AlphaTestEnable(FALSE);
}
