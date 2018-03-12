#ifndef CBoid_H_
#define CBoid_H_

class CBoid
{
public:
	CBoid(const D3DXVECTOR3& vPos, const D3DXVECTOR3& vLower, const D3DXVECTOR3& vUpper);
	~CBoid();

	void onTick(float fElapsedTime, const std::vector<CBoid*>& boids);
	inline void setPosition(const D3DXVECTOR3& vPos) { m_vPos = vPos; }
	inline const D3DXVECTOR3* getPosition() const { return &m_vPos; }
	inline const D3DXVECTOR3* getVelocity() const { return &m_vVel; }
	D3DXVECTOR3 Seek(const D3DXVECTOR3& target);
protected:
	void Limit(D3DXVECTOR3& v);
	void Flock(const std::vector<CBoid*>& boids);

	D3DXVECTOR3 m_vPos, m_vVel, m_vAcc;
	D3DXVECTOR3 m_vSep, m_vAli, m_vCoh;
	D3DXVECTOR3 m_vLower, m_vUpper;
	float m_fBorder, m_fMaxVel, m_fMaxVelSq, m_fMaxRadSq;
};

#endif
