#ifndef GPScene_H_
#define GPScene_H_

#include "Aim.h"
#include "Chamber.h"
#include "EnemyManager.h"
#include "HUD.h"
#include "Player.h"
#include "SkyDome.h"
#include "..\Engine\Skybox.h"

class GPScene
{
public:
	GPScene();
	~GPScene();

	void onInit();
	void onTick(float fElapsedTime);
	void onRender();
	void WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	GPAim m_aim;
	GPChamber m_chamber;
	GPEnemyManager m_enemies;
	GPHUD m_HUD;
	GPPlayer m_player;
	CSkyBox m_box;
	CSkyDome m_sky;

	bool m_bDome;
};

#endif
