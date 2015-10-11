#include "window.h"

window::window() : dialog(*this)
{
}

LRESULT window::OnCreate(LPCREATESTRUCT lpcs)
{
    this->m_hWndClient = this->dialog.Create(this->m_hWnd);

    input_dialog passinput;
    if(passinput.DoModal(*this))
    {
        this->dialog.load_archive(passinput.pass_160, "safe.txt");
        this->dialog.ShowWindow(SW_SHOW);
    }
    else
        this->DestroyWindow();

    return 0;
}

LRESULT window::OnFileAddnewfile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    this->dialog.on_loadfile();
    return 0;
}

LRESULT window::OnChangePassword(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if(this->dialog.archive.get() == NULL)
        return 0;

change_pass:
    input_dialog changepwd("Enter new password:");
    if(changepwd.DoModal(*this))
    {
        input_dialog changepwd2("Enter new password again:");
        if(changepwd2.DoModal(*this))
        {
            if(strcmp(changepwd.pass_160, changepwd2.pass_160) == 0)
                this->dialog.archive->change_password(changepwd.pass_160);
            else
            {
                this->MessageBoxA("Passwords didn't equal", NULL, MB_ICONERROR);
                goto change_pass;
            }
        }
    }
    return 0;
}

LRESULT window::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    this->SendMessageA(WM_CLOSE);
    return 0;
}
