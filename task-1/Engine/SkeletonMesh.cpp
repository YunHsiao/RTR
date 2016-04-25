#include "Utility.h"
#include "Direct3D.h"
#include "SkeletonMesh.h"
#include "StaticMesh.h"
#include <rmxftmpl.h>

#define BB_COLOR 0xffff3500

SkinMeshFrame::SkinMeshFrame()
{
	ZeroMemory(strFrameName, MAX_CHAR);
	D3DXMatrixIdentity(&matLocalMatrix);
	D3DXMatrixIdentity(&matFinalMatrix);
}

SkinMeshFrame::~SkinMeshFrame()
{
	for (unsigned int i = 0; i < vecChildFrame.size(); i++)
		Safe_Delete(vecChildFrame[i]);

	vecChildFrame.clear();
}

SkinMeshFrame* SkinMeshFrame::FindFrame(const char* strName)
{
	if(0 == strcmp(strName, strFrameName))
		return this;

	SkinMeshFrame* Frame = NULL;
	for (int i = 0; i < (int)vecChildFrame.size(); ++i)
	{
		Frame = vecChildFrame[i]->FindFrame(strName);
		if (NULL != Frame)
			return Frame;
	}

	return NULL;
}

void SkinMeshFrame::CloneFrame(SkinMeshFrame** ppFrame)
{
	if (NULL == ppFrame)
		return;

	Safe_Delete(*ppFrame);

	*ppFrame = new SkinMeshFrame;
	strcpy_s((*ppFrame)->strFrameName, MAX_CHAR, this->strFrameName);
	(*ppFrame)->matLocalMatrix = this->matLocalMatrix;

	(*ppFrame)->vecChildFrame.resize(this->vecChildFrame.size());
	for (int i = 0; i < (int)this->vecChildFrame.size(); ++i)
		this->vecChildFrame[i]->CloneFrame(&(*ppFrame)->vecChildFrame[i]);
}

void SkinMeshFrame::FillFrame(SkinMeshFrame* pFrame)
{
	if (NULL == pFrame)
		return;

	pFrame->matLocalMatrix = this->matLocalMatrix;
	for (int i = 0; i < (int)this->vecChildFrame.size(); ++i)
		this->vecChildFrame[i]->FillFrame(pFrame->vecChildFrame[i]);
}

void SkinMeshFrame::UpdateFrame(D3DXMATRIX *matParent)
{
	D3DXMATRIX matTmp;
	if (NULL == matParent)
	{
		D3DXMatrixIdentity(&matTmp);
		matParent = &matTmp;
	}

	// 计算继承变换矩阵
	matFinalMatrix = matLocalMatrix * (*matParent);

	// 更新子骨骼
	for (int i = 0; i < (int)vecChildFrame.size(); ++i)
		vecChildFrame[i]->UpdateFrame(&matFinalMatrix);
}

//-----------------------------------------------------------------

SkinMeshContainer::SkinMeshContainer():
pSkinInfo(NULL),
pOriginalMesh(NULL),
pRenderMesh(NULL),
pAdjacency(NULL),
pBoneMatrices(NULL)
{
	ZeroMemory(strSkinMeshName, MAX_CHAR);
}

SkinMeshContainer::~SkinMeshContainer()
{
	Safe_Release(pSkinInfo);
	Safe_Release(pOriginalMesh);
	Safe_Release(pRenderMesh);
	Safe_Release(pAdjacency);

	vecMatFrames.clear();

	for (unsigned int i = 0; i < m_vTexture.size(); i++)
		Safe_Release(m_vTexture[i]);
	m_vTexture.clear();

	Safe_Delete_Array(pBoneMatrices);
}

HRESULT SkinMeshContainer::UpdateSkinMesh()
{
	if (NULL == pSkinInfo || NULL == pOriginalMesh || NULL == pRenderMesh || NULL == pBoneMatrices)
		return E_FAIL;

	DWORD dNumBones = pSkinInfo->GetNumBones();
	for (DWORD i = 0; i < dNumBones; ++i)
	{
		pBoneMatrices[i] = *pSkinInfo->GetBoneOffsetMatrix(i);
		if (NULL != vecMatFrames[i])
			pBoneMatrices[i] *= *vecMatFrames[i];
	}

	void *SrcPtr, *DestPtr;
	pOriginalMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&SrcPtr);
	pRenderMesh->LockVertexBuffer(0, (void**)&DestPtr);

	pSkinInfo->UpdateSkinnedMesh(pBoneMatrices, NULL, SrcPtr, DestPtr);

	pRenderMesh->UnlockVertexBuffer();
	pOriginalMesh->UnlockVertexBuffer();
	return D3D_OK;
}

//-----------------------------------------------------------------

CSkeletonMesh::CSkeletonMesh():
m_pVB(NULL),
m_pRootFrame(NULL),
m_pCloneFrame(NULL),
m_pCurAnimSet(NULL),
m_bDebug(false),
m_bCurActionLoop(false),
m_fCurActionSpeed(1.f),
m_fCurActionElasped(0.f),
m_fCurActionBlendTime(0.f),
m_vMin(1e6f, 1e6f, 1e6f),
m_vMax(-1e6f, -1e6f, -1e6f)
{
	ZeroMemory(m_strFilePath, MAX_CHAR);
	ZeroMemory(m_strTmp, MAX_CHAR);
	D3DXMatrixIdentity(&m_matAdjust);
	D3DXMatrixIdentity(&m_matPlayer);
	D3DXMatrixIdentity(&m_matWorld);
}

