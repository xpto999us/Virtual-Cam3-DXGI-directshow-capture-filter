#define WIN32_LEAN_AND_MEAN 
#pragma warning(disable:4244)
#pragma warning(disable:4711)

#include <streams.h>
#include <stdio.h>
#include <olectl.h>
#include <dvdmedia.h>
#include <smartptr.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <memory>
#include <algorithm>
#include <string>
#pragma comment(lib, "D3D11.lib")

//////////////////////////////////////////////////////////////////////////
//  This file contains routines to register / Unregister the 
//  Directshow filter 'Virtual Cam'
//  We do not use the inbuilt BaseClasses routines as we need to register as
//  a capture source
//////////////////////////////////////////////////////////////////////////
#pragma comment(lib, "kernel32")
#pragma comment(lib, "user32")
#pragma comment(lib, "gdi32")
#pragma comment(lib, "advapi32")
#pragma comment(lib, "winmm")
#pragma comment(lib, "ole32")
#pragma comment(lib, "oleaut32")

#ifdef _DEBUG
    #pragma comment(lib, "strmbasd")
#else
    #pragma comment(lib, "strmbase")
#endif
#include <olectl.h>
#include <initguid.h>
#include <dllsetup.h>

#define CreateComObject(clsid, iid, var) CoCreateInstance( clsid, NULL, CLSCTX_INPROC_SERVER, iid, (void **)&var);
#define BYTES_ALIGN 4
#pragma once


#define DECLARE_PTR(type, ptr, expr) type* ptr = (type*)(expr);
#ifdef  DEBUG
#define REGISTER_FILTERGRAPH
#endif
bool m_bFormatAlreadySet=true;

//HBITMAP m_hDIB;
//BYTE *m_pDIBData;
HDC m_hDC, m_hDeskDC;
//BYTE *pData;
BITMAPINFO	lBmpInfo;
//std::unique_ptr<BYTE> pBuf(new BYTE[lBmpInfo.bmiHeader.biSizeImage]);
D3D11_MAPPED_SUBRESOURCE mapResource;
//unsigned int frame_count=100;

void cleanrefs();


EXTERN_C const GUID CLSID_Virtualcam3;

STDAPI AMovieSetupRegisterServer( CLSID   clsServer, LPCWSTR szDescription, LPCWSTR szFileName, LPCWSTR szThreadingModel = L"Both", LPCWSTR szServerType     = L"InprocServer32" );
STDAPI AMovieSetupUnregisterServer( CLSID clsServer );

DEFINE_GUID(CLSID_Virtualcam3, 
0x6f7b75af,0xefdf,0x4a03,0x97,0x71,0xe7,0x7f,0x4b,0x86,0x5e,0x84);

class DECLSPEC_UUID("6F7B75AF-EFDF-4A03-9771-E77F4B865E84") ICaptureConfig;

int width1 = GetSystemMetrics(SM_CXSCREEN);
int height1 = GetSystemMetrics(SM_CYSCREEN);
FILE *fp;char letra;
//int inic=0;
//int lresult=0;
 
HRESULT hr(E_FAIL);

	HDC _hdc;
	HWND _hwnd;
	HBITMAP _bitmap;
	//int _width=width1, _height=height1;
	HDC  lHDC;


template <typename T>
class CComPtrCustom
{
public:

	CComPtrCustom(T *aPtrElement)
		:element(aPtrElement)
	{
	}

	CComPtrCustom()
		:element(nullptr)
	{
	}

	virtual ~CComPtrCustom()
	{
		Release();
	}

	T* Detach()
	{
		auto lOutPtr = element;
		element = nullptr;
		return lOutPtr;
	}

	T* detach()
	{
		return Detach();
	}

	void Release()
	{
		if (element == nullptr)
			return;
		auto k = element->Release();
		element = nullptr;
	}

	CComPtrCustom& operator = (T *pElement)
	{
		Release();
		if (pElement == nullptr)
			return *this;

		auto k = pElement->AddRef();
		element = pElement;
		return *this;
	}

	void Swap(CComPtrCustom& other)
	{
		T* pTemp = element;
		element = other.element;
		other.element = pTemp;
	}

	T* operator->()
	{
		return element;
	}

	operator T*()
	{
		return element;
	}

	operator T*() const
	{
		return element;
	}


	T* get()
	{
		return element;
	}

	T* get() const
	{
		return element;
	}

	T** operator &()
	{
		return &element;
	}

	bool operator !()const
	{
		return element == nullptr;
	}

	operator bool()const
	{
		return element != nullptr;
	}

	bool operator == (const T *pElement)const
	{
		return element == pElement;
	}


	CComPtrCustom(const CComPtrCustom& aCComPtrCustom)
	{
		if (aCComPtrCustom.operator!())
		{
			element = nullptr;
			return;
		}

		element = aCComPtrCustom;
		auto h = element->AddRef();
		h++;
	}

	CComPtrCustom& operator = (const CComPtrCustom& aCComPtrCustom)
	{
		Release();
		element = aCComPtrCustom;
		auto k = element->AddRef();
		return *this;
	}

