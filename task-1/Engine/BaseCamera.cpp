#include "Utility.h"
#include "Direct3D.h"
#include "Camera.h"

CCamera::CCamera():
m_fFovy(D3DX_PI/180*59),
m_fYaw(0.f),
m_fPitch(0.f),
m_vecEyePosition(0.f, 0.f, 0.f),
m_vecRight(1.f, 0.f, 0.f),
m_vecUp(0.f, 1.f, 0.f),
m_vecDirection(0.f, 0.f, 1.f)
{
	m_pCenter.x = ::GetSystemMetrics(SM_CXSCREEN) / 2;
	m_pCenter.y = ::GetSystemMetrics(SM_CYSCREEN) / 2;
}

CCamera::~CCamera()
{
}

void CCamera::onInit() {
	UpdateProjection();
}

void CCamera::MoveCamera(EMoveCameraType eType, float fDistance)
{
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

void CCamera::onTick(float fElapsedTime) {
	UpdateAxis();
	ApplyCamera();
}

void CCamera::UpdateAxis() {
	D3DXMatrixRotationYawPitchRoll(&m_matView, m_fYaw, m_fPitch, 0.f);
	D3DXMatrixTranspose(&m_matView, &m_matView);
	m_vecRight.x = m_matView._11; m_vecRight.y = m_matView._21; m_vecRight.z = m_matView._31;
	m_vecUp.x = m_matView._12; m_vecUp.y = m_matView._22; m_vecUp.z = m_matView._32;
	m_vecDirection.x = m_matView._13; m_vecDirection.y = m_matView._23; m_vecDirection.z = m_matView._33;
}

void CCamera::ApplyCamera() {
	m_matView._41 = -D3DXVec3Dot(&m_vecRight, &m_vecEyePosition);
	m_matView._42 = -D3DXVec3Dot(&m_vecUp, &m_vecEyePosition);
	m_matView._43 = -D3DXVec3Dot(&m_vecDirection, &m_vecEyePosition);
	CDirect3D::getInstance()->SetTransform(D3DTS_VIEW, &m_matView);
}

void CCamera::UpdateProjection() {
	float fAspect = (float)CDirect3D::getInstance()->GetWindowWidth() / 
		(float)CDirect3D::getInstance()->GetWindowHeight();
	if (fAspect < 16.f / 9.f) m_fFovy = D3DX_PI / 180 * 74;
	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFovy, fAspect, 1.f, 5000.f);
	CDirect3D::getInstance()->SetTransform(D3DTS_PROJECTION, &m_matProj);
}
