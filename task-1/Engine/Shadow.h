#ifndef CShadow_H_
#define CShadow_H_

class CShadow
{
public:
	CShadow();
	~CShadow();

	void onInit(const D3DXVECTOR3* pSun);
	void onTick(float fElapsedTime);
	void onRender();
	void add(ID3DXMesh* pMesh, D3DXMATRIX* pMat);
private:
	void AddEdge(WORD* pEdges, DWORD& dwEdges, WORD v0, WORD v1);
	IDirect3DVertexBuffer9 *m_pVB;
	D3DXMATRIX m_matTrans;
	const D3DXVECTOR3* m_pSun;
	std::vector<ID3DXMesh*> m_vMesh;
	std::vector<IDirect3DVertexBuffer9*> m_vVB;
	std::vector<D3DXMATRIX*> m_vMat;
	std::vector<DWORD> m_dwTriangles;
};

#endif

