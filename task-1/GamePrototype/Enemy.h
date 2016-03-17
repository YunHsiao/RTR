#ifndef GPEnemy_H_
#define GPEnemy_H_

#include "..\Engine\SkeletonMesh.h"
#include "Chamber.h"

class GPEnemy : public CSkeletonMesh
{
public:
	GPEnemy();
	~GPEnemy();

	void onInit(GPChamber* pChamber);
	bool onTick(float fElapsedTime);
	void onRender();

	void die(const D3DXMATRIX& matView);

private:
	void spawn();
	GPChamber* m_pChamber;
	float m_fX, m_fZ;
};
#endif
