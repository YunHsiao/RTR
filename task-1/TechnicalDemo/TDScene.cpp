#include "..\Engine\Utility.h"
#include "..\Engine\Direct3D.h"
#include "..\Engine\Window.h"
#include "TDScene.h"

TDScene::TDScene()
:m_bDome(false)
,m_bWater(false)
,m_bInfo(false)
,m_bObserve(false)
,m_bShadow(false)
,m_bAim(false)
,m_bFog(true)
,m_iParallax(0)
,m_curCam(&m_camera)
{	
	m_info = " F1 显示/隐藏帮助\n\
			 F2 线框模式\n\
			 F3 实体模式\n\
			 F11 切换至游戏原型场景\n\n\
			 WSADQE移动摄像机\n\
			 方向键或鼠标右键拖动旋转摄像机\n\
			 鼠标滚轮缩放远近\n\
			 按下Shift键加快移动速度\n\n\
			 1 第三人称摄像机\n\
			 2 第一人称摄像机\n\
			 3 开启/关闭地形LOD\n\
			 4 开启/关闭LOD观察模式(IKJL 旋转平截头体)\n\
			 5 开启/关闭角色重力\n\
			 6 开启/关闭角色阴影\n\
			 7 开启/关闭水面\n\
			 8 开启/关闭天空穹庐\n\
			 9 开启/关闭天空穹庐透明\n\
			 0 开启/关闭天空穹庐坐标信息(初始化较慢)\n\
			 F 开启/关闭雾效\n\
			 = 切换镜子旋转方向\n\
			 [] 旋转镜子\n\
			 - 开启/关闭视差模型\n\
			 \\ 开启/关闭视差模型阴影\n\
			 <> 切换视差模型贴图&法线\n\
			 / 切换公告板计算模式";

}

TDScene::~TDScene()
{
}

void TDScene::onInit() {
	m_aim.onInit();
	m_camera.onInit();
	m_observer.onInit();
	m_misc.onInit();
	m_mirror.onInit(TEXT("res\\mir.jpg"), 0.964f, 0.f, 0.f);
	m_grass.onInit(TEXT("res\\grass.png"));
	m_model.onInit("res\\mesh\\hero.x", D3DXVECTOR3(-2.f, .8f, 0.f), .042f);
	m_model.SetAction("stand", true, 5.f, .5f);
	m_skeleton.onInit("res\\mesh\\heroine.x", D3DXVECTOR3(2.f, .8f, 0.f), .02f);
	m_skeleton.SetAction("stand", true, 5.f, .5f);
	m_shadow.onInit(m_sky.getSunPosition());
	m_shadow.add(m_skeleton.GetMesh(0), m_skeleton.GetWorldTransform());
	m_shadow.add(m_model.GetMesh(0), m_model.GetWorldTransform());
	m_sky.onInit(TEXT("res\\sky.png"), 126.f);
	m_box.onInit(std::string("res\\s"), 125.f);
	m_disc.onInit(TEXT("res\\mesh\\disc.x"));
	m_flock.onInit("res\\mesh\\flying_sparrow.x", D3DXVECTOR3(-75.f, 30.f, -75.f), D3DXVECTOR3(75.f, 90.f, 75.f));
	m_terrain.onInit(TEXT("res\\tex.jpg"), TEXT("res\\hei.bmp"));
	m_water.onInit("res\\water\\Water", ".png", 45, m_terrain.GetRadiusX(), m_terrain.GetRadiusZ());
	m_welcome.onInit(D3DXVECTOR3(0.f, 4.5f, 0.f), TEXT("res\\welcome.png"));
	CDirect3D::getInstance()->SetExponentialFog(.01f, 0xffaaaaaa);
	CDirect3D::getInstance()->EnableFog(m_bFog);
}

void TDScene::onTick(float fElapsedTime) {
	static float fTime(0.f);
	static int iFrameCount(0);
	fTime += fElapsedTime; iFrameCount++;
	m_observer.onTick(fElapsedTime);
	if (m_camera.onTick(fElapsedTime, &m_terrain)) m_grass.Add(
		m_camera.GetCameraPosition(), m_camera.GetCameraDirection(), &m_terrain);
	m_misc.onTick(fElapsedTime);
	m_flock.onTick(fElapsedTime);
	m_skeleton.onTick(fElapsedTime);
	m_mirror.onTick(fElapsedTime);
	m_model.onTick(fElapsedTime);
	if (m_iParallax == 2) m_disc.onTick(fElapsedTime);
	m_terrain.onTick(fElapsedTime, m_curCam->GetViewMatrix(), 
		m_curCam->GetProjMatrix(), m_curCam->GetCameraPosition());
	if (m_bShadow) m_shadow.onTick(fElapsedTime);
	if (m_bDome) m_sky.onTick(fElapsedTime);
	if (m_bWater) m_water.onTick(fElapsedTime);
	if (fTime > .5f) {
		sprintf_s(m_fps, 16, "FPS: %.1f", iFrameCount / fTime);
		fTime = 0.f; iFrameCount = 0;
	}
}

