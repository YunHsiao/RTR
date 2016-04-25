#include "Utility.h"
#include "Direct3D.h"
#include "Window.h"
#include "Shadow.h"

CShadow::CShadow()
:m_pVB(NULL)
{
	D3DXMatrixIdentity(&m_matTrans);
}

CShadow::~CShadow()
{
	Safe_Release(m_pVB);
	for (unsigned int i(0); i < m_vVB.size(); i++)
		Safe_Release(m_vVB[i]);
	m_vVB.clear();
}

void CShadow::onInit(const D3DXVECTOR3* pSun) {
	m_pSun = pSun;

	CDirect3D::getInstance()->CreateVertexBuffer(sizeof(SVertexRHWD)*4, 
		D3DUSAGE_WRITEONLY, SVertexRHWD::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL);
	SVertexRHWD* pVertices;
	m_pVB->Lock(0, sizeof(SVertexRHWD)*4, (void**) &pVertices, 0);
	RECT rWnd;
	GetClientRect(CWindow::getInstance()->getHWND(), &rWnd);
	float fW = static_cast<float>(rWnd.right - rWnd.left);
	float fH = static_cast<float>(rWnd.bottom - rWnd.top);
	pVertices[0] = SVertexRHWD(0.f, fH, 0.f, 1.f, 0x80000000);
	pVertices[1] = SVertexRHWD(0.f, 0.f, 0.f, 1.f, 0x80000000);
	pVertices[2] = SVertexRHWD(fW, fH, 0.f, 1.f, 0x80000000);
	pVertices[3] = SVertexRHWD(fW, 0.f, 0.f, 1.f, 0x80000000);
	m_pVB->Unlock();
}

void CShadow::add(ID3DXMesh* pMesh, D3DXMATRIX* pMat) {
	if (!pMesh) return;
	DWORD dwFaces(pMesh->GetNumFaces());
	IDirect3DVertexBuffer9* pVB;
	CDirect3D::getInstance()->CreateVertexBuffer(sizeof(D3DXVECTOR3)*dwFaces*18, 
		D3DUSAGE_WRITEONLY, D3DFVF_XYZ, D3DPOOL_DEFAULT, &pVB, NULL);
	m_vVB.push_back(pVB);
	m_vMesh.push_back(pMesh);
	m_vMat.push_back(pMat);
	m_dwTriangles.push_back(0);
}

void CShadow::onTick(float fElapsedTime) {
	for (unsigned int i(0); i < m_vMesh.size(); i++) {
		D3DXVECTOR3 vSun(*m_pSun); D3DXMATRIX mInv;
		D3DXMatrixInverse(&mInv, NULL, m_vMat[i]);
		D3DXVec3TransformCoord(&vSun, &vSun, &mInv);

		ID3DXMesh* pMesh;
		m_vMesh[i]->CloneMeshFVF(m_vMesh[i]->GetOptions(), D3DFVF_XYZ, 
			CDirect3D::getInstance()->GetD3D9Device(), &pMesh);

		D3DXVECTOR3 *pVertices, *pVolume;
		WORD* pIndices;
		pMesh->LockVertexBuffer(NULL, (void**) &pVertices);
		pMesh->LockIndexBuffer(NULL, (void**) &pIndices);
		DWORD dwFaces(pMesh->GetNumFaces());

		WORD* pEdges = new WORD[dwFaces*3*2];
		DWORD dwEdges(0);
		for (DWORD j(0); j < dwFaces; j++) {
			WORD wFace0 = pIndices[j*3];
			WORD wFace1 = pIndices[j*3+1];
			WORD wFace2 = pIndices[j*3+2];
			D3DXVECTOR3 v0 = pVertices[wFace0];
			D3DXVECTOR3 v1 = pVertices[wFace1];
			D3DXVECTOR3 v2 = pVertices[wFace2];
			D3DXVECTOR3 vCross1(v2-v1);
			D3DXVECTOR3 vCross2(v1-v0);
			D3DXVECTOR3 vNormal;
			D3DXVec3Cross(&vNormal, &vCross1, &vCross2);
			if (D3DXVec3Dot(&vNormal, m_pSun) > 0.f) {
				AddEdge(pEdges, dwEdges, wFace0, wFace1);
				AddEdge(pEdges, dwEdges, wFace1, wFace2);
				AddEdge(pEdges, dwEdges, wFace2, wFace0);
			}
		}
		m_dwTriangles[i] = dwEdges*2;
		m_vVB[i]->Lock(0, sizeof(D3DXVECTOR3)*m_dwTriangles[i]*3, (void**) &pVolume, NULL);
		// some (solvable, but expensive) issues here: 
		// (view port related) direction undetermined for segment v1 - v2,
		// so CULL_CW doesn't necessarily mean 'back side'.
		for (DWORD j(0); j < dwEdges; j++) {
			D3DXVECTOR3 v1 = pVertices[pEdges[2*j+0]];	
			D3DXVECTOR3 v2 = pVertices[pEdges[2*j+1]];  
			D3DXVECTOR3 v3 = (v1 - vSun);				
			D3DXVECTOR3 v4 = (v2 - vSun);
			pVolume[j*6] = v3;		
			pVolume[j*6+1] = v1;	
			pVolume[j*6+2] = v4;	// 1 2
			pVolume[j*6+3] = v4;	// |\|
			pVolume[j*6+4] = v1;	// 3 4
			pVolume[j*6+5] = v2;
		}
		m_vVB[i]->Unlock();
		pMesh->UnlockVertexBuffer();
		pMesh->UnlockIndexBuffer();
		Safe_Release(pMesh);
		Safe_Delete_Array(pEdges);
	}
}

