#include "Utility.h"
#include "Direct3D.h"
#include "Terrain.h"

CTerrain::CTerrain()
	: m_pVB(NULL)
	, m_pIB(NULL)
	, m_pFrustumVB(NULL)
	, m_pFrustumIB(NULL)
	, m_pTex(NULL)
	, m_pDet(NULL)
	, m_pVertices(NULL)
	, m_pNormals(NULL)
	, m_root(NULL)
	, m_iX(0)
	, m_iZ(0)
	, m_iVX(0)
	, m_iVZ(0)
	, m_iTriangles(0)
	, m_bLOD(false)
	, m_bObserve(false)
{
	D3DXMatrixIdentity(&m_matId);
	D3DXMatrixIdentity(&m_mat);
	D3DXMatrixIdentity(&m_matR);
	ZeroMemory(&m_mCyan, sizeof(m_mCyan));
	ZeroMemory(&m_mMagenta, sizeof(m_mMagenta));
	ZeroMemory(&m_mYellow, sizeof(m_mYellow));
}

CTerrain::~CTerrain()
{
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
	if (m_pFrustumVB) Safe_Release(m_pFrustumVB);
	if (m_pFrustumIB) Safe_Release(m_pFrustumIB);
	Safe_Release(m_pTex);
	Safe_Release(m_pDet);
	for (int i(0); i < m_iVZ; i++)
		Safe_Delete_Array(m_pVertices[i]);
	Safe_Delete_Array(m_pVertices);
	Safe_Delete_Array(m_pNormals);
	Safe_Delete(m_root);
}

