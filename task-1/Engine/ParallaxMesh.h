#ifndef CParallaxMesh_H_
#define CParallaxMesh_H_

class CParallaxMesh 
{
public:
	CParallaxMesh();
	~CParallaxMesh();

	bool onInit(LPCTSTR strMesh, LPCTSTR strTex, LPCTSTR strNH);
	void onRender(const D3DXMATRIX& matView, const D3DXMATRIX& matProj, const D3DXVECTOR3& vPos);
	inline void setTiling(float fTiling) { m_fTiling = fTiling; }
	inline void ToggleShadows() { m_bShadows = !m_bShadows; }

protected:
	float m_fDepth, m_fTiling;
	bool m_bShadows;
	D3DXMATRIX m_matWorld;
	ID3DXMesh* m_pStaticMesh;
	IDirect3DTexture9 *m_pTexture, *m_pNH;

	ID3DXEffect* m_pEffect;
	D3DXHANDLE m_hEffect;
	D3DXHANDLE m_hWorldMatrix;
	D3DXHANDLE m_hViewMatrix;
	D3DXHANDLE m_hWorldViewProjMatrix;
	D3DXHANDLE m_hCameraPosition;
	D3DXHANDLE m_hTex;
	D3DXHANDLE m_hTexNH;
	D3DXHANDLE m_hShadows;
	D3DXHANDLE m_hDepth;
	D3DXHANDLE m_hTiling;
};

#endif
