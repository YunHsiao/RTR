#ifndef CTerrain_H_
#define CTerrain_H_

enum ERenderStatus {
	ERS_PRUNED,
	ERS_VISIBLE,
	ERS_RECURSED
};

struct SNode {
	SNode *n, *e, *w, *s; // neighbors
	SNode *nw, *ne, *sw, *se; // subnodes
	int l, r, t, b, W, H, C, D; // huffman Code in octonary, Depth
	float diff; // max height Difference
	int status;
	SNode(int _l, int _r, int _t, int _b, int _W, int _H, int _C, int _D, int d) : l(_l), r(_r),
		t(_t), b(_b), W(_W), H(_H), nw(NULL), ne(NULL), sw(NULL), se(NULL),
		n(NULL), e(NULL), w(NULL), s(NULL), diff(0.f), C(_C), D(_D), status(ERS_PRUNED) {
		C <<= 3; C += d; _W >>= 1; _H >>= 1;
		if (H || W) nw = new SNode(l, l + W, t, t - H, _W, _H, C, D + 1, 1);
		if (W)		ne = new SNode(r - W, r, t, t - H, _W, _H, C, D + 1, 2);
		if (H)		sw = new SNode(l, l + W, b + H, b, _W, _H, C, D + 1, 3);
		if (H && W) se = new SNode(r - W, r, b + H, b, _W, _H, C, D + 1, 4);
	}
	~SNode() { Safe_Delete(nw); Safe_Delete(ne); Safe_Delete(sw); Safe_Delete(se); }
};

class CTerrain
{
public:
	CTerrain();
	~CTerrain();

	bool onInit(LPCTSTR strTex, LPCTSTR strHei, int iWidth = 0, int iLength = 0);
	void onTick(float fElapsedTime, const D3DXMATRIX& pView, const D3DXMATRIX& pProj, const D3DXVECTOR3& pPos);
	void onRender();

	void ToggleObserver();
	inline void ToggleLOD() { m_bLOD = !m_bLOD; InitDefaultIB(); }
	inline float GetRadiusX() { return m_fRX; }
	inline float GetRadiusZ() { return m_fRZ; }
	float GetHeight(float x, float z);
	const D3DXVECTOR3* Intersect(const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir);
private:
	void InitFrustum();
	bool InitDefaultIB();
	void InitQuadTree();
	void InitQuadTreeDiff(SNode* node);
	void InitQuadTreeNeighbors(SNode* node);
	void GenerateIB(SNode *node, DWORD *pIndices);
	void RecoverQuadTree(SNode *node);
	void PruneQuadTree(SNode *node, const D3DXVECTOR3& pPos);

	float DistanceToFrustumSq(D3DXVECTOR3* vWorld);
	SNode* FindNode(SNode* node, int C, int D);
	void GenerateQuadrantIB(SNode *target, SNode *node, DWORD *pIndices, int d);
	const D3DXVECTOR3* TriangleIntersection(const D3DXVECTOR3& vPos, 
		const D3DXVECTOR3& vDir, const D3DXVECTOR3& n, 
		const D3DXVECTOR3& p0, const D3DXVECTOR3& p1, const D3DXVECTOR3& p2);
	SNode *m_root;
	D3DXVECTOR3 **m_pVertices, *m_pNormals;
	IDirect3DVertexBuffer9 *m_pVB, *m_pFrustumVB;
	IDirect3DIndexBuffer9 *m_pIB, *m_pFrustumIB;
	IDirect3DTexture9 *m_pTex, *m_pDet;
	D3DXMATRIX m_matId, m_mat, m_matR;
	D3DMATERIAL9 m_mCyan, m_mMagenta, m_mYellow;
	int m_iX, m_iZ, m_iVX, m_iVZ, m_iTriangles, m_iVertices;
	bool m_bLOD, m_bObserve;
	float m_fRX, m_fRZ, m_fSegX, m_fSegZ;
};

#endif