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
	m_info = " F1 ��ʾ/���ذ���\n\
			 F2 �߿�ģʽ\n\
			 F3 ʵ��ģʽ\n\
			 F11 ������ʾ\n\n\
			 WSAD    �ƶ�\n\
			 Shift       Ǳ��\n\
			 Ctrl         �¶�\n\
			 ������ ���\n\n\
			 = ���һ������\n\
			 - �߳�һ������\n\
			 0 ������ײ�е���ģʽ\n\
			 YUGHVB ������ײ����С������\n\
			 IOJKNM ������ײ����������\n\
			 12 �ָ���ײ�������ݵ��ʼ״̬";
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
