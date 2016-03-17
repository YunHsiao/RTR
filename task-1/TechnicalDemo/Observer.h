#ifndef TDObserver_H_
#define TDObserver_H_

#include "..\Engine\BaseCamera.h"

class TDObserver : public CCamera
{
public:
	void onTick(float fElapsedTime);
protected:
	void KeyboardInput(float fElaspedTime);

};
#endif