CSkeletonMesh::~CSkeletonMesh()
{
	m_pCurAnimSet = NULL;
	for (AnimSetIT it = m_mapAnimSet.begin(); m_mapAnimSet.end() != it; it++)
		Safe_Delete(it->second);
	m_mapAnimSet.clear();

	for (MeshIT it = m_vStaticMesh.begin(); m_vStaticMesh.end() != it; it++)
		Safe_Delete(*it);
	m_vStaticMesh.clear();

	for (SkinIT it = m_vSkinMesh.begin(); m_vSkinMesh.end() != it; it++)
		Safe_Delete(*it);
	m_vSkinMesh.clear();

	Safe_Release(m_pVB);

	Safe_Delete(m_pRootFrame);
	Safe_Delete(m_pCloneFrame);
}

bool CSkeletonMesh::onInit(const char* strFilePath, D3DXVECTOR3& vPos, float fScale)
{
	strcpy_s(m_strFilePath, MAX_CHAR, strFilePath);
	D3DXMatrixScaling(&m_matAdjust, fScale, fScale, fScale);
	m_matPlayer._41 = vPos.x; m_matPlayer._42 = vPos.y; m_matPlayer._43 = vPos.z;
	return do_Init();
}

bool CSkeletonMesh::do_Init() {
	// 加载骨骼动画
	if (!ParseFile())
		return false;

	// 挂架蒙皮到骨骼
	if (!AttachSkinMeshToFrame())
		return false;

	if (FAILED(CDirect3D::getInstance()->CreateVertexBuffer(sizeof(SVertexD)*24, 
		D3DUSAGE_WRITEONLY, SVertexD::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL)))
		return false;

	// 挂接骨骼动画到骨骼
	AttachAnimationToFrame();

	// 克隆当前骨骼
	m_pRootFrame->CloneFrame(&m_pCloneFrame);
	m_pCurAnimSet = NULL;

	return true;
}

bool CSkeletonMesh::ParseFile()
{
	if (0 >= strlen(m_strFilePath))
		return false;

	ID3DXFile* pDXFile = NULL;
	if (FAILED(D3DXFileCreate(&pDXFile)))
		return false;

	if (FAILED(pDXFile->RegisterTemplates((LPVOID)D3DRM_XTEMPLATES, D3DRM_XTEMPLATE_BYTES)))
	{
		Safe_Release(pDXFile);
		return false;
	}

	ID3DXFileEnumObject* pDXEnum = NULL;
	if (FAILED(pDXFile->CreateEnumObject(m_strFilePath, DXFILELOAD_FROMFILE, &pDXEnum)))
	{
		Safe_Release(pDXFile);
		return false;
	}

	bool bParseResult = false;
	SIZE_T iCount = 0;
	if (SUCCEEDED(pDXEnum->GetChildren(&iCount)))
	{
		ID3DXFileData* pDXData = NULL;
		for (SIZE_T i = 0; i < iCount; ++i)
		{
			if (FAILED(pDXEnum->GetChild(i, &pDXData)))
			{
				bParseResult = false;
				break;
			}

			bParseResult = ParseObject(pDXData, NULL, NULL, false);
			Safe_Release(pDXData);
			if(!bParseResult)
				break;
		}
	}

	Safe_Release(pDXEnum);
	Safe_Release(pDXFile);
	return bParseResult;
}

bool CSkeletonMesh::ParseObject(ID3DXFileData* pDataObj, ID3DXFileData* pParentDataObj, void** Data, bool bReference)
{
	GUID Type = GUID_NULL;
	GetObjectGUID(pDataObj, &Type);
	if(TID_D3DRMFrame == Type && !bReference)
	{
		// 骨骼
		SkinMeshFrame* pFrame = new SkinMeshFrame;
		strcpy_s(pFrame->strFrameName, GetObjectName(pDataObj));
		if (NULL == Data)
		{
			if (m_pRootFrame) Safe_Delete(m_pRootFrame);
			m_pRootFrame = pFrame;
			Data = (void**)&m_pRootFrame;
		}
		else
		{
			SkinMeshFrame* pParentFrame = (SkinMeshFrame*)(*Data);
			pParentFrame->vecChildFrame.push_back(pFrame);
			Data = (void**)&pFrame;
		}
	}
	else if (TID_D3DRMFrameTransformMatrix == Type && !bReference)
	{
		// 骨骼本地初始变换矩阵
		SkinMeshFrame* pParentFrame = (SkinMeshFrame*)(*Data);
		if (pParentFrame)
		{
			SIZE_T Size = 0;
			pParentFrame->matLocalMatrix = *(D3DXMATRIX*)LockObjectData(pDataObj, &Size);
			UnLockObjectData(pDataObj);
		}
	}
	else if (TID_D3DRMMesh == Type && !bReference/* && !m_vSkinMesh.size()*/)
	{
		// 蒙皮
		if (FAILED(LoadSkinMesh(pDataObj, Data)))
			return false;
	}
	else if (TID_D3DRMAnimationSet == Type)
	{
		// 骨骼动画
		AnimationSet* pAnimSet = new AnimationSet;
		ZeroMemory(pAnimSet->AnimSetName, MAX_CHAR);
		strcpy_s(pAnimSet->AnimSetName, MAX_CHAR, GetObjectName(pDataObj));
		if (0 < strlen(pAnimSet->AnimSetName))
		{
			std::string strAnimSetName = pAnimSet->AnimSetName;
			m_mapAnimSet[strAnimSetName] = pAnimSet;
			m_pCurAnimSet = pAnimSet;
		}
		else
		{
			Safe_Delete(pAnimSet);
		}
	}
	else if (TID_D3DRMAnimation == Type && NULL != m_pCurAnimSet)
	{
		// 单根运动骨骼
		Animation* pAnim = new Animation;
		m_pCurAnimSet->vecAnimations.push_back(pAnim);

		int iIndex = m_pCurAnimSet->vecAnimations.size() - 1;
		Data = (void**)&m_pCurAnimSet->vecAnimations[iIndex];
	}
	else if (TID_D3DRMFrame == Type && bReference && NULL != m_pCurAnimSet && NULL != pParentDataObj && NULL != Data)
	{
		// 设置单根运动骨骼名称
		GUID Type = GUID_NULL;
		GetObjectGUID(pParentDataObj, &Type);
		if (TID_D3DRMAnimation == Type)
		{
			Animation* pAnim = (Animation*)(*Data);
			ZeroMemory(pAnim->FrameName, MAX_CHAR);
			strcpy_s(pAnim->FrameName, MAX_CHAR, GetObjectName(pDataObj));
		}

		// 引用骨骼数据不再递归解析
		return true;
	}
	else if (TID_D3DRMAnimationKey == Type)
	{
		// 加载单根骨骼运动帧数据
		LoadAnimationKey(pDataObj, pParentDataObj, Data);
	}

	return ParseChildObjects(pDataObj, Data, bReference);
}