bool CTerrain::onInit(LPCTSTR strTex, LPCTSTR strHei, int iWidthLog, int iLengthLog)
{
	D3DXCreateTextureFromFile(CDirect3D::getInstance()->GetD3D9Device(),
		strTex, &m_pTex);
	D3DXCreateTextureFromFile(CDirect3D::getInstance()->GetD3D9Device(),
		TEXT("res\\det.jpg"), &m_pDet);

	IDirect3DTexture9 *pHei(NULL);
	BYTE* cHeight(NULL);
	if (strHei) {
		D3DXCreateTextureFromFile(CDirect3D::getInstance()->GetD3D9Device(),
		strHei, &pHei);
		D3DSURFACE_DESC desc;
		pHei->GetLevelDesc(0, &desc);
		m_iX = desc.Width; m_iZ = desc.Height;
		cHeight = new BYTE[m_iX * m_iZ];
		ZeroMemory(cHeight, sizeof(cHeight));
		D3DLOCKED_RECT rect;
		pHei->LockRect(0, &rect, NULL, 0);
		memcpy_s(cHeight, m_iX * m_iZ, rect.pBits, m_iX * m_iZ);
		pHei->UnlockRect(NULL);
		Safe_Release(pHei);
	} else {
		m_iX = static_cast<int>(pow(2.f, iWidthLog)); 
		m_iZ = static_cast<int>(pow(2.f, iLengthLog));
	}
	m_iVX = m_iX + 1; m_iVZ = m_iZ + 1;
	m_fRX = 10.f / 64.f * m_iX;
	m_fRZ = 10.f / 64.f * m_iZ;
	m_iTriangles = m_iX*m_iZ * 2;
	m_iVertices = m_iVX * m_iVZ;
	if (FAILED(CDirect3D::getInstance()->CreateVertexBuffer(sizeof(SVertexNT2)*m_iVertices,
		D3DUSAGE_WRITEONLY, SVertexNT2::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL)))
		return false;
	m_fSegX = m_fRX*2.f / m_iX; m_fSegZ = m_fRZ*2.f / m_iZ;
	int cnt(0);
	m_pVertices = new D3DXVECTOR3*[m_iVZ];
	for (int i(0); i < m_iVZ; i++) {
		m_pVertices[i] = new D3DXVECTOR3[m_iVX];
		if (strHei) for (int j(0); j < m_iVX; j++) 
			m_pVertices[i][j] = D3DXVECTOR3(m_fSegX*j - m_fRX,
			i == m_iZ || j == m_iX? -1.f : cHeight[cnt++] * 20 / 255.f - 1.f, m_fSegZ*i - m_fRZ);
		else for (int j(0); j < m_iVX; j++) 
			m_pVertices[i][j] = D3DXVECTOR3(m_fSegX*j - m_fRX, -1.f, m_fSegZ*i - m_fRZ);
	}
	if (strHei) Safe_Delete_Array(cHeight);
	m_pNormals = new D3DXVECTOR3[m_iX*m_iZ*2];
	D3DXVECTOR3 n1, n2; cnt = 0;
	for (int i(0); i < m_iZ; i++) {
		for (int j(0); j < m_iX; j++, cnt++) {
			n1 = m_pVertices[i+1][j] - m_pVertices[i][j];
			n2 = m_pVertices[i][j+1] - m_pVertices[i][j];
			D3DXVec3Cross(&m_pNormals[cnt], &n1, &n2);
			D3DXVec3Normalize(&m_pNormals[cnt], &m_pNormals[cnt]);
			cnt++;
			n1 = m_pVertices[i][j+1] - m_pVertices[i+1][j+1];
			n2 = m_pVertices[i+1][j] - m_pVertices[i+1][j+1];
			D3DXVec3Cross(&m_pNormals[cnt], &n1, &n2);
			D3DXVec3Normalize(&m_pNormals[cnt], &m_pNormals[cnt]);
		}
	}

	SVertexNT2* pVertices;
	if (FAILED(m_pVB->Lock(0, sizeof(SVertexNT2)*m_iVertices, (void**)&pVertices, 0)))
		return false;
	float fTexX(1.f / 64.f), fTexZ(1.f / 64.f);
	cnt = 0;
	for (int i(0); i < m_iVZ; i++)
		for (int j(0); j < m_iVX; j++)
			pVertices[cnt++] = SVertexNT2(m_pVertices[i][j].x, m_pVertices[i][j].y,
			m_pVertices[i][j].z, 0.f, 1.f, 0.f, fTexX*j, fTexZ*i);
	m_pVB->Unlock();

	UINT iIndices(sizeof(DWORD)*m_iTriangles * 3);
	if (FAILED(CDirect3D::getInstance()->CreateIndexBuffer(iIndices,
		D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &m_pIB, NULL)))
		return false;

	m_mCyan.Ambient.b = m_mCyan.Diffuse.b = m_mCyan.Ambient.g = m_mCyan.Diffuse.g = 1.f;
	m_mMagenta.Ambient.b = m_mMagenta.Diffuse.b = m_mMagenta.Ambient.r = m_mMagenta.Diffuse.r = 1.f;
	m_mYellow.Ambient.r = m_mYellow.Diffuse.r = m_mYellow.Ambient.g = m_mYellow.Diffuse.g = 1.f;
	if (!InitDefaultIB()) return false;
	InitQuadTree();
	return true;
}

void CTerrain::ToggleObserver() {
	m_bObserve = !m_bObserve;
	if (!m_pFrustumVB) InitFrustum();
}

void CTerrain::InitQuadTree() {
	m_root = new SNode(0, m_iX, m_iZ, 0, m_iX / 2, m_iZ / 2, 0, -1, 0);
	InitQuadTreeDiff(m_root);
	InitQuadTreeNeighbors(m_root);
}

