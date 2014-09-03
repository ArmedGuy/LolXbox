#include "XBOXController.h"


CXBOXController::CXBOXController(int playerIndex)
{
    _controllerNum = playerIndex;
}

XINPUT_STATE CXBOXController::GetState() {
    ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));
    
    XInputGetState(_controllerNum, &_controllerState);

    return _controllerState;
}

bool CXBOXController::IsConnected() {
    ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

    DWORD Result = XInputGetState(_controllerNum, &_controllerState);
    if (Result == ERROR_SUCCESS) {
        return true;
    }
    else {
        return false;
    }
}

void CXBOXController::Vibrate(int leftVal, int rightVal) {
    XINPUT_VIBRATION Vibration;

    ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));

    Vibration.wLeftMotorSpeed = leftVal;
    Vibration.wRightMotorSpeed = rightVal;

    XInputSetState(_controllerNum, &Vibration);
}
