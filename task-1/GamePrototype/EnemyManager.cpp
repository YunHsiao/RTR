#include "..\Engine\Utility.h"
#include "..\Engine\Direct3D.h"
#include "EnemyManager.h"

GPEnemyManager::GPEnemyManager()
:m_pChamber(NULL)
{
}

GPEnemyManager::~GPEnemyManager()
{
}

void GPEnemyManager::onInit(GPChamber* pChamber) {
	for (unsigned int i(0); i < ENEMY_CNT; i++)
		m_vEnemies[i].onInit(pChamber);
}

void GPEnemyManager::onTick(float fElapsedTime) {
	for (unsigned int i(0); i < ENEMY_CNT; i++)
		m_vEnemies[i].onTick(fElapsedTime);
}

void GPEnemyManager::onRender() {
	for (unsigned int i(0); i < ENEMY_CNT; i++)
		m_vEnemies[i].onRender();
}

void GPEnemyManager::Shoot(const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir, 
		const D3DXMATRIX& matView) {
	for (unsigned int i(0); i < ENEMY_CNT; i++) {
		if (m_vEnemies[i].Intersect(vPos, vDir)) {
			m_vEnemies[i].die(matView);
			return;
		}
	}
}