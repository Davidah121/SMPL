#ifdef _WIN32
    #include <ole2.h>
    #include <objidl.h>
    #include <shlObj.h>
    #include <shellapi.h>
    #include <functional>

    class WinSingleton
    {
    public:
        static bool hasInit();
        static void tryInit();

    private:
        void init();
        void dispose();

        WinSingleton();
        ~WinSingleton();

        static bool isInit;
        static WinSingleton singleton;
    };

    class DragDrop : public IDropTarget
    {

    public:
        static const uint8_t DRAG_COPY = DROPEFFECT_COPY;
        static const uint8_t DRAG_ERROR = DROPEFFECT_NONE;
        static const uint8_t DRAG_MOVE = DROPEFFECT_MOVE;
        static const uint8_t DRAG_LINK = DROPEFFECT_LINK;
        
        DragDrop(HWND wnd);
        ~DragDrop();
        bool isValid();

        void setOnDragEnter(std::function<uint8_t(int, int, int)> func);
        void setOnDragOver(std::function<uint8_t(int, int, int)> func);
        void setOnDragLeave(std::function<uint8_t()> func);
        void setOnDragDrop(std::function<uint8_t(IDataObject*, int, int)> func);
    protected:
        //IUnknown methods
        virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObj);
        virtual ULONG STDMETHODCALLTYPE AddRef();
        virtual ULONG STDMETHODCALLTYPE Release();
        
        virtual HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
        virtual HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
        virtual HRESULT STDMETHODCALLTYPE DragLeave();
        virtual HRESULT STDMETHODCALLTYPE Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);

    private:
        HWND wndHandle;
        bool valid;
        int dataType;
    };
#endif