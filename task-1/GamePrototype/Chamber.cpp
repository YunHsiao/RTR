#include "..\Engine\Utility.h"
#include "..\Engine\Direct3D.h"
#include "Chamber.h"

GPChamber::GPChamber():
m_pVB(NULL),
m_fWall(10.f)
{
}

GPChamber::~GPChamber()
{
	Safe_Release(m_pVB);
	for (unsigned int i(0); i < m_vTextures.size(); i++)
		Safe_Release(m_vTextures[i]);
}

void GPChamber::onInit(float fZ) {
	// Textures
	IDirect3DTexture9* pTex;
	if (FAILED(D3DXCreateTextureFromFile(CDirect3D::getInstance()->GetD3D9Device(),
		TEXT("res\\iceworld\\Glacier.jpg"), &pTex))) return;
	m_vTextures.push_back(pTex);
	if (FAILED(D3DXCreateTextureFromFile(CDirect3D::getInstance()->GetD3D9Device(),
		TEXT("res\\iceworld\\Snow.jpg"), &pTex))) return;
	m_vTextures.push_back(pTex);
	if (FAILED(D3DXCreateTextureFromFile(CDirect3D::getInstance()->GetD3D9Device(),
		TEXT("res\\iceworld\\Eskimoan Construction.jpg"), &pTex))) return;
	m_vTextures.push_back(pTex);
	if (FAILED(D3DXCreateTextureFromFile(CDirect3D::getInstance()->GetD3D9Device(),
		TEXT("res\\iceworld\\Ice Diamonds.jpg"), &pTex))) return;
	m_vTextures.push_back(pTex);
	if (FAILED(D3DXCreateTextureFromFile(CDirect3D::getInstance()->GetD3D9Device(),
		TEXT("res\\iceworld\\Poseidon's Palace.jpg"), &pTex))) return;
	m_vTextures.push_back(pTex);
	if (FAILED(D3DXCreateTextureFromFile(CDirect3D::getInstance()->GetD3D9Device(),
		TEXT("res\\iceworld\\Chiseled Ice.jpg"), &pTex))) return;
	m_vTextures.push_back(pTex);
	if (FAILED(D3DXCreateTextureFromFile(CDirect3D::getInstance()->GetD3D9Device(),
		TEXT("res\\iceworld\\Muddy Ice.jpg"), &pTex))) return;
	m_vTextures.push_back(pTex);
	// Map Parameter
	float fX(fZ * .84f), fRight(fZ * .3f), fTop(fZ * .39f),
		fMid(fZ * .075f), fParapetL(fZ * .12f), fParapetD(fZ * .035f);
	m_fX = fX; m_fZ = fZ;
	// Walls
	m_vColliders.push_back(SRect(fRight-fX, fTop-fZ, -fMid, -fMid, m_fWall));
	m_vColliders.push_back(SRect(fRight-fX, fMid, -fMid, fZ-fTop, m_fWall));
	m_vColliders.push_back(SRect(fMid, fMid, fX-fRight, fZ-fTop, m_fWall));
	m_vColliders.push_back(SRect(fMid, fTop-fZ, fX-fRight, -fMid, m_fWall));
	// Parapets
	m_vColliders.push_back(SRect(fRight-fX-fParapetL, fTop-fZ, 
		fRight-fX-1e-3f, fTop+fParapetD-fZ, m_fWall*.25f));
	m_vColliders.push_back(SRect(-fX, -fParapetD*.5f, 
		fParapetL-fX, fParapetD*.5f, m_fWall*.25f));
	m_vColliders.push_back(SRect(fRight-fX-fParapetL, fZ-fTop-fParapetD, 
		fRight-fX-1e-3f, fZ-fTop, m_fWall*.25f));
	m_vColliders.push_back(SRect(fX-fRight+1e-3f, fZ-fTop-fParapetD, 
		fX-fRight+fParapetL, fZ-fTop, m_fWall*.25f));
	m_vColliders.push_back(SRect(fX-fParapetL, -fParapetD*.5f, 
		fX, fParapetD*.5f, m_fWall*.25f));
	m_vColliders.push_back(SRect(fX-fRight+1e-3f, fTop-fZ, 
		fX-fRight+fParapetL, fTop+fParapetD-fZ, m_fWall*.25f));
	
	m_iTriangles = (m_vColliders.size() + 1) * 10;
	CDirect3D::getInstance()->CreateVertexBuffer(sizeof(SVertexT)*m_iTriangles*3, 
		D3DUSAGE_WRITEONLY, SVertexT::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL);
	SVertexT* pVertex; unsigned int cnt(0);
	
	m_pVB->Lock(0, sizeof(SVertexT)*m_iTriangles*3, (void**)&pVertex, 0);
	writeQuad(pVertex, cnt, D3DXVECTOR3(fX, -1.f, -fZ), D3DXVECTOR3(-fX, m_fWall, -fZ));
	writeQuad(pVertex, cnt, D3DXVECTOR3(-fX, -1.f, -fZ), D3DXVECTOR3(-fX, m_fWall, fZ));
	writeQuad(pVertex, cnt, D3DXVECTOR3(-fX, -1.f, fZ), D3DXVECTOR3(fX, m_fWall, fZ));
	writeQuad(pVertex, cnt, D3DXVECTOR3(fX, -1.f, fZ), D3DXVECTOR3(fX, m_fWall, -fZ));
	writeQuad(pVertex, cnt, D3DXVECTOR3(-fX, -1.f, -fZ), D3DXVECTOR3(fX, -1.f, fZ));
	for (unsigned int i(0); i < m_vColliders.size(); i++)
		writeCube(pVertex, cnt, m_vColliders[i]);
	m_pVB->Unlock();
}

