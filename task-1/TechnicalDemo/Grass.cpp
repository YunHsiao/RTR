#include "..\Engine\Utility.h"
#include "..\Engine\Direct3D.h"
#include "Grass.h"

void TDGrass::onInit(LPCTSTR strTex) {
	m_bAccurate = false;
	unsigned int tx(0), tz(0);
	for (int i(0); i < 100; i++) {
		rand_s(&tx); rand_s(&tz);
		CBillBoards::Add(D3DXVECTOR3(-5.5f-(float)tx/UINT_MAX*5.f, -.5f, -5.f+(float)tz/UINT_MAX*10.f));
	}
	CBillBoards::onInit(strTex);
}

void TDGrass::Add(const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir, CTerrain* terrain) { 
	D3DXVECTOR3 vTemp;
	const D3DXVECTOR3* vIntersection(terrain->Intersect(vPos, vDir));
	if (!vIntersection) return;
	unsigned int tx(0), tz(0);
	for (int i(0); i < 100; i++) {
		rand_s(&tx); rand_s(&tz);
		vTemp.x = vIntersection->x + ((float) tx / UINT_MAX - .5f) * 5.f;
		vTemp.z = vIntersection->z + ((float) tz / UINT_MAX - .5f) * 5.f;
		vTemp.y = terrain->GetHeight(vTemp.x, vTemp.z) + .5f;
		CBillBoards::Add(vTemp);
	}
}

void TDGrass::onRender(const D3DXMATRIX& mView, const D3DXVECTOR3& vPos) {
	CDirect3D::getInstance()->AlphaTestEnable(TRUE);
	CDirect3D::getInstance()->AlphaFunction(D3DCMP_GREATER);
	CDirect3D::getInstance()->AlphaReference(0x80);
	CDirect3D::getInstance()->SetD3DFVF(SVertexT::FVF);
	CDirect3D::getInstance()->SetStreamSource(0, m_pVB, 0, sizeof(SVertexT));
	CDirect3D::getInstance()->SetTexture(0, m_pTexture);
	if (m_bAccurate) {
		D3DXVECTOR3 vUp(mView._12, mView._22, mView._32);
		for (unsigned int i(0); i < m_vPos.size(); i++) {
			D3DXVECTOR3 zaxis(m_vPos[i] - vPos); D3DXVec3Normalize(&zaxis, &zaxis);
			D3DXVECTOR3 xaxis; D3DXVec3Normalize(D3DXVec3Cross(&xaxis, &vUp, &zaxis), &xaxis);
			D3DXVECTOR3 yaxis; D3DXVec3Cross(&yaxis, &zaxis, &xaxis);
			m_matWorld._11 = xaxis.x; m_matWorld._12 = xaxis.y; m_matWorld._13 = xaxis.z;
			m_matWorld._21 = 0.f; m_matWorld._22 = 1.f; m_matWorld._23 = 0.f;
			m_matWorld._31 = zaxis.x; m_matWorld._32 = zaxis.y; m_matWorld._33 = zaxis.z;
			m_matWorld._41 = m_vPos[i].x; m_matWorld._42 = m_vPos[i].y; m_matWorld._43 = m_vPos[i].z;
			CDirect3D::getInstance()->SetTransform(D3DTS_WORLD, &m_matWorld);
			CDirect3D::getInstance()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		}
	} else {
		m_matWorld = mView;
		m_matWorld._12 = m_matWorld._32 = m_matWorld._41 = m_matWorld._42 = m_matWorld._43 = 0.f;
		m_matWorld._22 = 1.f;
		D3DXMatrixTranspose(&m_matWorld, &m_matWorld);
		for (unsigned int i(0); i < m_vPos.size(); i++) {
			m_matWorld._41 = m_vPos[i].x; m_matWorld._42 = m_vPos[i].y; m_matWorld._43 = m_vPos[i].z;
			CDirect3D::getInstance()->SetTransform(D3DTS_WORLD, &m_matWorld);
			CDirect3D::getInstance()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		}
	}
	CDirect3D::getInstance()->SetTexture(0, NULL);
	CDirect3D::getInstance()->AlphaTestEnable(FALSE);
}