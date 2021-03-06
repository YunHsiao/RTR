#include "..\Engine\Utility.h"
#include "..\Engine\Direct3D.h"
#include "..\Engine\Window.h"
#include "GPScene.h"

GPScene::GPScene()
:m_bDome(true)
{
}

GPScene::~GPScene()
{
}

void GPScene::onInit() {
	m_aim.onInit();
	m_box.onInit(std::string("res\\s"), 125.f);
	m_chamber.onInit(50.f);
	m_enemies.onInit(&m_chamber);
	m_HUD.onInit();
	m_player.onInit(0.f, 0.f, &m_chamber);
	m_sky.onInit(TEXT("res\\sky.png"), 125.f);
	m_sky.ToggleAlpha();
}

void GPScene::onTick(float fElapsedTime) {
	m_enemies.onTick(fElapsedTime);
	m_player.onTick(fElapsedTime);
	m_HUD.onTick(fElapsedTime);
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) m_enemies.Shoot
		(m_player.GetCameraPosition(), m_player.GetCameraDirection(),
		m_player.GetPosition());
	if (m_bDome) m_sky.onTick(fElapsedTime);
}

void GPScene::onRender() {
	m_aim.onRender();
	m_box.onRender();
	m_chamber.onRender();
	m_enemies.onRender();
	m_player.onRender();
	if (m_bDome) m_sky.onRender();
	m_HUD.onRender();
}

void GPScene::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_KEYUP:
		if (wParam == VK_F1) m_HUD.ToggleInfo();
		else if (wParam == VK_F2) CDirect3D::getInstance()->SetWireframeRenderMode();
		else if (wParam == VK_F3) CDirect3D::getInstance()->SetSolidRenderMode();
		else if (wParam == VK_OEM_PLUS) { m_enemies.Add(); m_HUD.Add(); }
		else if (wParam == VK_OEM_MINUS) { if (m_HUD.Count()) { m_enemies.Kick(); m_HUD.Subtract(); } }
		else if (wParam == '9') m_bDome = !m_bDome;
		else if (wParam == '0') m_enemies.ToggleDebug();
		break;
	case WM_RBUTTONDOWN:
		break;
	case WM_RBUTTONUP:
		break;
	case WM_MOUSEWHEEL:
		m_player.ZoomIn(GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	}
}