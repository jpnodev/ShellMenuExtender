#pragma once
#include <windows.h>
#include <shobjidl_core.h>
#include <string>

// Forward declaration
class MenuItemCommand;

class ShellMenuExtender : public IExplorerCommand {
    LONG m_refCount;
public:
    ShellMenuExtender();
    ~ShellMenuExtender() {}

    // IUnknown
    HRESULT __stdcall QueryInterface(REFIID riid, void** ppv) override;
    ULONG __stdcall AddRef() override;
    ULONG __stdcall Release() override;

    // IExplorerCommand
    HRESULT __stdcall GetTitle(IShellItemArray*, PWSTR* title) override;
    HRESULT __stdcall GetIcon(IShellItemArray*, PWSTR* icon) override;
    HRESULT __stdcall GetToolTip(IShellItemArray*, PWSTR* tip) override;
    HRESULT __stdcall GetCanonicalName(GUID* guid) override;
    HRESULT __stdcall GetState(IShellItemArray*, BOOL fOkToBeSlow, EXPCMDSTATE* cmdState) override;
    HRESULT __stdcall Invoke(IShellItemArray* items, IBindCtx* pbc) override;
    HRESULT __stdcall GetFlags(EXPCMDFLAGS* flags) override;
    HRESULT __stdcall EnumSubCommands(IEnumExplorerCommand** enumCommands) override;
};

// Individual menu item command
class MenuItemCommand : public IExplorerCommand {
    LONG m_refCount;
    int m_itemIndex;
public:
    MenuItemCommand(int itemIndex);
    ~MenuItemCommand() {}

    // IUnknown
    HRESULT __stdcall QueryInterface(REFIID riid, void** ppv) override;
    ULONG __stdcall AddRef() override;
    ULONG __stdcall Release() override;

    // IExplorerCommand
    HRESULT __stdcall GetTitle(IShellItemArray*, PWSTR* title) override;
    HRESULT __stdcall GetIcon(IShellItemArray*, PWSTR* icon) override;
    HRESULT __stdcall GetToolTip(IShellItemArray*, PWSTR* tip) override;
    HRESULT __stdcall GetCanonicalName(GUID* guid) override;
    HRESULT __stdcall GetState(IShellItemArray*, BOOL fOkToBeSlow, EXPCMDSTATE* cmdState) override;
    HRESULT __stdcall Invoke(IShellItemArray* items, IBindCtx* pbc) override;
    HRESULT __stdcall GetFlags(EXPCMDFLAGS* flags) override;
    HRESULT __stdcall EnumSubCommands(IEnumExplorerCommand** enumCommands) override;
};

// Enumerator for subcommands
class SubCommandEnumerator : public IEnumExplorerCommand {
    LONG m_refCount;
    int m_currentIndex;
    int m_totalCount;
public:
    SubCommandEnumerator(int count);
    ~SubCommandEnumerator() {}

    // IUnknown
    HRESULT __stdcall QueryInterface(REFIID riid, void** ppv) override;
    ULONG __stdcall AddRef() override;
    ULONG __stdcall Release() override;

    // IEnumExplorerCommand
    HRESULT __stdcall Next(ULONG celt, IExplorerCommand** pUICommand, ULONG* pceltFetched) override;
    HRESULT __stdcall Skip(ULONG celt) override;
    HRESULT __stdcall Reset() override;
    HRESULT __stdcall Clone(IEnumExplorerCommand** ppenum) override;
};
