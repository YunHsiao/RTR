#ifndef GPPlayerCamera_H_
#define GPPlayerCamera_H_

#include "..\Engine\BaseCamera.h"

class GPPlayerCamera : public CCamera
{
public:
	GPPlayerCamera();
	~GPPlayerCamera();
	
	void onTick(float fElapsedTime);
	inline void ZoomIn(short wheel) { m_fFollowDist += wheel/WHEEL_DELTA/2.f; 
		if (m_fFollowDist < 3.f) m_fFollowDist = 3.f; }
	void ApplyCamera(const D3DXVECTOR3& vPos);
	void RotateCameraDownRestrained(float fAngle);
	void Crouch(bool bCrouching) { m_vOffset.y = bCrouching ? 0.f : 1.f; }
protected:
	void KeyboardInput(float fElapsedTime);
	void MouseInput();
private:
	float m_fFollowDist;
	D3DXVECTOR3 m_vOffset;
};

#endif
