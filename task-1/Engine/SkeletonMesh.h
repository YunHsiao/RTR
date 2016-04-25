#ifndef CSkeletonMesh_H_
#define CSkeletonMesh_H_

#define MAX_CHAR 64
class CStaticMesh;

// ����
struct SkinMeshFrame
{
	SkinMeshFrame();
	~SkinMeshFrame();

	SkinMeshFrame* FindFrame(const char* strName);
	void CloneFrame(SkinMeshFrame** ppFrame);
	void FillFrame(SkinMeshFrame* pFrame);
	void UpdateFrame(D3DXMATRIX *matParent = NULL);

	char strFrameName[MAX_CHAR];
	D3DXMATRIX matLocalMatrix;					// ���ع����任����
	D3DXMATRIX matFinalMatrix;					// �̳и���������任����
	std::vector<SkinMeshFrame*> vecChildFrame;	// �ӹ���
};

// ��Ƥ
struct SkinMeshContainer
{
	SkinMeshContainer();
	~SkinMeshContainer();

	HRESULT UpdateSkinMesh();

	char strSkinMeshName[MAX_CHAR];
	LPD3DXSKININFO pSkinInfo;					// ��Ƥ��Ϣ
	LPD3DXMESH pOriginalMesh;					// ��ʼ��Ƥ
	LPD3DXMESH pRenderMesh;						// ��ǰ��Ⱦ��Ƥ
	ID3DXBuffer* pAdjacency;					// ��Ƥ�ڽӱ�
	std::vector<D3DXMATRIX*> vecMatFrames;		// ָ���������
	D3DXMATRIX *pBoneMatrices;					// ��ǰ֡��Ⱦ��������
	std::vector<D3DMATERIAL9> m_vMaterial;		// ������
	std::vector<IDirect3DTexture9*> m_vTexture;	// ��ͼ��
};

struct AnimationVectorKey
{
	float fTime;
	D3DXVECTOR3 vecKey;
};

struct AnimationQuaternionKey
{
	float fTime;
	D3DXQUATERNION quatKey;
};

struct AnimationMatrixKey
{
	float fTime;
	D3DXMATRIX matKey;
};

// �����������˶��任
struct Animation
{
	Animation()
	{
		ZeroMemory(FrameName, MAX_CHAR);
		pFrame = NULL;
	}

	~Animation()
	{
		pFrame = NULL;
	}

	// ��������
	char FrameName[MAX_CHAR];
	SkinMeshFrame* pFrame;

	// ���ű任
	std::vector<AnimationVectorKey> vecScaleKeys;

	// ��ת�任
	std::vector<AnimationQuaternionKey> vecQuatKeys;

	// ƽ�Ʊ任
	std::vector<AnimationVectorKey> vecTranslationKeys;

	// ����任
	std::vector<AnimationMatrixKey> vecMatKeys;
};

// ��������
struct AnimationSet
{
	AnimationSet()
	{
		ZeroMemory(AnimSetName, MAX_CHAR);
		fAnimationLength = 0.f;
	}

	~AnimationSet()
	{
		for (unsigned int i = 0; i < vecAnimations.size(); i++)
			Safe_Delete(vecAnimations[i]);

		vecAnimations.clear();
	}

	char AnimSetName[MAX_CHAR];				// ��������
	float fAnimationLength;					// ��������
	std::vector<Animation*> vecAnimations;
};


class CSkeletonMesh
{
public:
	CSkeletonMesh();
	~CSkeletonMesh();

	virtual bool onInit(const char* strFilePath, D3DXVECTOR3& vPos, float fScale);
	virtual bool onTick(float fElapsedTime);
	virtual void onRender(const D3DXMATRIX* matMirror = NULL);

