#ifndef GPAim_H_
#define GPAim_H_

#define AIM_LENGTH 8.f
#define AIM_SPACE 5.f
#define AIM_COLOR 0xffff3500

class GPAim
{
public:
	GPAim();
	~GPAim();

	void onInit();
	void onRender();

private:
	IDirect3DVertexBuffer9* m_pVB;
};
#endif
