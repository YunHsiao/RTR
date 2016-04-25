#include "..\Engine\Utility.h"
#include "..\Engine\Direct3D.h"
#include "Disc.h"

TDDisc::TDDisc():
m_iNH(4),
m_iTexture(4)
{
	D3DXMatrixRotationY(&m_matRot, 80);
	D3DXMatrixScaling(&m_matSca, .05f, .05f, .05f);
	m_matSca._42 = 10.f;
	m_matWorld = m_matSca * m_matRot;
}

TDDisc::~TDDisc()
{
	Safe_Release(m_pEffect);
	Safe_Release(m_pStaticMesh);
	for (TextureIT it(m_vTexture.begin()); it != m_vTexture.end(); it++)
		Safe_Release(*it);
	for (TextureIT it(m_vNH.begin()); it != m_vNH.end(); it++)
		Safe_Release(*it);
	m_vTexture.clear();
	m_vNH.clear();
	m_pTexture = NULL;
	m_pNH = NULL;
}

bool TDDisc::onInit(LPCTSTR strMesh)
{
	CParallaxMesh::onInit(strMesh, TEXT("res\\effect\\stones.bmp"), 
		TEXT("res\\effect\\stonesNH.tga"));
	m_vTexture.push_back(m_pTexture);
	m_vNH.push_back(m_pNH);

	if (!CreateTextureFromFile("res\\effect\\rocks.jpg")) return false;
	if (!CreateTextureFromFile("res\\effect\\wall.jpg")) return false;
	if (!CreateTextureFromFile("res\\effect\\concrete.bmp")) return false;
	if (!CreateTextureFromFile("res\\effect\\wood.jpg")) return false;

	if (!CreateTextureFromFile("res\\effect\\rocksNH.tga", true)) return false;
	if (!CreateTextureFromFile("res\\effect\\wallNH.tga", true)) return false;
	if (!CreateTextureFromFile("res\\effect\\bumpNH.tga", true)) return false;
	if (!CreateTextureFromFile("res\\effect\\woodNH.tga", true)) return false;
	if (!CreateTextureFromFile("res\\effect\\dentNH.tga", true)) return false;
	if (!CreateTextureFromFile("res\\effect\\saintNH.tga", true)) return false;
	update();
	return true;
}

bool TDDisc::CreateTextureFromFile(char* str, bool bNormalHeightTexture) {
	IDirect3DTexture9 *pTex;
	if (FAILED(D3DXCreateTextureFromFileExA(CDirect3D::getInstance()->GetD3D9Device(),
		str, 256, 256, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR, 0, NULL, NULL, &pTex)))
		return false;
	if (bNormalHeightTexture) m_vNH.push_back(pTex);
	else m_vTexture.push_back(pTex);
	return true;
}

void TDDisc::onTick(float fElapsedTime) {
	static float fAngle = 80.f;
	fAngle += fElapsedTime;
	D3DXMatrixRotationY(&m_matRot, fAngle);
	m_matWorld = m_matSca * m_matRot;
}

void TDDisc::onRender(const D3DXMATRIX& matView, const D3DXMATRIX& matProj, const D3DXVECTOR3& vPos)
{
	if (GetAsyncKeyState(VK_OEM_1) & 0x8000 && m_fDepth > 0.f) m_fDepth -= .01f;
	if (GetAsyncKeyState(VK_OEM_7) & 0x8000) m_fDepth += .01f;
	CParallaxMesh::onRender(matView, matProj, vPos);
}
