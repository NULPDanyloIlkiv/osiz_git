#include "window.h"

#include "gdi.h"

static const int nWindowWidth = 720;
static const int nWindowHeight = 360;

static const int nScreenWidth = 320;
static const int nScreenHeight = 180;

class _cCamera_ {

public:
	_cCamera_(void) :
		vecOffset(0.f, 0.f), fScale(1.f)
	{ /*Code...*/ }
	_cCamera_(
		vec2f vecOffset, float fScale
	) : vecOffset(vecOffset), fScale(fScale)
	{
		/*Code...*/
	}
	~_cCamera_(void) { /*Code...*/ }

	vec2f vecOffset{};
	float fScale = 1.f;

	vec2i WorldToDisplay(float fWorldX, float fWorldY)
	{ return(vec2f{ fWorldX - vecOffset.x, fWorldY - vecOffset.y } *vec2f{ fScale, fScale }); }
	vec2f DisplayToWorld(float fDisplayX, float fDisplayY)
	{ return(vec2f{ fDisplayX, fDisplayY } / vec2f{ fScale, fScale } + vecOffset); }

	void Focus(float fX, float fY, POINT pD) {
		vec2f vecTransformedCameraPos = vec2f{ fX, fY } - (vec2f)tovec(pD)
			/ vec2f{ 2.f * fScale, 2.f * fScale };
		vecOffset = vecTransformedCameraPos;
	}
};

class _cModel_ {
	std::vector
		<vec2f> m_vecPoints;

	struct {
		BYTE bPS, bPW;
		COLORREF colP;
	} m_sPen = { 0x0 };
	struct {
		BYTE bBS;
		COLORREF colB;
	} m_sBrush = { 0x0 };

public:
	_cModel_(COLORREF colP, COLORREF colB, const std::vector<vec2f>& vecPoints) :
		m_sPen({ PS_SOLID, 0x1, colP }), m_sBrush({ BS_SOLID, colB }), m_vecPoints(vecPoints)
	{ /*Code...*/ }
	_cModel_(BYTE bPS, BYTE bPW, COLORREF colP,
		BYTE bBS, COLORREF colB, const std::vector<vec2f>& vecPoints
	) : m_sPen({ bPS, bPW, colP }), m_sBrush({ bBS, colB }), m_vecPoints(vecPoints)
	{ /*Code...*/ }
	~_cModel_(void) { /*Code...*/ }

	void _Draw_(float fX, float fY, float fRadius, float fAngle,
		_cGDI_* const pGDI, _cCamera_* const pCamera
	)
	{
		pGDI->SelectNewPen(
			m_sPen.bPS, m_sPen.bPW, m_sPen.colP
		);
		pGDI->SelectNewBrush(
			m_sBrush.bBS, m_sBrush.colB
		);
		vec2f vecTransformedPos = pCamera->
			WorldToDisplay(fX, fY);

		float fTransformedRadius = pCamera->fScale * fRadius;
		pGDI->gdiDrawPolygonModel(m_vecPoints,
			vecTransformedPos.x, vecTransformedPos.y, fTransformedRadius, fAngle
		);
	}
};

class _cObject_;
typedef _cObject_*
	_pObject_;

class _cObject_ {
	friend class _cEngine_;

	std::wstring m_wsType = L"_NULL_";
public:
	_cObject_(float fX, float fY, const std::wstring& wsT) :
		vecPos(fX, fY), m_wsType(wsT)
	{ vecVel = { 0.f, 0.f }; sFlags.bIsExist = true; }
	virtual ~_cObject_(void) { /*Code...*/ }

	vec2f vecPos{}, vecVel{};

	struct {
		bool bIsExist : 0x1;
	} sFlags{};

	virtual void _Update_(
		float fElapsedTime
	) = 0;

	virtual void _Interact_(
		const _pObject_& pTarget
	) = 0;

	virtual void _Draw_(
		_cGDI_* const pGDI, _cCamera_* const pCamera
	) = 0;

