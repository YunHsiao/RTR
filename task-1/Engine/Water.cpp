#include "Utility.h"
#include "Direct3D.h"
#include "Water.h"

CWater::CWater()
:m_pMesh(NULL)
,m_iCur(0)
,m_iFrames(0)
{
	ZeroMemory(m_ppTexture, sizeof(m_ppTexture));
	D3DXMatrixTranslation(&m_mat, 0.f, -.12f, 0.f);
}

CWater::~CWater(void) {
	for(int i(0); i < m_iFrames; i++)
		Safe_Release(m_ppTexture[i]);
	Safe_Release(m_pMesh);
}

void CWater::onInit(const char* strTexPre, const char* strTexPost, int iFrames, float fRX, float fRZ) {
	m_iFrames = iFrames;
	m_fLX = fRX; m_fLZ = fRZ; m_iVX = (int) m_fLX/4; m_iVZ = (int) m_fLZ/4;
	m_iEX = m_iVX - 1; m_iEZ = m_iVZ - 1;
	m_lV = m_iVX * m_iVZ; m_lT = m_iEX * m_iEZ * 2;
	D3DXCreateMeshFVF(m_lT, m_lV, D3DXMESH_MANAGED, SVertexNT::FVF, 
		CDirect3D::getInstance()->GetD3D9Device(), &m_pMesh);

	SVertexNT* pVertices;
	m_pMesh->LockVertexBuffer(0, (void**) &pVertices);
	int cnt(0);
	float fSegX(m_fLX*2/m_iEX), fSegZ(m_fLZ*2/m_iEZ);
	for (int i(0); i < m_iVZ; i++) 
		for (int j(0); j < m_iVX; j++)
			pVertices[cnt++] = SVertexNT(j*fSegX-m_fLX, 0.f, 
			i*fSegZ-m_fLZ, 0.f, 1.f, 0.f, j*1.f, i*1.f);
	m_pMesh->UnlockVertexBuffer();

	WORD* pIndices;
	m_pMesh->LockIndexBuffer(0, (void**) &pIndices);
	cnt = 0;
	for (int i(0); i < m_iEZ; i++) {
		for (int j(0); j < m_iEX; j++) {
			pIndices[cnt++] = i*m_iVX+j;
			pIndices[cnt++] = (i+1)*m_iVX+j;
			pIndices[cnt++] = i*m_iVX+j+1;
			pIndices[cnt++] = i*m_iVX+j+1;
			pIndices[cnt++] = (i+1)*m_iVX+j;
			pIndices[cnt++] = (i+1)*m_iVX+j+1;
		}
	}
	m_pMesh->UnlockIndexBuffer();

	DWORD *pAttritube;
	m_pMesh->LockAttributeBuffer(0, &pAttritube);
	for(DWORD i(0); i < m_lT; i++)
		pAttritube[i] = 0;
	m_pMesh->UnlockAttributeBuffer();

	CHAR str[32];
	for(int i(0); i < m_iFrames; i++) {
		sprintf_s(str, "%s%d%s", strTexPre, i, strTexPost);
		D3DXCreateTextureFromFileA(CDirect3D::getInstance()->GetD3D9Device(),
			str, &m_ppTexture[i]);
	}
}

void CWater::onTick(float fElapsedTime) {
	static float fTemp(0.f);
	fTemp += fElapsedTime * .4f;

	static float w(D3DX_PI);	// 角速度 控制波形速度
	static float k(5.f);		// 波矢量 控制波形的平滑程度
	static float a(.3f);		// 振幅 控制波形的起伏程度
	SVertexNT *pVertices;
	m_pMesh->LockVertexBuffer(0, (void**) &pVertices);
	for(DWORD i(0); i < m_lV; i++) {
		pVertices[i].y = a*sin(k*pVertices[i].x-w*fTemp);
		pVertices[i].y += 2*a*cos(k*pVertices[i].z-w*fTemp);
	}
	m_pMesh->UnlockVertexBuffer();
	D3DXComputeNormals(m_pMesh, NULL);
	m_iCur = static_cast<int>(fTemp*100) % m_iFrames;
}

void CWater::onRender() {
	CDirect3D::getInstance()->SetTransform(D3DTS_WORLD, &m_mat);
	CDirect3D::getInstance()->SetTexture(0, m_ppTexture[m_iCur]);
	m_pMesh->DrawSubset(0);
	CDirect3D::getInstance()->SetTexture(0, NULL);
}