	_Check_return_ HRESULT CopyTo(T** ppT) throw()
	{
		if (ppT == NULL)
			return E_POINTER;
		*ppT = element;

		if (element)
			element->AddRef();

		return S_OK;
	}

	HRESULT CoCreateInstance(const CLSID aCLSID)
	{
		T* lPtrTemp;
		auto lresult = ::CoCreateInstance(aCLSID, NULL, CLSCTX_INPROC, IID_PPV_ARGS(&lPtrTemp));

		if (SUCCEEDED(lresult))
		{
			if (lPtrTemp != nullptr)
			{
				Release();
				element = lPtrTemp;
			}

		}

		return lresult;
	}

protected:

	T* element;
};

// Driver types supported
D3D_DRIVER_TYPE gDriverTypes[] =
{
	D3D_DRIVER_TYPE_HARDWARE  //D3D_DRIVER_TYPE_HARDWARE  esse é o tipo mais rapido
};
UINT gNumDriverTypes = ARRAYSIZE(gDriverTypes);

// Feature levels supported
D3D_FEATURE_LEVEL gFeatureLevels[] =
{
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0,
	D3D_FEATURE_LEVEL_9_1
};

//CComPtrCustom<IDXGIAdapter> lDxgiAdapter;
UINT gNumFeatureLevels = ARRAYSIZE(gFeatureLevels);
CComPtrCustom<ID3D11Device> lDevice;
	CComPtrCustom<ID3D11DeviceContext> lImmediateContext;
	CComPtrCustom<IDXGIOutputDuplication> lDeskDupl;
	CComPtrCustom<ID3D11Texture2D> lAcquiredDesktopImage;
	CComPtrCustom<ID3D11Texture2D> lGDIImage;    //inutil???
	CComPtrCustom<ID3D11Texture2D> lDestImage;
	DXGI_OUTPUT_DESC lOutputDesc;
	DXGI_OUTDUPL_DESC lOutputDuplDesc;
	UINT Output = 0;
    D3D11_TEXTURE2D_DESC desc;
	CComPtrCustom<IDXGIResource> lDesktopResource;
	DXGI_OUTDUPL_FRAME_INFO lFrameInfo;
//void LocalOutput(const char *str, ...);
//void LocalOutput(const wchar_t *str, ...);	
	
class Cvcam3Stream;
class Cvcam3 : public CSource  //  public ICaptureConfig
{
public:
   // DECLARE_IUNKNOWN;
    //////////////////////////////////////////////////////////////////////////
    //  IUnknown
    //////////////////////////////////////////////////////////////////////////
	STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);
   
    IFilterGraph *GetGraph() {return m_pGraph;}
	// ICaptureConfig 
    void SetCaptureRect(int x, int y, int w=width1, int h=height1, int fps=30); 

private:
    Cvcam3(LPUNKNOWN lpunk, HRESULT *phr);
};

class ICaptureConfig : public IUnknown 
{ 
public: 
    virtual void SetCaptureRect(int x=0, int y=0, int w=width1, int h=height1, int fps=30); 
}; 




class Cvcam3Stream : public CSourceStream, public IAMStreamConfig, public IKsPropertySet
{
public:
   // DECLARE_IUNKNOWN;
	//////////////////////////////////////////////////////////////////////////
    //  IUnknown
    //////////////////////////////////////////////////////////////////////////
	STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
    STDMETHODIMP_(ULONG) AddRef() { return GetOwner()->AddRef(); }                                                          \
    STDMETHODIMP_(ULONG) Release() { return GetOwner()->Release(); }
	
   // static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);
    
    
     //////////////////////////////////////////////////////////////////////////
    //  IQualityControl
    //////////////////////////////////////////////////////////////////////////
    STDMETHODIMP Notify(IBaseFilter * pSender, Quality q);

    //////////////////////////////////////////////////////////////////////////
    //  IAMStreamConfig
    //////////////////////////////////////////////////////////////////////////
    HRESULT STDMETHODCALLTYPE SetFormat(AM_MEDIA_TYPE *pmt);
    HRESULT STDMETHODCALLTYPE GetFormat(AM_MEDIA_TYPE **ppmt);
    HRESULT STDMETHODCALLTYPE GetNumberOfCapabilities(int *piCount, int *piSize);
    HRESULT STDMETHODCALLTYPE GetStreamCaps(int iIndex, AM_MEDIA_TYPE **pmt, BYTE *pSCC);

    //////////////////////////////////////////////////////////////////////////
    //  IKsPropertySet
    //////////////////////////////////////////////////////////////////////////
    HRESULT STDMETHODCALLTYPE Set(REFGUID guidPropSet, DWORD dwID, void *pInstanceData, DWORD cbInstanceData, void *pPropData, DWORD cbPropData);
    HRESULT STDMETHODCALLTYPE Get(REFGUID guidPropSet, DWORD dwPropID, void *pInstanceData,DWORD cbInstanceData, void *pPropData, DWORD cbPropData, DWORD *pcbReturned);
    HRESULT STDMETHODCALLTYPE QuerySupported(REFGUID guidPropSet, DWORD dwPropID, DWORD *pTypeSupport);
    
