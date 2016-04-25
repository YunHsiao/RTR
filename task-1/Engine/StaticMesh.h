#ifndef CStaticMesh_H_
#define CStaticMesh_H_

class CStaticMesh
{
public:
	CStaticMesh();
	~CStaticMesh();
	bool onInit(const char* strFilePath, D3DXVECTOR3& vPos, float fScale = 1.f, 
		float fYaw = 0.f, float fPitch = 0.f, float fRoll = 0.f);
	bool onInit(const char* strFilePath, D3DXVECTOR3& vPos, float fScale, 
		D3DXVECTOR3& vRot, float fAngle);
	// ��Ϊ����ģ��������ʱʹ��
	bool onInit(const char* strFilePath, ID3DXBuffer* pAdjacency, ID3DXBuffer* pBufferMaterial, 
		DWORD dNumMaterial, ID3DXMesh* pStaticMesh, D3DXMATRIX* pMatParent);
	void onRender(D3DXMATRIX* mat = NULL);

	DWORD GetFVF();
	DWORD GetNumBytesPerVertex();
	DWORD GetNumFaces();
	DWORD GetNumVertices();
	LPDIRECT3DVERTEXBUFFER9 GetVertexBuffer();
	LPDIRECT3DINDEXBUFFER9 GetIndexBuffer();
	inline D3DXMATRIX* GetWorldTransform() { return &m_matId; }
	inline ID3DXMesh* GetMesh() { return m_pStaticMesh; }
	inline void SetParentMatrix(D3DXMATRIX* pMatParent) { m_pMatParent = pMatParent; }
	inline void SetWorldMatrix(const D3DXMATRIX& matWorld) { m_matId = matWorld; }

protected:
	// �������Mesh������Normal�򴴽�
	void CreateNormal();

private:
	D3DXMATRIX m_matId;
	D3DXMATRIX* m_pMatParent;
	ID3DXMesh* m_pStaticMesh;
	ID3DXBuffer* m_pAdjacency;
	std::map<int, D3DMATERIAL9> m_mapMaterial;		// ������
	std::map<int, IDirect3DTexture9*> m_mapTexture;	// ��ͼ��
	typedef std::map<int, IDirect3DTexture9*>::iterator TextureIT;
};

#endif