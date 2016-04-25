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
	inline void Add() { m_iCount++; }
	inline void Subtract() { m_iCount--; }
	inline int Count() { return m_iCount; }

private:
	std::string m_info;
	char m_fps[32];
	bool m_bInfo;
	int m_iCount;
};

#endif