void CTerrain::InitQuadTreeDiff(SNode* node) {
	if (!node->H && !node->W) return;
	float diff(0.f), temp(0.f);
	if (node->nw) { InitQuadTreeDiff(node->nw); temp = node->nw->diff; if (temp > diff) diff = temp; }
	if (node->ne) { InitQuadTreeDiff(node->ne); temp = node->ne->diff; if (temp > diff) diff = temp; }
	if (node->sw) { InitQuadTreeDiff(node->sw); temp = node->sw->diff; if (temp > diff) diff = temp; }
	if (node->se) { InitQuadTreeDiff(node->se); temp = node->se->diff; if (temp > diff) diff = temp; }
	float l(m_pVertices[node->b + node->H][node->l].y),
		r(m_pVertices[node->b + node->H][node->r].y),
		t(m_pVertices[node->t][node->l + node->W].y),
		b(m_pVertices[node->b][node->l + node->W].y),
		c(m_pVertices[node->b + node->H][node->l + node->W].y),
		nw(m_pVertices[node->t][node->l].y),
		ne(m_pVertices[node->t][node->r].y),
		sw(m_pVertices[node->b][node->l].y),
		se(m_pVertices[node->b][node->r].y);
	temp = abs((nw + ne + sw + se) / 4 - c); if (temp > diff) diff = temp;
	temp = abs((nw + ne) / 2 - t); if (temp > diff) diff = temp;
	temp = abs((nw + sw) / 2 - l); if (temp > diff) diff = temp;
	temp = abs((sw + se) / 2 - b); if (temp > diff) diff = temp;
	temp = abs((ne + se) / 2 - r); if (temp > diff) diff = temp;
	node->diff = diff;
}

void CTerrain::InitQuadTreeNeighbors(SNode* node) { // mind-bending
	static int v[5] = { 0, 3, 4, 1, 2 }, h[5] = { 0, 2, 1, 4, 3 };
	if (!node) return;
	// North
	int iTarget(node->C), C(node->C);
	if (node->t < m_iZ) {
		for (int i(0); i <= node->D; i++) {
			int d(C & 7), offset(3 * i); C >>= 3;
			iTarget += (v[d] << offset) - (d << offset);
			if (d > 2) break;
		} node->n = FindNode(m_root, iTarget, node->D);
	}
	// East
	if (node->r < m_iX) {
		iTarget = node->C; C = node->C;
		for (int i(0); i <= node->D; i++) {
			int d(C & 7), offset(3 * i); C >>= 3;
			iTarget += (h[d] << offset) - (d << offset);
			if (d % 2) break;
		} node->e = FindNode(m_root, iTarget, node->D);
	}
	// West
	if (node->l) {
		iTarget = node->C; C = node->C;
		for (int i(0); i <= node->D; i++) {
			int d(C & 7), offset(3 * i); C >>= 3;
			iTarget += (h[d] << offset) - (d << offset);
			if (!(d % 2)) break;
		} node->w = FindNode(m_root, iTarget, node->D);
	}
	// South
	if (node->b) {
		iTarget = node->C; C = node->C;
		for (int i(0); i <= node->D; i++) {
			int d(C & 7), offset(3 * i); C >>= 3;
			iTarget += (v[d] << offset) - (d << offset);
			if (d <= 2) break;
		} node->s = FindNode(m_root, iTarget, node->D);
	}
	InitQuadTreeNeighbors(node->nw); InitQuadTreeNeighbors(node->ne);
	InitQuadTreeNeighbors(node->sw); InitQuadTreeNeighbors(node->se);
}

SNode* CTerrain::FindNode(SNode* node, int C, int D) {
	if (C == 0) return node;
	if (!node) return NULL;
	int offset(3 * D), d(C >> offset); C -= (d << offset);
	if (d == 1) return FindNode(node->nw, C, D - 1);
	else if (d == 2) return FindNode(node->ne, C, D - 1);
	else if (d == 3) return FindNode(node->sw, C, D - 1);
	else return FindNode(node->se, C, D - 1);
}

bool CTerrain::InitDefaultIB() {
	m_iTriangles = m_iX * m_iZ * 2;
	UINT iIndices(sizeof(DWORD)*m_iTriangles * 3);
	DWORD* pIndices;
	if (FAILED(m_pIB->Lock(0, iIndices, (void**)&pIndices, 0)))
		return false;
	int cnt = 0;
	for (int i(0); i < m_iZ; i++) {
		for (int j(0); j < m_iX; j++) {
			pIndices[cnt++] = i*m_iVX + j;
			pIndices[cnt++] = (i + 1)*m_iVX + j;
			pIndices[cnt++] = i*m_iVX + j + 1;
			pIndices[cnt++] = i*m_iVX + j + 1;
			pIndices[cnt++] = (i + 1)*m_iVX + j;
			pIndices[cnt++] = (i + 1)*m_iVX + j + 1;
		}
	}
	m_pIB->Unlock();
	return true;
}

