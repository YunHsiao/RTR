#ifndef TDCamera_H_
#define TDCamera_H_

#include "..\Engine\BaseCamera.h"
#include "SkeletonMesh.h"
#include "Terrain.h"

class TDCamera : public CCamera
{
public:
	TDCamera();
	~TDCamera();

	bool onTick(float fElapsedTime, CTerrain* terrain);
	void MoveCamera(EMoveCameraType eType, float fDistance);
	void EnableFollowing(CSkeletonMesh* pPlayer);
	void DisableFollowing();

	inline void ZoomIn(short wheel) { m_fFollowDist += wheel/WHEEL_DELTA/2.f; 
		if (m_fFollowDist < 3.f) m_fFollowDist = 3.f; }
	void ToggleGravity() { m_bGravity = !m_bGravity; }

protected:
	void KeyboardInput(float fElapsedTime);
	void MouseInput();

protected:
	D3DXVECTOR3 m_pvPos;
	D3DXVECTOR3 m_vOffset;
	D3DXMATRIX *m_pTransform;
	CSkeletonMesh *m_pPlayer;

	float m_fDistance, m_fFollowDist;
	bool m_bFollowing, m_bGravity, m_bMoving, m_bRunning;
	int m_iX, m_iY;
};
#endif
