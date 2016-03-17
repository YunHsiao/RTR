#ifndef TDDisc_H_
#define TDDisc_H_

#include "..\Engine\ParallaxMesh.h"

class TDDisc : public CParallaxMesh
{
public:
	TDDisc();
	~TDDisc();

	bool onInit(LPCTSTR strMesh);
	void onTick(float fElapsedTime);
	void onRender(const D3DXMATRIX& matView, const D3DXMATRIX& matProj, const D3DXVECTOR3& vPos);
	inline void changeTexture() { 
		m_iTexture = (m_iTexture + 1) % m_vTexture.size(); 
		m_iNH = m_iTexture;
		update();
	}
	inline void changeNormal() { 
		if (m_iTexture < 3) return;
		m_iNH = (m_iNH + 1) % m_vNH.size(); 
		update();
	}

private:
	inline void update() { 
		if (m_iNH == 3 || m_iNH == 5) m_fTiling = 4.f; 
		else m_fTiling = 1.f; 
		m_pTexture = m_vTexture[m_iTexture];
		m_pNH = m_vNH[m_iNH];
	}
	bool CreateTextureFromFile(char* str, bool bNormalHeightTexture = false);

	unsigned int m_iTexture, m_iNH;
	bool m_bShadows;
	D3DXMATRIX m_matRot, m_matSca;
	std::vector<IDirect3DTexture9*> m_vTexture, m_vNH;
	typedef std::vector<IDirect3DTexture9*>::iterator TextureIT;
};

#endif
