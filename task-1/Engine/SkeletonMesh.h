#ifndef CSkeletonMesh_H_
#define CSkeletonMesh_H_

#define MAX_CHAR 64
class CStaticMesh;

// 骨骼
struct SkinMeshFrame
{
	SkinMeshFrame();
	~SkinMeshFrame();

	SkinMeshFrame* FindFrame(const char* strName);
	void CloneFrame(SkinMeshFrame** ppFrame);
	void FillFrame(SkinMeshFrame* pFrame);
	void UpdateFrame(D3DXMATRIX *matParent = NULL);

	char strFrameName[MAX_CHAR];
	D3DXMATRIX matLocalMatrix;					// 本地骨骼变换矩阵
	D3DXMATRIX matFinalMatrix;					// 继承父矩阵骨骼变换矩阵
	std::vector<SkinMeshFrame*> vecChildFrame;	// 子骨骼
};

// 蒙皮
struct SkinMeshContainer
{
	SkinMeshContainer();
	~SkinMeshContainer();

	HRESULT UpdateSkinMesh();

	char strSkinMeshName[MAX_CHAR];
	LPD3DXSKININFO pSkinInfo;					// 蒙皮信息
	LPD3DXMESH pOriginalMesh;					// 初始蒙皮
	LPD3DXMESH pRenderMesh;						// 当前渲染蒙皮
	ID3DXBuffer* pAdjacency;					// 蒙皮邻接表
	std::vector<D3DXMATRIX*> vecMatFrames;		// 指向骨骼矩阵
	D3DXMATRIX *pBoneMatrices;					// 当前帧渲染骨骼矩阵
	std::vector<D3DMATERIAL9> m_vMaterial;		// 材质组
	std::vector<IDirect3DTexture9*> m_vTexture;	// 贴图组
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

// 单根骨骼的运动变换
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

	// 骨骼名称
	char FrameName[MAX_CHAR];
	SkinMeshFrame* pFrame;

	// 缩放变换
	std::vector<AnimationVectorKey> vecScaleKeys;

	// 旋转变换
	std::vector<AnimationQuaternionKey> vecQuatKeys;

	// 平移变换
	std::vector<AnimationVectorKey> vecTranslationKeys;

	// 矩阵变换
	std::vector<AnimationMatrixKey> vecMatKeys;
};

// 骨骼动画
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

	char AnimSetName[MAX_CHAR];				// 动画名称
	float fAnimationLength;					// 动画长度
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

	// 设置骨骼动画动作
	bool SetAction(std::string strActionName, bool bActionLoop, float fActionSpeed = 1.f, float fActionBlendTime = 0.f);
	bool IsCurActionEnd();
	inline D3DXMATRIX* GetAdjustTransform() { return &m_matAdjust; }
	inline D3DXMATRIX* GetTransform() { return &m_matPlayer; }
	inline D3DXMATRIX* GetWorldTransform() { return &m_matWorld; }
	inline ID3DXMesh* GetMesh(unsigned int i) { if (m_vSkinMesh.size() > i) 
		return m_vSkinMesh[i]->pRenderMesh; return NULL; }
	inline CStaticMesh* GetAttachedMesh(unsigned int i) { return m_vStaticMesh[i]; }

	// 获取骨骼继承父骨骼变换矩阵
	D3DXMATRIX* GetFrameFinalMatrix(const char* strFrameName);

	// 获取骨骼世界矩阵
	D3DXMATRIX* GetFrameWorldMatrix(const char* strFrameName);

	// 获取骨骼世界坐标位置
	D3DXVECTOR3* GetFramePosition(const char* strFrameName);

	void AttachMesh(CStaticMesh* pMesh, const char* strFrameName);

	// 包围盒(人工误差调整)
	void UpdateBoundingBox(const D3DXVECTOR3* vMinOffset = NULL, const D3DXVECTOR3* vMaxOffset = NULL);
	bool Intersect(const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir);
	void ToggleDebug() { m_bDebug = !m_bDebug; }

protected:
	bool do_Init();
	// 解析x文件
	bool ParseFile();
	bool ParseObject(ID3DXFileData* pDataObj, ID3DXFileData* pParentDataObj, void** Data, bool bReference);
	bool ParseChildObjects(ID3DXFileData* pDataObj, void** Data, bool bReference);

	// 加载蒙皮
	HRESULT LoadSkinMesh(ID3DXFileData* pDataObj, void** Data);

	// 加载动画数据
	void LoadAnimationKey(ID3DXFileData* pDataObj, ID3DXFileData* pParentDataObj, void** Data);

	// 将蒙皮绑定骨骼
	bool AttachSkinMeshToFrame();

	// 将动画数据绑定骨骼
	void AttachAnimationToFrame();

	// 更新当前动作
	void UpdateAction(float fElapsedTime);

	HRESULT GetObjectGUID(ID3DXFileData* pDataObj, GUID* type);
	char* GetObjectName(ID3DXFileData* pDataObj);
	LPCVOID LockObjectData(ID3DXFileData* pDataObj, SIZE_T* Size);
	void UnLockObjectData(ID3DXFileData* pDataObj);

	void writeLine(SVertexD* pVertices, unsigned int& iCnt, 
		const D3DXVECTOR3& p, const D3DXVECTOR3& q);

	char m_strFilePath[MAX_CHAR];
	char m_strTmp[MAX_CHAR];

	SkinMeshFrame* m_pRootFrame;					// 根骨骼
	SkinMeshFrame* m_pCloneFrame;					// 克隆骨骼,用于动作融合计算
	std::vector<SkinMeshContainer*> m_vSkinMesh;	// 蒙皮

	bool m_bCurActionLoop;							// 当前动作是否循环
	float m_fCurActionSpeed;						// 当前动作播放速度
	float m_fCurActionElasped;						// 当前动作播放时间
	float m_fCurActionBlendTime;					// 当前动作融合时间
	AnimationSet* m_pCurAnimSet;					// 当前播放骨骼动画
	std::map<std::string, AnimationSet*> m_mapAnimSet;	// 骨骼动画
	std::vector<CStaticMesh*> m_vStaticMesh;		// 子静态模型
	D3DXMATRIX m_matWorld;							// 世界矩阵
	D3DXMATRIX m_matAdjust, m_matPlayer;			// 模型调整矩阵，位移矩阵

	D3DXVECTOR3 m_vMin, m_vMax;						// AABB包围盒
	IDirect3DVertexBuffer9* m_pVB;
	bool m_bDebug;

	typedef std::map<std::string, AnimationSet*>::iterator AnimSetIT;
	typedef std::vector<CStaticMesh*>::iterator MeshIT;
	typedef std::vector<SkinMeshContainer*>::iterator SkinIT;
};
#endif