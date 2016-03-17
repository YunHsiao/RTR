#ifndef CBillBoard_H_
#define CBillBoard_H_

class CBillBoard
{
public:
	CBillBoard();
	virtual ~CBillBoard();

	void onInit(const D3DXVECTOR3& vPos, LPCTSTR strTex);
	void onRender(const D3DXMATRIX& mView, const D3DXVECTOR3& vPos);
private:
	D3DXMATRIX m_matWorld;
	D3DXVECTOR3 m_vPos;
	IDirect3DTexture9 *m_pTexture;
	IDirect3DVertexBuffer9 *m_pVB;
};
#endif