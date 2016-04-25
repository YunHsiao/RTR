#include "..\Engine\Utility.h"
#include "..\Engine\Direct3D.h"
#include "..\Engine\Window.h"
#include "Aim.h"

GPAim::GPAim():
m_pVB(NULL)
{
}

GPAim::~GPAim()
{
	Safe_Release(m_pVB);
}

void GPAim::onInit() {
	float fX = CWindow::getInstance()->getWinWidth() / 2.f;
	float fY = CWindow::getInstance()->getWinHeight() / 2.f;

	CDirect3D::getInstance()->CreateVertexBuffer(sizeof(SVertexRHWD)*8, 
		D3DUSAGE_WRITEONLY, SVertexRHWD::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL);

	SVertexRHWD* pVertices;
	m_pVB->Lock(0, sizeof(SVertexRHWD)*8, (void**)&pVertices, 0);
	pVertices[0].x = fX;				pVertices[1].x = fX;
	pVertices[0].y = fY - AIM_SPACE;	pVertices[1].y = fY - AIM_SPACE - AIM_LENGTH;
	pVertices[0].z = 0.f;				pVertices[1].z = 0.f;
	pVertices[0].rhw = 1.f;				pVertices[1].rhw = 1.f;
	pVertices[0].c = AIM_COLOR;			pVertices[1].c = AIM_COLOR;

	pVertices[2].x = fX + AIM_SPACE;	pVertices[3].x = fX + AIM_SPACE + AIM_LENGTH;
	pVertices[2].y = fY;				pVertices[3].y = fY;
	pVertices[2].z = 0.f;				pVertices[3].z = 0.f;
	pVertices[2].rhw = 1.f;				pVertices[3].rhw = 1.f;
	pVertices[2].c = AIM_COLOR;			pVertices[3].c = AIM_COLOR;

	pVertices[4].x = fX;				pVertices[5].x = fX;
	pVertices[4].y = fY + AIM_SPACE;	pVertices[5].y = fY + AIM_SPACE + AIM_LENGTH;
	pVertices[4].z = 0.f;				pVertices[5].z = 0.f;
	pVertices[4].rhw = 1.f;				pVertices[5].rhw = 1.f;
	pVertices[4].c = AIM_COLOR;			pVertices[5].c = AIM_COLOR;

	pVertices[6].x = fX - AIM_SPACE;	pVertices[7].x = fX - AIM_SPACE - AIM_LENGTH;
	pVertices[6].y = fY;				pVertices[7].y = fY;
	pVertices[6].z = 0.f;				pVertices[7].z = 0.f;
	pVertices[6].rhw = 1.f;				pVertices[7].rhw = 1.f;
	pVertices[6].c = AIM_COLOR;			pVertices[7].c = AIM_COLOR;
	m_pVB->Unlock();
}

void GPAim::onRender() {
	CDirect3D::getInstance()->SetD3DFVF(SVertexRHWD::FVF);
	CDirect3D::getInstance()->SetStreamSource(0, m_pVB, 0, sizeof(SVertexRHWD));
	CDirect3D::getInstance()->DrawPrimitive(D3DPT_LINELIST, 0, 4);
}