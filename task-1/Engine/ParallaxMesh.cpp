#include "Utility.h"
#include "Direct3D.h"
#include "ParallaxMesh.h"

CParallaxMesh::CParallaxMesh():
m_pStaticMesh(NULL),
m_pEffect(NULL),
m_pTexture(NULL),
m_pNH(NULL),
m_hEffect(NULL),
m_hWorldMatrix(NULL),
m_hViewMatrix(NULL),
m_hWorldViewProjMatrix(NULL),
m_hCameraPosition(NULL),
m_hTex(NULL),
m_hTexNH(NULL),
m_hDepth(NULL),
m_fDepth(.2f),
m_bShadows(TRUE),
m_fTiling(1.f)
{
	D3DXMatrixIdentity(&m_matWorld);
}

CParallaxMesh::~CParallaxMesh()
{
	Safe_Release(m_pEffect);
	Safe_Release(m_pStaticMesh);
	Safe_Release(m_pTexture);
	Safe_Release(m_pNH);
}

bool CParallaxMesh::onInit(LPCTSTR strMesh, LPCTSTR strTex, LPCTSTR strNH)
{
	DWORD dNumMaterial = 0;
	ID3DXBuffer *pAdjacency;
	if (FAILED(D3DXLoadMeshFromX(strMesh, D3DXMESH_MANAGED, CDirect3D::getInstance()->GetD3D9Device(),
		&pAdjacency, NULL, NULL, &dNumMaterial, &m_pStaticMesh)))
		return false;

	D3DVERTEXELEMENT9 decl[] = 
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
		{0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
		D3DDECL_END()
	};

	ID3DXMesh *pRemapMesh(NULL);
	if (FAILED(m_pStaticMesh->CloneMesh(D3DXMESH_MANAGED, decl, 
		CDirect3D::getInstance()->GetD3D9Device(), &pRemapMesh)))
		return false;
	
	// Generate Tangent Information
	D3DXComputeTangentFrameEx(pRemapMesh, D3DDECLUSAGE_TEXCOORD, 0,   
		D3DDECLUSAGE_TANGENT, 0, D3DDECLUSAGE_BINORMAL, 0,  
		D3DDECLUSAGE_NORMAL, 0, D3DXTANGENT_GENERATE_IN_PLACE,
		pAdjacency ? (DWORD*) pAdjacency->GetBufferPointer() : NULL,
		-1.01f, -0.01f, -1.01f, NULL, NULL);

	Safe_Release(pAdjacency);
	Safe_Release(m_pStaticMesh);
	m_pStaticMesh = pRemapMesh;

	if (FAILED(D3DXCreateTextureFromFileEx(CDirect3D::getInstance()->GetD3D9Device(),
		strTex, 256, 256, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR, 0, NULL, NULL, &m_pTexture)))
		return false;

	if (FAILED(D3DXCreateTextureFromFileEx(CDirect3D::getInstance()->GetD3D9Device(),
		strNH, 256, 256, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR, 0, NULL, NULL, &m_pNH)))
		return false;

	if (FAILED(CDirect3D::getInstance()->CompileEffectFromFile("res\\effect\\parallax.fx", &m_pEffect)))
		return false;

	m_hEffect = m_pEffect->GetTechniqueByName("POM");
	m_hWorldMatrix = m_pEffect->GetParameterByName(0, "g_mWorld");
	m_hViewMatrix = m_pEffect->GetParameterByName(0, "g_mView");
	m_hWorldViewProjMatrix = m_pEffect->GetParameterByName(0, "g_mWorldViewProj");
	m_hCameraPosition = m_pEffect->GetParameterByName(0, "g_vEye");
	m_hTex = m_pEffect->GetParameterByName(0, "tex");
	m_hTexNH = m_pEffect->GetParameterByName(0, "texNH");
	m_hShadows = m_pEffect->GetParameterByName(0, "g_bShadows");
	m_hDepth = m_pEffect->GetParameterByName(0, "g_fDepth");
	m_hTiling = m_pEffect->GetParameterByName(0, "g_fTiling");
	return true;
}

void CParallaxMesh::onRender(const D3DXMATRIX& matView, const D3DXMATRIX& matProj, const D3DXVECTOR3& vPos)
{
	if (NULL == m_pStaticMesh) return;

	static D3DXMATRIX matWorldViewProj;
	matWorldViewProj = m_matWorld * matView * matProj;
	m_pEffect->SetTechnique(m_hEffect);
	m_pEffect->SetMatrix(m_hWorldMatrix, &m_matWorld);
	m_pEffect->SetMatrix(m_hViewMatrix, &matView);
	m_pEffect->SetMatrix(m_hWorldViewProjMatrix, &matWorldViewProj);
	m_pEffect->SetValue(m_hCameraPosition, &vPos, sizeof(D3DXVECTOR3));
	m_pEffect->SetFloat(m_hDepth, m_fDepth);
	m_pEffect->SetBool(m_hShadows, m_bShadows);
	m_pEffect->SetFloat(m_hTiling, m_fTiling);
	m_pEffect->SetTexture(m_hTex, m_pTexture);
	m_pEffect->SetTexture(m_hTexNH, m_pNH);

	UINT uiPassCount = 0;
	m_pEffect->Begin(&uiPassCount, 0);
	for (UINT i = 0; i < uiPassCount; ++i) {
		m_pEffect->BeginPass(i);
		m_pStaticMesh->DrawSubset(i);
		m_pEffect->EndPass();
	}
	m_pEffect->End();
}