    //////////////////////////////////////////////////////////////////////////
    //  CSourceStream
    //////////////////////////////////////////////////////////////////////////
    Cvcam3Stream(HRESULT *phr, Cvcam3 *pParent, LPCWSTR pPinName);
    ~Cvcam3Stream();

    HRESULT FillBuffer(IMediaSample *pms);
    HRESULT DecideBufferSize(IMemAllocator *pIMemAlloc, ALLOCATOR_PROPERTIES *pProperties);
    HRESULT CheckMediaType(const CMediaType *pMediaType);
    HRESULT GetMediaType(int iPosition, CMediaType *pmt);
    HRESULT SetMediaType(const CMediaType *pmt);
    HRESULT OnThreadCreate(void);
	HRESULT OnThreadDestroy(void);
	// ICaptureConfig
	void SetCaptureRect(int x, int y, int w=width1, int h=height1, int fps=30);
    
private:
    Cvcam3 *m_pParent;
    REFERENCE_TIME m_rtLastTime;
    HBITMAP m_hLogoBmp;
    CCritSec m_cSharedState;
    IReferenceClock *m_pClock;
	
    int m_x, m_y, m_w=width1, m_h=height1; 
 
    
};


const AMOVIESETUP_MEDIATYPE AMSMediaTypesvcam3 = 
{ 
    &MEDIATYPE_Video, 
    &MEDIASUBTYPE_NULL 
};

const AMOVIESETUP_PIN AMSPinvcam3=
{
    L"Output",             // Pin string name
    FALSE,                 // Is it rendered
    TRUE,                  // Is it an output
    FALSE,                 // Can we have none
    FALSE,                 // Can we have many
    &CLSID_NULL,           // Connects to filter
    NULL,                  // Connects to pin
    1,                     // Number of types
    &AMSMediaTypesvcam3      // Pin Media types
};

const AMOVIESETUP_FILTER AMSFiltervcam3 =
{
    &CLSID_Virtualcam3,  // Filter CLSID
    L"Virtual Cam3",     // String name
    MERIT_DO_NOT_USE,      // Filter merit
    1,                     // Number pins
    &AMSPinvcam3             // Pin details
};

CFactoryTemplate g_Templates[] = 
{
    {
        L"Virtual Cam",
        &CLSID_Virtualcam3,
        Cvcam3::CreateInstance,
        NULL,
        &AMSFiltervcam3
    },

};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

STDAPI RegisterFilters( BOOL bRegister )
{
    HRESULT hr = NOERROR;
    WCHAR achFileName[MAX_PATH];
    char achTemp[MAX_PATH];
    ASSERT(g_hInst != 0);

    if( 0 == GetModuleFileNameA(g_hInst, achTemp, sizeof(achTemp))) 
        return AmHresultFromWin32(GetLastError());

    MultiByteToWideChar(CP_ACP, 0L, achTemp, lstrlenA(achTemp) + 1, 
                       achFileName, NUMELMS(achFileName));
  
    hr = CoInitialize(0);
    if(bRegister)
    {
        hr = AMovieSetupRegisterServer(CLSID_Virtualcam3, L"Virtual Cam3", achFileName, L"Both", L"InprocServer32");
    }

    if( SUCCEEDED(hr) )
    {
        IFilterMapper2 *fm = 0;
        hr = CreateComObject( CLSID_FilterMapper2, IID_IFilterMapper2, fm );
        if( SUCCEEDED(hr) )
        {
            if(bRegister)
            {
                IMoniker *pMoniker = 0;
                REGFILTER2 rf2;
                rf2.dwVersion = 1;
                rf2.dwMerit = MERIT_DO_NOT_USE;
                rf2.cPins = 1;
                rf2.rgPins = &AMSPinvcam3;
                hr = fm->RegisterFilter(CLSID_Virtualcam3, L"Virtual Cam3", &pMoniker, &CLSID_VideoInputDeviceCategory, NULL, &rf2);
            }
            else
            {
                hr = fm->UnregisterFilter(&CLSID_VideoInputDeviceCategory, 0, CLSID_Virtualcam3);
            }
        }

      // release interface
      //
      if(fm)
          fm->Release();
    }

    if( SUCCEEDED(hr) && !bRegister )
        hr = AMovieSetupUnregisterServer( CLSID_Virtualcam3 );

    CoFreeUnusedLibraries();
    CoUninitialize();
    return hr;
}

STDAPI DllRegisterServer()
{
    return RegisterFilters(TRUE);
}

STDAPI DllUnregisterServer()
{
    return RegisterFilters(FALSE);
}

extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID lpReserved)
{
	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}

//////////////////////////////////////////////////////////////////////////
//  Cvcam3 is the source filter which masquerades as a capture device
//////////////////////////////////////////////////////////////////////////
CUnknown * WINAPI Cvcam3::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
    ASSERT(phr);
    CUnknown *punk = new Cvcam3(lpunk, phr);
    return punk;
}

