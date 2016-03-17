#ifndef GPEnemyManager_H_
#define GPEnemyManager_H_

#include "..\Engine\SkeletonMesh.h"
#include "Chamber.h"
#include "Enemy.h"

#define ENEMY_CNT 10

class GPEnemyManager
{
public:
	GPEnemyManager();
	~GPEnemyManager();

	void onInit(GPChamber* pChamber);
	void onTick(float fElapsedTime);
	void onRender();

	void Shoot(const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir, 
		const D3DXMATRIX& matView);

private:
	GPEnemy m_vEnemies[ENEMY_CNT];
	GPChamber* m_pChamber;
};
#endif
