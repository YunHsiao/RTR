#include "..\Engine\Utility.h"
#include "..\Engine\Direct3D.h"
#include "..\Engine\Window.h"
#include "Aim.h"

#define AIM_LENGTH 8.f
#define AIM_SPACE 5.f
#define AIM_COLOR 0xffff3500

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

	SVertexRHWD* pVertex;
	m_pVB->Lock(0, sizeof(SVertexRHWD)*8, (void**)&pVertex, 0);
	pVertex[0].x = fX;				pVertex[1].x = fX;
	pVertex[0].y = fY - AIM_SPACE;	pVertex[1].y = fY - AIM_SPACE - AIM_LENGTH;
	pVertex[0].z = 0.f;				pVertex[1].z = 0.f;
	pVertex[0].rhw = 1.f;			pVertex[1].rhw = 1.f;
	pVertex[0].c = AIM_COLOR;		pVertex[1].c = AIM_COLOR;

	pVertex[2].x = fX + AIM_SPACE;	pVertex[3].x = fX + AIM_SPACE + AIM_LENGTH;
	pVertex[2].y = fY;				pVertex[3].y = fY;
	pVertex[2].z = 0.f;				pVertex[3].z = 0.f;
	pVertex[2].rhw = 1.f;			pVertex[3].rhw = 1.f;
	pVertex[2].c = AIM_COLOR;		pVertex[3].c = AIM_COLOR;

	pVertex[4].x = fX;				pVertex[5].x = fX;
	pVertex[4].y = fY + AIM_SPACE;	pVertex[5].y = fY + AIM_SPACE + AIM_LENGTH;
	pVertex[4].z = 0.f;				pVertex[5].z = 0.f;
	pVertex[4].rhw = 1.f;			pVertex[5].rhw = 1.f;
	pVertex[4].c = AIM_COLOR;		pVertex[5].c = AIM_COLOR;

	pVertex[6].x = fX - AIM_SPACE;	pVertex[7].x = fX - AIM_SPACE - AIM_LENGTH;
	pVertex[6].y = fY;				pVertex[7].y = fY;
	pVertex[6].z = 0.f;				pVertex[7].z = 0.f;
	pVertex[6].rhw = 1.f;			pVertex[7].rhw = 1.f;
	pVertex[6].c = AIM_COLOR;		pVertex[7].c = AIM_COLOR;
	m_pVB->Unlock();
}

void GPAim::onRender() {
	CDirect3D::getInstance()->SetD3DFVF(SVertexRHWD::FVF);
	CDirect3D::getInstance()->SetStreamSource(0, m_pVB, 0, sizeof(SVertexRHWD));
	CDirect3D::getInstance()->DrawPrimitive(D3DPT_LINELIST, 0, 4);
}