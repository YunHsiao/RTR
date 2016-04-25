#include "Utility.h"
#include "Direct3D.h"
#include "StaticMesh.h"

CStaticMesh::CStaticMesh():
m_pStaticMesh(NULL),
m_pAdjacency(NULL),
m_pMatParent(NULL)
{
	D3DXMatrixIdentity(&m_matId);
}

CStaticMesh::~CStaticMesh()
{
	Safe_Release(m_pAdjacency);
	Safe_Release(m_pStaticMesh);
	for (TextureIT it = m_mapTexture.begin(); m_mapTexture.end() != it; it++)
		Safe_Release(it->second);
	m_mapTexture.clear();
}


bool CStaticMesh::onInit(const char* strFilePath, D3DXVECTOR3& vPos, float fScale, 
			D3DXVECTOR3& vRot, float fAngle) {
	D3DXMATRIX mSca, mRot, mTra;
	D3DXMatrixScaling(&mSca, fScale, fScale, fScale);
	D3DXMatrixRotationAxis(&mRot, &vRot, fAngle);
	D3DXMatrixTranslation(&mTra, vPos.x, vPos.y, vPos.z);
	m_matId = mSca * mRot * mTra;
	ID3DXBuffer *pAdjacency, *pBufferMaterial;
	DWORD dNumMaterial;
	ID3DXMesh *pStaticMesh(NULL);
	D3DXLoadMeshFromXA(strFilePath, D3DXMESH_MANAGED, CDirect3D::getInstance()->GetD3D9Device(), 
		&pAdjacency, &pBufferMaterial, NULL, &dNumMaterial, &pStaticMesh);
	return onInit(strFilePath, pAdjacency, pBufferMaterial, dNumMaterial, pStaticMesh, NULL);
}

bool CStaticMesh::onInit(const char* strFilePath, ID3DXBuffer* pAdjacency, ID3DXBuffer* pBufferMaterial, 
							 DWORD dNumMaterial, ID3DXMesh* pStaticMesh, D3DXMATRIX* pMatParent)
{
	if (!pStaticMesh) return false;
	m_pAdjacency = pAdjacency;
	m_pStaticMesh = pStaticMesh;
	m_pMatParent = pMatParent;
	CreateNormal();

	if (NULL != pBufferMaterial && 0 < dNumMaterial) {
		LPD3DXMATERIAL pMaterial = (LPD3DXMATERIAL)pBufferMaterial->GetBufferPointer();
		for (DWORD i = 0; i < dNumMaterial; ++i) {
			pMaterial[i].MatD3D.Ambient = pMaterial[i].MatD3D.Diffuse;
			if (0.f >= pMaterial[i].MatD3D.Power)
				pMaterial[i].MatD3D.Power = 20.f;
			m_mapMaterial[i] = pMaterial[i].MatD3D;

			if (NULL != pMaterial[i].pTextureFilename) {
				std::string strTmp = strFilePath;
				int iPos = strTmp.find_last_of('\\');
				if (iPos < 0) strTmp = "";
				else strTmp = strTmp.substr(0, iPos+1);

				strTmp += pMaterial[i].pTextureFilename;
				D3DXCreateTextureFromFileA( CDirect3D::getInstance()->GetD3D9Device(), strTmp.c_str(), &m_mapTexture[i] );
			} else {
				D3DXCreateTextureFromFile( CDirect3D::getInstance()->GetD3D9Device(), TEXT("res\\def.jpg"), &m_mapTexture[i] );
			}
		}
	}

	Safe_Release(pBufferMaterial);
	return true;
}

bool CStaticMesh::onInit(const char* strFilePath, D3DXVECTOR3& vPos, 
		float fScale, float fYaw, float fPitch, float fRoll) {
	D3DXMATRIX mSca, mRot, mTra;
	D3DXMatrixScaling(&mSca, fScale, fScale, fScale);
	D3DXMatrixRotationYawPitchRoll(&mRot, fYaw, fPitch, fRoll);
	D3DXMatrixTranslation(&mTra, vPos.x, vPos.y, vPos.z);
	m_matId = mSca * mRot * mTra;
	ID3DXBuffer *pAdjacency, *pBufferMaterial;
	DWORD dNumMaterial;
	ID3DXMesh *pStaticMesh;
	D3DXLoadMeshFromXA(strFilePath, D3DXMESH_MANAGED, CDirect3D::getInstance()->GetD3D9Device(), 
		&pAdjacency, &pBufferMaterial, NULL, &dNumMaterial, &pStaticMesh);
	return onInit(strFilePath, pAdjacency, pBufferMaterial, dNumMaterial, pStaticMesh, NULL);
}

void CStaticMesh::CreateNormal()
{
	if (NULL == m_pStaticMesh)
		return;

	if (D3DFVF_NORMAL & m_pStaticMesh->GetFVF())
		return;

	ID3DXMesh* pNormalMesh;
	m_pStaticMesh->CloneMeshFVF(D3DXMESH_MANAGED, m_pStaticMesh->GetFVF() | D3DFVF_NORMAL, CDirect3D::getInstance()->GetD3D9Device(), &pNormalMesh);
	D3DXComputeNormals(pNormalMesh, NULL != m_pAdjacency ? (DWORD*)m_pAdjacency->GetBufferPointer() : NULL);

	Safe_Release(m_pStaticMesh);
	m_pStaticMesh = pNormalMesh;
}

void CStaticMesh::onRender(D3DXMATRIX* mat)
{
	if (NULL == m_pStaticMesh)
		return;

	if (m_pMatParent && mat)
		CDirect3D::getInstance()->SetTransform(D3DTS_WORLD, &(m_matId * *m_pMatParent * *mat));
	else 
		CDirect3D::getInstance()->SetTransform(D3DTS_WORLD, &m_matId);

	for (int i = 0; i < (int)m_mapMaterial.size(); ++i) {
		CDirect3D::getInstance()->SetMaterial(&m_mapMaterial[i]);
		if (m_mapTexture[i]) CDirect3D::getInstance()->SetTexture(0, m_mapTexture[i]);
		else CDirect3D::getInstance()->SetTexture(0, NULL);
		m_pStaticMesh->DrawSubset(i);
	}
	CDirect3D::getInstance()->SetTexture(0, NULL);
}

DWORD CStaticMesh::GetFVF()
{
	if (NULL == m_pStaticMesh)
		return 0;

	return m_pStaticMesh->GetFVF();
}

DWORD CStaticMesh::GetNumBytesPerVertex()
{
	if (NULL == m_pStaticMesh)
		return 0;

	return m_pStaticMesh->GetNumBytesPerVertex();
}

DWORD CStaticMesh::GetNumFaces()
{
	if (NULL == m_pStaticMesh)
		return 0;

	return m_pStaticMesh->GetNumFaces();
}

DWORD CStaticMesh::GetNumVertices()
{
	if (NULL == m_pStaticMesh)
		return 0;

	return m_pStaticMesh->GetNumVertices();
}

LPDIRECT3DVERTEXBUFFER9 CStaticMesh::GetVertexBuffer()
{
	if (NULL == m_pStaticMesh)
		return NULL;

	LPDIRECT3DVERTEXBUFFER9 pVB = NULL;
	m_pStaticMesh->GetVertexBuffer(&pVB);
	return pVB;
}

LPDIRECT3DINDEXBUFFER9 CStaticMesh::GetIndexBuffer()
{
	if (NULL == m_pStaticMesh)
		return NULL;

	LPDIRECT3DINDEXBUFFER9 pIB = NULL;
	m_pStaticMesh->GetIndexBuffer(&pIB);
	return pIB;
}