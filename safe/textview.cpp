#include "textview.h"
#include "window.h"

textview::textview() : modified_text(NULL), text_len(-1), font(NULL)
{
}

textview::~textview()
{
    if(this->font != NULL)
        delete this->font;
}

LRESULT textview::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    this->font = new CFont;
    HFONT font = this->font->CreateFontA(
        15, 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE, 
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
        DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "Consolas");
    this->DlgResize_Init(false, false);
    GET(CEdit, IDC_TEXTEDIT).SetFocus();
    GET(CEdit, IDC_TEXTEDIT).SetFont(*this->font);
    GET(CEdit, IDC_TEXTEDIT).SetLimitText(0);
    GET(CEdit, IDC_TEXTEDIT).SetWindowTextA(((std::string*)lParam)->c_str());
    GET(CEdit, IDC_TEXTEDIT).SetSel(-1, 0, TRUE);
    return 0;
}

LRESULT textview::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    if(this->modified_text && this->MessageBoxA("Save changes?", "Confirm", MB_YESNO | MB_ICONINFORMATION) == IDYES)
    {
        this->text_len = GET(CEdit, IDC_TEXTEDIT).GetWindowTextLengthA();
        if(this->text_len > 0)
        {
            this->modified_text = new char[this->text_len + 1];
            memset(this->modified_text, 0, this->text_len + 1);
            GET(CEdit, IDC_TEXTEDIT).GetWindowTextA(this->modified_text, this->text_len + 1);
        }
    }
    this->EndDialog(0);
    return 0;
}


LRESULT textview::OnTextChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    this->modified_text = (char*)true;
    this->SetWindowTextA("Text viewer *");
    return 0;
}
