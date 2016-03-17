#ifndef TDMisc_H_
#define TDMisc_H_

class TDMisc
{
public:
	TDMisc();
	~TDMisc();
	
	bool onInit();
	void onTick(float fElapsedTime);
	void onRender();
	inline ID3DXMesh* GetMesh() { return m_pTeapot; }
	inline D3DXMATRIX* GetWorldTransform() { return &m_matTeapot; }
private:
	IDirect3DVertexBuffer9 *m_pVB;
	IDirect3DIndexBuffer9 *m_pIB;
	ID3DXMesh *m_pTeapot;
	D3DMATERIAL9 m_material;
	D3DLIGHT9 m_light, m_light2;
	D3DXMATRIX m_matLeft, m_matRight, m_matTeapot, m_matId;
	SVertexD m_grid[11][11];
};

#endif