/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */


#include <windows.h>
#include <windowsx.h>
#include <urlmon.h>
#include <atlbase.h>
#include <atlcom.h>

#include "plugin.h"
#include "resource.h"
#include <malloc.h>
#include <errno.h>
#include <comdef.h>

int url_decode(const char *src, const char *slim, char *dst, char *dlim);


//////////////////////////////////////
//
// general initialization and shutdown
//
NPError NS_PluginInitialize()
{
  return NPERR_NO_ERROR;
}

void NS_PluginShutdown()
{
}

/////////////////////////////////////////////////////////////
//
// construction and destruction of our plugin instance object
//
nsPluginInstanceBase * NS_NewPluginInstance(nsPluginCreateData * aCreateDataStruct)
{
  if(!aCreateDataStruct)
    return NULL;

  nsPluginInstance * plugin = new nsPluginInstance(aCreateDataStruct->instance);
  return plugin;
}

void NS_DestroyPluginInstance(nsPluginInstanceBase * aPlugin)
{
  if(aPlugin)
    delete (nsPluginInstance *)aPlugin;
}

////////////////////////////////////////
//
// nsPluginInstance class implementation
//
nsPluginInstance::nsPluginInstance(NPP aInstance) : nsPluginInstanceBase(),
  mInstance(aInstance),
  mInitialized(FALSE)
{
  mhWnd = NULL;
}

nsPluginInstance::~nsPluginInstance()
{
}

static LRESULT CALLBACK PluginWinProc(HWND, UINT, WPARAM, LPARAM);
static WNDPROC lpOldProc = NULL;

NPBool nsPluginInstance::init(NPWindow* aWindow)
{
  if(aWindow == NULL)
    return FALSE;

  mhWnd = (HWND)aWindow->window;
  if(mhWnd == NULL)
    return FALSE;

  // subclass window so we can intercept window messages and
  // do our drawing to it
  lpOldProc = SubclassWindow(mhWnd, (WNDPROC)PluginWinProc);

  // associate window with our nsPluginInstance object so we can access 
  // it in the window procedure
  SetWindowLong(mhWnd, GWL_USERDATA, (LONG)this);

  mInitialized = TRUE;
  return TRUE;
}

void nsPluginInstance::shut()
{
  // subclass it back
  SubclassWindow(mhWnd, lpOldProc);
  mhWnd = NULL;
  mInitialized = FALSE;
}

NPBool nsPluginInstance::isInitialized()
{
  return mInitialized;
}

const char * nsPluginInstance::getVersion()
{
  return NPN_UserAgent(mInstance);
}

