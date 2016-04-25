#ifndef TDTargeting_H_
#define TDTargeting_H_

#define TARGETING_WIDTH 30.f
#define TARGETING_HEIGHT 15.f
#define TARGETING_COLOR 0xffffff00

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