void CTerrain::PruneQuadTree(SNode *node, const D3DXVECTOR3& vPos) {
	D3DXVECTOR3 *vCenter = &m_pVertices[node->b + node->H][node->l + node->W];
	float fRadiusSq(node->H * m_fSegZ + node->W * m_fSegX);
	fRadiusSq *= fRadiusSq;
	//float fThreshold(m_fSegZ * node->H + m_fSegX * node->W);
	//fThreshold *= fThreshold;
	if (DistanceToFrustumSq(&m_pVertices[node->b][node->l]) > fRadiusSq &&
		DistanceToFrustumSq(&m_pVertices[node->b][node->r]) > fRadiusSq &&
		DistanceToFrustumSq(&m_pVertices[node->t][node->l]) > fRadiusSq &&
		DistanceToFrustumSq(&m_pVertices[node->t][node->r]) > fRadiusSq)
		return; // Cull
	//if (!node->H && !node->W || D3DXVec3LengthSq(&(*m_pPos - *vCenter)) > 1e3f*fThreshold) { // Draw
	if (!node->H && !node->W || D3DXVec3LengthSq(&(vPos - *vCenter))
		/ (node->diff * node->diff * 1e5f) >= 1) { // Draw
		node->status = ERS_VISIBLE;
	} else { // Recurse
		node->status = ERS_RECURSED;
		if (node->nw) PruneQuadTree(node->nw, vPos);
		if (node->ne) PruneQuadTree(node->ne, vPos);
		if (node->sw) PruneQuadTree(node->sw, vPos);
		if (node->se) PruneQuadTree(node->se, vPos);
	}
}

float CTerrain::DistanceToFrustumSq(D3DXVECTOR3* vWorld) {
	D3DXVECTOR3 vProj, vNearest; int i(0);
	D3DXVec3TransformCoord(&vProj, vWorld, &m_mat);
	if (vProj.x < -1.f) vNearest.x = -1.f;
	else if (vProj.x > 1.f) vNearest.x = 1.f;
	else { vNearest.x = vProj.x; i++; }
	if (vProj.y < -1.f) vNearest.y = -1.f;
	else if (vProj.y > 1.f) vNearest.y = 1.f;
	else { vNearest.y = vProj.y; i++; }
	if (vProj.z < 0.f) vNearest.z = 0.f;
	else if (vProj.z > 1.f) vNearest.z = 1.f;
	else { vNearest.z = vProj.z; i++; }
	if (i == 3) return 0.f;
	D3DXVec3TransformCoord(&vNearest, &vNearest, &m_matR);
	return D3DXVec3LengthSq(&(*vWorld - vNearest));
}

void CTerrain::GenerateIB(SNode *node, DWORD *pIndices) {
	if (!node || node->status == ERS_PRUNED) return;
	if (node->status == ERS_RECURSED) { // Recurse
		GenerateIB(node->nw, pIndices);	GenerateIB(node->ne, pIndices);
		GenerateIB(node->sw, pIndices);	GenerateIB(node->se, pIndices);
	} else { // Draw
		GenerateQuadrantIB(node, node->n, pIndices, 1);
		GenerateQuadrantIB(node, node->e, pIndices, 2);
		GenerateQuadrantIB(node, node->w, pIndices, 3);
		GenerateQuadrantIB(node, node->s, pIndices, 4);
	}
}