bool CSkeletonMesh::ParseChildObjects(ID3DXFileData* pDataObj, void** Data, bool bReference)
{
	bool bParseResult = false;
	SIZE_T iCount = 0;
	if (SUCCEEDED(pDataObj->GetChildren(&iCount)))
	{
		bParseResult = true;
		ID3DXFileData* pSubData = NULL;
		for (SIZE_T i = 0; i < iCount; ++i)
		{
			if (FAILED(pDataObj->GetChild(i, &pSubData)))
				return false;

			bParseResult = ParseObject(pSubData, pDataObj, Data, pSubData->IsReference() ? true : bReference);
			Safe_Release(pSubData);
			if (!bParseResult)
				break;
		}
	}

	return bParseResult;
}

HRESULT CSkeletonMesh::LoadSkinMesh(ID3DXFileData* pDataObj, void** Data)
{
	if (NULL == pDataObj)
		return E_FAIL;

	HRESULT hr;
	ID3DXMesh* pLoadMesh = NULL;
	ID3DXSkinInfo* pSkinInfo = NULL;
	ID3DXBuffer* pAdjacency = NULL;
	ID3DXBuffer* pBufferMaterial = NULL;
	DWORD dNumMaterial = 0;
	if(FAILED(hr = D3DXLoadSkinMeshFromXof(pDataObj, D3DXMESH_MANAGED, CDirect3D::getInstance()->GetD3D9Device(),
		&pAdjacency, &pBufferMaterial, NULL, &dNumMaterial, &pSkinInfo, &pLoadMesh)))
		return hr;

	// 不存在骨骼动画读取为子静态模型
	if (NULL == pSkinInfo || 0 >= pSkinInfo->GetNumBones()) {
		Safe_Release(pSkinInfo);
		CStaticMesh* sm = new CStaticMesh();
		SkinMeshFrame* pFrame = (SkinMeshFrame*)(*Data);
		sm->onInit(m_strFilePath, pAdjacency, pBufferMaterial, dNumMaterial, pLoadMesh, &pFrame->matFinalMatrix);
		m_vStaticMesh.push_back(sm);
		return D3D_OK;
	}

	SkinMeshContainer* pSkinMesh = new SkinMeshContainer;

	strcpy_s(pSkinMesh->strSkinMeshName, MAX_CHAR, GetObjectName(pDataObj));
	pSkinMesh->pOriginalMesh = pLoadMesh;
	pSkinMesh->pAdjacency = pAdjacency;
	pSkinMesh->pSkinInfo = pSkinInfo;
	pSkinMesh->pOriginalMesh->CloneMeshFVF(D3DXMESH_MANAGED, pSkinMesh->pOriginalMesh->GetFVF(),
		CDirect3D::getInstance()->GetD3D9Device(), &pSkinMesh->pRenderMesh);

	// 设置材质
	if (NULL != pBufferMaterial && 0 < dNumMaterial)
	{
		LPD3DXMATERIAL pMaterial = (LPD3DXMATERIAL)pBufferMaterial->GetBufferPointer();
		for (DWORD i = 0; i < dNumMaterial; ++i)
		{
			// 创建材质
			pMaterial[i].MatD3D.Ambient = pMaterial[i].MatD3D.Diffuse;
			if (0.f >= pMaterial[i].MatD3D.Power)
				pMaterial[i].MatD3D.Power = 20.f;

			pSkinMesh->m_vMaterial.push_back(pMaterial[i].MatD3D);

			// 创建贴图
			if (NULL != pMaterial[i].pTextureFilename) {
				std::string strTmp = m_strFilePath;
				int iPos = strTmp.find_last_of('\\');
				if (iPos < 0) strTmp = "";
				else strTmp = strTmp.substr(0, iPos+1);

				strTmp += pMaterial[i].pTextureFilename;
				LPDIRECT3DTEXTURE9 pTex;
				if (FAILED(D3DXCreateTextureFromFileA(CDirect3D::getInstance()->
					GetD3D9Device(), strTmp.c_str(), &pTex ))) continue;
				pSkinMesh->m_vTexture.push_back(pTex);
			}
		}
	}
	m_vSkinMesh.push_back(pSkinMesh);
	Safe_Release(pBufferMaterial);
	return D3D_OK;
}

