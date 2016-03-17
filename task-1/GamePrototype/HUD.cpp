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
	m_info = " F1 ��ʾ/���ذ���\n\
			 F2 �߿�ģʽ\n\
			 F3 ʵ��ģʽ\n\
			 F11 ������ʾ\n\n\
			 WSAD    �ƶ�\n\
			 Shift       Ǳ��\n\
			 Ctrl         �¶�\n\
			 ������ ���\n";
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
