#include "..\Engine\Utility.h"
#include "..\Engine\Direct3D.h"
#include "..\Engine\Window.h"
#include "Camera.h"

TDCamera::TDCamera():
m_fDistance(3.f),
m_fFollowDist(5.f),
m_vOffset(0.f, 3.f, 0.f),
m_pPlayer(NULL),
m_bFollowing(false),
m_bGravity(true)
{
	m_vecEyePosition = D3DXVECTOR3(0.f, 2.5f, -3.7f);
	m_pCenter.x = ::GetSystemMetrics(SM_CXSCREEN) / 2;
	m_pCenter.y = ::GetSystemMetrics(SM_CYSCREEN) / 2;
}

TDCamera::~TDCamera()
{
}

void TDCamera::MoveCamera(EMoveCameraType eType, float fDistance)
{
	m_bMoving = true;
	if (m_bFollowing) {
		switch (eType) {
		case EMCT_Forward:
			m_pvPos.x += fDistance*sin(m_fYaw);
			m_pvPos.z += fDistance*cos(m_fYaw);
			break;
		case EMCT_Horizontal:
			m_pvPos.x += fDistance*cos(m_fYaw);
			m_pvPos.z -= fDistance*sin(m_fYaw);
			break;
		case EMCT_Vertical:
			m_pvPos.y += fDistance;
			break;
		}
	} else {
		switch (eType) {
		case EMCT_Forward:
			m_vecEyePosition += m_vecDirection * fDistance;
			break;
		case EMCT_Horizontal:
			m_vecEyePosition += m_vecRight * fDistance;
			break;
		case EMCT_Vertical:
			m_vecEyePosition.y += fDistance;
			break;
		}
	}
}

void TDCamera::KeyboardInput(float fElapsedTime) {
	m_bRunning = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0; m_bMoving = false;
	if (m_bRunning) m_fDistance = 30.f;
	else m_fDistance = 3.f;
	if (GetAsyncKeyState('E') & 0x8000) MoveCamera(EMCT_Vertical, fElapsedTime*m_fDistance);
	else if (GetAsyncKeyState('Q') & 0x8000) MoveCamera(EMCT_Vertical, -fElapsedTime*m_fDistance);
	if (GetAsyncKeyState('D') & 0x8000) MoveCamera(EMCT_Horizontal, fElapsedTime*m_fDistance);
	else if (GetAsyncKeyState('A') & 0x8000) MoveCamera(EMCT_Horizontal, -fElapsedTime*m_fDistance);
	if (GetAsyncKeyState('W') & 0x8000) MoveCamera(EMCT_Forward, fElapsedTime*m_fDistance);
	else if (GetAsyncKeyState('S') & 0x8000) MoveCamera(EMCT_Forward, -fElapsedTime*m_fDistance);
	if (GetAsyncKeyState(VK_UP) & 0x8000) RotateCameraDown(-fElapsedTime);
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000) RotateCameraDown(fElapsedTime);
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) RotateCameraRight(-fElapsedTime);
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) RotateCameraRight(fElapsedTime);
}

void TDCamera::MouseInput() {
	POINT pCur;
	GetCursorPos(&pCur);
	if (m_bFollowing) {
		m_fYaw += (pCur.x - m_pCenter.x) / 100.f;
		m_fPitch += (pCur.y - m_pCenter.y) / 100.f;
		SetCursorPos(m_pCenter.x, m_pCenter.y);
	} else {
		m_fYaw += (pCur.x - m_pBeg.x) / 100.f;
		m_fPitch += (pCur.y - m_pBeg.y) / 100.f;
		m_pBeg = pCur;
	}
}

bool TDCamera::onTick(float fElapsedTime, CTerrain* terrain)
{
	static float fTime(0.f);
	KeyboardInput(fElapsedTime);
	if (m_bDragging || m_bFollowing) MouseInput();
	UpdateAxis();
	if (m_bFollowing) {
		if (m_bGravity) m_pvPos.y = terrain->GetHeight(m_pvPos.x, m_pvPos.z); 
		m_vecEyePosition = m_pvPos - m_vecDirection * m_fFollowDist 
			+ m_vOffset - m_vecRight * (.5f * m_fFollowDist + .5f);
		D3DXMatrixRotationY(m_pTransform, m_fYaw - D3DX_PI);
		m_pTransform->_41 = m_pvPos.x; m_pTransform->_42 = m_pvPos.y; m_pTransform->_43 = m_pvPos.z;
	}
	ApplyCamera();
	if (m_bFollowing) {
		std::string action;
		if (m_bMoving) {
			if (m_bRunning) action = "run";
			else action = "walk";
		} else action = "stand";
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
			action = "attack";
			fTime += fElapsedTime;
			// Animation Specific Parameters
			if (fTime > 1.2f) {
				fTime = 0.f;
				return true;
			}
		} else fTime = .6f;
		m_pPlayer->SetAction(action.c_str(), true, 5.f, .5f);
	}
	return false;
}

void TDCamera::EnableFollowing(CSkeletonMesh* pPlayer) { 
	if (!pPlayer) return; 
	m_bFollowing = true; 
	m_pPlayer = pPlayer; 
	m_pTransform = pPlayer->GetTransform(); 
	m_pvPos.x = m_pTransform->_41; m_pvPos.y = m_pTransform->_42; m_pvPos.z = m_pTransform->_43; 
	CWindow::getInstance()->HideCursor(true);
	SetCursorPos(m_pCenter.x, m_pCenter.y);
}

void TDCamera::DisableFollowing() { 
	m_bFollowing = false; 
	m_pPlayer = NULL; 
	m_pTransform = NULL; 
	CWindow::getInstance()->HideCursor(false);
}