bool GPChamber::Validate(const D3DXVECTOR3& p, float offset) {
	if ((p.x < -m_fX+offset) || (p.x > m_fX-offset)
		|| (p.z < -m_fZ+offset) || (p.z > m_fZ-offset)) return false;
	for (unsigned int i(0); i < m_vColliders.size(); i++) 
		if (m_vColliders[i].Contains(p, offset)) return false;
	return true;
}

void GPChamber::writeQuad(SVertexT* pVertex, unsigned int& cnt, 
	const D3DXVECTOR3& p, const D3DXVECTOR3& q) { // 6 Vertices
	int axis(0);
	if (p.y == q.y) axis = 1;
	else if (p.z == q.z) axis = 2;
	float u, v;
	switch (axis) {
		case 0: u = (q.z - p.z) / m_fWall; v = (q.y - p.y) / m_fWall; break;
		case 1: u = (q.x - p.x) / m_fWall; v = (q.z - p.z) / m_fWall; break;
		case 2: u = (q.x - p.x) / m_fWall; v = (q.y - p.y) / m_fWall; break;
	}
	pVertex[cnt++] = SVertexT(p.x, p.y, p.z, 0.f, v);
	switch (axis) {
		case 0:
			pVertex[cnt++] = SVertexT(p.x, q.y, p.z, 0.f, 0.f);
			pVertex[cnt++] = SVertexT(p.x, p.y, q.z, u, v);
			pVertex[cnt++] = SVertexT(p.x, p.y, q.z, u, v);
			pVertex[cnt++] = SVertexT(p.x, q.y, p.z, 0.f, 0.f);
			break;
		case 1:
			pVertex[cnt++] = SVertexT(p.x, p.y, q.z, 0.f, 0.f);
			pVertex[cnt++] = SVertexT(q.x, p.y, p.z, u, v);
			pVertex[cnt++] = SVertexT(q.x, p.y, p.z, u, v);
			pVertex[cnt++] = SVertexT(p.x, p.y, q.z, 0.f, 0.f);
			break;
		case 2:
			pVertex[cnt++] = SVertexT(p.x, q.y, p.z, 0.f, 0.f);
			pVertex[cnt++] = SVertexT(q.x, p.y, p.z, u, v);
			pVertex[cnt++] = SVertexT(q.x, p.y, p.z, u, v);
			pVertex[cnt++] = SVertexT(p.x, q.y, p.z, 0.f, 0.f);
			break;
	}
	pVertex[cnt++] = SVertexT(q.x, q.y, q.z, u, 0.f);
}

void GPChamber::writeCube(SVertexT* pVertex, unsigned int& cnt, 
	const SRect& r) { // 30 Vertices
	writeQuad(pVertex, cnt, D3DXVECTOR3(r.xmin, -1.f, r.zmin), D3DXVECTOR3(r.xmax, r.h, r.zmin));
	writeQuad(pVertex, cnt, D3DXVECTOR3(r.xmax, -1.f, r.zmin), D3DXVECTOR3(r.xmax, r.h, r.zmax));
	writeQuad(pVertex, cnt, D3DXVECTOR3(r.xmax, -1.f, r.zmax), D3DXVECTOR3(r.xmin, r.h, r.zmax));
	writeQuad(pVertex, cnt, D3DXVECTOR3(r.xmin, -1.f, r.zmax), D3DXVECTOR3(r.xmin, r.h, r.zmin));
	writeQuad(pVertex, cnt, D3DXVECTOR3(r.xmin, r.h, r.zmin), D3DXVECTOR3(r.xmax, r.h, r.zmax));
}

void GPChamber::onRender() {
	if (!m_vColliders.size()) return;
	CDirect3D::getInstance()->SetD3DFVF(SVertexT::FVF);
	CDirect3D::getInstance()->SetStreamSource(0, m_pVB, 0, sizeof(SVertexT));
	CDirect3D::getInstance()->SetCullMode(D3DCULL_NONE);
	CDirect3D::getInstance()->SetTexture(0, m_vTextures[0]);
	CDirect3D::getInstance()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 8);
	CDirect3D::getInstance()->SetTexture(0, m_vTextures[1]);
	CDirect3D::getInstance()->DrawPrimitive(D3DPT_TRIANGLELIST, 24, 2);
	for (unsigned int i(0); i < m_vColliders.size(); i++) {
		CDirect3D::getInstance()->SetTexture(0, m_vTextures[i>4?6:i+2]);
		CDirect3D::getInstance()->DrawPrimitive(D3DPT_TRIANGLELIST, (i+1)*30, 10);
	}
	CDirect3D::getInstance()->SetCullMode(D3DCULL_CCW);
	CDirect3D::getInstance()->SetTexture(0, NULL);
}