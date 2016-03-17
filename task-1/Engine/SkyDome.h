#ifndef CSkyDome_H_ 
#define CSkyDome_H_ 

class CSkyDome
{ 
public: 
	CSkyDome();
	~CSkyDome();

	VOID onInit(LPCTSTR strTex, float fRadius);
	VOID onTick(float fElapsedTime);
	VOID onRender();
	void InitDebugInfo();
	void ToggleDebug() { m_bDebug = !m_bDebug; }
	void ToggleAlpha() { m_bAlpha = !m_bAlpha; }
	inline const D3DXVECTOR3* getSunPosition() { return &m_vSun; }
private: 
	IDirect3DVertexBuffer9 *m_pVB;
	IDirect3DTexture9 *m_pTexture;
	D3DXMATRIX m_matWorld;
	ID3DXMesh** m_info;
	D3DXMATRIX* m_matDebug;
	D3DXVECTOR3 m_vSun;
	int m_iNumVertices; 
	float m_fAngle;
	bool m_bDebug, m_bAlpha;
	static const float s_fR;
}; 
#endif