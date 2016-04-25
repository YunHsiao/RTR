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

	void die(const D3DXVECTOR3& vPlayerPos);

private:
	void spawn();
	void ParameterAdaptaion();

	GPChamber* m_pChamber;
	D3DXVECTOR3 m_vMinOffset, m_vMaxOffset; // BB Parameters
	bool m_bCrouching;
	float m_fX, m_fZ;
};
#endif
