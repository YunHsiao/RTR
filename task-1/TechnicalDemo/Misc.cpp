#include "..\Engine\Utility.h"
#include "..\Engine\Direct3D.h"
#include "Misc.h"

TDMisc::TDMisc()
:m_pVB(NULL)
,m_pIB(NULL)
,m_pTeapot(NULL)
{
	D3DXMatrixRotationX(&m_matId, D3DX_PI);
	m_matId._42 = 125.f*1.1f;
	D3DXMatrixRotationY(&m_matTeapot, -D3DX_PI);
	m_matTeapot._42 = 3.f;
}

TDMisc::~TDMisc()
{
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
	Safe_Release(m_pTeapot);
}

bool TDMisc::onInit()
{	
	D3DXCreateTeapot(CDirect3D::getInstance()->GetD3D9Device(), &m_pTeapot, NULL);
	ZeroMemory(&m_material, sizeof(D3DMATERIAL9));
	ZeroMemory(&m_light, sizeof(D3DLIGHT9));
	m_material.Ambient.r = m_material.Ambient.g = m_material.Ambient.b = .5f;
	m_material.Diffuse.r = m_material.Diffuse.g = m_material.Diffuse.b = .5f;
	m_material.Specular.a = m_material.Specular.r = m_material.Specular.g = m_material.Specular.b = .8f;
	m_material.Power = .9f;
	m_light.Type = D3DLIGHT_SPOT;
	m_light.Diffuse.r = 1.f;
	m_light.Falloff = 1.f;
	m_light.Attenuation0 = 1.f;	
	m_light.Theta = .2f;
	m_light.Phi = .6f;
	m_light.Range = 10.f;
	memcpy_s(&m_light2, sizeof(D3DLIGHT9), &m_light, sizeof(D3DLIGHT9));
	m_light2.Diffuse.r = 0.f;
	m_light2.Diffuse.b = 1.f;

	if( FAILED( CDirect3D::getInstance()->CreateVertexBuffer( sizeof(SVertexD)*644,
		D3DUSAGE_WRITEONLY, SVertexD::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL ) ) )
		return false;
	SVertexD* pVertices;
	if( FAILED( m_pVB->Lock( 0, sizeof(SVertexD)*44, (void**)&pVertices, 0 ) ) )
		return false;
	// Left Cube:
	// Top
	pVertices[0] = SVertexD(-1.5f, 2.f, -.5f, 0xffff0000);
	pVertices[1] = SVertexD(-1.5f, 2.f, .5f, 0xff00ff00);
	pVertices[2] = SVertexD(-.5f, 2.f, -.5f, 0xff0000ff);
	pVertices[3] = SVertexD(-.5f, 2.f, -.5f, 0xff0000ff);
	pVertices[4] = SVertexD(-1.5f, 2.f, .5f, 0xff00ff00);
	pVertices[5] = SVertexD(-.5f, 2.f, .5f, 0xffffff00);
	// Front
	pVertices[6] = SVertexD(-1.5f, 1.f, -.5f, 0xffff00ff);
	pVertices[7] = SVertexD(-1.5f, 2.f, -.5f, 0xffff0000);
	pVertices[8] = SVertexD(-.5f, 1.f, -.5f, 0xff00ffff);
	pVertices[9] = SVertexD(-.5f, 1.f, -.5f, 0xff00ffff);
	pVertices[10] = SVertexD(-1.5f, 2.f, -.5f, 0xffff0000);
	pVertices[11] = SVertexD(-.5f, 2.f, -.5f, 0xff0000ff);
	// Bottom
	pVertices[12] = SVertexD(-1.5f, 1.f, .5f, 0xff000000);
	pVertices[13] = SVertexD(-1.5f, 1.f, -.5f, 0xffff00ff);
	pVertices[14] = SVertexD(-.5f, 1.f, .5f, 0xffffffff);
	pVertices[15] = SVertexD(-.5f, 1.f, .5f, 0xffffffff);
	pVertices[16] = SVertexD(-1.5f, 1.f, -.5f, 0xffff00ff);
	pVertices[17] = SVertexD(-.5f, 1.f, -.5f, 0xff00ffff);
	// Back
	pVertices[18] = SVertexD(-.5f, 1.f, .5f, 0xffffffff);
	pVertices[19] = SVertexD(-.5f, 2.f, .5f, 0xffffff00);
	pVertices[20] = SVertexD(-1.5f, 1.f, .5f, 0xff000000);
	pVertices[21] = SVertexD(-1.5f, 1.f, .5f, 0xff000000);
	pVertices[22] = SVertexD(-.5f, 2.f, .5f, 0xffffff00);
	pVertices[23] = SVertexD(-1.5f, 2.f, .5f, 0xff00ff00);
	// Left
	pVertices[24] = SVertexD(-1.5f, 1.f, .5f, 0xff000000);
	pVertices[25] = SVertexD(-1.5f, 2.f, .5f, 0xff00ff00);
	pVertices[26] = SVertexD(-1.5f, 1.f, -.5f, 0xffff00ff);
	pVertices[27] = SVertexD(-1.5f, 1.f, -.5f, 0xffff00ff);
	pVertices[28] = SVertexD(-1.5f, 2.f, .5f, 0xff00ff00);
	pVertices[29] = SVertexD(-1.5f, 2.f, -.5f, 0xffff0000);
	// Right
	pVertices[30] = SVertexD(-.5f, 1.f, -.5f, 0xff00ffff);
	pVertices[31] = SVertexD(-.5f, 2.f, -.5f, 0xff0000ff);
	pVertices[32] = SVertexD(-.5f, 1.f, .5f, 0xffffffff);
	pVertices[33] = SVertexD(-.5f, 1.f, .5f, 0xffffffff);
	pVertices[34] = SVertexD(-.5f, 2.f, -.5f, 0xff0000ff);
	pVertices[35] = SVertexD(-.5f, 2.f, .5f, 0xffffff00);
	// Right Cube:
	// All Vertices
	pVertices[36] = SVertexD(.5f, 2.f, -.5f, 0xff954600);
	pVertices[37] = SVertexD(.5f, 2.f, .5f, 0xffffd3ad);
	pVertices[38] = SVertexD(1.5f, 2.f, -.5f, 0xffb7c7f4);
	pVertices[39] = SVertexD(1.5f, 2.f, .5f, 0xff021a5b);
	pVertices[40] = SVertexD(.5f, 1.f, -.5f, 0xff00690d);
	pVertices[41] = SVertexD(.5f, 1.f, .5f, 0xff9ce6a5);
	pVertices[42] = SVertexD(1.5f, 1.f, -.5f, 0xffc49062);
	pVertices[43] = SVertexD(1.5f, 1.f, .5f, 0xff495984);
	m_pVB->Unlock();

	if( FAILED( CDirect3D::getInstance()->CreateIndexBuffer( sizeof(DWORD)*36,
		D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &m_pIB, NULL ) ) )
		return false;

	DWORD* pIndices;
	if( FAILED( m_pIB->Lock( 0, sizeof(DWORD)*36, (void**)&pIndices, 0 ) ) )
		return false;
	// Top
	pIndices[0] = 0; pIndices[1] = 1; pIndices[2] = 2; 
	pIndices[3] = 2; pIndices[4] = 1; pIndices[5] = 3;
	// Front
	pIndices[6] = 4; pIndices[7] = 0; pIndices[8] = 6;
	pIndices[9] = 6; pIndices[10] = 0; pIndices[11] = 2;
	// Bottom
	pIndices[12] = 5; pIndices[13] = 4; pIndices[14] = 7;
	pIndices[15] = 7; pIndices[16] = 4; pIndices[17] = 6;
	// Back
	pIndices[18] = 7; pIndices[19] = 3; pIndices[20] = 5;
	pIndices[21] = 5; pIndices[22] = 3; pIndices[23] = 1;
	// Left
	pIndices[24] = 5; pIndices[25] = 1; pIndices[26] = 4;
	pIndices[27] = 4; pIndices[28] = 1; pIndices[29] = 0;
	// Right
	pIndices[30] = 6; pIndices[31] = 2; pIndices[32] = 7;
	pIndices[33] = 7; pIndices[34] = 2; pIndices[35] = 3;
	m_pIB->Unlock();

	for (UINT i(0); i <= 10; i++)
		for (UINT j(0); j <= 10; j++) 
			m_grid[i][j] = SVertexD(i-5.f, -2.f, j-5.f, 
			(j+i)%2 ? 0xffffffff : 0xff000000);

	return true;
}

