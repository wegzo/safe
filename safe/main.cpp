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

//int main()
//{
//    char pass[20+1] = "hunter1aaaaaaaaaaaaa";
//    filesystem f("safe.txt", pass);
//    f.get("test.txt")->second = "lel";
//    
//    //srand(time(NULL));
//
//    //// salt: 12 bytes, pass: 20 bytes (total 32 bytes)
//    //// generate salt
//    //std::string salt;
//    //for(int i = 0; i < 12; i++)
//    //    salt.push_back(rand() % 256);
//    //// password
//    //char pass[20+1] = "hunter1aaaaaaaaaaaaa";
//
//    //encryption e;
//    //// 256 bit key
//    //unsigned char key[256 / 8];
//    //e.derive_key(pass, salt, key);
//    //// 128bit randomly generated iv
//    //unsigned char iv[128 / 8];
//    //for(size_t i = 0; i < (128 / 8); i++)
//    //    iv[i] = rand() % 256;
//
//    //std::stringstream data;
//    //std::fstream data2("mtest.txt", std::ios::binary | std::ios::in | std::ios::out);
//    //data.write((char*)iv, sizeof(iv));
//    //for(int i = 0; i < 216; i++)
//    //data << "baaaaaaaaaaaaaab"; // 128bit
//    //data << "baaaaaa"; // 128bit
//    ////int ciph_len = e.encrypt(data, data.tellp(), key, iv, data2);
//    ////std::cout << ciph_len << std::endl;
//    ///*
//    //std::ifstream in("safe.txt", std::ios_base::binary);
//    //unsigned char iv2[128 / 8];
//    //in.seekg(0, std::ios_base::end);
//    //size_t s = in.tellg();
//    //in.seekg(0, std::ios_base::beg);
//    //int plain_len = e.decrypt(in, s, key, iv2, std::cout);
//    //std::cout << std::endl << plain_len << std::endl;
//    //system("pause");
//    //*/
//    //// 3479
//    //data2.seekg(0);
//    //int i = data2.tellg();
//    //unsigned char iv2[128 / 8];
//    //int plain_len = e.decrypt(data2, 3488, key, iv2, std::cout);
//    //std::cout << std::endl << plain_len << std::endl;
//    //system("pause");
//    
//    
//    return 0;
//}