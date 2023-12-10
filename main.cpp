#include "framework.h"

#include "window.h"

#include "gdi.h"
#include "camera.h"

#include "image.h"

static const POINT pPanel = { 0x0, 64 };

static const int nWindowWidth = 960 + pPanel.x;
static const int nWindowHeight = 540 + pPanel.y;

static const int nScreenWidth = nWindowWidth - pPanel.x;
static const int nScreenHeight = nWindowHeight - pPanel.y;

#define _BUTTON_NEXT_T_ 0x0
#define _BUTTON_PREV_T_ 0x1

#define _BUTTON_NEXT_I_ 0x2
#define _BUTTON_PREV_I_ 0x3

#define _BUTTON_ADD_Nbit_ 0x4
#define _BUTTON_DIV_Nbit_ 0x5

#define _BUTTON_F_ 0xA

#define _BUTTON_MODE_ 0xC

#define _TEXT_Nbit_ 0xF

class _cEngine_ :
	public _cWindow_
{
	std::unique_ptr
		<_cGDI_> gdi = nullptr;

	std::unique_ptr
		<_cCamera_> camera = nullptr;



    std::unique_ptr
        <_cTexture_> pBackGround = nullptr;



    std::vector<
        std::unique_ptr<_cImage_>
    > m_vecImages;

    struct _sData_ {
        _cImage_*
            _IMAGE_ = nullptr;
        size_t _INDEX_ = 0x0;
    } _DATA_;

    uint8_t m_Nbit = 0x1;

    bool m_bMode = 0x0;



    POINT m_pMouse = {};

    bool m_bMouseLB = 0x0;
    int m_nWheelDelta = 0x0;

    vec2i m_vecMousePos = {};

    vec2i m_vecPan = {
        0x0, 0x0
    };

    vec2f m_vecImageBegin = {}, m_vecImageEnd = {};



    HWND m_hButton_Next_T = NULL,
         m_hButton_Prev_T = NULL;

    HWND m_hButton_Next_I = NULL,
         m_hButton_Prev_I = NULL;

    HWND m_hButton_F = NULL;



    HWND m_hButton_Add_Nbit = NULL,
         m_hButton_Div_Nbit = NULL;

    HWND m_hText_Nbit = NULL;

    HWND m_hButton_Mode = NULL;

#define WM_UPDATE WM_USER + 0x0

	LRESULT HandleMessage(UINT _In_ uMsg,
		WPARAM _In_ wParam, LPARAM _In_ lParam
    )
	{
		switch (uMsg) {

		case WM_CREATE: {
		    auto _CREATE_CONTROL_ = [=](POINT const& p) {
		        m_hButton_Next_T = CreateWindow(L"BUTTON", L"+", WS_VISIBLE | WS_CHILD,
                    p.x, p.y, 16, pPanel.y / 0x2, m_hWnd, (HMENU)_BUTTON_NEXT_T_, NULL, NULL
                );
		        m_hButton_Prev_T = CreateWindow(L"BUTTON", L"-", WS_VISIBLE | WS_CHILD,
                    p.x, p.y +  + pPanel.y / 0x2, 16, pPanel.y / 0x2, m_hWnd, (HMENU)_BUTTON_PREV_T_, NULL, NULL
                );

                (void)_UpdateControl_0_();

                m_hButton_Prev_I = CreateWindow(L"BUTTON", L"[Prev Image]", WS_VISIBLE | WS_CHILD,
                    p.x + 16, p.y, 128, pPanel.y / 0x2, m_hWnd, (HMENU)_BUTTON_PREV_I_, NULL, NULL
                );
                m_hButton_Next_I = CreateWindow(L"BUTTON", L"[Next Image]", WS_VISIBLE | WS_CHILD,
                    p.x + 16 + 128, p.y, 128, pPanel.y / 0x2, m_hWnd, (HMENU)_BUTTON_NEXT_I_, NULL, NULL
                );

                m_hButton_F = CreateWindow(L"BUTTON", L"[Filter Image]", WS_VISIBLE | WS_CHILD,
                    p.x + 16, p.y + pPanel.y / 0x2, 256, pPanel.y / 0x2, m_hWnd, (HMENU)_BUTTON_F_, NULL, NULL
                );

                (void)_UpdateControl_1_();



                m_hButton_Div_Nbit = CreateWindow(L"BUTTON", L"-", WS_VISIBLE | WS_CHILD,
                    p.x + 16 + 256, p.y + 0x0, 16, pPanel.y, m_hWnd, (HMENU)_BUTTON_DIV_Nbit_, NULL, NULL
                );
                m_hButton_Add_Nbit = CreateWindow(L"BUTTON", L"+", WS_VISIBLE | WS_CHILD,
                    p.x + 16 + 64 + 256, p.y + 0x0, 16, pPanel.y, m_hWnd, (HMENU)_BUTTON_ADD_Nbit_, NULL, NULL
                );

                m_hText_Nbit = CreateWindow(L"STATIC", L"Nbit", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER,
                    p.x + 16 + 256 + 16, p.y + pPanel.y / 0x3, 64 - 16, pPanel.y / 0x3, m_hWnd, (HMENU)_TEXT_Nbit_, NULL, NULL
                );



                m_hButton_Mode = CreateWindow(L"BUTTON", L"[Mode]", WS_VISIBLE | WS_CHILD | WS_BORDER | BS_AUTOCHECKBOX,
                    p.x + 16 + 256 + 64 + 16, p.y + pPanel.y / 0x3, 128, pPanel.y / 0x3, m_hWnd, (HMENU)_BUTTON_MODE_, NULL, NULL
                );

                (void)_UpdateControl_Nbit_();
		    };

            _CREATE_CONTROL_(
                { 0x0, 0x0 }
            );

			HDC hDC = GetDC(m_hWnd);
			gdi = std::make_unique
				<_cGDI_>(hDC, POINT{ nScreenWidth, nScreenHeight });
            camera = std::make_unique
                <_cCamera_>(1.f);



            pBackGround = std::make_unique
                <_cTexture_>(nScreenWidth, nScreenHeight);

            for (int y0 = 0x0; y0 < pBackGround->GetSize().y; y0++)
                for (int x0 = 0x0; x0 < pBackGround->GetSize().x; x0++) {
                    pBackGround->SetColour(x0, y0, RGBA((BYTE)std::min((x0 + y0) / 0x3, 0xFF)));
                }



            LPWSTR *szArglist = NULL;
            int nArgs = 0x0;

            szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);

            if (!szArglist) {
                DWORD dwError = ERROR_SUCCESS;

                exit(dwError);
            }

            for (int i = 0x1; i < nArgs; i++) {
                _cImage_* pI = nullptr;

                pI = _LoadImage_(szArglist[i]);
            }

            LocalFree(szArglist);



            (void)_SelectImage_(NULL);

            if (
                _DATA_._IMAGE_ != nullptr
            ) { (void)_SelectData_(_SOURCE_); }



            (void)SendMessage(
                m_hWnd, WM_UPDATE, 0x0, 0x0
            );

		} break;

		case WM_CLOSE: {
			if (!DestroyWindow(m_hWnd))
			{ exit(Helper::ShowLastError(L"DestroyWindow(m_hWnd)")); }
        } break;
		case WM_DESTROY: {
			PostQuitMessage(EXIT_SUCCESS);
		} break;

		case WM_SIZE: {
			m_wSize.store(
				{ (LONG)GET_X_LPARAM(lParam), (LONG)GET_Y_LPARAM(lParam) }
			);

			POINT p = m_wSize;

			gdi->ClearRect(
                { 0x0, 0x0, 0x1 * p.x + pPanel.x, 0x0 * p.y + pPanel.y }
            );
		} break;

        case WM_WINDOWPOSCHANGING: {
            WINDOWPOS* pWP = reinterpret_cast<WINDOWPOS*>(lParam);
                pWP->flags |= SWP_NOCOPYBITS;
        } break;

        case WM_GETMINMAXINFO: {
            MINMAXINFO* lpMMI = reinterpret_cast
                <MINMAXINFO*>(lParam);

            lpMMI->ptMinTrackSize = { 640, 360 };
        } break;



		case WM_PAINT: {
            _cTexture_* pI = nullptr;

            size_t sI = NULL;

		    if (
                _DATA_._IMAGE_ != nullptr
            ) {
                pI = _DATA_._IMAGE_->_Data_();

                sI = _DATA_._IMAGE_->_DataSize_();
            }

            _cTexture_* pB = nullptr;
            pB = pBackGround.get();

            gdi->gdiDrawSampleTexture(
                0x0, 0x0, gdi->GetResolution().x, gdi->GetResolution().y, pB
            );

		    gdi->gdiDrawSampleTexture(
                m_vecImageBegin.x, m_vecImageBegin.y, m_vecImageEnd.x, m_vecImageEnd.y, pI
            );

            wchar_t wcFileName[256]{};
            wsprintf(wcFileName,
                L"File Name: [%s]", pI != nullptr ? pI->GetFileName() : L"NULL"
            );

            gdi->gdiDrawString(0xF, 0xF,
                wcFileName, DT_NOCLIP, (COLORREF)eColour::WHITE
            );

            if (pI != nullptr) {
                gdi->gdiDrawString(0xF, 0xF + 0xF,
                    L"Size: [" + std::to_wstring(
                        sI / (1024.f * 1024.f)
                    ) + L".MB]", DT_NOCLIP, (COLORREF)eColour::WHITE
                );
            }

            POINT p = m_wSize;

            RECT r = {
                pPanel.x, pPanel.y, p.x, p.y
            };
			gdi->Show(r);
		} break;
        case WM_ERASEBKGND: {
            /*Code...*/
		} return(0x1);


		case WM_MOUSEMOVE: {
			UNREFERENCED_PARAMETER(wParam);
			int32_t _x = GET_X_LPARAM(lParam),
				_y = GET_Y_LPARAM(lParam);
			m_pMouse = { _x, _y };

			if (
                m_pMouse.x > pPanel.x && m_pMouse.y > pPanel.y
            )
            {
                m_vecMousePos = tovec(
                    _WindowToDisplayCoord_(m_pMouse)
                );

                if (m_bMouseLB)
                    (void)SendMessage(
                        m_hWnd, WM_UPDATE, 0x0, 0x0
                    );
            }
		} break;
		case WM_LBUTTONDOWN: {
            if (
                m_pMouse.x > pPanel.x && m_pMouse.y > pPanel.y
            ) { m_bMouseLB = 0x1; m_vecPan = m_vecMousePos; }
		} break;
		case WM_LBUTTONUP: {
			m_bMouseLB = 0x0;
		} break;
		case WM_MOUSEWHEEL: {
			UNREFERENCED_PARAMETER(lParam);

			int32_t _nDelta = GET_WHEEL_DELTA_WPARAM(wParam);

			if (
                m_pMouse.x > pPanel.x && m_pMouse.y > pPanel.y
            )
            {
                m_nWheelDelta = _nDelta;

                (void)SendMessage(
                    m_hWnd, WM_UPDATE, 0x0, 0x0
                );
            }
		} break;



		case WM_UPDATE: {
		    UNREFERENCED_PARAMETER(wParam);
		    UNREFERENCED_PARAMETER(lParam);

		    _cTexture_* pI = nullptr;
		    if (
                _DATA_._IMAGE_ != nullptr
            ) { pI = _DATA_._IMAGE_->_Data_(); }

            if (m_bMouseLB) {
                camera->vecOffset -= (vec2f)(
                    m_vecMousePos - m_vecPan
                ) / vec2f{ camera->fScale };
                m_vecPan = m_vecMousePos;
            }

            vec2f vecMouseBeforeZoomPos = camera->DisplayToWorld(
                m_vecMousePos.x, m_vecMousePos.y
            );



            if (
                m_nWheelDelta > 0x0
            ) {
                camera->fScale += camera->fScale * 0.3f;
            } else if (
                m_nWheelDelta < 0x0
            ) {
                camera->fScale -= camera->fScale * 0.3f;
            } m_nWheelDelta = 0x0;

            _ClampZoom_(pI);

            vec2f vecMouseAfterZoomPos = camera->DisplayToWorld(
                m_vecMousePos.x, m_vecMousePos.y
            );

            camera->vecOffset += (
                vecMouseBeforeZoomPos - vecMouseAfterZoomPos
            );

            if (pI != nullptr) {
                camera->vecOffset.x = clamp(camera->vecOffset.x,
                    -0.5f * gdi->GetResolution().x / camera->fScale,

                    pI->GetSize().x - 0.5f * gdi->GetResolution().x / camera->fScale
                );
                camera->vecOffset.y = clamp(camera->vecOffset.y,
                    -0.5f * gdi->GetResolution().y / camera->fScale,

                    pI->GetSize().y - 0.5f * gdi->GetResolution().y / camera->fScale
                );



                vec2i v0 = {},
                    v1 = tovec(
                        pI->GetSize()
                    );

                m_vecImageBegin = camera->WorldToDisplay(
                    (float)v0.x, (float)v0.y
                );
                m_vecImageEnd = camera->WorldToDisplay(
                    (float)v1.x, (float)v1.y
                );
		    }

            (void)InvalidateRect(
                m_hWnd, NULL, NULL
            );
		} break;



        case WM_COMMAND: {
            auto _MESSAGE_BOX_ = [=](wchar_t* wcM, wchar_t* wcT, UINT uType) {
                int32_t ID = MessageBox(m_hWnd, wcM, wcT,
                    uType | MB_DEFBUTTON1 | MB_APPLMODAL | MB_SETFOREGROUND
                ); return(ID);
            };

			int32_t _ID_ = LOWORD(wParam);

			switch (_ID_)
			{
            case(_BUTTON_NEXT_T_): case(_BUTTON_PREV_T_): {
                if (!m_vecImages.empty()) {
                    size_t nIndex = _DATA_._INDEX_;

                    int32_t i = (
                        _ID_ == _BUTTON_NEXT_T_
                    ) ? 0x1 : -0x1;

                     (void)_SelectImage_(
                        nIndex = (nIndex += i) % m_vecImages.size()
                    );
				}
            } break;

            case(_BUTTON_NEXT_I_): case(_BUTTON_PREV_I_): {
                if (_DATA_._IMAGE_ != nullptr) {
                    size_t nIndex = _DATA_._IMAGE_->_Index_();

                    int32_t i = (
                        _ID_ == _BUTTON_NEXT_I_
                    ) ? 0x1 : -0x1;

                     (void)_SelectData_(
                        nIndex = (nIndex += i) % _DATA_._IMAGE_->_Size_()
                    );
				}
            } break;

			case(_BUTTON_F_): {
				if (_DATA_._IMAGE_ != nullptr) {
                    switch(
                        _MESSAGE_BOX_(L"[Start filtering a Image?]", L"[Image Filtering]", MB_YESNO)
                    ) {
                        case(IDYES): {
                            (void)ShowWindow(m_hWnd, SW_HIDE);

                            size_t nIndex = _SOURCE_;

                            if (m_bMode) {
                                nIndex = _DATA_._IMAGE_->_Dithering_RGB_(m_Nbit);
                            } else {
                                nIndex = _DATA_._IMAGE_->_Quantisation_RGB_(m_Nbit);
                            }

                            (void)ShowWindow(m_hWnd, SW_SHOW);

                            wchar_t* wcMessage = NULL;

                            if (nIndex != _SOURCE_) {
                                (void)_SelectData_(nIndex);

                                wcMessage = L"[Filtering - Complete!]";
                            } else {
                                wcMessage = L"[Filtering - Failed!]";
                            }

                            (void)_MESSAGE_BOX_(wcMessage,
                                L"[Image Filtering]", MB_OK
                            );
                        } break;

                        case(IDNO): {
                            /*Code...*/
                        } break;

                    default:
                        break;
                    }
				}
			} break;

            case(_BUTTON_ADD_Nbit_): case(_BUTTON_DIV_Nbit_): {
                int32_t i = (
                    _ID_ == _BUTTON_ADD_Nbit_
                ) ? 0x1 : -0x1;

                m_Nbit += i;

                m_Nbit = clamp(
                    m_Nbit, 0x1, 0x8
                );

                (void)_UpdateControl_Nbit_();
            } break;

            case(_BUTTON_MODE_): {
                m_bMode = !m_bMode;

                (void)_UpdateControl_Nbit_();
            } break;



			default:
				break;
			}

			(void)SendMessage(
                m_hWnd, WM_UPDATE, 0x0, 0x0
            );
        } break;



		default:
			break;

		}
		return(DefWindowProc(m_hWnd, uMsg, wParam, lParam));
	}



	_cImage_* _LoadImage_(
        const std::wstring& wsFileName
    )
    {
        std::unique_ptr<_cImage_> pI = std::make_unique
            <_cImage_>(
                wsFileName.data()
            );

        if (
            pI->_Data_()->IsValid()
        )
        {
            m_vecImages.push_back(
                std::move(pI)
            );

            _cImage_* pI = _SelectImage_(
                m_vecImages.size() - 0x1
            );

            return(pI);
        }

        return(nullptr);
    }

	_cImage_* _SelectImage_(size_t nIndex) {
        if (
            m_vecImages.size() <= nIndex
        ) { return(_DATA_._IMAGE_ = nullptr); }

        _DATA_._INDEX_ = nIndex;

        _DATA_._IMAGE_ = m_vecImages[
            _DATA_._INDEX_
        ].get();

        camera->vecOffset = vec2f {
            0x0, 0x0
        };
        camera->fScale = 0.f;

        _ClampZoom_(
            _DATA_._IMAGE_->_Data_()
        );

        (void)_UpdateControl_0_();

        (void)_UpdateControl_1_();

        return(_DATA_._IMAGE_);
	}

	_cTexture_* _SelectData_(size_t nIndex) {
        if (
            _DATA_._IMAGE_ != nullptr
        )
        {
            if (
                _DATA_._IMAGE_->_Size_() <= nIndex
            ) { return(nullptr); }

            _DATA_._IMAGE_->_SelectData_(
                nIndex
            );

            (void)_UpdateControl_1_();

            return(
                _DATA_._IMAGE_->_Data_()
            );
        }

        return(nullptr);
	}

	void _ClampZoom_(
        const _cTexture_* pT
    )
    {
        if (!pT) { return; }

        std::pair<INT,INT> p[] = {
            { pT->GetSize().x, gdi->GetResolution().x }, { pT->GetSize().y, gdi->GetResolution().y }
        }, MIN = p[0x0], MAX = p[0x1];

        if (
            p[0x0].first > p[0x1].first
        ) { std::swap(MIN,MAX); }

        float fMAX = (0x2 << (0xF - 0x1)) / (float)std::max(
            pT->GetSize().x, pT->GetSize().y
        );

        camera->fScale = clamp(
            camera->fScale, (float)MAX.second / (float)MAX.first, fMAX
        );
	}

	POINT _WindowToDisplayCoord_(POINT p) const {
		if (GetWndSize().x != NULL && GetWndSize().y != NULL)
			return { (p.x * gdi->GetResolution().x) / GetWndSize().x,
				(p.y * gdi->GetResolution().y) / GetWndSize().y
			};
		else { return { NULL, NULL }; }
	}



    DWORD _UpdateControl_0_(void) {
	    DWORD dwError = ERROR_SUCCESS;

        (void)EnableWindow(
            m_hButton_Next_T, !m_vecImages.empty() && _DATA_._INDEX_ < m_vecImages.size() - 0x1
        );
        (void)EnableWindow(
            m_hButton_Prev_T, !m_vecImages.empty() && _DATA_._INDEX_ > 0x0
        );

        (void)EnableWindow(
            m_hButton_F, !m_vecImages.empty()
        );

        (void)SetFocus(m_hWnd);

        return(dwError);
	}

	DWORD _UpdateControl_1_(void) {
	    DWORD dwError = ERROR_SUCCESS;

	    const _cImage_* pI = _DATA_._IMAGE_;

        (void)EnableWindow(
            m_hButton_Next_I, pI != nullptr && pI->_Index_() != pI->_Size_() - 0x1
        );
        (void)EnableWindow(
            m_hButton_Prev_I, pI != nullptr && pI->_Index_() != _SOURCE_
        );

        (void)EnableWindow(
            m_hButton_F, pI != nullptr
        );

        (void)SetFocus(m_hWnd);

        return(dwError);
	}

	DWORD _UpdateControl_Nbit_(void) {
	    DWORD dwError = ERROR_SUCCESS;

        (void)EnableWindow(
            m_hButton_Add_Nbit, m_Nbit < 0x8
        );
        (void)EnableWindow(
            m_hButton_Div_Nbit, m_Nbit > 0x1
        );



        wchar_t wcNbit[256] = { 0x0 };

        wsprintf(wcNbit,
            L"%ibit", m_Nbit
        );

        (void)SetWindowText(
            m_hText_Nbit, wcNbit
        );



        wchar_t wcMode[256] = { 0x0 };

        wsprintf(wcMode,
            L"%ls", m_bMode ? L"[Dithering]" : L"[Quantisation]"
        );

        (void)SetWindowText(
            m_hButton_Mode, wcMode
        );



        (void)SetFocus(m_hWnd);

        return(dwError);
	}

