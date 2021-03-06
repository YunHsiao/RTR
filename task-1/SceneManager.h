#ifndef CSceneManager_H_
#define CSceneManager_H_

#include "TechnicalDemo\TDScene.h"
#include "GamePrototype\GPScene.h"

class CSceneManager
{
public:
	static CSceneManager* getInstance() { return &s_director; }
	CSceneManager();
	~CSceneManager();

	void Go();

	void onInit();
	void onTick(float fElapsedTime);
	void onRender();
	void WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	static CSceneManager s_director;
	TDScene m_scene;
	GPScene m_prototype;
	bool m_bPrototype;
};
#endif