void CTerrain::GenerateQuadrantIB(SNode *target, SNode *node, DWORD *pIndices, int d) {
	if (node && node->status == ERS_RECURSED) { // Recurse
		switch (d) {
		case 1: // North
			GenerateQuadrantIB(target, node->sw, pIndices, d);
			GenerateQuadrantIB(target, node->se, pIndices, d);
			break;
		case 2: // East
			GenerateQuadrantIB(target, node->nw, pIndices, d);
			GenerateQuadrantIB(target, node->sw, pIndices, d);
			break;
		case 3: // West
			GenerateQuadrantIB(target, node->ne, pIndices, d);
			GenerateQuadrantIB(target, node->se, pIndices, d);
			break;
		case 4: // South
			GenerateQuadrantIB(target, node->nw, pIndices, d);
			GenerateQuadrantIB(target, node->ne, pIndices, d);
			break;
		}
	} else { // Draw
		if (node) {
			switch (d) {
			case 1: // North
				pIndices[m_iTriangles * 3] = node->b * m_iVX + node->l;
				pIndices[m_iTriangles * 3 + 1] = node->b * m_iVX + node->r;
				pIndices[m_iTriangles * 3 + 2] = (target->b + target->H) * m_iVX + target->l + target->W;
				m_iTriangles++;
				break;
			case 2: // East
				pIndices[m_iTriangles * 3] = node->t * m_iVX + node->l;
				pIndices[m_iTriangles * 3 + 1] = node->b * m_iVX + node->l;
				pIndices[m_iTriangles * 3 + 2] = (target->b + target->H) * m_iVX + target->l + target->W;
				m_iTriangles++;
				break;
			case 3: // West
				pIndices[m_iTriangles * 3] = node->b * m_iVX + node->r;
				pIndices[m_iTriangles * 3 + 1] = node->t * m_iVX + node->r;
				pIndices[m_iTriangles * 3 + 2] = (target->b + target->H) * m_iVX + target->l + target->W;
				m_iTriangles++;
				break;
			case 4: // South
				pIndices[m_iTriangles * 3] = node->t * m_iVX + node->r;
				pIndices[m_iTriangles * 3 + 1] = node->t * m_iVX + node->l;
				pIndices[m_iTriangles * 3 + 2] = (target->b + target->H) * m_iVX + target->l + target->W;
				m_iTriangles++;
				break;
			}
		} else { // corner cases
			switch (d) {
			case 1: // North
				pIndices[m_iTriangles * 3] = target->t * m_iVX + target->l;
				pIndices[m_iTriangles * 3 + 1] = target->t * m_iVX + target->r;
				pIndices[m_iTriangles * 3 + 2] = (target->b + target->H) * m_iVX + target->l + target->W;
				m_iTriangles++;
				break;
			case 2: // East
				pIndices[m_iTriangles * 3] = target->t * m_iVX + target->r;
				pIndices[m_iTriangles * 3 + 1] = target->b * m_iVX + target->r;
				pIndices[m_iTriangles * 3 + 2] = (target->b + target->H) * m_iVX + target->l + target->W;
				m_iTriangles++;
				break;
			case 3: // West
				pIndices[m_iTriangles * 3] = target->b * m_iVX + target->l;
				pIndices[m_iTriangles * 3 + 1] = target->t * m_iVX + target->l;
				pIndices[m_iTriangles * 3 + 2] = (target->b + target->H) * m_iVX + target->l + target->W;
				m_iTriangles++;
				break;
			case 4: // South
				pIndices[m_iTriangles * 3] = target->b * m_iVX + target->r;
				pIndices[m_iTriangles * 3 + 1] = target->b * m_iVX + target->l;
				pIndices[m_iTriangles * 3 + 2] = (target->b + target->H) * m_iVX + target->l + target->W;
				m_iTriangles++;
				break;
			}
		}
	}
}

void CTerrain::RecoverQuadTree(SNode *node) {
	if (!node || node->status == ERS_PRUNED) return;
	node->status = ERS_PRUNED;
	RecoverQuadTree(node->nw); RecoverQuadTree(node->ne);
	RecoverQuadTree(node->sw); RecoverQuadTree(node->se);
}

VOID CTerrain::onTick(float fElapsedTime, const D3DXMATRIX& pView, const D3DXMATRIX& pProj, const D3DXVECTOR3& pPos) {
	if (!m_bLOD) return;
	D3DXMATRIX temp = pView * pProj;
	if (temp == m_mat) return;
	m_mat = temp;
	D3DXMatrixInverse(&m_matR, NULL, &m_mat);
	DWORD *pIndices; m_iTriangles = 0;
	PruneQuadTree(m_root, pPos);
	m_pIB->Lock(0, sizeof(DWORD)*m_iTriangles * 3, (void**)&pIndices, 0);
	GenerateIB(m_root, pIndices);
	m_pIB->Unlock();
	RecoverQuadTree(m_root);
}