Cvcam3::Cvcam3(LPUNKNOWN lpunk, HRESULT *phr) : 
    CSource(NAME("Virtual Cam3"), lpunk, CLSID_Virtualcam3)
{
    ASSERT(phr);
    CAutoLock cAutoLock(&m_cStateLock);
    // Create the one and only output pin
    m_paStreams = (CSourceStream **) new Cvcam3Stream*[1];
    m_paStreams[0] = new Cvcam3Stream(phr, this, L"Virtual Cam");
}

HRESULT Cvcam3::QueryInterface(REFIID riid, void **ppv)
{
    //Forward request for IAMStreamConfig & IKsPropertySet to the pin
    if(riid == _uuidof(IAMStreamConfig) || riid == _uuidof(IKsPropertySet))
        return m_paStreams[0]->QueryInterface(riid, ppv);
    else
        return CSource::QueryInterface(riid, ppv);
	if(riid == __uuidof(ICaptureConfig))   
        {   
           return GetInterface((ICaptureConfig*)this, ppv);   
        }   
       else   
           return CSource::QueryInterface(riid, ppv);
}

void Cvcam3::SetCaptureRect(int x, int y, int w, int h, int fps)   
    {   
        ((Cvcam3Stream*)m_paStreams[0])->SetCaptureRect(x, y, w, h, fps);   
    }   
//////////////////////////////////////////////////////////////////////////
// Cvcam3Stream is the one and only output pin of Cvcam3 which handles 
// all the stuff.
//////////////////////////////////////////////////////////////////////////
Cvcam3Stream::Cvcam3Stream(HRESULT *phr, Cvcam3 *pParent, LPCWSTR pPinName) :
    CSourceStream(NAME("Virtual Cam"),phr, pParent, pPinName), m_pParent(pParent)
{
    // Set the default media type as 320x240x24@15
    GetMediaType(4, &m_mt);
	m_hDeskDC = GetDC(GetDesktopWindow());   
        m_hDC = CreateCompatibleDC(0); 
		SetCaptureRect(0, 0, width1, height1, 30);
}

void Cvcam3Stream::SetCaptureRect(int x, int y, int w, int h, int fps)   
    {   
	    
	if(IsConnected()) return;  
	
       D3D_FEATURE_LEVEL lFeatureLevel;
	   
	  	// Create device
		for (UINT DriverTypeIndex = 0; DriverTypeIndex < gNumDriverTypes; ++DriverTypeIndex)
		{
			hr = D3D11CreateDevice(nullptr,gDriverTypes[DriverTypeIndex],nullptr,
				0,gFeatureLevels, gNumFeatureLevels, D3D11_SDK_VERSION,&lDevice,
				&lFeatureLevel,&lImmediateContext);

			if (SUCCEEDED(hr))
			{
								  //FALTA FAZER LOOP???
				// Device creation success, no need to loop anymore
			}
			//lDevice.Release();
			//lImmediateContext.Release();
		}
		/*
    fp=fopen("c:\\w1\\log.txt","w+");
	setvbuf (fp, NULL ,_IONBF, 1024 );
	letra=0x41; fputc(letra,fp);
	fclose(fp);
       	*/
		//if (FAILED(hr))	return  hr;
        lDevice->AddRef();
		lImmediateContext->AddRef();
		//Sleep(100);

		//if (lDevice == nullptr)	break;
		
		// Get DXGI device
		CComPtrCustom<IDXGIDevice> lDxgiDevice=nullptr;

		hr = lDevice->QueryInterface(IID_PPV_ARGS(&lDxgiDevice));

		//if (FAILED(hr))	break;

		// Get DXGI adapter
		CComPtrCustom<IDXGIAdapter> lDxgiAdapter=nullptr;
		hr = lDxgiDevice->GetParent(__uuidof(IDXGIAdapter),reinterpret_cast<void**>(&lDxgiAdapter));

		//if (FAILED(hr))	break;

		//lDxgiDevice.Release();
		
		// Get output
		CComPtrCustom<IDXGIOutput> lDxgiOutput=nullptr;
		hr = lDxgiAdapter->EnumOutputs(Output,&lDxgiOutput);

		//if (FAILED(hr))
		//	break;

		lDxgiAdapter.Release();
     
		hr = lDxgiOutput->GetDesc(&lOutputDesc);

		//if (FAILED(hr))
		//	break;

		// QI for Output 1
		CComPtrCustom<IDXGIOutput1> lDxgiOutput1=nullptr;

		hr = lDxgiOutput->QueryInterface(IID_PPV_ARGS(&lDxgiOutput1));

		//if (FAILED(hr))
		//	break;

		lDxgiOutput.Release();
        
		if (lDeskDupl) {
		lDeskDupl->Release();
		lDeskDupl = nullptr;
	                   }
        					   
		// Create desktop duplication
		hr = lDxgiOutput1->DuplicateOutput(lDevice,&lDeskDupl);

		//if (FAILED(hr))
		//	break;

		lDxgiOutput1.Release();
		lDxgiOutput1=nullptr;
		
		// Create GUI drawing texture
		
		RtlZeroMemory(&lOutputDesc, sizeof(DXGI_OUTPUT_DESC));  // a verificar
		lDeskDupl->GetDesc(&lOutputDuplDesc);       // a verificar

		//D3D11_TEXTURE2D_DESC desc;
  
		//if (FAILED(hr))
		//	break;

		//if (lGDIImage == nullptr)
		//	break;					

		// Create CPU access texture

		desc.Width = lOutputDuplDesc.ModeDesc.Width;
		desc.Height = lOutputDuplDesc.ModeDesc.Height;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;  //lOutputDuplDesc.ModeDesc.Format;
		desc.ArraySize = 1;
		desc.BindFlags = 0;
		desc.MiscFlags = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.MipLevels = 1;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;  // desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE; 
		desc.Usage = D3D11_USAGE_STAGING;
	
		
		hr = lDevice->CreateTexture2D(&desc, NULL, &lDestImage);
	
		//if (FAILED(hr))
		//	break;

		//if (lDestImage == nullptr)
		//	break;

		//CComPtrCustom<IDXGIResource> lDesktopResource;
		
		
		//CODIGO ANTIGO DO Vcam3
		
        m_x = x;   
        m_y = y;   
        m_w = width1;   
        m_h = height1;   
		       
        // Measure capture rate 
     		
        DWORD tick = GetTickCount();  
        
        double actualFPS = 1000.0 / (double)tick;   
        // if(fps < actualFPS)
            fps=30;			
            actualFPS = fps;   
       
        DECLARE_PTR(VIDEOINFOHEADER, pvi, m_mt.AllocFormatBuffer(sizeof(VIDEOINFOHEADER)));   
        ZeroMemory(pvi, sizeof(VIDEOINFOHEADER));   
       
        pvi->bmiHeader.biCompression = BI_RGB;   
        pvi->bmiHeader.biBitCount    = 32; //GetDeviceCaps(m_hDeskDC, BITSPIXEL);   
        pvi->bmiHeader.biSize       = sizeof(BITMAPINFOHEADER);   
        pvi->bmiHeader.biWidth      = width1; //m_w;   
        pvi->bmiHeader.biHeight     = height1; //m_h;   
        pvi->bmiHeader.biPlanes     = 1;   
        pvi->bmiHeader.biSizeImage  = GetBitmapSize(&pvi->bmiHeader);   
        pvi->bmiHeader.biClrImportant = 0;   
       
        pvi->AvgTimePerFrame = 10000000 / actualFPS;   
       
        SetRectEmpty(&(pvi->rcSource)); // we want the whole image area rendered.   
        SetRectEmpty(&(pvi->rcTarget)); // no particular destination rectangle   
       
        m_mt.SetType(&MEDIATYPE_Video);   
        m_mt.SetFormatType(&FORMAT_VideoInfo);   
        m_mt.SetTemporalCompression(FALSE);   
       
        // Work out the GUID for the subtype from the header info.   
        const GUID SubTypeGUID = GetBitmapSubtype(&pvi->bmiHeader);   
        m_mt.SetSubtype(&SubTypeGUID);   
        m_mt.SetSampleSize(pvi->bmiHeader.biSizeImage);  
        
}	