void CSkeletonMesh::LoadAnimationKey(ID3DXFileData* pDataObj, ID3DXFileData* pParentDataObj, void** Data)
{
	if (NULL == m_pCurAnimSet || NULL == pDataObj || NULL == pParentDataObj || NULL == Data)
		return;

	GUID Type = GUID_NULL;
	GetObjectGUID(pParentDataObj, &Type);
	if (TID_D3DRMAnimation != Type)
		return;

	Animation* pAnim = (Animation*)(*Data);
	SIZE_T Size = 0;
	DWORD* pDataPtr = (DWORD*)LockObjectData(pDataObj, &Size);

	// 变换类型: 0-旋转;1-缩放;2-平移;4-矩阵
	DWORD TransformType = *pDataPtr++;

	// 关键帧数量
	DWORD dNumKeys = *pDataPtr++;
	switch (TransformType)
	{
	case 0:
		for (DWORD i = 0; i < dNumKeys; ++i)
		{
			AnimationQuaternionKey QuatKey;

			// 获取动画时间帧
			QuatKey.fTime = *pDataPtr++ / 1000.f;
			if (m_pCurAnimSet->fAnimationLength < QuatKey.fTime)
				m_pCurAnimSet->fAnimationLength = QuatKey.fTime;

			// 忽略四元数成员计数(应为4个元素)
			pDataPtr++;

			// 设置四元数
			QuatKey.quatKey.w = *(float*)pDataPtr++;
			QuatKey.quatKey.x = *(float*)pDataPtr++;
			QuatKey.quatKey.y = *(float*)pDataPtr++;
			QuatKey.quatKey.z = *(float*)pDataPtr++;
			pAnim->vecQuatKeys.push_back(QuatKey);
		}
		break;

	case 1:
		for (DWORD i = 0; i < dNumKeys; ++i)
		{
			AnimationVectorKey vecScaleKey;

			// 获取动画时间帧
			vecScaleKey.fTime = *pDataPtr++ / 1000.f;
			if (m_pCurAnimSet->fAnimationLength < vecScaleKey.fTime)
				m_pCurAnimSet->fAnimationLength = vecScaleKey.fTime;

			// 忽略缩放成员计数(应为3个元素)
			pDataPtr++;

			// 设置缩放
			vecScaleKey.vecKey = *(D3DXVECTOR3*)pDataPtr;
			pDataPtr += 3;
			pAnim->vecScaleKeys.push_back(vecScaleKey);
		}
		break;

	case 2:
		for (DWORD i = 0; i < dNumKeys; ++i)
		{
			AnimationVectorKey vecTranslationKey;

			// 获取动画时间帧
			vecTranslationKey.fTime = *pDataPtr++ / 1000.f;
			if (m_pCurAnimSet->fAnimationLength < vecTranslationKey.fTime)
				m_pCurAnimSet->fAnimationLength = vecTranslationKey.fTime;

			// 忽略平移成员计数(应为3个元素)
			pDataPtr++;

			// 设置平移数据
			vecTranslationKey.vecKey = *(D3DXVECTOR3*)pDataPtr;
			pDataPtr += 3;
			pAnim->vecTranslationKeys.push_back(vecTranslationKey);
		}
		break;

	case 4:
		for (DWORD i = 0; i < dNumKeys; ++i)
		{
			AnimationMatrixKey matKey;

			// 获取动画时间帧
			matKey.fTime = *pDataPtr++ / 1000.f;
			if (m_pCurAnimSet->fAnimationLength < matKey.fTime)
				m_pCurAnimSet->fAnimationLength = matKey.fTime;

			// 忽略矩阵成员计数(应为16个元素)
			pDataPtr++;

			// 设置矩阵数据
			matKey.matKey = *(D3DXMATRIX*)pDataPtr;
			pDataPtr += 16;
			pAnim->vecMatKeys.push_back(matKey);
		}
		break;
	}

	UnLockObjectData(pDataObj);
}

bool CSkeletonMesh::AttachSkinMeshToFrame() {
	for (unsigned int j(0); j < m_vSkinMesh.size(); j++) {
		if (NULL == m_vSkinMesh[j]->pSkinInfo) return false;
		DWORD dNumBones = m_vSkinMesh[j]->pSkinInfo->GetNumBones();
		if (dNumBones <= 0) return false;
		m_vSkinMesh[j]->pBoneMatrices = new D3DXMATRIX[dNumBones];
		for (DWORD i(0); i < dNumBones; i++) {
			const char* strName = m_vSkinMesh[j]->pSkinInfo->GetBoneName(i);
			if (!strName) continue;
			SkinMeshFrame* pFrame = m_pRootFrame->FindFrame(strName);
			if (pFrame) m_vSkinMesh[j]->vecMatFrames.push_back(&pFrame->matFinalMatrix);
			else m_vSkinMesh[j]->vecMatFrames.push_back((D3DXMATRIX*)NULL);
		}
	}
	return true;
}

void CSkeletonMesh::AttachAnimationToFrame()
{
	for (AnimSetIT it = m_mapAnimSet.begin(); m_mapAnimSet.end() != it; ++it)
	{
		int iCount = (int)it->second->vecAnimations.size();
		for (int i = 0; i < iCount; ++i)
			it->second->vecAnimations[i]->pFrame = m_pRootFrame->FindFrame(it->second->vecAnimations[i]->FrameName);
	}
}

HRESULT CSkeletonMesh::GetObjectGUID(ID3DXFileData* pDataObj, GUID* type)
{
	*type = GUID_NULL;
	if (NULL == pDataObj)
		return E_FAIL;

	return pDataObj->GetType(type);
}

char* CSkeletonMesh::GetObjectName(ID3DXFileData* pDataObj)
{
	ZeroMemory(m_strTmp, MAX_CHAR);
	if (NULL != pDataObj)
	{
		SIZE_T Size = 0;
		pDataObj->GetName(NULL, &Size);
		pDataObj->GetName(m_strTmp, &Size);
	}

	return m_strTmp;
}

