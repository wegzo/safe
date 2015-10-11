#pragma once

#include "wtl.h"
#include "filesystem.h"
#include <boost/scoped_ptr.hpp>
#include <string>

class window;
class formview;

class input_dialog : public CDialogImpl<input_dialog>
{
private:
    const std::string text;
public:
    enum {IDD = IDD_PASSINPUTDLG};
    CHAR pass_160[filesystem::pass_len + 1]; // contains null char

    input_dialog(const std::string& text = "Enter password to open archive:");
    ~input_dialog();

    BEGIN_MSG_MAP(input_dialog)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_HANDLER(IDOK, BN_CLICKED, OnBnClickedOk)
        COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnBnClickedCancel)
    END_MSG_MAP()

    LRESULT OnBnClickedOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};

class formview : public CDialogImpl<formview>, public CDialogResize<formview>
{
private:
    CListViewCtrl listview_m;
    window& parent;

    void add_item(const std::string& name, size_t size);
    void remove_item_and_file();
    void open_file();
public:
    enum {IDD = IDD_FORMVIEW};
    boost::scoped_ptr<filesystem> archive;

    formview(window&);
    ~formview();

    void open_file(const std::string& filename);
    void on_loadfile();
    void load_archive(const char* pass_160, const std::string& path);

    BEGIN_MSG_MAP(formview)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        CHAIN_MSG_MAP(CDialogResize<formview>)
        MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
        COMMAND_ID_HANDLER(ID_POPUP_ADD, OnPopupAdd)
        MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
        COMMAND_ID_HANDLER(ID_POPUP_ADDNEWFILE, OnPopupAddnewfile)
        COMMAND_ID_HANDLER(ID_POPUP_REMOVEFILE, OnPopupRemovefile)
        NOTIFY_HANDLER(IDC_LIST2, LVN_KEYDOWN, OnKeyPress)
        COMMAND_ID_HANDLER(ID_POPUP_OPEN, OnPopupOpen)
        NOTIFY_HANDLER(IDC_LIST2, NM_DBLCLK, OnDoubleClick)
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(formview)
        DLGRESIZE_CONTROL(IDC_LIST2, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    END_DLGRESIZE_MAP()

    LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnContextMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnPopupAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnShowWindow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnPopupAddnewfile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnPopupRemovefile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnKeyPress(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
    LRESULT OnPopupOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnDoubleClick(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
};