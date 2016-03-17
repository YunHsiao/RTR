#include "..\Engine\Utility.h"
#include "..\Engine\Direct3D.h"
#include "PlayerCamera.h"

GPPlayerCamera::GPPlayerCamera() 
:m_fFollowDist(3.f)
,m_vOffset(0.f, 3.f, 0.f)
{
	
}

GPPlayerCamera::~GPPlayerCamera()
{

}

void GPPlayerCamera::onTick(float fElaspedTime) {
	KeyboardInput(fElaspedTime);
	MouseInput();
	UpdateAxis();
}

//void GPPlayerCamera::UpdatePosition(const D3DXVECTOR3& vPos, GPChamber* chamber) {
//	float fDist(m_fFollowDist);
//	while (fDist > -1.f) {
//		m_vecEyePosition = vPos - m_vecDirection * fDist 
//			+ m_vOffset - m_vecRight * (.5f * fDist + 1.f);
//		if (chamber->Validate(m_vecEyePosition, 1.f)) break;
//		fDist -= .1f;
//	}
//}

void GPPlayerCamera::ApplyCamera(const D3DXVECTOR3& vPos) {
	m_vecEyePosition = vPos - m_vecDirection * m_fFollowDist 
		+ m_vOffset - m_vecRight * (.5f * m_fFollowDist + 1.f);
	CCamera::ApplyCamera();
}

void GPPlayerCamera::RotateCameraDownRestrained(float fAngle) {
	if ((fAngle > 0.f && m_fPitch < D3DX_PI / 6) || 
		(fAngle < 0.f && m_fPitch > -D3DX_PI / 9)) 
		m_fPitch += fAngle;
}

void GPPlayerCamera::KeyboardInput(float fElaspedTime) {
	if (GetAsyncKeyState(VK_UP) & 0x8000) RotateCameraDownRestrained(-fElaspedTime);
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000) RotateCameraDownRestrained(fElaspedTime);
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) RotateCameraRight(-fElaspedTime);
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) RotateCameraRight(fElaspedTime);
}

void GPPlayerCamera::MouseInput() {
	POINT pCur;
	GetCursorPos(&pCur);
	RotateCameraRight((pCur.x - m_pCenter.x) / 100.f);
	RotateCameraDownRestrained((pCur.y - m_pCenter.y) / 100.f);
	SetCursorPos(m_pCenter.x, m_pCenter.y);
}