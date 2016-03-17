#ifndef TDTargeting_H_
#define TDTargeting_H_

class TDTargeting
{
public:
	TDTargeting();
	~TDTargeting();

	void onInit();
	void onRender();

private:
	IDirect3DVertexBuffer9* m_pVB;
};
#endif
