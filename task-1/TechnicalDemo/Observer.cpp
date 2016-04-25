#include "..\Engine\Utility.h"
#include "Observer.h"

void TDObserver::KeyboardInput(float fElapsedTime) {
	if (GetAsyncKeyState('I') & 0x8000) RotateCameraDown(-fElapsedTime);
	else if (GetAsyncKeyState('K') & 0x8000) RotateCameraDown(fElapsedTime);
	if (GetAsyncKeyState('J') & 0x8000) RotateCameraRight(-fElapsedTime);
	else if (GetAsyncKeyState('L') & 0x8000) RotateCameraRight(fElapsedTime);
}

void TDObserver::onTick(float fElapsedTime) {
	KeyboardInput(fElapsedTime);
	CCamera::onTick(fElapsedTime);
}