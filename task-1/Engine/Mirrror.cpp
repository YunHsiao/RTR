#include "Utility.h"
#include "Direct3D.h"
#include "Mirror.h"

CMirror::CMirror()
:m_pVB(NULL)
,m_pTexture(NULL)
{
	D3DXMatrixIdentity(&m_matTrans);
}

CMirror::~CMirror()
{
	Safe_Release(m_pVB);
	Safe_Release(m_pTexture);
}

void CMirror::onInit(LPCTSTR strTex, float fYaw, float fPitch, float fRow) {
	m_fAngle[0] = fYaw; m_fAngle[1] = fPitch; m_fAngle[2] = fRow;
	updateTransform();

	D3DXCreateTextureFromFile(CDirect3D::getInstance()->GetD3D9Device(), 
		strTex, &m_pTexture);
	CDirect3D::getInstance()->CreateVertexBuffer(sizeof(SVertexT)*4, 
		D3DUSAGE_WRITEONLY, SVertexT::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL);
	SVertexT* pVertices;
	m_pVB->Lock(0, sizeof(SVertexT)*4, (void**) &pVertices, 0);
	pVertices[0] = SVertexT(-9/8.f, -2.f, 0.f, 0.f, 1.f);
	pVertices[1] = SVertexT(-9/8.f, 2.f, 0.f, 0.f, 0.f);
	pVertices[2] = SVertexT(9/8.f, -2.f, 0.f, 1.f, 1.f);
	pVertices[3] = SVertexT(9/8.f, 2.f, 0.f, 1.f, 0.f);
	m_pVB->Unlock();
}

void CMirror::updateTransform() {
	D3DXMatrixRotationYawPitchRoll(&m_matTrans, m_fAngle[0], m_fAngle[1], m_fAngle[2]);
	m_matTrans._41 = 4.f; m_matTrans._42 = 3.f; m_matTrans._43 = 1.f;

	D3DXPLANE plane(0.0f, 0.0f, 1.0f, 0.0f);
	D3DXMatrixInverse(&m_matMirror, NULL, &m_matTrans);
	D3DXMatrixTranspose(&m_matMirror, &m_matMirror);
	D3DXPlaneTransform(&plane, &plane, &m_matMirror);
	D3DXMatrixReflect(&m_matMirror, &plane);
}

void CMirror::onRender() {
	CDirect3D::getInstance()->GetD3D9Device()->Clear(0, 0, D3DCLEAR_STENCIL, 0, 1.0f, 0);
	CDirect3D::getInstance()->StencilEnable(TRUE);
	CDirect3D::getInstance()->DepthWriteEnable(FALSE);
	CDirect3D::getInstance()->SetStencilFunc(D3DCMP_ALWAYS);
	CDirect3D::getInstance()->SetStencilPass(D3DSTENCILOP_REPLACE);
	CDirect3D::getInstance()->SetStencilReference(1);

	CDirect3D::getInstance()->SetStreamSource(0, m_pVB, 0, sizeof(SVertexT));
	CDirect3D::getInstance()->SetD3DFVF(SVertexT::FVF);
	CDirect3D::getInstance()->SetTransform(D3DTS_WORLD, &m_matTrans);
	CDirect3D::getInstance()->SetTexture(0, m_pTexture);
	CDirect3D::getInstance()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	CDirect3D::getInstance()->SetTexture(0, NULL);

	CDirect3D::getInstance()->SetStencilFunc(D3DCMP_EQUAL);
	CDirect3D::getInstance()->SetStencilPass(D3DSTENCILOP_KEEP);
	CDirect3D::getInstance()->SetStencilMask(0xffffffff);
	CDirect3D::getInstance()->DepthWriteEnable(TRUE);
	CDirect3D::getInstance()->StencilEnable(FALSE);
	CDirect3D::getInstance()->GetD3D9Device()->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
}