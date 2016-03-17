#include "..\Engine\Utility.h"
#include "..\Engine\Direct3D.h"
#include "..\Engine\StaticMesh.h"
#include "Player.h"

GPPlayer::GPPlayer() 
:m_vPos(0.f, -1.2f, 0.f)
,m_fDistance(10.f)
,m_bSneaking(false)
,m_bMoving(false)
,m_bCrouching(false)
,m_pTransform(NULL)
,m_pChamber(NULL)
{

}

GPPlayer::~GPPlayer()
{

}

void GPPlayer::onInit(float fX, float fZ, GPChamber* pChamber) {
	m_pChamber = pChamber;
	m_vPos.x = fX; m_vPos.z = fZ;
	m_camera.onInit();
	m_model.onInit("res\\mesh\\arab.x", m_vPos, .05f);
	CStaticMesh* gun = new CStaticMesh;
	gun->onInit("res\\mesh\\ak47.x", // Parameters adapted manually
		D3DXVECTOR3(11.36f, -.51f, -.54f), 10.28f, -.434f, 3.12f, .12f);
	m_model.AttachMesh(gun, "Bip01_R_Hand");
	m_pTransform = m_model.GetTransform();
}

void GPPlayer::onTick(float fElapsedTime) {
	m_camera.onTick(fElapsedTime);
	KeyboardInput(fElapsedTime);
	m_camera.ApplyCamera(m_vPos);
	m_model.onTick(fElapsedTime);
}

void GPPlayer::KeyboardInput(float fElapsedTime) {
	// Position Update
	m_bSneaking = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
	if (m_bSneaking) m_fDistance = 5.f;
	else m_fDistance = 10.f;
	m_bCrouching = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
	
	m_vPrev = m_vPos; m_bMoving = false;
	float fSin(sin(m_camera.GetYaw())), fCos(cos(m_camera.GetYaw()));
	if (GetAsyncKeyState('D') & 0x8000) Move(fElapsedTime*m_fDistance, false, fSin, fCos);
	else if (GetAsyncKeyState('A') & 0x8000) Move(-fElapsedTime*m_fDistance, false, fSin, fCos);
	if (GetAsyncKeyState('W') & 0x8000) Move(fElapsedTime*m_fDistance, true, fSin, fCos);
	else if (GetAsyncKeyState('S') & 0x8000) Move(-fElapsedTime*m_fDistance, true, fSin, fCos);
	m_bCrouching = m_bCrouching && !m_bMoving;
	m_camera.Crouch(m_bCrouching);
	if (m_bCrouching) m_vPos.y = -1.7f;
	else m_vPos.y = -1.2f;
	D3DXMatrixRotationY(m_pTransform, m_camera.GetYaw() - D3DX_PI);
	m_pTransform->_41 = m_vPos.x; m_pTransform->_42 = m_vPos.y; m_pTransform->_43 = m_vPos.z;

	// Animation Update
	std::string action;
	float fActionSpeed(.05f);
	if (m_bMoving) {
		action = "Run";
		if (m_bSneaking) fActionSpeed = .03f;
	} else if (m_bCrouching) action = "CrouchIdle";
	else action = "Idle";
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) action = "Shoot";
	m_model.SetAction(action.c_str(), true, fActionSpeed, .0075f);
}

void GPPlayer::Move(float fDistance, bool bZ, float fSin, float fCos) {
	m_bMoving = true;
	if (bZ) {
		m_vPos.x += fDistance*fSin;
		if (!m_pChamber->Validate(m_vPos, 1.f)) m_vPos.x = m_vPrev.x;
		m_vPos.z += fDistance*fCos;
		if (!m_pChamber->Validate(m_vPos, 1.f)) m_vPos.z = m_vPrev.z;
	} else {
		m_vPos.x += fDistance*fCos;
		if (!m_pChamber->Validate(m_vPos, 1.f)) m_vPos.x = m_vPrev.x;
		m_vPos.z -= fDistance*fSin;
		if (!m_pChamber->Validate(m_vPos, 1.f)) m_vPos.z = m_vPrev.z;
	}
}

void GPPlayer::onRender() {
	m_model.onRender();
}

void GPPlayer::ParameterAdaptation() {
	static float sca(11.f);
	static D3DXVECTOR3 rot(-.1f * D3DX_PI, D3DX_PI, 0.f);
	static D3DXVECTOR3 tra(10.f, 0.f, 0.f);
	static CStaticMesh *gun(m_model.GetAttachedMesh(0));
	if (GetAsyncKeyState(VK_NUMPAD1) & 0x8000) rot.x += .01f;
	if (GetAsyncKeyState(VK_NUMPAD2) & 0x8000) rot.x -= .01f;
	if (GetAsyncKeyState(VK_NUMPAD4) & 0x8000) rot.y += .01f;
	if (GetAsyncKeyState(VK_NUMPAD5) & 0x8000) rot.y -= .01f;
	if (GetAsyncKeyState(VK_NUMPAD7) & 0x8000) rot.z += .01f;
	if (GetAsyncKeyState(VK_NUMPAD8) & 0x8000) rot.z -= .01f;
	if (GetAsyncKeyState('N') & 0x8000) tra.x += .01f;
	if (GetAsyncKeyState('M') & 0x8000) tra.x -= .01f;
	if (GetAsyncKeyState('J') & 0x8000) tra.y += .01f;
	if (GetAsyncKeyState('K') & 0x8000) tra.y -= .01f;
	if (GetAsyncKeyState('I') & 0x8000) tra.z += .01f;
	if (GetAsyncKeyState('O') & 0x8000) tra.z -= .01f;
	if (GetAsyncKeyState('1') & 0x8000) sca += .01f;
	if (GetAsyncKeyState('2') & 0x8000) sca -= .01f;
	D3DXMATRIX mSca, mRot, mTra;
	D3DXMatrixScaling(&mSca, sca, sca, sca);
	D3DXMatrixRotationYawPitchRoll(&mRot, rot.x, rot.y, rot.z);
	D3DXMatrixTranslation(&mTra, tra.x, tra.y, tra.z);
	gun->SetWorldMatrix(mSca * mRot * mTra);
}