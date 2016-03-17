#include "..\Engine\Utility.h"
#include "Observer.h"

void TDObserver::KeyboardInput(float fElaspedTime) {
	if (GetAsyncKeyState('I') & 0x8000) RotateCameraDown(-fElaspedTime);
	else if (GetAsyncKeyState('K') & 0x8000) RotateCameraDown(fElaspedTime);
	if (GetAsyncKeyState('J') & 0x8000) RotateCameraRight(-fElaspedTime);
	else if (GetAsyncKeyState('L') & 0x8000) RotateCameraRight(fElaspedTime);
}

void TDObserver::onTick(float fElapsedTime) {
	KeyboardInput(fElapsedTime);
	CCamera::onTick(fElapsedTime);
}