#include "Utility.h"
#include "Boid.h"

CBoid::CBoid(const D3DXVECTOR3& vPos, const D3DXVECTOR3& vLower, const D3DXVECTOR3& vUpper)
:m_vPos(vPos)
,m_vLower(vLower)
,m_vUpper(vUpper)
,m_fBorder(5.f)
,m_fMaxVel(100.f)
,m_fMaxRadSq(100.f)
,m_fMaxVelSq(m_fMaxVel * m_fMaxVel)
{
	unsigned t1, t2; rand_s(&t1); rand_s(&t2);
	float phi = (float) t1 / UINT_MAX * D3DX_PI;
	float theta = (float) t2 / UINT_MAX * 2.f * D3DX_PI;
	m_vVel.x = sinf(phi) * cosf(theta);
	m_vVel.y = sinf(phi) * sinf(theta);
	m_vVel.z = cosf(phi);
}

CBoid::~CBoid()
{

}

void CBoid::onTick(float fElapsedTime, const std::vector<CBoid*>& boids)
{
	m_vAcc.x = m_vAcc.y = m_vAcc.z = 0.f;
	Flock(boids);

	// Update
	m_vVel += m_vAcc * fElapsedTime;
	Limit(m_vVel);
	m_vPos += m_vVel * fElapsedTime;

	// Border
	if (m_vPos.x < m_vLower.x - m_fBorder) m_vPos.x = m_vUpper.x - m_fBorder;
	else if (m_vPos.x > m_vUpper.x + m_fBorder) m_vPos.x = m_vLower.x + m_fBorder;
	else if (m_vPos.y < m_vLower.y - m_fBorder) m_vPos.y = m_vUpper.y - m_fBorder;
	else if (m_vPos.y > m_vUpper.y + m_fBorder) m_vPos.y = m_vLower.y + m_fBorder;
	else if (m_vPos.z < m_vLower.z - m_fBorder) m_vPos.z = m_vUpper.z - m_fBorder;
	else if (m_vPos.z > m_vUpper.z + m_fBorder) m_vPos.z = m_vLower.z + m_fBorder;
}

void CBoid::Limit(D3DXVECTOR3& v)
{
	static float fLengthSq;
	fLengthSq = D3DXVec3LengthSq(&v);
	if (fLengthSq > m_fMaxVelSq) 
		v *= m_fMaxVel / sqrtf(fLengthSq);
}

D3DXVECTOR3 CBoid::Seek(const D3DXVECTOR3& target)
{
	D3DXVECTOR3 desired = target - m_vPos;
	D3DXVec3Normalize(&desired, &desired);
	desired *= m_fMaxVel;
	desired -= m_vVel;
	Limit(desired);
	return desired;
}

void CBoid::Flock(const std::vector<CBoid*>& boids)
{
	m_vSep.x = m_vSep.y = m_vSep.z = 0.f;
	m_vAli.x = m_vAli.y = m_vAli.z = 0.f;
	m_vCoh.x = m_vCoh.y = m_vCoh.z = 0.f;
	D3DXVECTOR3 vDir, vPos;
	float fDistSq, iCount = 0.f;
	for (unsigned i = 0; i < boids.size(); i++) {
		if (boids[i] == this) continue;
		vPos = *boids[i]->getPosition();
		vDir = m_vPos - vPos;
		fDistSq = D3DXVec3LengthSq(&vDir);
		if (fDistSq < m_fMaxRadSq) {
			D3DXVec3Normalize(&vDir, &vDir);
			vDir /= sqrtf(fDistSq); // Weight by distance
			m_vSep += vDir;
			m_vAli += *boids[i]->getVelocity();
			m_vCoh += vPos;
			iCount++;
		}
	}
	if (iCount) {
		D3DXVec3Normalize(&m_vSep, &m_vSep);
		m_vSep *= m_fMaxVel;
		m_vSep -= m_vVel;
		Limit(m_vSep);
		m_vAcc += m_vSep * 1.f;

		D3DXVec3Normalize(&m_vAli, &m_vAli);
		m_vAli *= m_fMaxVel;
		m_vAli -= m_vVel;
		Limit(m_vAli);
		m_vAcc += m_vAli * .5f;

		m_vCoh = Seek(m_vCoh / iCount);
		m_vAcc += m_vCoh * 1.f;
		//if (GetAsyncKeyState(VK_NUMPAD9) & 0x8000) {
		//	char str[256];
		//	sprintf_s(str, 256, "Sep: %.2f %.2f %.2f Ali: %.2f %.2f %.2f Coh: %.2f %.2f %.2f\n", 
		//		m_vSep.x, m_vSep.y, m_vSep.z, m_vAli.x, m_vAli.y, m_vAli.z, m_vCoh.x, m_vCoh.y, m_vCoh.z);
		//	OutputDebugStringA(str);
		//}
	}
}
