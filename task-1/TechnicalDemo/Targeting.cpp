#include "..\Engine\Utility.h"
#include "..\Engine\Direct3D.h"
#include "..\Engine\Window.h"
#include "Targeting.h"

TDTargeting::TDTargeting():
m_pVB(NULL)
{
}

TDTargeting::~TDTargeting()
{
	Safe_Release(m_pVB);
}

void TDTargeting::onInit() {
	float fX = CWindow::getInstance()->getWinWidth() / 2.f;
	float fY = CWindow::getInstance()->getWinHeight() / 2.f;

	CDirect3D::getInstance()->CreateVertexBuffer(sizeof(SVertexRHWD)*5, 
		D3DUSAGE_WRITEONLY, SVertexRHWD::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL);

	SVertexRHWD* pVertex;
	m_pVB->Lock(0, sizeof(SVertexRHWD)*5, (void**)&pVertex, 0);
	pVertex[0] = SVertexRHWD(fX, fY - TARGETING_HEIGHT, 0.f, 1.f, TARGETING_COLOR);
	pVertex[1] = SVertexRHWD(fX - TARGETING_WIDTH, fY, 0.f, 1.f, TARGETING_COLOR);
	pVertex[2] = SVertexRHWD(fX, fY + TARGETING_HEIGHT, 0.f, 1.f, TARGETING_COLOR);
	pVertex[3] = SVertexRHWD(fX + TARGETING_WIDTH, fY, 0.f, 1.f, TARGETING_COLOR);
	pVertex[4] = SVertexRHWD(fX, fY - TARGETING_HEIGHT, 0.f, 1.f, TARGETING_COLOR);
	m_pVB->Unlock();
}

void TDTargeting::onRender() {
	CDirect3D::getInstance()->SetD3DFVF(SVertexRHWD::FVF);
	CDirect3D::getInstance()->SetStreamSource(0, m_pVB, 0, sizeof(SVertexRHWD));
	CDirect3D::getInstance()->DrawPrimitive(D3DPT_LINESTRIP, 0, 4);
}