void CTerrain::InitFrustum() {
	if (m_pFrustumVB) return;
	CDirect3D::getInstance()->CreateVertexBuffer(sizeof(SVertexD) * 8,
		D3DUSAGE_WRITEONLY, SVertexD::FVF, D3DPOOL_DEFAULT, &m_pFrustumVB, NULL);
	SVertexD* pVertices;
	m_pFrustumVB->Lock(0, sizeof(SVertexD) * 8, (void**)&pVertices, 0);
	pVertices[0] = SVertexD(-1.f, 1.f, 0.f, 0x80ffffff);
	pVertices[1] = SVertexD(-1.f, 1.f, 1.f, 0x80000000);
	pVertices[2] = SVertexD(1.f, 1.f, 0.f, 0x80ffffff);
	pVertices[3] = SVertexD(1.f, 1.f, 1.f, 0x80000000);
	pVertices[4] = SVertexD(-1.f, -1.f, 0.f, 0x80ffffff);
	pVertices[5] = SVertexD(-1.f, -1.f, 1.f, 0x80000000);
	pVertices[6] = SVertexD(1.f, -1.f, 0.f, 0x80ffffff);
	pVertices[7] = SVertexD(1.f, -1.f, 1.f, 0x80000000);
	m_pFrustumVB->Unlock();

	CDirect3D::getInstance()->CreateIndexBuffer(sizeof(DWORD) * 36,
		D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &m_pFrustumIB, NULL);

	DWORD* pIndices;
	m_pFrustumIB->Lock(0, sizeof(DWORD) * 36, (void**)&pIndices, 0);
	// Top
	pIndices[0] = 0; pIndices[1] = 1; pIndices[2] = 2;
	pIndices[3] = 2; pIndices[4] = 1; pIndices[5] = 3;
	// Bottom
	pIndices[6] = 5; pIndices[7] = 4; pIndices[8] = 7;
	pIndices[9] = 7; pIndices[10] = 4; pIndices[11] = 6;
	// Front
	pIndices[12] = 4; pIndices[13] = 0; pIndices[14] = 6;
	pIndices[15] = 6; pIndices[16] = 0; pIndices[17] = 2;
	// Back
	pIndices[18] = 7; pIndices[19] = 3; pIndices[20] = 5;
	pIndices[21] = 5; pIndices[22] = 3; pIndices[23] = 1;
	// Left
	pIndices[24] = 5; pIndices[25] = 1; pIndices[26] = 4;
	pIndices[27] = 4; pIndices[28] = 1; pIndices[29] = 0;
	// Right
	pIndices[30] = 6; pIndices[31] = 2; pIndices[32] = 7;
	pIndices[33] = 7; pIndices[34] = 2; pIndices[35] = 3;
	m_pFrustumIB->Unlock();
}