	// ���ù�����������
	bool SetAction(std::string strActionName, bool bActionLoop, float fActionSpeed = 1.f, float fActionBlendTime = 0.f);
	bool IsCurActionEnd();
	inline D3DXMATRIX* GetAdjustTransform() { return &m_matAdjust; }
	inline D3DXMATRIX* GetTransform() { return &m_matPlayer; }
	inline D3DXMATRIX* GetWorldTransform() { return &m_matWorld; }
	inline ID3DXMesh* GetMesh(unsigned int i) { if (m_vSkinMesh.size() > i) 
		return m_vSkinMesh[i]->pRenderMesh; return NULL; }
	inline CStaticMesh* GetAttachedMesh(unsigned int i) { return m_vStaticMesh[i]; }

	// ��ȡ�����̳и������任����
	D3DXMATRIX* GetFrameFinalMatrix(const char* strFrameName);

	// ��ȡ�����������
	D3DXMATRIX* GetFrameWorldMatrix(const char* strFrameName);

	// ��ȡ������������λ��
	D3DXVECTOR3* GetFramePosition(const char* strFrameName);

	void AttachMesh(CStaticMesh* pMesh, const char* strFrameName);

	// ��Χ��(�˹�������)
	void UpdateBoundingBox(const D3DXVECTOR3* vMinOffset = NULL, const D3DXVECTOR3* vMaxOffset = NULL);
	bool Intersect(const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir);
	void ToggleDebug() { m_bDebug = !m_bDebug; }

protected:
	bool do_Init();
	// ����x�ļ�
	bool ParseFile();
	bool ParseObject(ID3DXFileData* pDataObj, ID3DXFileData* pParentDataObj, void** Data, bool bReference);
	bool ParseChildObjects(ID3DXFileData* pDataObj, void** Data, bool bReference);

	// ������Ƥ
	HRESULT LoadSkinMesh(ID3DXFileData* pDataObj, void** Data);

	// ���ض�������
	void LoadAnimationKey(ID3DXFileData* pDataObj, ID3DXFileData* pParentDataObj, void** Data);

	// ����Ƥ�󶨹���
	bool AttachSkinMeshToFrame();

	// ���������ݰ󶨹���
	void AttachAnimationToFrame();

	// ���µ�ǰ����
	void UpdateAction(float fElapsedTime);

	HRESULT GetObjectGUID(ID3DXFileData* pDataObj, GUID* type);
	char* GetObjectName(ID3DXFileData* pDataObj);
	LPCVOID LockObjectData(ID3DXFileData* pDataObj, SIZE_T* Size);
	void UnLockObjectData(ID3DXFileData* pDataObj);

	void writeLine(SVertexD* pVertices, unsigned int& iCnt, 
		const D3DXVECTOR3& p, const D3DXVECTOR3& q);

	char m_strFilePath[MAX_CHAR];
	char m_strTmp[MAX_CHAR];

	SkinMeshFrame* m_pRootFrame;					// ������
	SkinMeshFrame* m_pCloneFrame;					// ��¡����,���ڶ����ںϼ���
	std::vector<SkinMeshContainer*> m_vSkinMesh;	// ��Ƥ

	bool m_bCurActionLoop;							// ��ǰ�����Ƿ�ѭ��
	float m_fCurActionSpeed;						// ��ǰ���������ٶ�
	float m_fCurActionElasped;						// ��ǰ��������ʱ��
	float m_fCurActionBlendTime;					// ��ǰ�����ں�ʱ��
	AnimationSet* m_pCurAnimSet;					// ��ǰ���Ź�������
	std::map<std::string, AnimationSet*> m_mapAnimSet;	// ��������
	std::vector<CStaticMesh*> m_vStaticMesh;		// �Ӿ�̬ģ��
	D3DXMATRIX m_matWorld;							// �������
	D3DXMATRIX m_matAdjust, m_matPlayer;			// ģ�͵�������λ�ƾ���

	D3DXVECTOR3 m_vMin, m_vMax;						// AABB��Χ��
	IDirect3DVertexBuffer9* m_pVB;
	bool m_bDebug;

	typedef std::map<std::string, AnimationSet*>::iterator AnimSetIT;
	typedef std::vector<CStaticMesh*>::iterator MeshIT;
	typedef std::vector<SkinMeshContainer*>::iterator SkinIT;
};
#endif