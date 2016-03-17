#include "Utility.h"
#include "Direct3D.h"
#include "SkyDome.h"

const float CSkyDome::s_fR = D3DX_PI / 180.0f;

CSkyDome::CSkyDome()
:m_pVB(NULL)
,m_pTexture(NULL)
,m_info(NULL)
,m_matDebug(NULL)
,m_iNumVertices(0)
,m_fAngle(0.f)
,m_bDebug(false)
,m_bAlpha(true)
{
}

CSkyDome::~CSkyDome() {
	Safe_Release(m_pTexture);
	Safe_Release(m_pVB);
	if (m_matDebug) Safe_Delete_Array(m_matDebug);
	if (m_info) {
		for (int i(0); i < m_iNumVertices; i++) 
			Safe_Release(m_info[i]);
		Safe_Delete_Array(m_info);
	}
}

VOID CSkyDome::onInit(LPCTSTR strTex, float fRadius)
{   
	int dtheta(15), dphi(15);
	m_iNumVertices = (360/dtheta) * (90/dphi) * 4;
	m_vSun = D3DXVECTOR3(0.f, fRadius, 0.f); // noon

	D3DXCreateTextureFromFile(CDirect3D::getInstance()->GetD3D9Device(), 
		strTex, &m_pTexture);
	if(FAILED(CDirect3D::getInstance()->CreateVertexBuffer(sizeof(SVertexT)*m_iNumVertices, 
		D3DUSAGE_WRITEONLY, SVertexT::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL))) 
		return; 
	SVertexT* pVertices;
	m_pVB->Lock(0, 0, (void**) &pVertices, 0);
	int n(0);
	for (int phi(0); phi < 90; phi += dphi) {
		for (int theta(0); theta < 360; theta += dtheta) {
			pVertices[n++] = SVertexT(
				fRadius * sinf(phi * s_fR) * cosf(theta * s_fR), 
				fRadius * cosf(phi * s_fR), 
				fRadius * sinf(phi * s_fR) * sinf(theta * s_fR), fRadius);
			pVertices[n++] = SVertexT(
				fRadius * sinf((phi + dphi) * s_fR) * cosf(theta * s_fR),
				fRadius * cosf((phi + dphi) * s_fR),
				fRadius * sinf((phi + dphi) * s_fR) * sinf(theta * s_fR), fRadius);
			pVertices[n++] = SVertexT(
				fRadius * sinf(phi * s_fR) * cosf((theta + dtheta) * s_fR),
				fRadius * cosf(phi * s_fR),
				fRadius * sinf(phi * s_fR) * sinf((theta + dtheta) * s_fR), fRadius);
			pVertices[n++] = SVertexT(
				fRadius * sinf((phi + dphi) * s_fR) * cosf((theta + dtheta) * s_fR),
				fRadius * cosf((phi + dphi) * s_fR),
				fRadius * sinf((phi + dphi) * s_fR) * sinf((theta + dtheta) * s_fR), fRadius);
		}
	}
	m_pVB->Unlock();
}

