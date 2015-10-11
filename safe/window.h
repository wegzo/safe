#pragma once

#include "formview.h"

#define WIN_WIDTH 308
#define WIN_HEIGHT 260
#define GET(type, item) reinterpret_cast<type&>(this->GetDlgItem(item))

class window : public CFrameWindowImpl<window>
{
private:
public:
    formview dialog;
    window();

    DECLARE_FRAME_WND_CLASS("safe", IDR_MAINFRAME);

    // msg_map_ex for others than cwindowimpl/cdialogimpl classes
    BEGIN_MSG_MAP(window)
        MSG_WM_CREATE(OnCreate)
        CHAIN_MSG_MAP(CFrameWindowImpl<window>)
        COMMAND_ID_HANDLER(ID_FILE_ADDNEWFILE, OnFileAddnewfile)
        COMMAND_ID_HANDLER(ID_FILE_CHANGEPASSWORD, OnChangePassword)
        COMMAND_ID_HANDLER(ID_FILE_EXIT, OnFileExit)
    END_MSG_MAP()

    LRESULT OnCreate(LPCREATESTRUCT);
    LRESULT OnFileAddnewfile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnChangePassword(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};