LPCVOID CSkeletonMesh::LockObjectData(ID3DXFileData* pDataObj, SIZE_T* Size)
{
	if (NULL == pDataObj)
		return NULL;

	LPCVOID pData = NULL;
	pDataObj->Lock(Size, &pData);
	return pData;
}

void CSkeletonMesh::UnLockObjectData(ID3DXFileData* pDataObj)
{
	if (NULL == pDataObj)
		return;

	pDataObj->Unlock();
}

D3DXMATRIX* CSkeletonMesh::GetFrameFinalMatrix(const char* strFrameName)
{
	if (NULL == strFrameName || NULL == m_pRootFrame)
		return NULL;

	SkinMeshFrame* pFrame = m_pRootFrame->FindFrame(strFrameName);
	if (NULL == pFrame)
		return NULL;

	return &pFrame->matFinalMatrix;
}

D3DXMATRIX* CSkeletonMesh::GetFrameWorldMatrix(const char* strFrameName)
{
	static D3DXMATRIX matFrameWorld;
	D3DXMATRIX* matFinalWorld = GetFrameFinalMatrix(strFrameName);
	if (NULL == matFinalWorld)
		return NULL;

	matFrameWorld = (*matFinalWorld) * m_matWorld;
	return &matFrameWorld;
}

D3DXVECTOR3* CSkeletonMesh::GetFramePosition(const char* strFrameName)
{
	static D3DXVECTOR3 vecFramePosition;
	D3DXMATRIX* matFrameWorld = GetFrameWorldMatrix(strFrameName);
	if (NULL == matFrameWorld)
		return NULL;

	vecFramePosition.x = (*matFrameWorld)._41;
	vecFramePosition.y = (*matFrameWorld)._42;
	vecFramePosition.z = (*matFrameWorld)._43;
	return &vecFramePosition;
}

void CSkeletonMesh::AttachMesh(CStaticMesh* pMesh, const char* strFrameName) {
	pMesh->SetParentMatrix(GetFrameFinalMatrix(strFrameName));
	m_vStaticMesh.push_back(pMesh);
}

bool CSkeletonMesh::SetAction(std::string strActionName, bool bActionLoop, float fActionSpeed, float fActionBlendTime)
{
	// 没有找到对应动作
	if (m_mapAnimSet.end() == m_mapAnimSet.find(strActionName))
		return false;
	m_fCurActionSpeed = fActionSpeed;
	// 重复设置相同动作
	if (m_pCurAnimSet == m_mapAnimSet[strActionName] && !IsCurActionEnd())
		return false;

	// 记录当前动作骨骼变换
	m_pRootFrame->FillFrame(m_pCloneFrame);

	m_pCurAnimSet = m_mapAnimSet[strActionName];
	m_bCurActionLoop = bActionLoop;
	m_fCurActionElasped = 0.f;
	m_fCurActionBlendTime = fActionBlendTime;
	if (m_pCurAnimSet->fAnimationLength < m_fCurActionBlendTime)
		m_fCurActionBlendTime = m_pCurAnimSet->fAnimationLength;

	return true;
}

bool CSkeletonMesh::IsCurActionEnd()
{
	if (NULL == m_pCurAnimSet)
		return true;

	if (m_bCurActionLoop)
		return false;

	if (m_pCurAnimSet->fAnimationLength <= m_fCurActionElasped)
		return true;

	return false;
}

