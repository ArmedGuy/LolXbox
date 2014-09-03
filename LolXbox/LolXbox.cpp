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

void MouseEvent(int key) {
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.mouseData = 0;

    input.mi.dx = 65536 / 2.0;
    input.mi.dy = 65536 / 2.0;
    input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | key;

    SendInput(1, &input, sizeof(input));
}
void PressKey(int key, int scan) {
    INPUT inp;
    inp.type = INPUT_KEYBOARD;
    inp.ki.wVk = key;
    inp.ki.wScan = scan;
    inp.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;

    SendInput(1, &inp, sizeof(inp));
}
void ReleaseKey(int key, int scan) {
    INPUT inp;
    inp.type = INPUT_KEYBOARD;
    inp.ki.wVk = key;
    inp.ki.wScan = scan;
    inp.ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;

    SendInput(1, &inp, sizeof(inp));
}

void ProcessMovement(XINPUT_STATE state){

    bool movePlayer = false;
    PLAYER_MOVEMENT move = { 0, 0 };
    /// player moves left stick left<->right
    if (state.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE || state.Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
        movePlayer = true;
        move.x = (int)((state.Gamepad.sThumbLX / 32767.0) * 200);
    }
    // left stick up<->down
    if (state.Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE || state.Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
        movePlayer = true;
        move.y = (int)((state.Gamepad.sThumbLY / 32767.0) * 200 * -1);
    }


    if (state.Gamepad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE || state.Gamepad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
        if (state.Gamepad.sThumbRX > 0) {
            ReleaseKey(VK_RIGHT, 0xCD);
            ReleaseKey(VK_LEFT, 0xCB);
            PressKey(VK_RIGHT, 0xCD);
        }
        else {
            ReleaseKey(VK_RIGHT, 0xCD);
            ReleaseKey(VK_LEFT, 0xCB);
            PressKey(VK_LEFT, 0xCB);
        }
    }
    else {
        ReleaseKey(VK_RIGHT, 0xCD);
        ReleaseKey(VK_LEFT, 0xCB);
    }
    // left stick up<->down
    if (state.Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE || state.Gamepad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
        if (state.Gamepad.sThumbRY > 0) {
            ReleaseKey(VK_UP, 0xC8);
            ReleaseKey(VK_DOWN, 0xD0);
            PressKey(VK_UP, 0xC8);
        }
        else {
            ReleaseKey(VK_UP, 0xC8);
            ReleaseKey(VK_DOWN, 0xD0);
            PressKey(VK_DOWN, 0xD0);
        }
    }
    else {
        ReleaseKey(VK_UP, 0xC8);
        ReleaseKey(VK_DOWN, 0xD0);
    }
    ReleaseKey(VK_SPACE, 0xB9);
    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {
        PressKey(VK_SPACE, 0xB9);
    }

    if (movePlayer) {
        if (rightPushedDown == false) {
            MouseEvent(MOUSEEVENTF_RIGHTUP);
            MouseEvent(MOUSEEVENTF_RIGHTDOWN);
            /*
            INPUT spaceInput;
            spaceInput.type = INPUT_KEYBOARD;
            spaceInput.ki.wVk = VK_SPACE;
            spaceInput.ki.wScan = 0x39;
            SendInput(1, &spaceInput, sizeof(spaceInput));
            */

            rightPushedDown = true;
        }

        SetCursorPos((screenWidth / 2) + move.x, (screenHeight / 2) + move.y);
    }
    else {
        rightPushedDown = false;
        if(isLockedMode) SetCursorPos(screenWidth / 2, screenHeight / 2);
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


            if (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
            {
                break;
            }
            // center cursor on start
            if (state.Gamepad.wButtons & XINPUT_GAMEPAD_START) {
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
