#ifndef CWATER_H
#define	CWATER_H

class CWater {
public:
	CWater();
	~CWater();

	void onInit(const char* strTexPre, const char* strTexPost, int iFrames, float fRX, float fRZ);
	void onTick(float fElapsedTime);
	void onRender();
	inline ID3DXMesh* GetMesh() { return m_pMesh; }

private:
	ID3DXMesh *m_pMesh;
	IDirect3DTexture9* m_ppTexture[45];
	D3DXMATRIX m_mat;
	float m_fLX, m_fLZ; // Length
	int m_iVX, m_iVZ, m_iEX, m_iEZ, m_iCur; // Vertices, Edges, Current
	int m_iFrames;
	DWORD m_lV, m_lT;
};
#endif