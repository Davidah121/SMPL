#ifdef _WIN32
    #include "ext/DragDrop.h"
    #include "StringTools.h"
    
    WinSingleton WinSingleton::singleton;
    bool WinSingleton::isInit = false;

    WinSingleton::WinSingleton()
    {
        init();
    }

    WinSingleton::~WinSingleton()
    {
        dispose();
    }

    bool WinSingleton::hasInit()
    {
        return isInit;
    }

    void WinSingleton::tryInit()
    {
        if(!isInit)
            singleton.init();
    }

    void WinSingleton::init()
    {
        // glib::StringTools::println("INIT");
        HRESULT res = OleInitialize(NULL);
        isInit = (res == NO_ERROR);
    }

    void WinSingleton::dispose()
    {
        OleUninitialize();
        isInit = false;
        // glib::StringTools::println("UNINIT");
    }

    DragDrop::DragDrop(HWND wnd)
    {
        wndHandle = wnd;
    }
    DragDrop::~DragDrop()
    {

    }
    bool DragDrop::isValid()
    {
        return valid;
    }

    // IUnknown methods
    HRESULT STDMETHODCALLTYPE DragDrop::QueryInterface(REFIID riid, void** ppvObj)
    {
        return S_OK;
    }

    STDMETHODIMP_(ULONG) DragDrop::AddRef()
    {
        return 0;
    }
    STDMETHODIMP_(ULONG) DragDrop::Release()
    {
        return 0;
    }
    
    STDMETHODIMP DragDrop::DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
    {
        smpl::StringTools::println("DRAG ENTER");
        if(pdwEffect == NULL)
			return E_INVALIDARG;

        //check data type and set
        

		*pdwEffect |= DROPEFFECT_COPY;

		return S_OK;
    }

    STDMETHODIMP DragDrop::DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
    {
        //put visual feedback here. Just change return to specify.
        smpl::StringTools::println("STUFF");
        if(pdwEffect == NULL)
			return E_INVALIDARG;

		*pdwEffect |= DROPEFFECT_COPY;

		return S_OK;
    }

    STDMETHODIMP DragDrop::DragLeave()
    {
        //remove visual feedback
        smpl::StringTools::println("DRAG EXIT");
        return S_OK;
    }

    STDMETHODIMP DragDrop::Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
    {
        //Actual DragDrop Stuff
        smpl::StringTools::println("DROP");

        if(pdwEffect == NULL)
			return E_INVALIDARG;

		*pdwEffect |= DROPEFFECT_COPY;

		// FORMATETC format;
		// STGMEDIUM medium;
		// format.cfFormat = CF_HDROP;
		// format.ptd = NULL;
		// format.dwAspect = DVASPECT_CONTENT;
		// format.lindex = -1;
		// format.tymed = TYMED_HGLOBAL;
		// medium.tymed = TYMED_HGLOBAL;
		// HRESULT res = pDataObj->GetData(&format, &medium);
		// HDROP drop = (HDROP)medium.hGlobal;
		// wchar_t* fileName = NULL;
		// //See https://docs.microsoft.com/en-us/windows/desktop/api/shellapi/nf-shellapi-dragqueryfilew
		// UINT filePathesCount = DragQueryFile(drop, 0xFFFFFFFF, NULL, 512);//If "0xFFFFFFFF" as the second parameter: return the count of files dropped
		// UINT longestFileNameLength = 0;

		// for(UINT i = 0; i < filePathesCount; ++i)
		// {
		// 	//If NULL as the third parameter: return the length of the path, not counting the trailing '0'
		// 	UINT fileNameLength = DragQueryFile(drop, i, NULL, 512) + 1;
		// 	if(fileNameLength > longestFileNameLength)
		// 	{
		// 		if(fileName != NULL)
		// 		{
		// 			free(fileName);
		// 		}
		// 		longestFileNameLength = fileNameLength;
		// 		fileName = (wchar_t*)malloc(longestFileNameLength * sizeof(*fileName));
		// 	}
		// 	DragQueryFile(drop, i, fileName, fileNameLength);

		// 	std::wcout << fileName << std::endl;
		// }
		// if(fileName != NULL)
		// {
		// 	free(fileName);
		// }
		// ReleaseStgMedium(&medium);

		return S_OK;
    }

#endif