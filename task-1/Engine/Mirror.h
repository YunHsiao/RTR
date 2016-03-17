#ifndef CMirror_H_
#define CMirror_H_

class CMirror
{
public:
	CMirror();
	~CMirror();

	void onInit(LPCTSTR strTex, float fYaw, float fPitch, float fRow);
	void onRender();
	const D3DXMATRIX* GetMirror() { return &m_matMirror; }
protected:
	void updateTransform();
	IDirect3DVertexBuffer9 *m_pVB;
	IDirect3DTexture9 *m_pTexture;
	D3DXMATRIX m_matTrans, m_matMirror;
	float m_fAngle[3];
};

#endif
