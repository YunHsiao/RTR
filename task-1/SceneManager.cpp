#include "Engine\Utility.h"
#include "Engine\Window.h"
#include "Engine\Direct3D.h"
#include "SceneManager.h"

#define UNLIMITED_FPS

CSceneManager CSceneManager::s_director;

CSceneManager::CSceneManager()
:m_bPrototype(false)
{
}

CSceneManager::~CSceneManager()
{
}

void CSceneManager::Go()
{
	onInit();
	MSG msg;
	DWORD dwCurrent(0), dwDuration(0);
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			static DWORD dwLast = timeGetTime();
			dwCurrent = timeGetTime();
			dwDuration = dwCurrent - dwLast;
#ifndef UNLIMITED_FPS
			if (dwDuration < 16) {
				Sleep( 16 - dwDuration );
				dwCurrent = timeGetTime();
				dwDuration = dwCurrent - dwLast;
			}
#endif
			onTick( dwDuration * 1e-3f );
			onRender();
			dwLast = dwCurrent;
		}
	}
}

void CSceneManager::onInit()
{ 
	CWindow::getInstance()->setWinSize(1280, 720);//设置窗口的大小
	CWindow::getInstance()->onInit();
	CDirect3D::getInstance()->onInit();
	m_scene.onInit();
	m_prototype.onInit();
	CWindow::getInstance()->showWindow();
}

void CSceneManager::onTick(float fElapsedTime) {
	if (m_bPrototype) m_prototype.onTick(fElapsedTime);
	else m_scene.onTick(fElapsedTime);
}

void CSceneManager::onRender()
{
	CDirect3D::getInstance()->PreRender(D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL);
	if (m_bPrototype) m_prototype.onRender();
	else m_scene.onRender();
	CDirect3D::getInstance()->PostRender();
}

void CSceneManager::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KEYUP && wParam == VK_F11) {
		m_bPrototype = !m_bPrototype;
		CWindow::getInstance()->HideCursor(m_bPrototype);
	}
	if (m_bPrototype) m_prototype.WndProc(hWnd, uMsg, wParam, lParam);
	else m_scene.WndProc(hWnd, uMsg, wParam, lParam);
}