void CSkeletonMesh::UpdateAction(float fElapsedTime)
{
	if (NULL == m_pCurAnimSet)
		return;

	if (m_pCurAnimSet->fAnimationLength < m_fCurActionElasped)
	{
		if (m_bCurActionLoop)
			m_fCurActionElasped = m_fCurActionElasped - m_pCurAnimSet->fAnimationLength;
		else
			m_fCurActionElasped = m_pCurAnimSet->fAnimationLength;
	}

	unsigned int iKey1 = 0;
	unsigned int iKey2 = 0;
	float fScaler = 0.f;
	for (int i = 0; i < (int)m_pCurAnimSet->vecAnimations.size(); ++i)
	{
		if (NULL == m_pCurAnimSet->vecAnimations[i]->pFrame)
			continue;

		// 归一化本地骨骼变换矩阵
		static D3DXMATRIX matActionTransform;
		D3DXMatrixIdentity(&matActionTransform);

		// 计算缩放骨骼变换
		if (0 < m_pCurAnimSet->vecAnimations[i]->vecScaleKeys.size())
		{
			iKey1 = iKey2 = 0;
			for (unsigned int j = 0; j < m_pCurAnimSet->vecAnimations[i]->vecScaleKeys.size(); ++j)
			{
				if (m_pCurAnimSet->vecAnimations[i]->vecScaleKeys[j].fTime <= m_fCurActionElasped)
					iKey1 = j;
				else
					break;
			}

			iKey2 = iKey1 + 1;
			if (m_pCurAnimSet->vecAnimations[i]->vecScaleKeys.size() <= iKey2)
				iKey2 = iKey1;

			fScaler = m_fCurActionElasped - m_pCurAnimSet->vecAnimations[i]->vecScaleKeys[iKey1].fTime;
			if (0.f > fScaler || iKey1 == iKey2 || m_pCurAnimSet->vecAnimations[i]->vecScaleKeys[iKey2].fTime == m_pCurAnimSet->vecAnimations[i]->vecScaleKeys[iKey1].fTime)
				fScaler = 0.f;
			else
				fScaler /= m_pCurAnimSet->vecAnimations[i]->vecScaleKeys[iKey2].fTime - m_pCurAnimSet->vecAnimations[i]->vecScaleKeys[iKey1].fTime;

			static D3DXVECTOR3 vecScale;
			vecScale = m_pCurAnimSet->vecAnimations[i]->vecScaleKeys[iKey1].vecKey + (m_pCurAnimSet->vecAnimations[i]->vecScaleKeys[iKey2].vecKey - m_pCurAnimSet->vecAnimations[i]->vecScaleKeys[iKey1].vecKey) * fScaler;

			static D3DXMATRIX matScale;
			D3DXMatrixScaling(&matScale, vecScale.x, vecScale.y, vecScale.z);
			matActionTransform *= matScale;
		}

		// 计算旋转骨骼变换
		if (0 < m_pCurAnimSet->vecAnimations[i]->vecQuatKeys.size())
		{
			iKey1 = iKey2 = 0;
			for (unsigned int j = 0; j < m_pCurAnimSet->vecAnimations[i]->vecQuatKeys.size(); ++j)
			{
				if (m_pCurAnimSet->vecAnimations[i]->vecQuatKeys[j].fTime <= m_fCurActionElasped)
					iKey1 = j;
				else
					break;
			}

			iKey2 = iKey1 + 1;
			if (m_pCurAnimSet->vecAnimations[i]->vecQuatKeys.size() <= iKey2)
				iKey2 = iKey1;

			fScaler = m_fCurActionElasped - m_pCurAnimSet->vecAnimations[i]->vecQuatKeys[iKey1].fTime;
			if (0.f > fScaler || iKey1 == iKey2 || m_pCurAnimSet->vecAnimations[i]->vecQuatKeys[iKey2].fTime == m_pCurAnimSet->vecAnimations[i]->vecQuatKeys[iKey1].fTime)
				fScaler = 0.f;
			else
				fScaler /= m_pCurAnimSet->vecAnimations[i]->vecQuatKeys[iKey2].fTime - m_pCurAnimSet->vecAnimations[i]->vecQuatKeys[iKey1].fTime;

			static D3DXQUATERNION quatRotation;
			D3DXQuaternionSlerp(&quatRotation, &m_pCurAnimSet->vecAnimations[i]->vecQuatKeys[iKey1].quatKey, &m_pCurAnimSet->vecAnimations[i]->vecQuatKeys[iKey2].quatKey, fScaler);

			static D3DXMATRIX matRotation;

			// 四元数转换矩阵,结果为右手坐标系矩阵
			D3DXMatrixRotationQuaternion(&matRotation, &quatRotation);

			// 转置矩阵,将右手坐标系变换回左手坐标系
			D3DXMatrixTranspose(&matRotation, &matRotation);
			matActionTransform *= matRotation;
		}

		// 计算平移骨骼变换
		if (0 < m_pCurAnimSet->vecAnimations[i]->vecTranslationKeys.size())
		{
			iKey1 = iKey2 = 0;
			for (unsigned int j = 0; j < m_pCurAnimSet->vecAnimations[i]->vecTranslationKeys.size(); ++j)
			{
				if (m_pCurAnimSet->vecAnimations[i]->vecTranslationKeys[j].fTime <= m_fCurActionElasped)
					iKey1 = j;
				else
					break;
			}

			iKey2 = iKey1 + 1;
			if (m_pCurAnimSet->vecAnimations[i]->vecTranslationKeys.size() <= iKey2)
				iKey2 = iKey1;

			fScaler = m_fCurActionElasped - m_pCurAnimSet->vecAnimations[i]->vecTranslationKeys[iKey1].fTime;
			if (0.f > fScaler || iKey1 == iKey2 || m_pCurAnimSet->vecAnimations[i]->vecTranslationKeys[iKey2].fTime == m_pCurAnimSet->vecAnimations[i]->vecTranslationKeys[iKey1].fTime)
				fScaler = 0.f;
			else
				fScaler /= m_pCurAnimSet->vecAnimations[i]->vecTranslationKeys[iKey2].fTime - m_pCurAnimSet->vecAnimations[i]->vecTranslationKeys[iKey1].fTime;

			static D3DXVECTOR3 vecTranslation;
			vecTranslation = m_pCurAnimSet->vecAnimations[i]->vecTranslationKeys[iKey1].vecKey + (m_pCurAnimSet->vecAnimations[i]->vecTranslationKeys[iKey2].vecKey - m_pCurAnimSet->vecAnimations[i]->vecTranslationKeys[iKey1].vecKey) * fScaler;

			static D3DXMATRIX matTranslation;
			D3DXMatrixTranslation(&matTranslation, vecTranslation.x, vecTranslation.y, vecTranslation.z);
			matActionTransform *= matTranslation;
		}

		// 计算矩阵骨骼变换
		if (0 < m_pCurAnimSet->vecAnimations[i]->vecMatKeys.size())
		{
			iKey1 = iKey2 = 0;
			for (int j = 0; j < (int)m_pCurAnimSet->vecAnimations[i]->vecMatKeys.size(); ++j)
			{
				if (m_pCurAnimSet->vecAnimations[i]->vecMatKeys[j].fTime <= m_fCurActionElasped)
					iKey1 = j;
			}

			iKey2 = iKey1 + 1;
			if (m_pCurAnimSet->vecAnimations[i]->vecMatKeys.size() <= iKey2)
				iKey2 = iKey1;

			fScaler = m_fCurActionElasped - m_pCurAnimSet->vecAnimations[i]->vecMatKeys[iKey1].fTime;
			if (0.f > fScaler || iKey1 == iKey2 || m_pCurAnimSet->vecAnimations[i]->vecMatKeys[iKey2].fTime == m_pCurAnimSet->vecAnimations[i]->vecMatKeys[iKey1].fTime)
				fScaler = 0.f;
			else
				fScaler /= m_pCurAnimSet->vecAnimations[i]->vecMatKeys[iKey2].fTime - m_pCurAnimSet->vecAnimations[i]->vecMatKeys[iKey1].fTime;

			static D3DXMATRIX matTransform;
			matTransform = m_pCurAnimSet->vecAnimations[i]->vecMatKeys[iKey1].matKey + (m_pCurAnimSet->vecAnimations[i]->vecMatKeys[iKey2].matKey - m_pCurAnimSet->vecAnimations[i]->vecMatKeys[iKey1].matKey) * fScaler;
			matActionTransform *= matTransform;
		}

		// 混合变换矩阵
		if (NULL != m_pCloneFrame && 0.f < m_fCurActionBlendTime)
		{
			if (m_fCurActionElasped < m_fCurActionBlendTime)
			{
				SkinMeshFrame* pFrame = m_pCloneFrame->FindFrame(m_pCurAnimSet->vecAnimations[i]->pFrame->strFrameName);
				if (NULL != pFrame)
				{
					fScaler = m_fCurActionElasped /  m_fCurActionBlendTime;
					matActionTransform = pFrame->matLocalMatrix + (matActionTransform - pFrame->matLocalMatrix) * fScaler;
				}
			}
			else
			{
				// 仅由上次动作首次切换当前动作时做动作融合,如当前动作为循环动作,则不循环融合动作
				m_fCurActionBlendTime = 0.f;
			}
		}

		m_pCurAnimSet->vecAnimations[i]->pFrame->matLocalMatrix = matActionTransform;
	}

	m_fCurActionElasped += fElapsedTime * m_fCurActionSpeed;
}