	const wchar_t* _Type_(void)
	{ return(m_wsType.data()); }
};

class _cBall_;
typedef _cBall_*
	_pBall_;

class _cBall_ :
	public _cObject_
{
	friend class _cEngine_;
public:
	_cBall_(float fX, float fY, float fR,
		const std::wstring& wsT = L"_BALL_"
	) : _cObject_(fX, fY, wsT), fRadius(fR)
	{ /*Code...*/ }
	virtual ~_cBall_(void) { /*Code...*/ }

	float fRadius = 0.f;

	virtual void _Update_(float fElapsedTime) {
		vecPos += vec2f{
			vecVel.x * fElapsedTime, vecVel.y * fElapsedTime
		};
	}
	virtual void _Interact_(
		const _pObject_& pTarget
	)
	{
		/*Code...*/
	}
	virtual void _Draw_(
		_cGDI_* const pGDI, _cCamera_* const pCamera
	)
	{
		/*Code...*/
	}
};

class _cTable_ {
	friend class _cEngine_;

	std::vector<
		std::unique_ptr<_cBall_>
	> vecBalls;
public:
	_cTable_(void) {
		/*Code...*/
	}
	~_cTable_(void) { /*Code...*/ }

	template <class BallType, typename ...ArgumentTypes>
	BallType* _CreateBall_(
		ArgumentTypes&& ...args
	)
	{
		std::unique_ptr<BallType> b = std::make_unique
			<BallType>(std::forward<ArgumentTypes>(args)...);

		BallType* pB = b.get();
		vecBalls.push_back(
			std::move(b)
		); return(pB);
	}

private:
	void _Clear_(void) {
		if (!vecBalls.empty())
			vecBalls.erase(std::remove_if(vecBalls.begin(), vecBalls.end(),
				[](const std::unique_ptr<_cBall_>& b) { return(!b.get()->sFlags.bIsExist); }), vecBalls.end()
			);
	}
};