VOID CTerrain::onRender() {
	CDirect3D::getInstance()->SetTexture(0, m_pTex);
	CDirect3D::getInstance()->SetTexture(1, m_pDet);
	CDirect3D::getInstance()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	CDirect3D::getInstance()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	CDirect3D::getInstance()->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	CDirect3D::getInstance()->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
	CDirect3D::getInstance()->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
	CDirect3D::getInstance()->SetD3DFVF(SVertexNT2::FVF);
	CDirect3D::getInstance()->SetStreamSource(0, m_pVB, 0, sizeof(SVertexNT2));
	CDirect3D::getInstance()->SetIndices(m_pIB);
	CDirect3D::getInstance()->SetTransform(D3DTS_WORLD, &m_matId);
	CDirect3D::getInstance()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
		0, 0, m_iVertices, 0, m_iTriangles);
	CDirect3D::getInstance()->SetTexture(0, NULL);
	CDirect3D::getInstance()->SetTexture(1, NULL);
	CDirect3D::getInstance()->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	if (m_bObserve) {
		CDirect3D::getInstance()->SetCullMode(D3DCULL_NONE);
		CDirect3D::getInstance()->AlphaBlendEnable(TRUE);
		CDirect3D::getInstance()->SetSrcRenderBlendFactor(D3DBLEND_SRCALPHA);
		CDirect3D::getInstance()->SetDestRenderBlendFactor(D3DBLEND_INVSRCALPHA);
		CDirect3D::getInstance()->SetD3DFVF(SVertexD::FVF);
		CDirect3D::getInstance()->SetStreamSource(0, m_pFrustumVB, 0, sizeof(SVertexD));
		CDirect3D::getInstance()->SetIndices(m_pFrustumIB);
		CDirect3D::getInstance()->SetTransform(D3DTS_WORLD, &m_matR);
		CDirect3D::getInstance()->D3DLightEnable(TRUE);
		CDirect3D::getInstance()->SetMaterial(&m_mCyan);
		CDirect3D::getInstance()->DrawIndexedPrimitive
			(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 4);
		CDirect3D::getInstance()->SetMaterial(&m_mMagenta);
		CDirect3D::getInstance()->DrawIndexedPrimitive
			(D3DPT_TRIANGLELIST, 0, 0, 8, 12, 4);
		CDirect3D::getInstance()->SetMaterial(&m_mYellow);
		CDirect3D::getInstance()->DrawIndexedPrimitive
			(D3DPT_TRIANGLELIST, 0, 0, 8, 24, 4);
		CDirect3D::getInstance()->SetMaterial(NULL);
		CDirect3D::getInstance()->D3DLightEnable(FALSE);
		CDirect3D::getInstance()->AlphaBlendEnable(FALSE);
		CDirect3D::getInstance()->SetCullMode(D3DCULL_CCW);
	}
}

float CTerrain::GetHeight(float fX, float fZ) {
	float fRow((fZ + m_fRZ) / m_fSegZ), fCol((fX + m_fRX) / m_fSegX);
	int iRow(static_cast<int>(fRow)), iCol(static_cast<int>(fCol));
	if (iRow >= m_iZ || iRow < 0 || iCol >= m_iX || iCol < 0) return -1.f;
	float bl(m_pVertices[iRow][iCol].y), br(m_pVertices[iRow][iCol+1].y),
		tl(m_pVertices[iRow+1][iCol].y), tr(m_pVertices[iRow+1][iCol+1].y);
	fRow -= iRow; fCol -= iCol;
	bl += (br - bl) * fCol; tl += (tr - tl) * fCol;
	bl += (tl - bl) * fRow;
	return bl;
}

const D3DXVECTOR3* CTerrain::Intersect(const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir) {
	float fRow((vPos.z + m_fRZ) / m_fSegZ), fCol((vPos.x + m_fRX) / m_fSegX);
	int iRow(static_cast<int>(fRow)), iCol(static_cast<int>(fCol));
	const D3DXVECTOR3* vIntersection(NULL);
	D3DXVECTOR3 vCheck = vPos;
	while (iRow >= 0 && iRow < m_iZ && iCol >= 0 && iCol < m_iX) {
		int index = iRow * m_iX * 2 + iCol * 2;
		vIntersection = TriangleIntersection(vPos, vDir, m_pNormals[index], 
			m_pVertices[iRow][iCol], m_pVertices[iRow][iCol+1], m_pVertices[iRow+1][iCol]);
		if (vIntersection) return vIntersection;
		vIntersection = TriangleIntersection(vPos, vDir, m_pNormals[index+1], 
			m_pVertices[iRow+1][iCol+1], m_pVertices[iRow][iCol+1], m_pVertices[iRow+1][iCol]);
		if (vIntersection) return vIntersection;

		// Optimization inspired by Bresenham
		bool bX(vDir.x > 0.f), bZ(vDir.z > 0.f);
		float fRowMargin(m_pVertices[iRow + bZ][0].z - vPos.z);
		float fColMargin(m_pVertices[0][iCol + bX].x - vPos.x);
		float fEz(vDir.z / vDir.x * fColMargin);
		if (fabsf(fEz) > fabsf(fRowMargin)) bZ ? iRow++ : iRow--;
		else bX ? iCol++ : iCol--;
	}
	return NULL;
}

