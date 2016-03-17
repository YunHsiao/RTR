#include "..\Engine\Utility.h"
#include "..\Engine\Direct3D.h"
#include "..\Engine\Window.h"
#include "HUD.h"

GPHUD::GPHUD()
:m_bInfo(false)
{
}

GPHUD::~GPHUD()
{
}

void GPHUD::onInit() {
	m_info = " F1 显示/隐藏帮助\n\
			 F2 线框模式\n\
			 F3 实体模式\n\
			 F11 技术演示\n\n\
			 WSAD    移动\n\
			 Shift       潜行\n\
			 Ctrl         下蹲\n\
			 鼠标左键 射击\n";
}

void GPHUD::onTick(float fElapsedTime) {
	static float fTime(0.f);
	static int iFrameCount(0);
	fTime += fElapsedTime; iFrameCount++;
	if (fTime > .5f) {
		sprintf_s(m_fps, 16, "FPS: %.1f", iFrameCount / fTime);
		fTime = 0.f; iFrameCount = 0;
	}
}

void GPHUD::onRender() {
	static const char *info = m_info.c_str();
	static long lWidth(CWindow::getInstance()->getWinWidth());
	static long lHeight(CWindow::getInstance()->getWinHeight());
	static RECT rect = { lWidth - 70, 0, lWidth, lHeight };
	CDirect3D::getInstance()->DrawText(info, NULL, 0xffffff00, m_bInfo?-1:17);
	CDirect3D::getInstance()->DrawText(m_fps, &rect, 0xffffff00, -1);
}