void TDScene::DrawText() {
	static const char *info = m_info.c_str();
	static long lWidth(CWindow::getInstance()->getWinWidth());
	static long lHeight(CWindow::getInstance()->getWinHeight());
	static RECT rect = { lWidth - 70, 0, lWidth, lHeight };
	CDirect3D::getInstance()->DrawText(info, NULL, TARGETING_COLOR, m_bInfo?-1:17);
	CDirect3D::getInstance()->DrawText(m_fps, &rect, TARGETING_COLOR, -1);
}

void TDScene::onRender() {
	m_misc.onRender();
	m_box.onRender();
	m_skeleton.onRender();
	m_model.onRender();
	if (m_bAim) m_aim.onRender();
	if (m_iParallax) m_disc.onRender(m_curCam->GetViewMatrix(), 
		m_curCam->GetProjMatrix(), m_curCam->GetCameraPosition());
	if (m_bDome) m_sky.onRender();
	if (m_bWater) m_water.onRender();
	m_welcome.onRender(m_camera.GetViewMatrix(), m_camera.GetCameraPosition());
	m_grass.onRender(m_camera.GetViewMatrix(), m_camera.GetCameraPosition());
	m_flock.onRender();
	m_terrain.onRender();
	if (m_bShadow) m_shadow.onRender();
	m_mirror.onRender();
	m_skeleton.onRender(m_mirror.GetMirror());
	m_model.onRender(m_mirror.GetMirror());
	DrawText();
}

void TDScene::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_KEYUP:
		if (wParam == '1') { m_camera.EnableFollowing(&m_skeleton); m_bAim = true; }
		else if (wParam == '2')  { m_camera.DisableFollowing(); m_bAim = false; }
		else if (wParam == '3') m_terrain.ToggleLOD();
		else if (wParam == '4') ToggleObserve();
		else if (wParam == '5') m_camera.ToggleGravity();
		else if (wParam == '6') m_bShadow = !m_bShadow;
		else if (wParam == '7') m_bWater = !m_bWater;
		else if (wParam == '8') m_bDome = !m_bDome;
		else if (wParam == '9') m_sky.ToggleAlpha();
		else if (wParam == '0') m_sky.ToggleDebug();
		else if (wParam == VK_OEM_PLUS) m_mirror.changeDirection();
		else if (wParam == VK_OEM_COMMA) m_disc.changeTexture();
		else if (wParam == VK_OEM_PERIOD) m_disc.changeNormal();
		else if (wParam == VK_OEM_MINUS) m_iParallax = (m_iParallax+1)%3;
		else if (wParam == VK_OEM_5) m_disc.ToggleShadows();
		else if (wParam == VK_OEM_2) m_grass.ToggleAccurateCalculation();
		else if (wParam == 'F') { m_bFog = !m_bFog; CDirect3D::getInstance()->EnableFog(m_bFog); }
		else if (wParam == VK_F1) m_bInfo = !m_bInfo;
		else if (wParam == VK_F2) CDirect3D::getInstance()->SetWireframeRenderMode();
		else if (wParam == VK_F3) CDirect3D::getInstance()->SetSolidRenderMode();
		break;
	case WM_RBUTTONDOWN:
		POINT p; GetCursorPos(&p);
		m_camera.BeginDragging(p);
		break;
	case WM_RBUTTONUP:
		m_camera.EndDragging();
		break;
	case WM_MOUSEWHEEL:
		m_camera.ZoomIn(GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	}
}

void TDScene::ToggleObserve() {
	m_bObserve = !m_bObserve; 
	m_terrain.ToggleObserver();
	m_observer.SetPitch(m_camera.GetPitch());
	m_observer.SetYaw(m_camera.GetYaw());
	m_observer.SetCameraPosition(m_camera.GetCameraPosition());
	if (m_bObserve) m_curCam = &m_observer;
	else m_curCam = &m_camera;
}