float Determinant(const D3DXVECTOR3& a, const D3DXVECTOR3& b, const D3DXVECTOR3& c) {
	return a.x * b.y * c.z + b.x * c.y * a.z + c.x * a.y * b.z - 
		(c.x * b.y * a.z + b.x * a.y * c.z + a.x * c.y * b.z);
}

float Sign(const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, int axis) {
	switch (axis) {
	case 0:
		return v0.y * v1.z - v0.z * v1.y;
	case 1:
		return v0.z * v1.x - v0.x * v1.z;
	case 2:
		return v0.x * v1.y - v0.y * v1.x;
	}
	return 0.f;
}

const D3DXVECTOR3* CTerrain::TriangleIntersection(const D3DXVECTOR3& vPos, 
		const D3DXVECTOR3& vDir, const D3DXVECTOR3& n, 
		const D3DXVECTOR3& p0, const D3DXVECTOR3& p1, const D3DXVECTOR3& p2) {

	static D3DXVECTOR3 vIntersection(0.f, .5f, 0.f);
	if (D3DXVec3Dot(&vDir, &n) >= 0.f) return NULL;
	float fDist = D3DXVec3Dot(&(vPos - p0), &n);
	if (fDist < 0.f) return NULL;
	fDist /= D3DXVec3Dot(&vDir, &(-n));
	vIntersection = vPos + vDir * fDist;

	/* side-checking optimal */
	float fSign(Sign(p2 - p0, vIntersection - p0, 0)); int axis(0);
	if (abs(fSign) < 1e-6) {
		fSign = Sign(p2 - p0, vIntersection - p0, ++axis);
		if (abs(fSign) < 1e-6) fSign = Sign(p2 - p0, vIntersection - p0, ++axis);
	}
	if (Sign(p0 - p1, vIntersection - p1, axis) * fSign < 0) return NULL;
	if (Sign(p1 - p2, vIntersection - p2, axis) * fSign < 0) return NULL;
	return &vIntersection;

	/* side-checking *
	D3DXVECTOR3 v0, v1, v2;
	D3DXVec3Cross(&v0, &(p2 - p0), &(vIntersection - p0));
	D3DXVec3Cross(&v1, &(p0 - p1), &(vIntersection - p1));
	D3DXVec3Cross(&v2, &(p1 - p2), &(vIntersection - p2));
	if (D3DXVec3Dot(&v0, &v1) > 0 && D3DXVec3Dot(&v1, &v2) > 0) 
		return &vIntersection;
	return NULL;

	/* barycentric coordinates *
	int fD = Determinant(p0, p1, p2) > 0 ? 1 : -1;
	float b0 = Determinant(vIntersection, p1, p2) * fD;
	float b1 = Determinant(p0, vIntersection, p2) * fD;
	float b2 = Determinant(p0, p1, vIntersection) * fD;
	if (b1 >= 0 && b2 >= 0 && b0 >= 0) return &vIntersection;
	return NULL;
	
	/* brute-force *
	D3DXVECTOR3 v0 = p0 - vIntersection;
	D3DXVECTOR3 v1 = p1 - vIntersection;
	D3DXVECTOR3 v2 = p2 - vIntersection;
	D3DXVec3Normalize(&v0, &v0);
	D3DXVec3Normalize(&v1, &v1);
	D3DXVec3Normalize(&v2, &v2);
	float a0 = acosf(D3DXVec3Dot(&v0, &v1));
	float a1 = acosf(D3DXVec3Dot(&v0, &v2));
	float a2 = acosf(D3DXVec3Dot(&v1, &v2));
	if (abs(a0 + a1 + a2 - D3DX_PI * 2.f) < 1e-6)
		return &vIntersection;
	return NULL;
	/**/
}