void TDMisc::onTick(float fElapsedTime) {
	static float strench(1.f), acc(0.f);
	strench = sin(acc);
	acc += fElapsedTime;
	SVertexD* pVertices;
	m_pVB->Lock(0, sizeof(SVertexD) * 644, (void**)&pVertices, 0);
	UINT cnt(44);
	for (UINT i(0); i < 10; i++) {
		for (UINT j(0); j < 10; j++) {
			pVertices[cnt++] = m_grid[i][j] + m_grid[i][j] * strench;
			pVertices[cnt++] = m_grid[i][j + 1] + m_grid[i][j] * strench;
			pVertices[cnt++] = m_grid[i + 1][j] + m_grid[i][j] * strench;
			pVertices[cnt++] = m_grid[i][j + 1] + m_grid[i][j] * strench;
			pVertices[cnt++] = m_grid[i + 1][j + 1] + m_grid[i][j] * strench;
			pVertices[cnt++] = m_grid[i + 1][j] + m_grid[i][j] * strench;
		}
	}
	m_pVB->Unlock();
	static float fAngle(0.f);
	float sn(sin(fAngle)), cs(cos(fAngle));
	D3DXMatrixRotationX(&m_matLeft, fAngle);
	m_matLeft._42 = 1.5f - 1.5f * cs;
	m_matLeft._43 = -1.5f * sn;
	D3DXMatrixRotationY(&m_matRight, fAngle);
	m_matRight._41 = 1-cs;
	m_matRight._43 = sn;
	fAngle += fElapsedTime;
	if (fAngle > D3DX_PI*2) fAngle -= D3DX_PI*2;

	D3DXVECTOR3 vDir(4*sn, -4.5f, 4*cs), vDir2(-4*sn, -4.5f, -4*cs);
	D3DXVECTOR3 vPos(m_matTeapot._41, m_matTeapot._42, m_matTeapot._43);
	m_light.Position = vPos - vDir; m_light2.Position = vPos - vDir2;
	D3DXVec3Normalize(&vDir, &vDir); D3DXVec3Normalize(&vDir2, &vDir2);
	m_light.Direction = vDir; m_light2.Direction = vDir2;
}

