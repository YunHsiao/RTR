#ifndef TDScene_H_
#define TDScene_H_

#include "Misc.h"
#include "Camera.h"
#include "Observer.h"
#include "Targeting.h"
#include "Grass.h"
#include "SimpleMirror.h"
#include "Disc.h"
#include "Water.h"
#include "..\Engine\BaseCamera.h"
#include "..\Engine\SkeletonMesh.h"
#include "..\Engine\Shadow.h"
#include "..\Engine\Skybox.h"
#include "..\Engine\SkyDome.h"
#include "..\Engine\Terrain.h"

class TDScene
{
public:
	TDScene();
	~TDScene();

	void onInit();
	void onTick(float fElapsedTime);
	void onRender();
	void WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void ToggleObserve();
	void DrawText();
private:
	TDCamera m_camera; 
	TDObserver m_observer;
	CCamera *m_curCam;
	TDMisc m_misc;
	TDTargeting m_aim;
	TDGrass m_grass;
	TDSimpleMirror m_mirror;
	TDDisc m_disc;
	CShadow m_shadow;
	CSkeletonMesh m_skeleton, m_model;
	CSkyBox m_box;
	CSkyDome m_sky;
	CTerrain m_terrain;
	CWater m_water;
	std::string m_info;
	char m_fps[16];
	bool m_bDome, m_bObserve, m_bWater, m_bInfo, m_bShadow, m_bAim;
	int m_iParallax;
};

#endif
