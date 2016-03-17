#ifndef GPHUD_H_
#define GPHUD_H_

class GPHUD
{
public:
	GPHUD();
	~GPHUD();

	void onInit();
	void onTick(float fElapsedTime);
	void onRender();

	inline void ToggleInfo() { m_bInfo = !m_bInfo; }
private:
	std::string m_info;
	char m_fps[16];
	bool m_bInfo;
};

#endif