void TDMisc::onRender() {
	CDirect3D::getInstance()->SetD3DFVF(SVertexD::FVF);
	CDirect3D::getInstance()->SetStreamSource(0, m_pVB, 0, sizeof(SVertexD));
	CDirect3D::getInstance()->SetIndices(m_pIB);
	CDirect3D::getInstance()->SetTransform(D3DTS_WORLD, &m_matLeft);
	CDirect3D::getInstance()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 12);
	CDirect3D::getInstance()->SetTransform(D3DTS_WORLD, &m_matRight);
	CDirect3D::getInstance()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 36, 0, 8, 0, 12);
	CDirect3D::getInstance()->SetTransform(D3DTS_WORLD, &m_matId);
	CDirect3D::getInstance()->DrawPrimitive(D3DPT_TRIANGLELIST, 44, 200);

	CDirect3D::getInstance()->SetTransform(D3DTS_WORLD, &m_matTeapot);
	CDirect3D::getInstance()->D3DLightEnable(TRUE);
	CDirect3D::getInstance()->SetLight(0, &m_light);
	CDirect3D::getInstance()->LightEnable(0, TRUE);
	CDirect3D::getInstance()->SetLight(1, &m_light2);
	CDirect3D::getInstance()->LightEnable(1, TRUE);
	CDirect3D::getInstance()->SetMaterial(&m_material);
	m_pTeapot->DrawSubset(0);
	CDirect3D::getInstance()->D3DLightEnable(FALSE);
}