class _cEngine_ :
	public _cWindow_
{
	bool m_bAppActive = 0x0;
	POINT m_pMouse{};

	std::unique_ptr
		<_cGDI_> gdi = nullptr;

	std::unique_ptr
		<_cCamera_> camera = nullptr;

	std::unique_ptr
		<_cTable_> table = nullptr;

	class _cMobile_ :
		public _cBall_
	{

		float m_fAngle = 0.f;
		float m_fSpeed = 0.f;

		std::unique_ptr
			<_cModel_> m_model = nullptr;

		_cTable_* const m_pTable = nullptr;

	public:
		_cMobile_(float fX, float fY, float fS, float fR,
			COLORREF colPen, COLORREF colBrush, _cTable_* const pTable
		) : _cBall_(fX, fY, fR, L"_MOBILE_"), m_fSpeed(fS), m_pTable(pTable)
		{
			m_model = std::make_unique
				<_cModel_>(colPen, colBrush,
					std::vector<vec2f> {
						{ 1.f, 1.f }, { -1.f, 1.f },
						{ -1.f, -1.f }, { 1.f, -1.f }
					}
			);
		}
		~_cMobile_(void) { /*Code...*/ }

		void _Update_(float fElapsedTime) {
			auto fClip = [&](float& fV) {
				float fS = sign(fV);
					fV -= fS * 16.f * fElapsedTime;
				if (sign(fV) != fS)
					{ fV = 0.f; }
				return(fV);
			};
			vecVel = vec2f{
				fClip(vecVel.x), fClip(vecVel.y)
			};

			float fD = length(vecVel);
			if (fD > m_fSpeed)
				vecVel = {
					(vecVel.x / fD) * m_fSpeed, (vecVel.y / fD) * m_fSpeed
				};
			vecPos += vec2f{
				vecVel.x * fElapsedTime, vecVel.y * fElapsedTime
			};
		}

		void _Interact_(const _pObject_& pTarget) {
			if (!std::wcscmp(pTarget->_Type_(), L"_PROJECTILE_")) {
				_pProjectile_ _pPrj_ = reinterpret_cast
					<_pProjectile_>(pTarget);
				_ChangeVelocity_(
					_pPrj_->vecVel.x, _pPrj_->vecVel.y
				);
				/*if ((fRadius -= _pPrj_->fRadius) < 0.f)
				{ sFlags.bIsExist = 0x0; }*/
			}
		}

		void _Draw_(
			_cGDI_* const pGDI, _cCamera_* const pCamera
		)
		{
			m_model.get()->_Draw_(vecPos.x, vecPos.y, fRadius,
				m_fAngle, pGDI, pCamera
			);
		}

		void _ChangeVelocity_(
			float fDX, float fDY
		)
		{
			vecVel += vec2f{ fDX, fDY };
			m_fAngle = atan2(
				vecVel.y, vecVel.x
			);
		}
	};
	typedef _cMobile_* _pMobile_;
	_pMobile_ pPlayer = nullptr;

	class _cProjectile_ :
		public _cBall_
	{
		float m_fDistance = 0.f, m_fD = 0.f;

		std::unique_ptr
			<_cModel_> m_model = nullptr;

		_cTable_* const m_pTable = nullptr;

	public:
		_cProjectile_(float fX, float fY,
			float fDX, float fDY, float fS, float fD, float fR,
				COLORREF colPen, COLORREF colBrush, _cTable_* const pTable
		) : _cBall_(fX, fY, fR, L"_PROJECTILE_"), m_fDistance(fD), m_pTable(pTable)
		{
			vecVel = { fDX, fDY };
			vecVel /= length(vecVel);

			vecVel = {
				vecVel.x * fS, vecVel.y * fS
			};

			m_model = std::make_unique
				<_cModel_>(colPen, colBrush,
					std::vector<vec2f> {
						{ -1.f, 0.f }, { 0.f, -0.5f },
						{ 1.f, 0.f }, { 0.f, 0.5f }
					}
			);
		}
		~_cProjectile_(void) { /*Code...*/ }

		void _Update_(float fElapsedTime) {
			vecPos += vec2f{
				vecVel.x * fElapsedTime, vecVel.y * fElapsedTime
			};
			m_fD += length(vec2f
				{ vecVel.x * fElapsedTime, vecVel.y * fElapsedTime }
			);
			if (m_fD > m_fDistance)
			{ sFlags.bIsExist = 0x0; }
		}

		void _Interact_(const _pObject_& pTarget) {
			if (!std::wcscmp(pTarget->_Type_(), L"_MOBILE_")) {
				_pMobile_ _pMob_ = reinterpret_cast
					<_pMobile_>(pTarget);
				sFlags.bIsExist = 0x0;

				if (m_pTable)
					(void)m_pTable->_CreateBall_<_cExplosion_>(
						vecPos.x, vecPos.y, 6.f, 16.f
					);
			}
		}

		void _Draw_(
			_cGDI_* const pGDI, _cCamera_* const pCamera
		)
		{
			m_model.get()->_Draw_(vecPos.x, vecPos.y, fRadius,
				atan2(vecVel.y, vecVel.x), pGDI, pCamera
			);
		}
	};
	typedef _cProjectile_* _pProjectile_;

	class _cExplosion_ :
		public _cBall_
	{
		float m_fSize = 0.f,
			m_fSpeed = 0.f;
	public:
		_cExplosion_(float fX, float fY, float fSize, float fSpeed) :
			_cBall_(fX, fY, 1.f, L"_EXPLOSION_"), m_fSize(fSize), m_fSpeed(fSpeed)
		{
			/*Code...*/
		}
		~_cExplosion_(void) { /*Code...*/ }

		void _Update_(float fElapsedTime) {
			if ((fRadius += m_fSpeed * fElapsedTime) > m_fSize)
			{ sFlags.bIsExist = 0x0; }
			vecPos += vec2f{
				vecVel.x * fElapsedTime, vecVel.y * fElapsedTime
			};
		}

		void _Interact_(const _pObject_& pTarget) {
			/*Code...*/
		}

		void _Draw_(
			_cGDI_* const pGDI, _cCamera_* const pCamera
		)
		{
			pGDI->SelectNewPen(
				PS_SOLID, 0x3, RGB(0xFF, 0xFF, 0xFF)
			);
			pGDI->SelectDefaultBrush(NULL_BRUSH);
			pGDI->gdiDrawCircle(
				pCamera->WorldToDisplay(
					vecPos.x, vecPos.y
				),
				(INT)std::roundf(
					pCamera->fScale * fRadius
				)
			);
		}
	};
	typedef _cExplosion_* _pExplosion_;

	enum _eSetup_BALL_VS_BALL_ {
		_NULL_ = 0x0,
		_STATIC_ = 0x1,
		_DYNAMIC_ = 0x2
	};

	std::map<
		std::pair<std::wstring, std::wstring>, size_t
	> mapSetupBall = {
		{ {L"_MOBILE_", L"_MOBILE_"}, (size_t)_DYNAMIC_},
		{ {L"_PROJECTILE_", L"_MOBILE_"}, (size_t)_STATIC_ }
	};

	LRESULT HandleMessage(UINT _In_ uMsg,
		WPARAM _In_ wParam, LPARAM _In_ lParam)
	{
		switch (uMsg) {

		case WM_CREATE: {
			HDC hDC = GetDC(m_hWnd);
			gdi = std::make_unique
				<_cGDI_>(hDC, POINT{ nScreenWidth, nScreenHeight });
			camera = std::make_unique
				<_cCamera_>(vec2f{ 0.f, 0.f }, 1.f);
			table = std::make_unique<_cTable_>();

			pPlayer = table.get()->_CreateBall_<_cMobile_>((float)0xF, (float)0xF, 64.f, 10.f,
				RGB(0xFF, 0xFF, 0xFF), RGB(0xF, 0xFF, 0xF), table.get()
			);

			for (int i = 0x0; i < 0x6; i++)
				table.get()->_CreateBall_<_cMobile_>(
					(float)(rand() % gdi.get()->GetResolution().x), (float)(rand() % gdi.get()->GetResolution().y),
						64.f, 3.f + (rand() / (float)RAND_MAX) * 10.f, RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xF, 0xF), table.get()
				);
		} break;

		case WM_CLOSE:
			if (!DestroyWindow(m_hWnd))
			{ exit(Helper::ShowLastError(L"DestroyWindow(m_hWnd)")); }
			break;
		case WM_DESTROY: {
			PostQuitMessage(EXIT_SUCCESS);
		} break;

		case WM_SIZE: {
			m_wSize.store(
				{ (LONG)GET_X_LPARAM(lParam), (LONG)GET_Y_LPARAM(lParam) }
			);
		} break;

		case WM_PAINT: {
			gdi.get()->Show(m_wSize);
		} break;

		case WM_MOUSEMOVE: {
			UNREFERENCED_PARAMETER(wParam);
			int32_t _x = GET_X_LPARAM(lParam),
				_y = GET_Y_LPARAM(lParam);
			m_pMouse = { _x, _y };
		} break;
		case WM_LBUTTONDOWN: {
			if (pPlayer) {
				POINT pMouseInDisplay = _WindowToDisplayCoord_(m_pMouse);
				vec2f vecDir = camera.get()->DisplayToWorld(
					(float)pMouseInDisplay.x, (float)pMouseInDisplay.y
				) - pPlayer->vecPos;
				vecDir /= length(vecDir);

				float fR = 3.f;

				table.get()->_CreateBall_<_cProjectile_>(pPlayer->vecPos.x + vecDir.x * (pPlayer->fRadius + fR + 1.f), pPlayer->vecPos.y + vecDir.y * (pPlayer->fRadius + fR + 1.f),
					vecDir.x, vecDir.y, 128.f, 128.f, fR, RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xF), table.get()
				);
				table.get()->_CreateBall_<_cExplosion_>(
					pPlayer->vecPos.x + vecDir.x * 16.f, pPlayer->vecPos.y + vecDir.y * 16.f, 6.f, 64.f
				);
			}
		} break;
		case WM_LBUTTONUP: {
			//Code...
		} break;
		case WM_MOUSEWHEEL: {
			UNREFERENCED_PARAMETER(lParam);
			INT _nDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			if (_nDelta > 0x0)
				camera.get()->fScale +=
					camera.get()->fScale * 0.1f;
			if (_nDelta < 0x0)
				camera.get()->fScale -=
					camera.get()->fScale * 0.1f;
		}

		default:
			break;

		}
		return(DefWindowProc(m_hWnd, uMsg, wParam, lParam));
	}

	bool _DoWork_(float fElapsedTime) {
		Helper::PrintDebugString(
			L"FPS : " + std::to_wstring(1.f / fElapsedTime) + L'\n'
		);

		auto fIsCirclesOverlap = [&](float x0, float y0, float r0, float x1, float y1, float r1) {
			return ((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0)) <= (r0 + r1) * (r0 + r1);
		};
		auto fIsPointInCircle = [&](float x0, float y0, float r0, float px, float py) {
			return ((x0 - px) * (x0 - px) + (y0 - py) * (y0 - py)) < (r0 * r0);
		};

		gdi.get()->SetBGColor(
			RGB(0xF, 0xF, 0xF)
		);
		gdi.get()->Clear();

		float fScale = camera.get()->fScale * 64.f;
		vec2f vecVisible = tovec(
			gdi.get()->GetResolution()
		) / vec2f{ fScale, fScale };

		vec2i vecOffset = {
			(int)(camera.get()->fScale * camera.get()->vecOffset.x) % (int)fScale,
			(int)(camera.get()->fScale * camera.get()->vecOffset.y) % (int)fScale
		};
		gdi.get()->SelectNewPen(
			PS_SOLID, 0x1, RGB(0xFF, 0xFF, 0xFF)
		);
		gdi.get()->SelectNewBrush(
			BS_SOLID, RGB(0xF, 0xF, 0xF)
		);
		for (int i = -(0x1); i <= vecVisible.x + 0x1; i++)
			for (int j = -(0x1); j <= vecVisible.y + 0x1; j++)
				gdi.get()->gdiDrawRectangle(
					{ -vecOffset.x + (int)(i * fScale), -vecOffset.y + (int)(j * fScale) }, { -vecOffset.x + (int)((i + 0x1) * fScale), -vecOffset.y + (int)((j + 0x1) * fScale) }
				);

		if (pPlayer) {
			bool bKey[0x5]{};
			for (int i = 0x0; i < 0x5; i++) {
				wchar_t wcKey[] = { 0x20, L'A', L'D', L'W', L'S' };
				bKey[i] = (0x8000 & GetAsyncKeyState(wcKey[i])) != 0x0;
			}
			if (bKey[0x0]) { /*Code...*/ }
			for (int i = 0x0; i <= 0x1; i++) {
				if (bKey[i + 0x1])
					pPlayer->_ChangeVelocity_(
						i == 0x0 ? -(128.f * fElapsedTime) : (128.f * fElapsedTime), 0.f
					);
				if (bKey[i + 0x3])
					pPlayer->_ChangeVelocity_(
						0.f, i == 0x0 ? -(128.f * fElapsedTime) : (128.f * fElapsedTime)
					);
			}
			camera.get()->Focus(
				pPlayer->vecPos.x, pPlayer->vecPos.y, gdi.get()->GetResolution()
			);
		}

		for (auto& b : table.get()->vecBalls) {
			_pMobile_ pEnemy = reinterpret_cast
				<_pMobile_>(b.get());
			if (pPlayer != pEnemy && !std::wcscmp(pEnemy->_Type_(), L"_MOBILE_")) {
				vec2f vecD = pPlayer->vecPos - pEnemy->vecPos;
				float fD = length(vecD);
				if (fD < 128.f) {
					vecD = {
						vecD.x / fD, vecD.y / fD
					};
					pEnemy->_ChangeVelocity_(
						vecD.x * 64.f * fElapsedTime, vecD.y * 64.f * fElapsedTime
					);
				}
			}
		}


		size_t nBallsSize = table.get()->
			vecBalls.size();

		for (size_t i = 0x0; i < nBallsSize; i += 0x1)
			table.get()->vecBalls[i].get()->_Update_(
				fElapsedTime
			);

		std::vector<
			std::pair<_pObject_, _pObject_>
		> vecCollidingObjects;

		for (size_t i = 0x0; i < nBallsSize; i += 0x1) {
			std::unique_ptr<_cBall_>&
				target = table.get()->vecBalls[i];

			for (size_t j = i + 0x1; j < nBallsSize; j += 0x1) {
				std::unique_ptr<_cBall_>&
					ball = table.get()->vecBalls[j];

				if (fIsCirclesOverlap(target->vecPos.x, target->vecPos.y, target->fRadius,
					ball->vecPos.x, ball->vecPos.y, ball->fRadius)
				)
				{
					_pBall_ pBall[] = {
						target.get(), ball.get()
					};
					size_t nSetup = (size_t)_NULL_;
					for (int i = 0x0; i <= 0x1; i += 0x1) {
						std::pair<std::wstring, std::wstring> pairType = {
							pBall[0x0]->_Type_(), pBall[0x1]->_Type_()
						};
						if (mapSetupBall.find(pairType) != mapSetupBall.end())
						{ nSetup = mapSetupBall[pairType]; break; }
						else { std::swap(pBall[0x0], pBall[0x1]); }
					}

					vecCollidingObjects.push_back(
						std::make_pair(pBall[0x0], pBall[0x1])
					);
					switch (nSetup) {
					case(_NULL_): break;

					case(_STATIC_): {
						_CircleVsCircle_STATIC_(
							pBall[0x0], pBall[0x1]
						);
					} break;
					case(_DYNAMIC_): {
						_CircleVsCircle_DYNAMIC_(
							pBall[0x0], pBall[0x1]
						);
					} break;

					default:
						break;
					}
				}
			}
		}
		for (auto& o : vecCollidingObjects) {
			_pObject_ pObject[] = {
				o.first, o.second
			};
			for (int i = 0x0; i < 0x2; i += 0x1)
				pObject[i % (0x2)]->_Interact_(
					pObject[(i + 0x1) % (0x2)]
				);
		}

		for (size_t i = 0x0; i < nBallsSize; i += 0x1) {
			std::unique_ptr<_cBall_>&
				b = table.get()->vecBalls[i];
			if (b.get()->sFlags.bIsExist)
			{ b.get()->_Draw_(gdi.get(), camera.get()); }
		}

		for (auto& o : vecCollidingObjects) {
			_pObject_ pObject[] = {
				o.first, o.second
			};
			if (pObject[0x0]->sFlags.bIsExist && pObject[0x1]->sFlags.bIsExist) {
				gdi.get()->SelectNewPen(
					PS_SOLID, 0x1, RGB(0xF, 0xF, 0xFF)
				);
				gdi.get()->gdiDrawLine(
					(vec2i)camera.get()->WorldToDisplay(pObject[0x0]->vecPos.x, pObject[0x0]->vecPos.y),
					(vec2i)camera.get()->WorldToDisplay(pObject[0x1]->vecPos.x, pObject[0x1]->vecPos.y)
				);
			}
		}

		table.get()->_Clear_();

		(void)InvalidateRect(
			m_hWnd, NULL, NULL
		);
		return(true);
	}

	void _CircleVsCircle_STATIC_(_pBall_& pTarget, _pBall_& pBall) {
		float fD = sqrtf((pTarget->vecPos.x - pBall->vecPos.x) * (pTarget->vecPos.x - pBall->vecPos.x) +
			(pTarget->vecPos.y - pBall->vecPos.y) * (pTarget->vecPos.y - pBall->vecPos.y));
		float fOverlap = 1.f * (fD - pTarget->fRadius - pBall->fRadius);

		vec2f vecNewPos{};

		vecNewPos = pTarget->vecPos - vec2f{ fOverlap, fOverlap }
			*(pTarget->vecPos - pBall->vecPos) / vec2f{ fD, fD };
		pTarget->vecPos = vecNewPos;
	}
	void _CircleVsCircle_DYNAMIC_(_pBall_& pTarget, _pBall_& pBall) {
		float fD = sqrtf((pTarget->vecPos.x - pBall->vecPos.x) * (pTarget->vecPos.x - pBall->vecPos.x) +
			(pTarget->vecPos.y - pBall->vecPos.y) * (pTarget->vecPos.y - pBall->vecPos.y));
		float fOverlap = 0.5f * (fD - pTarget->fRadius - pBall->fRadius);

		vec2f vecNewPos{};

		vecNewPos = pTarget->vecPos - vec2f{ fOverlap, fOverlap }
			*(pTarget->vecPos - pBall->vecPos) / vec2f{ fD, fD };
		pTarget->vecPos = vecNewPos;

		vecNewPos = pBall->vecPos + vec2f{ fOverlap, fOverlap }
			*(pTarget->vecPos - pBall->vecPos) / vec2f{ fD, fD };
		pBall->vecPos = vecNewPos;
	}

	POINT _WindowToDisplayCoord_(POINT p) const {
		if (GetWndSize().x != NULL && GetWndSize().y != NULL)
			return { (p.x * gdi.get()->GetResolution().x) / GetWndSize().x,
				(p.y * gdi.get()->GetResolution().y) / GetWndSize().y
			};
		else { return { NULL, NULL }; }
	}

