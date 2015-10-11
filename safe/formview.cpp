#include "formview.h"
#include "window.h"
#include "textview.h"
#include <sstream>

input_dialog::input_dialog(const std::string& text) : text(text)
{
    memset(this->pass_160, 0, sizeof(this->pass_160));
}

input_dialog::~input_dialog()
{
    memset(this->pass_160, 0, sizeof(this->pass_160));
}


formview::formview(window& parent) : parent(parent)
{
}

formview::~formview()
{
    if(this->archive.get() != NULL)
        this->archive->save_archive();
}

void formview::open_file(const std::string& filename)
{
    if(this->archive.get() == NULL)
        return;

    textview txtvw;
    std::string data;
    filesystem::files_t::iterator file = this->archive->get(filename);
    if(file == this->archive->get_all().end())
        return;
    data.assign(file->first.data(), file->first.size());
    data += '\0';
    //txtvw.set_text(data);
    txtvw.DoModal(GetActiveWindow(), (LPARAM)&data);
    if(txtvw.text_len >= 0)
    {
        file->first.clear();
        if(txtvw.text_len > 0)
        {
            const size_t len = strlen(txtvw.modified_text);
            file->first.reserve(len);
            for(size_t i = 0; i < len; i++)
                file->first.push_back(txtvw.modified_text[i]);
        }

        /*std::ostringstream sts;
        sts.imbue(std::locale(""));
        sts << txtvw.text_len;
        this->listview_m.SetItemText(item.iItem, 1, sts.str().c_str());*/
        // populate list again
        this->listview_m.DeleteAllItems();
        const filesystem::files_t& files = this->archive->get_all();
        for(filesystem::files_t::const_iterator it = files.begin(); it != files.end(); it++)
            this->add_item(it->second, it->first.size());
    }
}

void formview::open_file()
{
    LVITEMA item;
    WTL::CString item_name;
    // not resetting memory causes kernelbase to crash if opening file with enter key in release mode
    memset(&item, 0, sizeof(item));

    if(!this->listview_m.GetSelectedItem(&item))
        return;
    this->listview_m.GetItemText(item.iItem, 0, item_name);
    this->open_file(item_name.operator LPCSTR());
}

void formview::add_item(const std::string& name, size_t size)
{
    LVITEMA item;
    memset(&item, 0, sizeof(item));
    item.pszText = const_cast<char*>(name.c_str());
    item.mask = LVIF_TEXT;
    item.iItem = 0;
    item.iSubItem = 0;
    if(this->listview_m.InsertItem(&item) == -1)
        return;

    std::ostringstream sts;
    sts.imbue(std::locale(""));
    sts << size;
    std::string sizestr = sts.str();
    item.pszText = const_cast<char*>(sizestr.c_str());
    item.mask = LVIF_TEXT;
    item.iItem = 0;
    item.iSubItem = 1;
    if(this->listview_m.SetItem(&item) == -1)
        return;
}

void formview::load_archive(const char* pass_160, const std::string& path)
{
    try
    {
        this->archive.reset(new filesystem);
        this->archive->load_archive(path, pass_160);

        const filesystem::files_t& files = this->archive->get_all();
        for(filesystem::files_t::const_iterator it = files.begin(); it != files.end(); it++)
            this->add_item(it->second, it->first.size());
    }
    catch(encryption::error e)
    {
        this->archive.reset(NULL);
    }
    catch(std::exception)
    {
        this->archive.reset(NULL);
    }
}

void formview::on_loadfile()
{
    if(this->archive.get() == NULL)
        return;

    COMDLG_FILTERSPEC filetypes = {L"All files", L"*.*"};
    CShellFileOpenDialog filedialog(
        NULL, FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST,
        NULL, &filetypes, 1);
    if(filedialog.DoModal(*this) == IDOK)
    {
        WTL::CString filepath;
        filedialog.GetFilePath(filepath);
        CPath filename(filepath);
        filename.StripPath();
        
        if(this->archive->add(filepath.operator LPCSTR(), filename.operator LPCSTR()))
            this->add_item(filename.operator LPCSTR(), this->archive->get_all().back().first.size());
        else
            this->MessageBoxA("Filename already exists or filename is too long", NULL, MB_ICONERROR);
    }
}

