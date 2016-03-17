#include "..\Engine\Utility.h"
#include "..\Engine\Direct3D.h"
#include "Enemy.h"

GPEnemy::GPEnemy()
:m_pChamber(NULL)
{
}

GPEnemy::~GPEnemy()
{
}

void GPEnemy::onInit(GPChamber* pChamber) {
	m_pChamber = pChamber; 
	m_fX = pChamber->getX();
	m_fZ = pChamber->getZ();
	strcpy_s(m_strFilePath, MAX_CHAR, "res\\mesh\\arab.x");
	D3DXMatrixScaling(&m_matAdjust, .05f, .05f, .05f);
	ParseFile();
	AttachSkinMeshToFrame();
	AttachAnimationToFrame();
	m_pRootFrame->CloneFrame(&m_pCloneFrame);
	spawn();
}

void GPEnemy::die(const D3DXMATRIX& matView) {
	m_matPlayer._11 = matView._11; m_matPlayer._12 = matView._21; m_matPlayer._13 = matView._31;
	m_matPlayer._21 = 0.f; m_matPlayer._22 = 1.f; m_matPlayer._23 = 0.f;
	m_matPlayer._31 = matView._13; m_matPlayer._32 = matView._23; m_matPlayer._33 = matView._33;
	if (m_pCurAnimSet == m_mapAnimSet["Idle"]) {
		SetAction("Dead", false, .05f, .0075f);
		m_matPlayer._42 = -2.f;
	} else if (m_pCurAnimSet == m_mapAnimSet["CrouchIdle"]) {
		SetAction("Crouch_Dead", false, .05f, .0075f);
		m_matPlayer._42 = -2.5f;
	}
}

void GPEnemy::spawn() {
	unsigned int t(0); D3DXVECTOR3 vPos;
	do { rand_s(&t); vPos.x = (t * 2.f / UINT_MAX - 1.f) * m_fX;
		rand_s(&t); vPos.z = (t * 2.f / UINT_MAX - 1.f) * m_fZ;
	} while (!m_pChamber->Validate(vPos, 1.f)); 
	D3DXMatrixRotationY(&m_matPlayer, (float) t / UINT_MAX * D3DX_PI * 2);
	m_matPlayer._41 = vPos.x; m_matPlayer._42 = t % 2 ? -1.2f : -1.7f; m_matPlayer._43 = vPos.z; 
	SetAction(t % 2 ? "Idle" : "CrouchIdle", true, .05f, .0075f);
	UpdateBoundingBox();
}

bool GPEnemy::onTick(float fElapsedTime) {
	CSkeletonMesh::onTick(fElapsedTime);
	if (IsCurActionEnd()) spawn();
	return true;
}

void GPEnemy::onRender() {
	CSkeletonMesh::onRender();
}