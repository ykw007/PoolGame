#include "poolgame.h"

const int AUDIO=1, VIDEO=2;

CVideo::CVideo(HWND hWnd,LPCTSTR szFileName)
{
	Clear();
	m_hWnd = hWnd;
	lstrcpy(m_szFileName,szFileName);
}

void CVideo::Clear()
{
	m_hWnd = NULL;

	
	// DirectShow interfaces
	m_pGB   = NULL;
	m_pMC   = NULL;
	m_pME   = NULL;
	m_pVW   = NULL;
	m_pBA   = NULL;
	m_pBV   = NULL;
	m_pMS   = NULL;
	m_pMP	= NULL;
	m_pFS	= NULL;

	m_psCurrent = Init;
	m_bAudioOnly = FALSE;
	m_dwGraphRegister = 0;
}

void CVideo::Destroy()
{
	CloseInterfaces();
	Clear();
}

void CVideo::CloseInterfaces()
{
    HRESULT hr;

    // Relinquish ownership (IMPORTANT!) after hiding video window
    if(m_pVW)
    {
        hr = m_pVW->put_Visible(OAFALSE);
        hr = m_pVW->put_Owner(NULL);
    }

    // Disable event callbacks
    if (m_pME)
        hr = m_pME->SetNotifyWindow((OAHWND)NULL, 0, 0);

#ifdef REGISTER_FILTERGRAPH
    if (m_dwGraphRegister)
    {
        RemoveGraphFromRot(m_dwGraphRegister);
        m_dwGraphRegister = 0;
    }
#endif

    // Release and zero DirectShow interfaces
    RELEASE(m_pME);
    RELEASE(m_pMS);
    RELEASE(m_pMP);
    RELEASE(m_pMC);
    RELEASE(m_pBA);
    RELEASE(m_pBV);
    RELEASE(m_pVW);
    RELEASE(m_pFS);
    RELEASE(m_pGB);
}

HRESULT CVideo::InitVideoWindow(int nMultiplier, int nDivider)
{
    LONG lHeight, lWidth;
    HRESULT hr = S_OK;
    RECT rect;

    if (!m_pBV)
        return S_OK;

    // Read the default video size
    hr = m_pBV->GetVideoSize(&lWidth, &lHeight);
    if (hr == E_NOINTERFACE)
        return S_OK;

	// Account for requests of normal, half, or double size
    lWidth  = lWidth  * nMultiplier / nDivider;
    lHeight = lHeight * nMultiplier / nDivider;

    JIF(m_pVW->SetWindowPosition(m_lX, m_lY, lWidth, lHeight));

    return hr;
}

HRESULT CVideo::PlayMovie()
{
    USES_CONVERSION;
    WCHAR wFile[MAX_PATH];
    HRESULT hr;

    if (!m_szFileName)
        return E_POINTER;

    // Clear open dialog remnants before calling RenderFile()
    UpdateWindow(m_hWnd);

    // Convert filename to wide character string
    wcsncpy(wFile, T2W(m_szFileName), NUMELMS(wFile)-1);
    wFile[MAX_PATH-1] = 0;

    // Get the interface for DirectShow's GraphBuilder
    JIF(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
                         IID_IGraphBuilder, (void **)&m_pGB));

    if(SUCCEEDED(hr))
    {
        // Have the graph builder construct its the appropriate graph automatically
        JIF(m_pGB->RenderFile(wFile, NULL));

        // QueryInterface for DirectShow interfaces
        JIF(m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC));
        JIF(m_pGB->QueryInterface(IID_IMediaEventEx, (void **)&m_pME));
        JIF(m_pGB->QueryInterface(IID_IMediaSeeking, (void **)&m_pMS));
        JIF(m_pGB->QueryInterface(IID_IMediaPosition, (void **)&m_pMP));

        // Query for video interfaces, which may not be relevant for audio files
        JIF(m_pGB->QueryInterface(IID_IVideoWindow, (void **)&m_pVW));
        JIF(m_pGB->QueryInterface(IID_IBasicVideo, (void **)&m_pBV));

        // Query for audio interfaces, which may not be relevant for video-only files
        JIF(m_pGB->QueryInterface(IID_IBasicAudio, (void **)&m_pBA));

        // Is this an audio-only file (no video component)?
        //CheckVisibility();

        // Have the graph signal event via window callbacks for performance
        //JIF(m_pME->SetNotifyWindow((OAHWND)m_hWnd, WM_GRAPHNOTIFY, 0));

        if (!m_bAudioOnly)
        {
            // Setup the video window
            JIF(m_pVW->put_Owner((OAHWND)m_hWnd));
            JIF(m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN));

            JIF(InitVideoWindow(1, 1));
            //GetFrameStepInterface();
        }

#ifdef REGISTER_FILTERGRAPH
        hr = AddGraphToRot(m_pGB, &m_dwGraphRegister);
        if (FAILED(hr))
        {
            Msg(TEXT("Failed to register filter graph with ROT!  hr=0x%x"), hr);
            m_dwGraphRegister = 0;
        }
#endif

        // Run the graph to play the media file
        JIF(m_pMC->Run());

        m_psCurrent=Running;
        SetFocus(m_hWnd);
    }

    return hr;
}

#ifdef REGISTER_FILTERGRAPH

HRESULT CVideo::AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) 
{
    IMoniker * pMoniker;
    IRunningObjectTable *pROT;
    if (FAILED(GetRunningObjectTable(0, &pROT))) 
    {
        return E_FAIL;
    }

    WCHAR wsz[128];
    wsprintfW(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, 
              GetCurrentProcessId());

    HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
    if (SUCCEEDED(hr)) 
    {
        // Use the ROTFLAGS_REGISTRATIONKEEPSALIVE to ensure a strong reference
        // to the object.  Using this flag will cause the object to remain
        // registered until it is explicitly revoked with the Revoke() method.
        //
        // Not using this flag means that if GraphEdit remotely connects
        // to this graph and then GraphEdit exits, this object registration 
        // will be deleted, causing future attempts by GraphEdit to fail until
        // this application is restarted or until the graph is registered again.
        hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph, 
                            pMoniker, pdwRegister);
        pMoniker->Release();
    }

    pROT->Release();
    return hr;
}

void CVideo::RemoveGraphFromRot(DWORD pdwRegister)
{
    IRunningObjectTable *pROT;

    if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) 
    {
        pROT->Revoke(pdwRegister);
        pROT->Release();
    }
}

#endif


