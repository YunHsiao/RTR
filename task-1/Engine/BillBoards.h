#ifndef CBillBoards_H_
#define CBillBoards_H_

class CBillBoards
{
public:
	CBillBoards();
	virtual ~CBillBoards();

	void onInit(LPCTSTR strTex);
	virtual void onRender(const D3DXMATRIX& mView, const D3DXVECTOR3& vPos);
	void Add(const D3DXVECTOR3& vPos) { m_vPos.push_back(vPos); }
protected:
	D3DXMATRIX m_matWorld;
	std::vector<D3DXVECTOR3> m_vPos;
	IDirect3DTexture9 *m_pTexture;
	IDirect3DVertexBuffer9 *m_pVB;
};
#endif