static LRESULT CALLBACK PluginWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
    case WM_PAINT:
      {
        // draw a frame and display the string
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc;
        GetClientRect(hWnd, &rc);

		FillRect(hdc, &rc, GetStockBrush(WHITE_BRUSH));
        FrameRect(hdc, &rc, GetStockBrush(BLACK_BRUSH));

        // get our plugin instance object and ask it for the version string
        nsPluginInstance *plugin = (nsPluginInstance *)GetWindowLong(hWnd, GWL_USERDATA);
        if (plugin) {
			CComBSTR _string;
			CComBSTR::LoadStringResource(IDS_STRING_MODULETEXT, _string.m_str);
			::DrawTextW(hdc, _string, _string.Length(), &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        }
        else {
          char string[] = "Error occured";
          DrawText(hdc, string, strlen(string), &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        }

        EndPaint(hWnd, &ps);
      }
      break;
    default:
      break;
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}


NPError nsPluginInstance::NewStream(NPMIMEType type, NPStream* stream, 
                        NPBool seekable, uint16* stype)
{
	USES_CONVERSION;

	LPSTR url = new char[strlen(stream->url)];
	url_decode(stream->url, stream->url + strlen(stream->url), url, url+strlen(stream->url));
	int bufsize = ::MultiByteToWideChar(CP_UTF8, 0, url, strlen(url), NULL, 0);
	LPWSTR urlw = new WCHAR[bufsize];
	int i = ::MultiByteToWideChar(CP_UTF8, 0, url, strlen(url), urlw, bufsize);
	if (i) urlw[bufsize] = L'\0';

	/// Try to detect application to run
	HRESULT _hr = S_OK;
	CLSID _clsid;

	_hr = ::GetClassFileOrMime(NULL,		// LPBC pBC,
						urlw,		// LPCWSTR szFilename,
						NULL,		// LPVOID pBuffer,
						0,			// DWORD cbSize,
						A2W(type),	// LPCWSTR szMime,
						0,			// DWORD dwReserved,
						&_clsid		//CLSID *pclsid
						);

	//CComPtr<IMoniker> _moniker;
	//_hr = ::CreateClassMoniker(_clsid, &_moniker);
	//if (FAILED(_hr)) return NPERR_GENERIC_ERROR;

	CComPtr<IMoniker> _urlMoniker;
	_hr = ::CreateURLMoniker(NULL, urlw, &_urlMoniker);
	if (FAILED(_hr)) return NPERR_GENERIC_ERROR;

	CComPtr<IBindCtx> _bc;
	_hr = ::CreateBindCtx(0, &_bc);
	if (FAILED(_hr)) return NPERR_GENERIC_ERROR;

	//CComPtr<IStream> _pStr;
	//_hr = _urlMoniker->BindToStorage(_bc, NULL, IID_IStream, (void **)&_pStr);
	//if (FAILED(_hr)) return NPERR_GENERIC_ERROR;

	CComPtr<IOleObject> _pObj;
	_hr = _urlMoniker->BindToObject(_bc, NULL, IID_IOleObject, (void **)&_pObj);
	if (FAILED(_hr)) 
	{
		::MessageBox(NULL, _com_error(_hr).ErrorMessage(), NULL, MB_OK);
		return NPERR_GENERIC_ERROR;
	}

	_hr = _pObj->DoVerb(OLEIVERB_SHOW, NULL, NULL, 0, NULL, NULL);
	if (FAILED(_hr)) return NPERR_GENERIC_ERROR;

	/// Report user what's happened (debug)

	//if (i) 
	//{
	//	::MessageBoxW(NULL, urlw, NULL, MB_OK);
	//}
	//else
	//{
	//	::MessageBoxW(NULL, L"Did not work", NULL, MB_OK);
	//}
	return NPERR_NO_ERROR; 
}


int url_decode(const char *src, const char *slim, char *dst, char *dlim)
{
	int state = 0, code;
	char *start = dst;

	if (dst >= dlim) {
		return 0;
	}
	dlim--; /* ensure spot for '\0' */

	while (src < slim && dst < dlim) {
		switch (state) {
		case 0:
			if (*src == '%') {
				state = 1;
			} else {
				*dst++ = *src;
			}
			break;
		case 1:
			code = isdigit(*src) ? *src - '0' : toupper(*src) - 'A' + 10;
		case 2:
			if (isxdigit(*src) == 0) {
				errno = EILSEQ;
				return -1;
			}
			if (state == 2) {
				*dst++ = (code * 16) + (isdigit(*src) ? *src - '0' : toupper(*src) - 'A' + 10);
				state = 0;
			} else {
				state = 2;
			}
			break;
		}
		src++;
	}
	*dst = '\0'; /* I'll be back */

	return dst - start;
}


//
//// Convert UTF-8 characters to 8-bit characters from the active locale.
//// UTF8 and Result MAY be the same buffer.
//string& UTF8Decode(IN const string& UTF8, OUT string& Result)
//{
//	int InputBytes = strlen(UTF8.c_str())+0;        // MSVCP strings tend to append extra nulls, so don't
//	// process them.
//
//	// Again, we need to convert the UTF-8 string to Unicode before we can convert it to 8-bit.
//	int UnicodeChars = MultiByteToWideChar(CP_UTF8, 0, UTF8.c_str(), InputBytes, 0, 0);
//	LPWSTR UnicodeBuffer = (LPWSTR)alloca(UnicodeChars*sizeof(WCHAR));
//	MultiByteToWideChar(CP_UTF8, 0, UTF8.c_str(), InputBytes, UnicodeBuffer, UnicodeChars);
//
//	// Now that we've got everything translated to Unicode, we can convert it to 8-bit characters.
//	int SingleByteChars = WideCharToMultiByte(CP_ACP, 0, UnicodeBuffer, UnicodeChars, 0, 0, 0, 0);
//	LPSTR SingleByteBuffer = (LPSTR)alloca(SingleByteChars);
//	WideCharToMultiByte(CP_ACP, 0, UnicodeBuffer, UnicodeChars, SingleByteBuffer, SingleByteChars, 0,
//		0);
//
//	Result.assign(SingleByteBuffer, SingleByteChars);
//
//	return Result;
//}