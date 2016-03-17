#include "..\Engine\Utility.h"
#include "SimpleMirror.h"

TDSimpleMirror::TDSimpleMirror()
:m_iDir(0)
{
}

void TDSimpleMirror::onTick(float fElapsedTime) {
	int bL(GetAsyncKeyState(VK_OEM_4) & 0x8000), bR(GetAsyncKeyState(VK_OEM_6) & 0x8000);
	if (!bL && !bR) return;
	if (bL) m_fAngle[m_iDir] -= fElapsedTime;
	if (bR) m_fAngle[m_iDir] += fElapsedTime;
	updateTransform();
}
