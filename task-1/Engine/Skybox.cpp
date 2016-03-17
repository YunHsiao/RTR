#include "Utility.h"
#include "Direct3D.h"
#include "Skybox.h"

CSkyBox::CSkyBox()
:m_pVB(NULL)
{
	D3DXMatrixIdentity(&m_matId);
}

CSkyBox::~CSkyBox()
{
	Safe_Release(m_pVB);
	for (int i = 0; i < 6; ++i)
		Safe_Release(m_pTextures[i]);
}

void CSkyBox::onInit(std::string strTexPrefix, float fRadius) {
	std::string strTex = strTexPrefix + "UP.jpg";
	D3DXCreateTextureFromFileA(CDirect3D::getInstance()->GetD3D9Device(), 
		strTex.c_str(), &m_pTextures[0]);
	strTex = strTexPrefix + "DN.jpg";
	D3DXCreateTextureFromFileA(CDirect3D::getInstance()->GetD3D9Device(), 
		strTex.c_str(), &m_pTextures[1]);
	strTex = strTexPrefix + "LF.jpg";
	D3DXCreateTextureFromFileA(CDirect3D::getInstance()->GetD3D9Device(), 
		strTex.c_str(), &m_pTextures[2]);
	strTex = strTexPrefix + "RT.jpg";
	D3DXCreateTextureFromFileA(CDirect3D::getInstance()->GetD3D9Device(), 
		strTex.c_str(), &m_pTextures[3]);
	strTex = strTexPrefix + "FR.jpg";
	D3DXCreateTextureFromFileA(CDirect3D::getInstance()->GetD3D9Device(), 
		strTex.c_str(), &m_pTextures[4]);
	strTex = strTexPrefix + "BK.jpg";
	D3DXCreateTextureFromFileA(CDirect3D::getInstance()->GetD3D9Device(), 
		strTex.c_str(), &m_pTextures[5]);

	CDirect3D::getInstance()->CreateVertexBuffer(sizeof(SVertexT)*24, 
		D3DUSAGE_WRITEONLY, SVertexT::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL);
	SVertexT* pVertices;
	float L(fRadius);
	m_pVB->Lock(0, sizeof(SVertexT)*24, (void**) &pVertices, 0);
	// Top
	pVertices[0] = SVertexT(-L, L, L, 0.f, 1.f);
	pVertices[1] = SVertexT(-L, L, -L, 0.f, 0.f);
	pVertices[2] = SVertexT(L, L, L, 1.f, 1.f);
	pVertices[3] = SVertexT(L, L, -L, 1.f, 0.f);
	// Bottom
	pVertices[4] = SVertexT(-L, -L, -L, 0.f, 1.f);
	pVertices[5] = SVertexT(-L, -L, L, 0.f, 0.f);
	pVertices[6] = SVertexT(L, -L, -L, 1.f, 1.f);
	pVertices[7] = SVertexT(L, -L, L, 1.f, 0.f);
	// Left
	pVertices[8] = SVertexT(-L, -L, -L, 0.f, 1.f);
	pVertices[9] = SVertexT(-L, L, -L, 0.f, 0.f);
	pVertices[10] = SVertexT(-L, -L, L, 1.f, 1.f);
	pVertices[11] = SVertexT(-L, L, L, 1.f, 0.f);
	// Right
	pVertices[12] = SVertexT(L, -L, L, 0.f, 1.f);
	pVertices[13] = SVertexT(L, L, L, 0.f, 0.f);
	pVertices[14] = SVertexT(L, -L, -L, 1.f, 1.f);
	pVertices[15] = SVertexT(L, L, -L, 1.f, 0.f);
	// Front
	pVertices[16] = SVertexT(L, -L, -L, 0.f, 1.f);
	pVertices[17] = SVertexT(L, L, -L, 0.f, 0.f);
	pVertices[18] = SVertexT(-L, -L, -L, 1.f, 1.f);
	pVertices[19] = SVertexT(-L, L, -L, 1.f, 0.f);
	// Back
	pVertices[20] = SVertexT(-L, -L, L, 0.f, 1.f);
	pVertices[21] = SVertexT(-L, L, L, 0.f, 0.f);
	pVertices[22] = SVertexT(L, -L, L, 1.f, 1.f);
	pVertices[23] = SVertexT(L, L, L, 1.f, 0.f);
	m_pVB->Unlock();
}

void CSkyBox::onRender() {
	BOOL bEnable(CDirect3D::getInstance()->GetD3DLightEnable());
	CDirect3D::getInstance()->D3DLightEnable(FALSE);
	CDirect3D::getInstance()->SetTextureAddressMode(D3DTADDRESS_CLAMP);
	CDirect3D::getInstance()->SetStreamSource(0, m_pVB, 0, sizeof(SVertexT));
	CDirect3D::getInstance()->SetD3DFVF(SVertexT::FVF);
	CDirect3D::getInstance()->SetTransform(D3DTS_WORLD, &m_matId);
	for (int i(0); i < 6; i++) {
		CDirect3D::getInstance()->SetTexture(0, m_pTextures[i]);
		CDirect3D::getInstance()->DrawPrimitive(D3DPT_TRIANGLESTRIP, i * 4, 2);
	}
	CDirect3D::getInstance()->SetTexture(0, NULL);
	CDirect3D::getInstance()->SetTextureAddressMode(D3DTADDRESS_WRAP);
	CDirect3D::getInstance()->D3DLightEnable(bEnable);
}