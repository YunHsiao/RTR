#ifndef GPAim_H_
#define GPAim_H_

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