void formview::remove_item_and_file()
{
    if(this->archive.get() == NULL)
        return;

    LVITEMA item;
    WTL::CString item_name;
    if(!this->listview_m.GetSelectedItem(&item))
        return;
    this->listview_m.GetItemText(item.iItem, 0, item_name);

    if(this->MessageBoxA("Are you sure?", "Confirm", MB_YESNO | MB_ICONINFORMATION) == IDYES &&
        this->archive->remove(item_name.operator LPCSTR()))
        this->listview_m.DeleteItem(item.iItem);
}

LRESULT input_dialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    GET(CStatic, IDC_STATIC_DLG).SetWindowTextA(text.c_str());
    return 0;
}

LRESULT formview::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    this->DlgResize_Init(false, false);

    CRect rect;
    this->parent.GetClientRect(&rect);
    this->listview_m = GET(CListViewCtrl, IDC_LIST2);
    //this->listview_m.SubclassWindow(GET(CListViewCtrl, IDC_LIST2));
    const int vscrlx = GetSystemMetrics(SM_CXVSCROLL);
    this->listview_m.InsertColumn(0, "Name", LVCFMT_LEFT, rect.Width() - rect.Width() / 3);
    this->listview_m.InsertColumn(1, "Size (bytes)", LVCFMT_RIGHT, rect.Width() / 3 - vscrlx);
    this->listview_m.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    return 0;
}

LRESULT formview::OnContextMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    CMenu formviewmenu;
    const POINT point = {LOWORD(lParam), HIWORD(lParam)};
    formviewmenu.LoadMenuA(IDR_LISTVIEWMENU);

    {
        LVITEMA item;
        CMenuHandle listviewmenu = formviewmenu.GetSubMenu(1);
        if(this->listview_m.GetSelectedItem(&item))
        {
            listviewmenu.SetMenuDefaultItem(ID_POPUP_OPEN);
            listviewmenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, *this);
            return 0;
        }
    }
    {
        CRect rect;
        this->listview_m.GetWindowRect(&rect);
        CMenuHandle listviewmenu = formviewmenu.GetSubMenu(0);
        if(rect.PtInRect(point))
            listviewmenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, *this);
    }

    return 0;
}

LRESULT formview::OnPopupAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    this->on_loadfile();
    return 0;
}


LRESULT input_dialog::OnBnClickedOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    const int linelen = GET(CEdit, IDC_EDIT1).LineLength();
    if(linelen <= 0 || linelen > (sizeof(this->pass_160) - 1))
    {
        this->MessageBoxA("Password length shall not exceed 20 characters", NULL, MB_ICONERROR);
        return 0;
    }

    GET(CEdit, IDC_EDIT1).GetWindowTextA(this->pass_160, sizeof(this->pass_160));
    this->EndDialog(1);
    return 0;
}

LRESULT input_dialog::OnBnClickedCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    this->EndDialog(0);
    return 0;
}


LRESULT formview::OnShowWindow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    return 0;
}

LRESULT formview::OnPopupAddnewfile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    this->on_loadfile();
    return 0;
}


LRESULT formview::OnPopupRemovefile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    this->remove_item_and_file();
    return 0;
}


LRESULT formview::OnKeyPress(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
    LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
    if(pLVKeyDow->wVKey == VK_DELETE)
        this->remove_item_and_file();
    else if(pLVKeyDow->wVKey == VK_RETURN)
        this->open_file();
    return 0;
}

LRESULT formview::OnPopupOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    this->open_file();
    return 0;
}


LRESULT formview::OnDoubleClick(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
    this->open_file();
    return 0;
}