Cvcam3Stream::~Cvcam3Stream()
{

if (lAcquiredDesktopImage)
        {
        lAcquiredDesktopImage->Release();
        lAcquiredDesktopImage = NULL;
        }
if (lDesktopResource) lDesktopResource.Release(); 

if (lDeskDupl)
{lDeskDupl->Release();
lDeskDupl=nullptr;
}

if(lGDIImage)                       //inutil????
{lGDIImage->Release();
lGDIImage=nullptr;
}

if (lDestImage)
{lDestImage->Release();
lDestImage=nullptr;
}

//cleanrefs();
//if (sptr)
//{delete[] sptr; sptr=nullptr;}

} //~cvcam3stream

HRESULT Cvcam3Stream::QueryInterface(REFIID riid, void **ppv)
{   
    // Standard OLE stuff
    if(riid == _uuidof(IAMStreamConfig))
        *ppv = (IAMStreamConfig*)this;
    else if(riid == _uuidof(IKsPropertySet))
        *ppv = (IKsPropertySet*)this;
    else
        return CSourceStream::QueryInterface(riid, ppv);

    AddRef();
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//  This is the routine where we create the data being output by the Virtual
//  Camera device.
//////////////////////////////////////////////////////////////////////////

HRESULT Cvcam3Stream::FillBuffer(IMediaSample *pms)
{
	
	REFERENCE_TIME rtNow;
    
    REFERENCE_TIME avgFrameTime = ((VIDEOINFOHEADER*)m_mt.pbFormat)->AvgTimePerFrame;

    rtNow = m_rtLastTime;
    m_rtLastTime += avgFrameTime;
    pms->SetTime(&rtNow, &m_rtLastTime);
    pms->SetSyncPoint(TRUE);
	
	
	lDeskDupl->ReleaseFrame();
	//lDeskDupl->AcquireNextFrame(0, &lFrameInfo, &lDesktopResource)) && lFrameInfo.LastPresentTime.QuadPart)
     hr=lDeskDupl->AcquireNextFrame(33, &lFrameInfo, &lDesktopResource); 
	 if (FAILED(hr)) goto velho;
     if (hr == DXGI_ERROR_WAIT_TIMEOUT)	goto velho;
		// QI for ID3D11Texture2D

		// If still holding old frame, destroy it
       if (lAcquiredDesktopImage)
        {
        lAcquiredDesktopImage->Release();
        lAcquiredDesktopImage = NULL;
        }
		
		hr = lDesktopResource->QueryInterface(IID_PPV_ARGS(&lAcquiredDesktopImage));
		
		lDesktopResource.Release();
		//lDeskDupl->ReleaseFrame();
       	//if (lAcquiredDesktopImage == nullptr)
		//	break;
				
		// Copy image into GDI drawing texture
    velho:
		lImmediateContext->CopyResource(lDestImage, lAcquiredDesktopImage);
		lAcquiredDesktopImage.Release();
		//UINT subresource = D3D11CalcSubresource(0, 0, 0);
		hr=lImmediateContext->Map(lDestImage,0,D3D11_MAP_READ, 0,&mapResource);   //hr=lImmediateContext->Map(lDestImage,subresource,D3D11_MAP_READ, NULL,&mapResource);
		
		lImmediateContext->Unmap(lDestImage, 0);    //lImmediateContext->Unmap(lDestImage, subresource);
		
		/*
	    REFERENCE_TIME rtNow;   
        REFERENCE_TIME avgFrameTime = ((VIDEOINFOHEADER*)m_mt.pbFormat)->AvgTimePerFrame;   
       avgFrameTime = ((VIDEOINFOHEADER*)m_mt.pbFormat)->AvgTimePerFrame;
        rtNow = m_rtLastTime;   
        m_rtLastTime += avgFrameTime;
		pms->SetTime(&rtNow, &m_rtLastTime);
        pms->SetSyncPoint(TRUE);
		*/
       // long lDataLen; 
	    BYTE *pData = (BYTE*)mapResource.pData;
        pms->GetPointer(&pData);   
        
        
		//memcpy(pData,mapResource.pData, mapResource.DepthPitch);  // OK!!!!!!!!!!!!
		
		BYTE* sptr = reinterpret_cast<BYTE*>(mapResource.pData);
		UINT offset = height1 -1;
		//memcpy(pData, sptr, mapResource.DepthPitch);
		
		for (size_t h = 0; h < lOutputDuplDesc.ModeDesc.Height; ++h)      
		{
			
			memcpy_s(pData, mapResource.RowPitch, sptr + (offset * mapResource.RowPitch), mapResource.RowPitch );  //memcpy_s(dptr, lBmpRowPitch, sptr, lRowPitch);
			offset= offset-1;
			pData += mapResource.RowPitch;
			
		}
	
	return NOERROR;
} // FillBuffer


//
// Notify
// Ignore quality management messages sent from the downstream filter
STDMETHODIMP Cvcam3Stream::Notify(IBaseFilter * pSender, Quality q)
{
    return E_NOTIMPL;
} // Notify

//////////////////////////////////////////////////////////////////////////
// This is called when the output format has been negotiated
//////////////////////////////////////////////////////////////////////////
HRESULT Cvcam3Stream::SetMediaType(const CMediaType *pmt)
{
    DECLARE_PTR(VIDEOINFOHEADER, pvi, pmt->Format());
    HRESULT hr = CSourceStream::SetMediaType(pmt);
	//MakeDIB();
    return hr;
}

// See Directshow help topic for IAMStreamConfig for details on this method
HRESULT Cvcam3Stream::GetMediaType(int iPosition, CMediaType *pmt)
{
	
CheckPointer(pmt, E_POINTER);
    CAutoLock cAutoLock(m_pFilter->pStateLock());
	if(m_bFormatAlreadySet) {
		// you can only have one option, buddy, if setFormat already called. (see SetFormat's msdn)
		if(iPosition != 0)
          return E_INVALIDARG;
		VIDEOINFO *pvi = (VIDEOINFO *) m_mt.Format();

		// Set() copies these in for us pvi->bmiHeader.biSizeImage  = GetBitmapSize(&pvi->bmiHeader); // calculates the size for us, after we gave it the width and everything else we already chucked into it
        // pmt->SetSampleSize(pvi->bmiHeader.biSizeImage);
		// nobody uses sample size anyway :P

		pmt->Set(m_mt);
		VIDEOINFOHEADER *pVih1 = (VIDEOINFOHEADER*) m_mt.pbFormat;
		VIDEOINFO *pviHere = (VIDEOINFO  *) pmt->pbFormat;
		return S_OK;
	}	


   if(iPosition < 0) return E_INVALIDARG;
    if(iPosition > 8) return VFW_S_NO_MORE_ITEMS;

    if(iPosition == 0) 
    {
		iPosition=1;
		
	    //*pmt = m_mt;	
		//return S_OK;
    }

   // DECLARE_PTR(VIDEOINFOHEADER, pvi, pmt->AllocFormatBuffer(sizeof(VIDEOINFOHEADER)));
   // ZeroMemory(pvi, sizeof(VIDEOINFOHEADER));
	VIDEOINFO *pvi = (VIDEOINFO *) pmt->AllocFormatBuffer(sizeof(VIDEOINFO));
	 if(NULL == pvi)
        return(E_OUTOFMEMORY);
	ZeroMemory(pvi, sizeof(VIDEOINFO));

    pvi->bmiHeader.biCompression = BI_RGB;
    pvi->bmiHeader.biBitCount    = 32;
    pvi->bmiHeader.biSize       = sizeof(BITMAPINFOHEADER);
    pvi->bmiHeader.biWidth      = width1;   // width1 * iPosition; 
    pvi->bmiHeader.biHeight     = height1; // height1 * iPosition;
    pvi->bmiHeader.biPlanes     = 1;
    pvi->bmiHeader.biSizeImage  = GetBitmapSize(&pvi->bmiHeader);
    pvi->bmiHeader.biClrImportant = 0;

    pvi->AvgTimePerFrame = 1000000;

    SetRectEmpty(&(pvi->rcSource)); // we want the whole image area rendered.
    SetRectEmpty(&(pvi->rcTarget)); // no particular destination rectangle

    pmt->SetType(&MEDIATYPE_Video);
    pmt->SetFormatType(&FORMAT_VideoInfo);
    pmt->SetTemporalCompression(FALSE);
 
     // Work out the GUID for the subtype from the header info.
	if(*pmt->Subtype() == GUID_NULL) {
      const GUID SubTypeGUID = GetBitmapSubtype(&pvi->bmiHeader);
      pmt->SetSubtype(&SubTypeGUID);
	}	
/*	
    // Work out the GUID for the subtype from the header info.
    const GUID SubTypeGUID = GetBitmapSubtype(&pvi->bmiHeader);
    pmt->SetSubtype(&SubTypeGUID);
    pmt->SetSampleSize(pvi->bmiHeader.biSizeImage);
*/    
	
return NOERROR;
} // GetMediaType


// This method is called to see if a given output format is supported
HRESULT Cvcam3Stream::CheckMediaType(const CMediaType *pMediaType)
{
    VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)(pMediaType->Format());
    if(*pMediaType != m_mt)  return E_INVALIDARG;
    return S_OK;
} // CheckMediaType

