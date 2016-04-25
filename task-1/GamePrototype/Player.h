#ifndef GPPlayer_H_
#define GPPlayer_H_

#include "PlayerCamera.h"
#include "Chamber.h"
#include "..\Engine\SkeletonMesh.h"

class GPPlayer
{
public:
	GPPlayer();
	~GPPlayer();

	void onInit(float fX, float fZ, GPChamber* pChamber);
	void onTick(float fElapsedTime);
	void onRender();

	inline D3DXVECTOR3& GetPosition() { return m_vPos; }
	inline void ZoomIn(short wheel) { m_camera.ZoomIn(wheel); }
	inline const D3DXMATRIX& GetViewMatrix() { return m_camera.GetViewMatrix(); }
	inline const D3DXMATRIX& GetProjMatrix() { return m_camera.GetProjMatrix(); }
	inline D3DXVECTOR3& GetCameraPosition() { return m_camera.GetCameraPosition(); }
	inline D3DXVECTOR3& GetCameraDirection() { return m_camera.GetCameraDirection(); }
private:
	void KeyboardInput(float fElapsedTime);
	void Move(float fDistance, bool bZ, float fSin, float fCos);
	void ParameterAdaptation();

	GPChamber* m_pChamber;
	GPPlayerCamera m_camera;
	CSkeletonMesh m_model;

	bool m_bSneaking, m_bCrouching, m_bMoving;
	float m_fDistance;
	D3DXVECTOR3 m_vPos, m_vPrev;
	D3DXMATRIX *m_pTransform;
};

#endif
