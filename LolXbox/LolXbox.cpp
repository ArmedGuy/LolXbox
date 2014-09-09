// LolXbox.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;


int screenWidth = 0;
int screenHeight = 0;

bool isLockedMode = false;
bool rightPushedDown = false;

typedef struct _PLAYER_MOVEMENT {
    int x;
    int y;
} PLAYER_MOVEMENT;

typedef struct {
    bool x;
    bool a;
    bool b;
    bool y;
    bool leftShoulder;
    bool rightShoulder;
    bool dpadDown;
    bool start;
} BUTTONS_PRESSED;

BUTTONS_PRESSED pressed = { false, false, false, false, false };

void MouseEvent(int key) {
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.mouseData = 0;

    input.mi.dx = 65536 / 2.0;
    input.mi.dy = 65536 / 2.0;
    input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | key;

    SendInput(1, &input, sizeof(input));
}
void PressKey(int key) {
    INPUT inp;
    inp.type = INPUT_KEYBOARD;
    inp.ki.wScan = key;
    inp.ki.dwFlags = KEYEVENTF_SCANCODE;

    SendInput(1, &inp, sizeof(inp));
}
void ReleaseKey(int key) {
INPUT inp;
inp.type = INPUT_KEYBOARD;
inp.ki.wScan = key;
inp.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;

SendInput(1, &inp, sizeof(inp));
}
bool ButtonPressed(XINPUT_STATE state, int button) {
    return (state.Gamepad.wButtons & button) != 0;
}


#define MOUSE_RADIUS 400

void ProcessMovement(XINPUT_STATE state){
    // TODO: e^x-1 på move
    bool movePlayer = false;
    PLAYER_MOVEMENT move = { 0, 0 };
    /// player moves left stick left<->right
    if (state.Gamepad.sThumbLX > 500 || state.Gamepad.sThumbLX < -500) {
        move.x = round(((state.Gamepad.sThumbLX / 32767.0)) * MOUSE_RADIUS);
    }
    // left stick up<->down
    if (state.Gamepad.sThumbLY > 500 || state.Gamepad.sThumbLY < -500) {
        move.y = round(((state.Gamepad.sThumbLY / 32767.0)) * -1 * MOUSE_RADIUS);
    }

    if (state.Gamepad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE || state.Gamepad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
        if (state.Gamepad.sThumbRX > 0) {
            ReleaseKey(DIK_RIGHT);
            ReleaseKey(DIK_LEFT);
            PressKey(DIK_RIGHT);
        }
        else {
            ReleaseKey(DIK_RIGHT);
            ReleaseKey(DIK_LEFT);
            PressKey(DIK_LEFT);
        }
    }
    else {
        ReleaseKey(DIK_RIGHT);
        ReleaseKey(DIK_LEFT);
    }
    // left stick up<->down
    if (state.Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE || state.Gamepad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
        if (state.Gamepad.sThumbRY > 0) {
            ReleaseKey(DIK_UP);
            ReleaseKey(DIK_DOWN);
            PressKey(DIK_UP);
        }
        else {
            ReleaseKey(DIK_UP);
            ReleaseKey(DIK_DOWN);
            PressKey(DIK_DOWN);
        }
    }
    else {
        ReleaseKey(DIK_UP);
        ReleaseKey(DIK_DOWN);
    }

    if (ButtonPressed(state, XINPUT_GAMEPAD_LEFT_SHOULDER) && !pressed.leftShoulder) {
        pressed.leftShoulder = true;
    }
    if (!ButtonPressed(state, XINPUT_GAMEPAD_LEFT_SHOULDER) && pressed.leftShoulder) {
        pressed.leftShoulder = false;

        MouseEvent(MOUSEEVENTF_RIGHTDOWN);
        Sleep(10);
        MouseEvent(MOUSEEVENTF_RIGHTUP);
    }

    if (ButtonPressed(state, XINPUT_GAMEPAD_RIGHT_SHOULDER) && !pressed.rightShoulder) {
        pressed.rightShoulder = true;

        MouseEvent(MOUSEEVENTF_LEFTDOWN);
        Sleep(10);
        MouseEvent(MOUSEEVENTF_LEFTUP);
    }
    if (!ButtonPressed(state, XINPUT_GAMEPAD_RIGHT_SHOULDER) && pressed.rightShoulder) {
        pressed.rightShoulder = false;
    }

    if(isLockedMode) 
        SetCursorPos((screenWidth / 2) + move.x, (screenHeight / 2) + move.y);

}

