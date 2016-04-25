#include "..\Engine\Utility.h"
#include "..\Engine\Direct3D.h"
#include "..\Engine\Window.h"
#include "HUD.h"
#include "Aim.h"

GPHUD::GPHUD()
:m_bInfo(false)
,m_iCount(10)
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
			 鼠标左键 射击\n\n\
			 = 添加一个敌人\n\
			 - 踢出一个敌人\n\
			 0 开启碰撞盒调试模式\n\
			 YUGHVB 调整碰撞盒最小点坐标\n\
			 IOJKNM 调整碰撞盒最大点坐标\n\
			 12 恢复碰撞盒两操纵点初始状态";
}

void GPHUD::onTick(float fElapsedTime) {
	static float fTime(0.f);
	static int iFrameCount(0);
	static float fFPS(60.f);
	fTime += fElapsedTime; iFrameCount++;
	if (fTime > .5f) {
		fFPS = iFrameCount / fTime;
		fTime = 0.f; iFrameCount = 0;
	}
	sprintf_s(m_fps, 32, "FPS: %.1f\nCount: %d", fFPS, m_iCount);
}

void GPHUD::onRender() {
	static const char *info = m_info.c_str();
	static long lWidth(CWindow::getInstance()->getWinWidth());
	static long lHeight(CWindow::getInstance()->getWinHeight());
	static RECT rect = { lWidth - 70, 0, lWidth, lHeight };
	CDirect3D::getInstance()->DrawText(info, NULL, AIM_COLOR, m_bInfo?-1:17);
	CDirect3D::getInstance()->DrawText(m_fps, &rect, AIM_COLOR, -1);
}
