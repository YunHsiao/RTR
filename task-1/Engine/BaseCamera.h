#ifndef CCamera_H_
#define CCamera_H_

enum EMoveCameraType
{
	EMCT_Forward,
	EMCT_Vertical,
	EMCT_Horizontal
};

class CCamera
{
public:
	CCamera();
	virtual ~CCamera();

	void onInit();
	virtual void onTick(float fElaspedTime);
	virtual void MoveCamera(EMoveCameraType eType, float fDistance);
	void ApplyCamera();

	inline void RotateCameraDown(float fAngle) { m_fPitch += fAngle; }
	inline void RotateCameraRight(float fAngle) { m_fYaw += fAngle; }
	inline void SetFov(float fFovy) { m_fFovy = fFovy; }
	inline void BeginDragging(POINT& p) { m_bDragging = true; m_pBeg = p; }
	inline void EndDragging() { m_bDragging = false; }
	inline void SetCameraPosition(const D3DXVECTOR3& vecEyePosition) { m_vecEyePosition = vecEyePosition; }
	inline D3DXVECTOR3& GetCameraPosition() { return m_vecEyePosition; }
	inline D3DXVECTOR3& GetCameraDirection() { return m_vecDirection; }
	inline D3DXVECTOR3& GetCameraRight() { return m_vecRight; }
	inline D3DXVECTOR3& GetCameraUp() { return m_vecUp; }
	inline const D3DXMATRIX& GetViewMatrix() { return m_matView; }
	inline const D3DXMATRIX& GetProjMatrix() { return m_matProj; }
	inline void SetPitch(float fPitch) { m_fPitch = fPitch; }
	inline void SetYaw(float fYaw) { m_fYaw = fYaw; }
	inline float GetPitch() { return m_fPitch; }
	inline float GetYaw() { return m_fYaw; }

protected:
	void UpdateAxis();
	virtual void KeyboardInput(float fElaspedTime) {}
	virtual void MouseInput() {}
	void UpdateProjection();

protected:
	D3DXVECTOR3 m_vecEyePosition;
	D3DXVECTOR3 m_vecRight, m_vecUp, m_vecDirection;
	POINT m_pBeg, m_pCenter;

	float m_fFovy, m_fYaw, m_fPitch;
	bool m_bDragging;

	D3DXMATRIX m_matView;
	D3DXMATRIX m_matProj;
};
#endif