void CShadow::AddEdge(WORD* pEdges, DWORD& dwEdges, WORD v0, WORD v1) {
	for (DWORD i(0); i < dwEdges; i++) {
		if ((pEdges[i*2] == v0 && pEdges[i*2+1] == v1) ||
			(pEdges[i*2] == v1 && pEdges[i*2+1] == v0)) {
			if (dwEdges > 1) {
				pEdges[i*2] = pEdges[(dwEdges-1)*2];
				pEdges[i*2+1] = pEdges[(dwEdges-1)*2+1];
			}
			dwEdges--;
			return;
		}
	}
	pEdges[dwEdges*2] = v0;
	pEdges[dwEdges*2+1] = v1;
	dwEdges++;
}

void CShadow::onRender() {
	// using z-pass algorithm
	CDirect3D::getInstance()->DepthWriteEnable(FALSE);
	CDirect3D::getInstance()->StencilEnable(TRUE);
	CDirect3D::getInstance()->AlphaBlendEnable(TRUE);
	// offset half way the buffer (8bit) to prevent "inside reverse" 
	CDirect3D::getInstance()->GetD3D9Device()->Clear(0, 0, D3DCLEAR_STENCIL, 0, 1.0f, 1<<7); 
	CDirect3D::getInstance()->SetSrcRenderBlendFactor(D3DBLEND_ZERO);
	CDirect3D::getInstance()->SetDestRenderBlendFactor(D3DBLEND_ONE);
	CDirect3D::getInstance()->SetStencilFunc(D3DCMP_ALWAYS);
	CDirect3D::getInstance()->SetD3DFVF(D3DFVF_XYZ);
	for (unsigned int i(0); i < m_vMesh.size(); i++) {
		CDirect3D::getInstance()->SetTransform(D3DTS_WORLD, m_vMat[i]);
		CDirect3D::getInstance()->SetStreamSource(0, m_vVB[i], 0, sizeof(D3DXVECTOR3));

		CDirect3D::getInstance()->SetCullMode(D3DCULL_CW); // back side
		CDirect3D::getInstance()->SetStencilPass(D3DSTENCILOP_DECR);
		CDirect3D::getInstance()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_dwTriangles[i]);

		CDirect3D::getInstance()->SetCullMode(D3DCULL_CCW); // front side
		CDirect3D::getInstance()->SetStencilPass(D3DSTENCILOP_INCR);
		CDirect3D::getInstance()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_dwTriangles[i]);
	}
	CDirect3D::getInstance()->SetSrcRenderBlendFactor(D3DBLEND_SRCALPHA);
	CDirect3D::getInstance()->SetDestRenderBlendFactor(D3DBLEND_INVSRCALPHA);
	CDirect3D::getInstance()->SetStencilReference(1<<7);
	CDirect3D::getInstance()->SetStencilFunc(D3DCMP_LESS); // 1<<7 less than current stencil
	CDirect3D::getInstance()->SetStencilPass(D3DSTENCILOP_KEEP);
	CDirect3D::getInstance()->SetD3DFVF(SVertexRHWD::FVF);
	CDirect3D::getInstance()->SetStreamSource(0, m_pVB, 0, sizeof(SVertexRHWD));
	CDirect3D::getInstance()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	CDirect3D::getInstance()->AlphaBlendEnable(FALSE);
	CDirect3D::getInstance()->StencilEnable(FALSE);
	CDirect3D::getInstance()->DepthWriteEnable(TRUE);
}