// This method is called after the pins are connected to allocate buffers to stream data
HRESULT Cvcam3Stream::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties)
{
    CAutoLock cAutoLock(m_pFilter->pStateLock());
    HRESULT hr = NOERROR;

    VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *) m_mt.Format();
    pProperties->cBuffers = 1;
    pProperties->cbBuffer = pvi->bmiHeader.biSizeImage;

    ALLOCATOR_PROPERTIES Actual;
    hr = pAlloc->SetProperties(pProperties,&Actual);

    if(FAILED(hr)) return hr;
    if(Actual.cbBuffer < pProperties->cbBuffer) return E_FAIL;

    return NOERROR;
} // DecideBufferSize

// Called when graph is run
HRESULT Cvcam3Stream::OnThreadCreate()
{
	/*
    fp=fopen("c:\\w1\\log.txt","w+");
	setvbuf (fp, NULL ,_IONBF, 1024 );
	letra=0x41; fputc(letra,fp);
	fclose(fp);
	*/
	m_rtLastTime = 0;
    return NOERROR;
} // OnThreadCreate

HRESULT Cvcam3Stream::OnThreadDestroy()
{
    m_rtLastTime = 0;
    return NOERROR;
} // OnThreadCreate


//////////////////////////////////////////////////////////////////////////
//  IAMStreamConfig
//////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE Cvcam3Stream::SetFormat(AM_MEDIA_TYPE *pmt)
{
    DECLARE_PTR(VIDEOINFOHEADER, pvi, m_mt.pbFormat);
    m_mt = *pmt;
    IPin* pin; 
    ConnectedTo(&pin);
    if(pin)
    {
        IFilterGraph *pGraph = m_pParent->GetGraph();
        pGraph->Reconnect(this);
    }
    return S_OK;
}
 

