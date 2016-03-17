#ifndef TDGrass_H_
#define TDGrass_H_

#include "..\Engine\BillBoards.h"
#include "Terrain.h"

class TDGrass : public CBillBoards
{
public:
	void onInit(LPCTSTR strTex);
	void Add(const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir, CTerrain* terrain);
};
#endif