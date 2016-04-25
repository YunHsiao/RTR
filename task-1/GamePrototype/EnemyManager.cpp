#include "..\Engine\Utility.h"
#include "..\Engine\Direct3D.h"
#include "EnemyManager.h"

GPEnemyManager::GPEnemyManager()
:m_pChamber(NULL)
{
}

GPEnemyManager::~GPEnemyManager()
{
	for (unsigned int i(0); i < m_vEnemies.size(); i++)
		Safe_Delete(m_vEnemies[i]);
}

void GPEnemyManager::onInit(GPChamber* pChamber) {
	m_pChamber = pChamber;
	for (unsigned int i(0); i < 10; i++) Add();
}

void GPEnemyManager::onTick(float fElapsedTime) {
	for (unsigned int i(0); i < m_vEnemies.size(); i++)
		m_vEnemies[i]->onTick(fElapsedTime);
}

void GPEnemyManager::onRender() {
	for (unsigned int i(0); i < m_vEnemies.size(); i++)
		m_vEnemies[i]->onRender();
}

void GPEnemyManager::Shoot(const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir, const D3DXVECTOR3& vPlayerPos) {
	for (unsigned int i(0); i < m_vEnemies.size(); i++) {
		if (m_vEnemies[i]->Intersect(vPos, vDir)) {
			m_vEnemies[i]->die(vPlayerPos);
			return;
		}
	}
}