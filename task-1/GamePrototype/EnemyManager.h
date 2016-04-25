#ifndef GPEnemyManager_H_
#define GPEnemyManager_H_

#include "..\Engine\SkeletonMesh.h"
#include "Chamber.h"
#include "Enemy.h"

class GPEnemyManager
{
public:
	GPEnemyManager();
	~GPEnemyManager();

	void onInit(GPChamber* pChamber);
	void onTick(float fElapsedTime);
	void onRender();

	void Shoot(const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir, const D3DXVECTOR3& vPlayerPos);

	inline void ToggleDebug() { for (unsigned int i(0); i < m_vEnemies.size(); i++) m_vEnemies[i]->ToggleDebug(); }
	inline void Add() { m_vEnemies.push_back(new GPEnemy()); m_vEnemies.back()->onInit(m_pChamber); }
	inline void Kick() { if (m_vEnemies.size()) { delete m_vEnemies.back(); m_vEnemies.pop_back(); } }
	inline int Count() { return m_vEnemies.size(); }

private:
	std::vector<GPEnemy*> m_vEnemies;
	GPChamber* m_pChamber;
};
#endif