public:
	_cEngine_(const _In_ HINSTANCE& hInst,
		_In_ int nCmdShow = SW_SHOWNORMAL
	) : _cWindow_(hInst, nCmdShow) {
		m_bAppActive = 0x1;
	}
	~_cEngine_(void) { /*Code...*/ }

	DWORD Start(void) {
		std::chrono::system_clock::time_point
			tp0 = std::chrono::system_clock::now(),
			tp1 = std::chrono::system_clock::now();
		std::chrono::duration
			<float> elapsedTime(0.f);

		MSG msg = { 0x0 };
		while (m_bAppActive)
			if (PeekMessage(&msg, NULL, 0x0, 0x0, PM_REMOVE))
			switch(msg.message) {
				case WM_QUIT: {
					m_bAppActive = 0x0;
				} break;
				default: {
					if (!TranslateAccelerator(msg.hwnd, m_hAccelTable, &msg))
					{ TranslateMessage(&msg); DispatchMessage(&msg); }
				} break;

			}
			else {
				tp1 = std::chrono::system_clock::now();
				elapsedTime = tp1 - tp0; tp0 = tp1;

				if (!_DoWork_(elapsedTime.count())) {
					PostMessage(m_hWnd,
						WM_CLOSE, 0x0, 0x0
					);
				}
			}
		return((int)msg.wParam);
	}
};

int wTMain(wTParam) {
	UNREFERENCED_PARAMETER(hNULLInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	Helper::CreateConsole();

	_cEngine_ eng(hInstance, nCmdShow);
	eng.CreateC((LPWSTR)L"MainWndClass", LoadCursor(NULL, IDC_HAND));
	eng.CreateW((LPWSTR)L"Window-[1]", nWindowWidth, nWindowHeight,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE);

	DWORD dwResult = ERROR_SUCCESS;
	dwResult = eng.Start();
	if (dwResult != ERROR_SUCCESS)
	{ exit(Helper::ShowFormatError(L"[RETURN] - Failed!", dwResult)); }
	else { return(dwResult); }
}