public:
	_cEngine_(const _In_ HINSTANCE& hInst,
		_In_ int nCmdShow = SW_SHOWNORMAL
	) : _cWindow_(hInst, nCmdShow)
	{
		/*Code...*/
	}
	~_cEngine_(void) { /*Code...*/ }

	DWORD Start(void) {
		MSG msg = { 0x0 };
		while (GetMessage(&msg, NULL, 0x0, 0x0))
			if (!TranslateAccelerator(msg.hwnd, m_hAccelTable, &msg))
                { TranslateMessage(&msg); DispatchMessage(&msg); }

		return(
            (int)msg.wParam
        );
	}
};

int TMain(TParam) {
	UNREFERENCED_PARAMETER(hNULLInstance);

	/*Helper::CreateConsole();*/

	_cEngine_ eng(
        hInstance, nCmdShow
    );
	eng.CreateC(
        (LPWSTR)L"_WndClass_", LoadCursor(NULL, IDC_HAND)
    );
	eng.CreateW(
        (LPWSTR)L"[OSIZ-Kursova]",
             nWindowWidth, nWindowHeight, WS_OVERLAPPEDWINDOW | WS_VISIBLE
    );

	DWORD dwResult = ERROR_SUCCESS;
	dwResult = eng.Start();
	if (dwResult != ERROR_SUCCESS)
	{ exit(Helper::ShowFormatError(L"[RETURN] - Failed!", dwResult)); }
	else { return(dwResult); }
}
