#ifndef TDSimpleMirror_H_
#define TDSimpleMirror_H_

#include "..\Engine\Mirror.h"

class TDSimpleMirror : public CMirror
{
public:
	TDSimpleMirror();

	void onTick(float fElapsedTime);
	void changeDirection() { m_iDir++; if (m_iDir > 2) m_iDir = 0; }
private:
	int m_iDir;
};

#endif