bool CSkeletonMesh::onTick(float fElapsedTime)
{
	if (!m_pRootFrame || !m_vSkinMesh.size())
		return false;

	// 更新动作
	UpdateAction(fElapsedTime);

	// 更新骨骼
	m_pRootFrame->UpdateFrame(NULL);

	// 更新蒙皮
	for (SkinIT it(m_vSkinMesh.begin()); it != m_vSkinMesh.end(); it++) 
		(*it)->UpdateSkinMesh();

	m_matWorld = m_matAdjust * m_matPlayer;

	return true;
}

void CSkeletonMesh::UpdateBoundingBox(const D3DXVECTOR3* vMinOffset, const D3DXVECTOR3* vMaxOffset) {
	if (!m_vSkinMesh.size()) return;
	m_vMin = D3DXVECTOR3(1e6f, 1e6f, 1e6f);
	m_vMax = D3DXVECTOR3(-1e6f, -1e6f, -1e6f);
	/* copy the original info *
	D3DXVECTOR3* pVertices;
	ID3DXMesh* pMesh;
	m_vSkinMesh[0]->pRenderMesh->CloneMeshFVF(D3DXMESH_MANAGED, D3DFVF_XYZ, 
		CDirect3D::getInstance()->GetD3D9Device(), &pMesh);
	pMesh->LockVertexBuffer(0, (void**) &pVertices);
	for (unsigned int i(0); i < pMesh->GetNumVertices(); i++) {
			if (pVertices[i].x < m_vMin.x) m_vMin.x = pVertices[i].x; 
			if (pVertices[i].y < m_vMin.y) m_vMin.y = pVertices[i].y; 
			if (pVertices[i].z < m_vMin.z) m_vMin.z = pVertices[i].z; 
			if (pVertices[i].x > m_vMax.x) m_vMax.x = pVertices[i].x;
			if (pVertices[i].y > m_vMax.y) m_vMax.y = pVertices[i].y;
			if (pVertices[i].z > m_vMax.z) m_vMax.z = pVertices[i].z;
	}
	pMesh->UnlockVertexBuffer();
	Safe_Release(pMesh);
	/* read in place */
	char* pVertices;
	DWORD bytes(m_vSkinMesh[0]->pRenderMesh->GetNumBytesPerVertex());
	m_vSkinMesh[0]->pRenderMesh->LockVertexBuffer(0, (void**) &pVertices);
	for (unsigned int i(0); i < m_vSkinMesh[0]->pRenderMesh->GetNumVertices(); 
			i++, pVertices += bytes) {
		float x(*(float*)pVertices), y(*(float*)(pVertices+4)), z(*(float*)(pVertices+8));
		if (x < m_vMin.x) m_vMin.x = x; 
		if (y < m_vMin.y) m_vMin.y = y; 
		if (z < m_vMin.z) m_vMin.z = z; 
		if (x > m_vMax.x) m_vMax.x = x;
		if (y > m_vMax.y) m_vMax.y = y;
		if (z > m_vMax.z) m_vMax.z = z;
	}
	m_vSkinMesh[0]->pRenderMesh->UnlockVertexBuffer();
	/**/
	if (vMinOffset && vMaxOffset) { m_vMin += *vMinOffset; m_vMax += *vMaxOffset; }

	// Update rendering data
	SVertexD *pVertices2; unsigned int iCnt(0);
	m_pVB->Lock(0, sizeof(SVertexD)*24, (void**)&pVertices2, 0);
	writeLine(pVertices2, iCnt, m_vMin, D3DXVECTOR3(m_vMin.x, m_vMin.y, m_vMax.z));
	writeLine(pVertices2, iCnt, m_vMin, D3DXVECTOR3(m_vMin.x, m_vMax.y, m_vMin.z));
	writeLine(pVertices2, iCnt, m_vMin, D3DXVECTOR3(m_vMax.x, m_vMin.y, m_vMin.z));
	writeLine(pVertices2, iCnt, m_vMax, D3DXVECTOR3(m_vMax.x, m_vMax.y, m_vMin.z));
	writeLine(pVertices2, iCnt, m_vMax, D3DXVECTOR3(m_vMax.x, m_vMin.y, m_vMax.z));
	writeLine(pVertices2, iCnt, m_vMax, D3DXVECTOR3(m_vMin.x, m_vMax.y, m_vMax.z));
	writeLine(pVertices2, iCnt, D3DXVECTOR3(m_vMin.x, m_vMin.y, m_vMax.z), D3DXVECTOR3(m_vMin.x, m_vMax.y, m_vMax.z));
	writeLine(pVertices2, iCnt, D3DXVECTOR3(m_vMin.x, m_vMin.y, m_vMax.z), D3DXVECTOR3(m_vMax.x, m_vMin.y, m_vMax.z));
	writeLine(pVertices2, iCnt, D3DXVECTOR3(m_vMin.x, m_vMax.y, m_vMin.z), D3DXVECTOR3(m_vMin.x, m_vMax.y, m_vMax.z));
	writeLine(pVertices2, iCnt, D3DXVECTOR3(m_vMin.x, m_vMax.y, m_vMin.z), D3DXVECTOR3(m_vMax.x, m_vMax.y, m_vMin.z));
	writeLine(pVertices2, iCnt, D3DXVECTOR3(m_vMax.x, m_vMin.y, m_vMin.z), D3DXVECTOR3(m_vMax.x, m_vMin.y, m_vMax.z));
	writeLine(pVertices2, iCnt, D3DXVECTOR3(m_vMax.x, m_vMin.y, m_vMin.z), D3DXVECTOR3(m_vMax.x, m_vMax.y, m_vMin.z));
	m_pVB->Unlock();
}