void CSkyDome::InitDebugInfo() {
	if (m_info) return;
	m_info = new ID3DXMesh*[m_iNumVertices];
	m_matDebug = new D3DXMATRIX[m_iNumVertices];
	HDC hdc = CreateCompatibleDC(NULL);
	INT nHeight = -MulDiv( 10, GetDeviceCaps( hdc, LOGPIXELSY ), 72 );
	HFONT hFont;
	hFont = CreateFont( nHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		TEXT("Consolas") );
	SelectObject( hdc, hFont );

	SVertexT* pVertices;
	CHAR buf[32];
	m_pVB->Lock(0, 0, (void**) &pVertices, 0); 
	int n(0), iVertices(m_iNumVertices/4);
	for (int i(0); i < iVertices; i++) {
		sprintf_s(buf, 32, "%d", n);
		D3DXCreateTextA(CDirect3D::getInstance()->GetD3D9Device(), hdc, buf,
			0.001f, .05f, &m_info[n], NULL, NULL);
		D3DXMatrixTranslation(&m_matDebug[n], pVertices[n].x*1.1f, pVertices[n].y, pVertices[n].z*1.1f);
		n++;
		sprintf_s(buf, 32, "%d(%.2f, %.2f)", n, pVertices[n].u, pVertices[n].v);
		D3DXCreateTextA(CDirect3D::getInstance()->GetD3D9Device(), hdc, buf,
			0.001f, .05f, &m_info[n], NULL, NULL);
		D3DXMatrixTranslation(&m_matDebug[n], pVertices[n].x*1.1f, pVertices[n].y+1.f, pVertices[n].z*1.1f);
		n++;
		sprintf_s(buf, 32, "%d", n);
		D3DXCreateTextA(CDirect3D::getInstance()->GetD3D9Device(), hdc, buf,
			0.001f, .05f, &m_info[n], NULL, NULL);
		D3DXMatrixTranslation(&m_matDebug[n], pVertices[n].x*1.1f, pVertices[n].y+2.f, pVertices[n].z*1.1f);
		n++;
		sprintf_s(buf, 32, "%d(%.2f, %.2f, %.2f)", n, pVertices[n].x*1.1f, pVertices[n].y, pVertices[n].z*1.1f);
		D3DXCreateTextA(CDirect3D::getInstance()->GetD3D9Device(), hdc, buf,
			0.001f, .05f, &m_info[n], NULL, NULL);
		D3DXMatrixTranslation(&m_matDebug[n], pVertices[n].x, pVertices[n].y+3.f, pVertices[n].z);
		n++;
	}
	m_pVB->Unlock();
	DeleteObject( hFont );
	DeleteDC( hdc );
}

VOID CSkyDome::onTick(float fElapsedTime) {
	m_fAngle += fElapsedTime * 0.03f;
	D3DXMatrixRotationY(&m_matWorld, -m_fAngle);
	m_matWorld._42 = -3.f;
}

VOID CSkyDome::onRender() {
	CDirect3D::getInstance()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	CDirect3D::getInstance()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	CDirect3D::getInstance()->AlphaBlendEnable(TRUE);
	if (m_bAlpha) {
		CDirect3D::getInstance()->SetSrcRenderBlendFactor(D3DBLEND_SRCCOLOR);
		CDirect3D::getInstance()->SetDestRenderBlendFactor(D3DBLEND_INVSRCCOLOR);
	} else {
		CDirect3D::getInstance()->SetSrcRenderBlendFactor(D3DBLEND_SRCALPHA);
		CDirect3D::getInstance()->SetDestRenderBlendFactor(D3DBLEND_INVSRCALPHA);
	}

	CDirect3D::getInstance()->SetTransform(D3DTS_WORLD, &m_matWorld);
	CDirect3D::getInstance()->SetStreamSource(0, m_pVB, 0, sizeof(SVertexT));
	CDirect3D::getInstance()->SetD3DFVF(SVertexT::FVF);
	CDirect3D::getInstance()->SetTexture(0, m_pTexture);
	CDirect3D::getInstance()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, m_iNumVertices-2);
	CDirect3D::getInstance()->AlphaBlendEnable(FALSE);
	CDirect3D::getInstance()->SetTexture(0, NULL);
	if (!m_bDebug) return;
	if (!m_info) InitDebugInfo();
	D3DXMATRIX mat;
	float fAngle;
	for (int i(0); i < m_iNumVertices; i++) {
		fAngle = -atan2f(m_matDebug[i]._43, m_matDebug[i]._41);
		if (m_matDebug[i]._43 > 0.f) fAngle += D3DX_PI * 2;
		D3DXMatrixRotationY(&mat, fAngle);
		mat = mat * m_matDebug[i] * m_matWorld;
		CDirect3D::getInstance()->SetTransform(D3DTS_WORLD, &mat);
		m_info[i]->DrawSubset(0);
	}
}