HRESULT STDMETHODCALLTYPE Cvcam3Stream::GetFormat(AM_MEDIA_TYPE **ppmt)
{
    *ppmt = CreateMediaType(&m_mt);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE Cvcam3Stream::GetNumberOfCapabilities(int *piCount, int *piSize)
{
    *piCount = 8;
    *piSize = sizeof(VIDEO_STREAM_CONFIG_CAPS);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE Cvcam3Stream::GetStreamCaps(int iIndex, AM_MEDIA_TYPE **pmt, BYTE *pSCC)
{
    *pmt = CreateMediaType(&m_mt);
    DECLARE_PTR(VIDEOINFOHEADER, pvi, (*pmt)->pbFormat);

    if (iIndex == 0) iIndex = 4;

    pvi->bmiHeader.biCompression = BI_RGB;
    pvi->bmiHeader.biBitCount    = 32;
    pvi->bmiHeader.biSize       = sizeof(BITMAPINFOHEADER);
    pvi->bmiHeader.biWidth      =width1;//width1 * iIndex;
    pvi->bmiHeader.biHeight     =height1; //height1 * iIndex;
    pvi->bmiHeader.biPlanes     = 1;
    pvi->bmiHeader.biSizeImage  = GetBitmapSize(&pvi->bmiHeader);
    pvi->bmiHeader.biClrImportant = 0;

    SetRectEmpty(&(pvi->rcSource)); // we want the whole image area rendered.
    SetRectEmpty(&(pvi->rcTarget)); // no particular destination rectangle

    (*pmt)->majortype = MEDIATYPE_Video;
    (*pmt)->subtype = MEDIASUBTYPE_RGB32;
    (*pmt)->formattype = FORMAT_VideoInfo;
    (*pmt)->bTemporalCompression = FALSE;
    (*pmt)->bFixedSizeSamples= FALSE;
    (*pmt)->lSampleSize = pvi->bmiHeader.biSizeImage;
    (*pmt)->cbFormat = sizeof(VIDEOINFOHEADER);
    
    DECLARE_PTR(VIDEO_STREAM_CONFIG_CAPS, pvscc, pSCC);
    
    pvscc->guid = FORMAT_VideoInfo;
    pvscc->VideoStandard = AnalogVideo_None;
    pvscc->InputSize.cx = width1;
    pvscc->InputSize.cy = height1;
    pvscc->MinCroppingSize.cx = width1;
    pvscc->MinCroppingSize.cy = height1;
    pvscc->MaxCroppingSize.cx = width1;
    pvscc->MaxCroppingSize.cy = height1;
    pvscc->CropGranularityX = 1;
    pvscc->CropGranularityY = 1;
    pvscc->CropAlignX = 1;
    pvscc->CropAlignY = 1;

    pvscc->MinOutputSize.cx = width1;
    pvscc->MinOutputSize.cy = height1;
    pvscc->MaxOutputSize.cx = width1;
    pvscc->MaxOutputSize.cy = height1;
    pvscc->OutputGranularityX = 1;
    pvscc->OutputGranularityY = 1;
    pvscc->StretchTapsX = 0;
    pvscc->StretchTapsY = 0;
    pvscc->ShrinkTapsX = 0;
    pvscc->ShrinkTapsY = 0;
    pvscc->MinFrameInterval = 200000;   //50 fps
    pvscc->MaxFrameInterval = 50000000; // 0.2 fps
    pvscc->MinBitsPerSecond = (80 * 60 * 3 * 8) / 5;
    pvscc->MaxBitsPerSecond = width1 * height1 * 3 * 8 * 50;
	
	SetFormat(*pmt);

    return S_OK;
}



//////////////////////////////////////////////////////////////////////////
// IKsPropertySet
//////////////////////////////////////////////////////////////////////////


HRESULT Cvcam3Stream::Set(REFGUID guidPropSet, DWORD dwID, void *pInstanceData, 
                        DWORD cbInstanceData, void *pPropData, DWORD cbPropData)
{// Set: Cannot set any properties.
    return E_NOTIMPL;
}

// Get: Return the pin category (our only property). 
HRESULT Cvcam3Stream::Get(
    REFGUID guidPropSet,   // Which property set.
    DWORD dwPropID,        // Which property in that set.
    void *pInstanceData,   // Instance data (ignore).
    DWORD cbInstanceData,  // Size of the instance data (ignore).
    void *pPropData,       // Buffer to receive the property data.
    DWORD cbPropData,      // Size of the buffer.
    DWORD *pcbReturned     // Return the size of the property.
)
{
    if (guidPropSet != AMPROPSETID_Pin)             return E_PROP_SET_UNSUPPORTED;
    if (dwPropID != AMPROPERTY_PIN_CATEGORY)        return E_PROP_ID_UNSUPPORTED;
    if (pPropData == NULL && pcbReturned == NULL)   return E_POINTER;
    
    if (pcbReturned) *pcbReturned = sizeof(GUID);
    if (pPropData == NULL)          return S_OK; // Caller just wants to know the size. 
    if (cbPropData < sizeof(GUID))  return E_UNEXPECTED;// The buffer is too small.
        
    *(GUID *)pPropData = PIN_CATEGORY_CAPTURE;
    return S_OK;
}

// QuerySupported: Query whether the pin supports the specified property.
HRESULT Cvcam3Stream::QuerySupported(REFGUID guidPropSet, DWORD dwPropID, DWORD *pTypeSupport)
{
    if (guidPropSet != AMPROPSETID_Pin) return E_PROP_SET_UNSUPPORTED;
    if (dwPropID != AMPROPERTY_PIN_CATEGORY) return E_PROP_ID_UNSUPPORTED;
    // We support getting this property, but not setting it.
    if (pTypeSupport) *pTypeSupport = KSPROPERTY_SUPPORT_GET; 
    return S_OK;
}


void cleanrefs()
{
if (lAcquiredDesktopImage)
        {
        lAcquiredDesktopImage->Release();
        lAcquiredDesktopImage = NULL;
        }
if (lDesktopResource) lDesktopResource.Release(); 

if (lDeskDupl)
{lDeskDupl->Release();
lDeskDupl=nullptr;
}

if(lGDIImage)                       //inutil????
{lGDIImage->Release();
lGDIImage=nullptr;
}

if (lDestImage)
{lDestImage->Release();
lDestImage=nullptr;
}

//if (sptr)
//{delete[] sptr; sptr=nullptr;}

}	