void ProcessAttacks(XINPUT_STATE state) {
    bool xPressed = ButtonPressed(state, XINPUT_GAMEPAD_X);
    bool aPressed = ButtonPressed(state, XINPUT_GAMEPAD_A);
    bool bPressed = ButtonPressed(state, XINPUT_GAMEPAD_B);
    bool yPressed = ButtonPressed(state, XINPUT_GAMEPAD_Y);
    bool dpadDownPressed = ButtonPressed(state, XINPUT_GAMEPAD_DPAD_DOWN);

    if (dpadDownPressed && !pressed.dpadDown)
    {
        pressed.dpadDown = true;
        PressKey(DIK_LCONTROL);
    }

    if (xPressed && !pressed.x) {
        pressed.x = true;
        PressKey(DIK_Q);
    }
    if (!xPressed && pressed.x) {
        pressed.x = false;
        ReleaseKey(DIK_Q);
    }

    if (aPressed && !pressed.a) {
        pressed.a = true;
        PressKey(DIK_W);
    }
    if (!aPressed && pressed.a) {
        pressed.a = false;
        ReleaseKey(DIK_W);
    }

    if (bPressed && !pressed.b) {
        pressed.b = true;
        PressKey(DIK_E);
    }
    if (!bPressed && pressed.b) {
        pressed.b = false;
        ReleaseKey(DIK_E);
    }

    if (yPressed && !pressed.y) {
        pressed.y = true;
        PressKey(DIK_R);
    }
    if (!yPressed && pressed.y) {
        pressed.y = false;
        ReleaseKey(DIK_R);
    }

    if (!dpadDownPressed && pressed.dpadDown)
    {
        pressed.dpadDown = false;
        ReleaseKey(DIK_LCONTROL);
    }
}




void GetDesktopResolution(int& horizontal, int& vertical)
{
    RECT desktop;
    // Get a handle to the desktop window
    const HWND hDesktop = GetDesktopWindow();
    // Get the size of screen to the variable desktop
    GetWindowRect(hDesktop, &desktop);
    // The top left corner will have coordinates (0,0)
    // and the bottom right corner will have coordinates
    // (horizontal, vertical)
    horizontal = desktop.right;
    vertical = desktop.bottom;
}



CXBOXController* ctrl;
int _tmain(int argc, _TCHAR* argv[])
{
    ctrl = new CXBOXController(0);
    printf("Getting desktop resolution: \n");
    GetDesktopResolution(screenWidth, screenHeight);
    cout << screenWidth << "x" << screenHeight << " px\n";


    while (true) {
        if (ctrl->IsConnected()) {
            XINPUT_STATE state = ctrl->GetState();

            ProcessMovement(state);
            ProcessAttacks(state);


            if (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
            {
                break;
            }
            // center cursor on start
            bool startPressed = ButtonPressed(state, XINPUT_GAMEPAD_START);
            if (startPressed && !pressed.start) {
                pressed.start = true;
            }
            if (!startPressed && pressed.start) {
                pressed.start = false;
                isLockedMode = !isLockedMode;
            }
            Sleep(10);
        }
        else
        {
            std::cout << "\n\tERROR! PLAYER 1 - XBOX 360 Controller Not Found!\n";
            std::cout << "Press Any Key To Exit.";
            std::cin.get();
            break;
        }
    }
	return 0;
}
