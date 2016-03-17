#include "..\Engine\Utility.h"
#include "Grass.h"

void TDGrass::onInit(LPCTSTR strTex) {
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