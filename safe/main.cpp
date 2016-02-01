#include "encryption.h"
#include "filesystem.h"
#include "window.h"
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>

CAppModule _Module;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    _Module.Init(NULL, hInstance);

    MSG msg;
    {
        window win;
        RECT r = {CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT + WIN_WIDTH, CW_USEDEFAULT + WIN_HEIGHT};
        if(win.CreateEx(NULL, &r) == NULL)
            return 1;
        win.ShowWindow(nCmdShow);
        win.UpdateWindow();
        if(strlen(lpCmdLine) > 0)
        {
            std::stringstream sts;
            sts << lpCmdLine;
            win.dialog.open_file(sts.str());
        }

        while(GetMessage(&msg, NULL, 0, 0) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    _Module.Term();
    return msg.wParam;
}