void CSkeletonMesh::writeLine(SVertexD* pVertices, unsigned int& iCnt, 
		const D3DXVECTOR3& p, const D3DXVECTOR3& q) {
	pVertices[iCnt++] = SVertexD(p.x, p.y, p.z, BB_COLOR);
	pVertices[iCnt++] = SVertexD(q.x, q.y, q.z, BB_COLOR);
}

bool CSkeletonMesh::Intersect(const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir) {
	D3DXVECTOR3 vInv(1.f / vDir.x, 1.f / vDir.y, 1.f / vDir.z);
	D3DXVECTOR3 vMin, vMax;
	D3DXVec3TransformCoord(&vMin, &m_vMin, &m_matWorld);
	D3DXVec3TransformCoord(&vMax, &m_vMax, &m_matWorld);
	float t1 = (vMin.x - vPos.x) * vInv.x;
	float t2 = (vMax.x - vPos.x) * vInv.x;
	float t3 = (vMin.y - vPos.y) * vInv.y;
	float t4 = (vMax.y - vPos.y) * vInv.y;
	float t5 = (vMin.z - vPos.z) * vInv.z;
	float t6 = (vMax.z - vPos.z) * vInv.z;
	float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));
	if (tmax < 0 || tmin > tmax) return false;
	return true;
}

void CSkeletonMesh::onRender(const D3DXMATRIX* matMirror)
{
	if (!m_pRootFrame || !m_vSkinMesh.size() || !m_vSkinMesh[0]->pRenderMesh)
		return;

	if (matMirror) {
		CDirect3D::getInstance()->SetTransform(D3DTS_WORLD, &(m_matWorld* *matMirror));
		CDirect3D::getInstance()->SetCullMode(D3DCULL_CW);
		CDirect3D::getInstance()->StencilEnable(TRUE);
		//CDirect3D::getInstance()->AlphaBlendEnable(TRUE);
		//CDirect3D::getInstance()->SetSrcRenderBlendFactor(D3DBLEND_DESTCOLOR);
		//CDirect3D::getInstance()->SetDestRenderBlendFactor(D3DBLEND_ZERO);
	} else 
		CDirect3D::getInstance()->SetTransform(D3DTS_WORLD, &m_matWorld);

	for (SkinIT it(m_vSkinMesh.begin()); it != m_vSkinMesh.end(); it++) {
		for (unsigned int i = 0; i < (*it)->m_vMaterial.size(); i++) {
			CDirect3D::getInstance()->SetMaterial(&(*it)->m_vMaterial[i]);
			if ((*it)->m_vTexture.size() > i) 
				CDirect3D::getInstance()->SetTexture(0, (*it)->m_vTexture[i]);
			else CDirect3D::getInstance()->SetTexture(0, NULL);
			(*it)->pRenderMesh->DrawSubset(i);
		}
	}

	for (MeshIT it = m_vStaticMesh.begin(); m_vStaticMesh.end() != it; it++)
		(*it)->onRender(matMirror?&(m_matWorld* *matMirror):&m_matWorld);

	if (m_bDebug) {
		CDirect3D::getInstance()->SetD3DFVF(SVertexD::FVF);
		CDirect3D::getInstance()->SetStreamSource(0, m_pVB, 0, sizeof(SVertexD));
		CDirect3D::getInstance()->DrawPrimitive(D3DPT_LINELIST, 0, 12);
	}

	CDirect3D::getInstance()->SetTexture(0, NULL);
	if (matMirror) {
		//CDirect3D::getInstance()->AlphaBlendEnable(FALSE);
		CDirect3D::getInstance()->StencilEnable(FALSE);
		CDirect3D::getInstance()->SetCullMode(D3DCULL_CCW);
	}
}