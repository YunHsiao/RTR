#ifndef CSkybox_H_
#define CSkybox_H_

class CSkyBox
{
public:
	CSkyBox();
	~CSkyBox();

	void onInit(std::string strTexPrefix, float fRadius);
	void onRender();
private:
	IDirect3DVertexBuffer9 *m_pVB;
	IDirect3DTexture9 *m_pTextures[6];
	D3DXMATRIX m_matId;
};

#endif

