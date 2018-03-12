#include "Utility.h"
#include "Flock.h"
#include "Direct3D.h"

#define INITIAL_BOIDS 100

CFlock::CFlock()
{
	D3DXMatrixScaling(&m_matAdjust, .03f, .03f, .03f);
}

CFlock::~CFlock()
{
	for (unsigned i = 0; i < m_boids.size(); i++)
		Safe_Delete(m_boids[i]);
	m_boids.clear();
}

void CFlock::onInit(const char* strFilePath, const D3DXVECTOR3& vLowerBound, 
					const D3DXVECTOR3& vUpperBound)
{
	m_vInitPos = (vLowerBound + vUpperBound) / 2.f;
	m_vLowerBound = vLowerBound;
	m_vUpperBound = vUpperBound;
	for (unsigned i = 0; i < INITIAL_BOIDS; i++)
		m_boids.push_back(new CBoid(m_vInitPos, m_vLowerBound, m_vUpperBound));
	m_mesh.onInit(strFilePath, m_vInitPos, .03f);
	m_mesh.SetAction("fly", true, 2.f);
}

void CFlock::onTick(float fElapsedTime)
{	
	static bool bFreeze(false);
	if (GetAsyncKeyState(VK_NUMPAD5) & 0x8000)
		for (unsigned i = 0; i < m_boids.size(); i++)
			m_boids[i]->setPosition(m_vInitPos);
	if (GetAsyncKeyState(VK_NUMPAD1) & 0x8000) bFreeze = true;
	if (GetAsyncKeyState(VK_NUMPAD2) & 0x8000) bFreeze = false;
	if (bFreeze) return;

	for (unsigned i = 0; i < m_boids.size(); i++)
		m_boids[i]->onTick(fElapsedTime, m_boids);
	m_mesh.onTick(fElapsedTime);

	/**
	static D3DXVECTOR3 vEye(0.f, 0.f, 0.f), vAt(0.f, 0.f, 1.f), vUp(0.f, 1.f, 0.f), v;
	if (GetAsyncKeyState('Z') & 0x8000) vAt.x += .1f;
	else if (GetAsyncKeyState('X') & 0x8000) vAt.x -= .1f;
	if (GetAsyncKeyState('C') & 0x8000) vAt.y += .1f;
	else if (GetAsyncKeyState('V') & 0x8000) vAt.y -= .1f;
	if (GetAsyncKeyState('B') & 0x8000) vAt.z += .1f;
	else if (GetAsyncKeyState('N') & 0x8000) vAt.z -= .1f;
	if (GetAsyncKeyState('M') & 0x8000) vAt.x = vAt.y = 0.f, vAt.z = 1.f;
	v.x = -vAt.x; v.y = vAt.z > 0.f ? -vAt.y : vAt.y; v.z = vAt.z;
	D3DXMatrixLookAtLH(&m_matAdjust, &vEye, &v, &vUp);
	m_matAdjust._11 *= .03f; m_matAdjust._12 *= .03f; m_matAdjust._13 *= .03f;
	m_matAdjust._21 *= .03f; m_matAdjust._22 *= .03f; m_matAdjust._23 *= .03f;
	m_matAdjust._31 *= .03f; m_matAdjust._32 *= .03f; m_matAdjust._33 *= .03f;
	m_matAdjust._42 = 10.f;
	/**/
}

void CFlock::onRender()
{
	static D3DXMATRIX *pMat = m_mesh.GetWorldTransform();
	static const D3DXVECTOR3 *pPos;
	static D3DXVECTOR3 vDir, vUp, vRight;
	static D3DXVECTOR3 vAxis(0.f, 1.f, 0.f);
	for (unsigned i = 0; i < m_boids.size(); i++) {
		pPos = m_boids[i]->getPosition();
		pMat->_41 = pPos->x; pMat->_42 = pPos->y; pMat->_43 = pPos->z;
		D3DXVec3Normalize(&vDir, m_boids[i]->getVelocity()); 
		vDir.x = -vDir.x; vDir.y = vDir.z > 0.f ? -vDir.y : vDir.y;
		pMat->_13 = vDir.x; pMat->_23 = vDir.y; pMat->_33 = vDir.z;
		D3DXVec3Cross(&vRight, &vAxis, &vDir);
		pMat->_11 = vRight.x; pMat->_21 = vRight.y; pMat->_31 = vRight.z;
		D3DXVec3Cross(&vUp, &vDir, &vRight);
		pMat->_12 = vUp.x; pMat->_22 = vUp.y; pMat->_32 = vUp.z;
		*pMat = m_matAdjust * *pMat;
		m_mesh.onRender();
	}
	//*pMat = m_matAdjust;
	//pMat->_42 = 10.f;
	//m_mesh.onRender();
}