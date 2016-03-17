#ifndef GPChamber_H_
#define GPChamber_H_

struct SRect {
	float xmin, zmin;
	float xmax, zmax;
	float h;
	SRect(float _xmin, float _zmin, float _xmax, float _zmax, float _h) :
		xmin(_xmin), zmin(_zmin), xmax(_xmax), zmax(_zmax), h(_h) {}

	bool SRect::Contains(const D3DXVECTOR3& p, float offset = 0.f) {
		return (p.x >= xmin-offset) && (p.x <= xmax+offset)
			&& (p.z >= zmin-offset) && (p.z <= zmax+offset);
	}
};

class GPChamber
{
public:
	GPChamber();
	~GPChamber();

	void onInit(float fZ);
	void onRender();

	bool Validate(const D3DXVECTOR3& p, float offset = 0.f);
	inline float getX() { return m_fX; }
	inline float getZ() { return m_fZ; }
private:
	void writeQuad(SVertexT* pVertex, unsigned int& cnt, const D3DXVECTOR3& p, const D3DXVECTOR3& q);
	void writeCube(SVertexT* pVertex, unsigned int& cnt, const SRect& r);

	unsigned int m_iTriangles;
	float m_fX, m_fZ, m_fWall;
	IDirect3DVertexBuffer9* m_pVB;
	std::vector<IDirect3DTexture9*> m_vTextures;
	std::vector<SRect> m_vColliders;
};
#endif
