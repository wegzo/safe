#pragma once

#include "wtl.h"
#include <string>

class textview : public CDialogImpl<textview>, public CDialogResize<textview>
{
private:
    CEdit edit;
    CFont* font;
public:
    enum {IDD = IDD_TEXTVIEW};
    char* modified_text;
    int text_len;

    textview();
    ~textview();

    BEGIN_MSG_MAP(textview)
        CHAIN_MSG_MAP(CDialogResize<textview>)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_CLOSE, OnClose)
        COMMAND_HANDLER(IDC_TEXTEDIT, EN_CHANGE, OnTextChange)
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(textview)
        DLGRESIZE_CONTROL(IDC_TEXTEDIT, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    END_DLGRESIZE_MAP()

    LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnTextChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};