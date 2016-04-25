#include "..\Engine\Utility.h"
#include "..\Engine\Direct3D.h"
#include "Enemy.h"

GPEnemy::GPEnemy()
:m_pChamber(NULL)
,m_vMinOffset(0.f, 0.f, 0.f)
,m_vMaxOffset(0.f, 0.f, 0.f)
{
}

GPEnemy::~GPEnemy()
{
}

void GPEnemy::onInit(GPChamber* pChamber) {
	m_pChamber = pChamber; 
	m_fX = pChamber->getX();
	m_fZ = pChamber->getZ();
	strcpy_s(m_strFilePath, MAX_CHAR, "res\\mesh\\gign.x");
	D3DXMatrixScaling(&m_matAdjust, .05f, .05f, .05f);
	do_Init();
	spawn();
}

void GPEnemy::die(const D3DXVECTOR3& vPlayerPos) {
	D3DXVECTOR3 vPos(m_matPlayer._41, m_matPlayer._42, m_matPlayer._43),
		vDir(vPlayerPos - vPos);
	float fAngle(atan2(vDir.x, vDir.z) - 1.5f);
	D3DXMatrixRotationY(&m_matPlayer, fAngle);
	m_matPlayer._41 = vPos.x; m_matPlayer._42 = .86f; m_matPlayer._43 = vPos.z; 
	SetAction("Headshot", false, 8.f, 1.f);
}

void GPEnemy::spawn() {
	unsigned int t(0); D3DXVECTOR3 vPos;
	if (m_fX) { 
		do { rand_s(&t); vPos.x = (t * 2.f / UINT_MAX - 1.f) * m_fX;
			rand_s(&t); vPos.z = (t * 2.f / UINT_MAX - 1.f) * m_fZ;
		} while (!m_pChamber->Validate(vPos, 1.f));  
	}
	D3DXMatrixRotationY(&m_matPlayer, (float) t / UINT_MAX * D3DX_PI * 10);
	m_matPlayer._41 = vPos.x; m_matPlayer._43 = vPos.z; m_bCrouching = t % 2 != 0;
	 // Parameters Adapted Manually
	if (m_bCrouching) {
		m_matPlayer._42 = .09f; 
		SetAction("CrouchIdle", true, 8.f, 1.f);
		m_vMinOffset = D3DXVECTOR3(0.f, 0.f, 0.f);
		m_vMaxOffset = D3DXVECTOR3(-9.f, 0.f, 0.f);
	} else {
		m_matPlayer._42 = .9f; 
		SetAction("Idle", true, 8.f, 1.f);
		m_vMinOffset = D3DXVECTOR3(4.1f, 0.f, 9.6f);
		m_vMaxOffset = D3DXVECTOR3(-11.5f, 0.f, -2.75f);
	}
}

bool GPEnemy::onTick(float fElapsedTime) {
	if (m_bDebug) ParameterAdaptaion();
	CSkeletonMesh::onTick(fElapsedTime);
	if (IsCurActionEnd()) spawn();
	UpdateBoundingBox(&m_vMinOffset, &m_vMaxOffset);
	return true;
}

void GPEnemy::onRender() {
	CSkeletonMesh::onRender();
}

void GPEnemy::ParameterAdaptaion() {
	if (GetAsyncKeyState('Y')) m_vMinOffset.x -= .05f;
	if (GetAsyncKeyState('U')) m_vMinOffset.x += .05f;
	if (GetAsyncKeyState('G')) m_vMinOffset.y -= .05f;
	if (GetAsyncKeyState('H')) m_vMinOffset.y += .05f;
	if (GetAsyncKeyState('V')) m_vMinOffset.z -= .05f;
	if (GetAsyncKeyState('B')) m_vMinOffset.z += .05f;
	if (GetAsyncKeyState('I')) m_vMaxOffset.x -= .05f;
	if (GetAsyncKeyState('O')) m_vMaxOffset.x += .05f;
	if (GetAsyncKeyState('J')) m_vMaxOffset.y -= .05f;
	if (GetAsyncKeyState('K')) m_vMaxOffset.y += .05f;
	if (GetAsyncKeyState('N')) m_vMaxOffset.z -= .05f;
	if (GetAsyncKeyState('M')) m_vMaxOffset.z += .05f;
	if (GetAsyncKeyState('1')) m_vMinOffset = D3DXVECTOR3(0.f, 0.f, 0.f);
	if (GetAsyncKeyState('2')) m_vMaxOffset = D3DXVECTOR3(0.f, 0.f, 0.f);
}