// OSInfo.cpp
//

/*
Module : OSInfo.cpp
Purpose: Implementation of a comprehensive function to perform OS version detection
Created: PJN / 11-05-1996
History: PJN / 24-02-1997 A number of updates including support for NT 3.1, 
                          single mode dos in Windows 95 and better Windows
                          version detecion under real mode dos.
         PJN / 13-09-1998 1.  Added explicit support for Windows 98 
                          2.  Updated documentation to use HTML. 
                          3.  Minor update to the web page describing it. 
         PJN / 22-06-1999 1.  UNICODE enabled the code.
                          2.  Removed need for the dwOSVersionInfoSize variable
                          3.  Added support for detecting Build Number of 95 and 98 from DOS code path.
                          4.  Now ships as standard with VC 5 workspace files
                          5.  Added explicit support for Windows 95 SP 1
                          6.  Added explicit support for Windows 95 OSR 2
                          7.  Added explicit support for Windows 98 Second Edition
                          8.  Added explicit support for Windows 2000
                          9.  Added explicit support for Windows CE
                          10. Added explicit support for Windows Terminal Server's
                          11. Added explicit support for NT Stand Alone Server's.
                          12. Added explicit support for NT Primary Domain Controller's
                          13. Added explicit support for NT Backup Domain Controller's
         PJN / 23-07-1999 Tested out support for Windows 98 SE, minor changes required
         PJN / 26-07-1999 Added explicit support for Windows 98 SP 1
         PJN / 28-07-1999 1. Fixed a problem when application is build in non-huge/large 
                          memory model in Win16
                          2. Added explicit support for returning NT and Win9x service pack information 
                          from Win32 and Win16 code paths
                          3. Updated test program to not bother reporting on any info which does not 
                          exist. e.g. if there is no service pack installed, then we don't bother 
                          displaying any info about service packs
                          4. Added explicit support for NT Enterprise Edition



Copyright (c) 1996 - 1999 by PJ Naughter.  
All rights reserved.
*/


/////////////////////////////////  Includes  //////////////////////////////////
#include "stdafx.h"
#include <windows.h> 
#ifdef _WIN32
#include <tchar.h>
#else
#include <ctype.h>
#include <stdlib.h>
#endif
#include <string.h>
#include <stdarg.h>
#include "OSInfo.h"


/////////////////////////////////  Local function / variables /////////////////



#ifndef _WIN32
  //taken from Win32 SDK winbase.h file
  #define VER_PLATFORM_WIN32s             0
  #define VER_PLATFORM_WIN32_WINDOWS      1
  #define VER_PLATFORM_WIN32_NT           2
  #define VER_PLATFORM_WIN32_CE           3
#elif !defined(UNDER_CE)
  BOOL WhichNTProduct(DWORD& dwVersion);
#endif //ifndef _WIN32
                                     
                                     
#if defined(_WIN32)
  //Taken from Windows CE winbase.h file
  #ifndef VER_PLATFORM_WIN32_CE
    #define VER_PLATFORM_WIN32_CE         3
  #endif
#endif //defined(_WIN32) 

#if defined(_WIN32) && !defined(UNDER_CE)

  // Function pointers to stuff in Kernel (32 bit)
  typedef BOOL (WINAPI *lpfnGetVersionEx) (LPOSVERSIONINFO);

  //Functions to detect if we are running on Terminal Server
  BOOL ValidateProductSuite(LPCTSTR lpszSuiteToValidate);
  BOOL IsTerminalServicesEnabled(); 
  BOOL IsEnterpriseServer();
  WORD GetNTServicePackFromRegistry();
  WORD GetNTServicePackFromCSDString(LPCTSTR pszCSDVersion);
#endif


#if defined(_WINDOWS) && !defined(_WIN32)     //required for universal thunks
  #define HINSTANCE32              DWORD
  #define HFILE32                  DWORD
  #define HWND32                   DWORD

  // #defines for WOWCallProc32() parameter conversion
  #define PARAM_01                 0x00000001

  // #defines for dwCapBits
  #define WOW_LOADLIBRARY          0x0001
  #define WOW_FREELIBRARY          0x0002
  #define WOW_GETPROCADDRESS       0x0004
  #define WOW_CALLPROC             0x0008
  #define WOW_VDMPTR32             0x0010
  #define WOW_VDMPTR16             0x0020
  #define WOW_HWND32               0x0040

  // Wrappers for functions in Kernel (16 bit)
  HINSTANCE32 WINAPI    WOWLoadLibraryEx32  (LPSTR, HFILE32, DWORD);
  BOOL        WINAPI    WOWFreeLibrary32    (HINSTANCE32);
  FARPROC     WINAPI    WOWGetProcAddress32 (HINSTANCE32, LPCSTR);
  DWORD       WINAPI    WOWGetVDMPointer32  (LPVOID, UINT);
  DWORD       FAR CDECL WOWCallProc32       (FARPROC, DWORD, DWORD, ...);
  UINT        WINAPI    WOWCreateVDMPointer16(DWORD, DWORD);
  UINT        WINAPI    WOWDeleteVDMPointer16(UINT);
  HWND32      WINAPI    WOWHwndToHwnd32      (HWND);
  
  //////////////// OSVERSIONINFO taken from Win32 sdk header file
  typedef struct _OSVERSIONINFO
  { 
    DWORD dwOSVersionInfoSize; 
    DWORD dwMajorVersion; 
    DWORD dwMinorVersion; 
    DWORD dwBuildNumber; 
    DWORD dwPlatformId; 
    char szCSDVersion[128]; 
  } OSVERSIONINFO, *POSVERSIONINFO, FAR *LPOSVERSIONINFO; 

  // Function pointers to stuff in Kernel (16 bit)
  typedef HINSTANCE32 (WINAPI *lpfnLoadLibraryEx32W) (LPSTR, HFILE32, DWORD);
  typedef BOOL        (WINAPI *lpfnFreeLibrary32W)   (HINSTANCE32);
  typedef FARPROC     (WINAPI *lpfnGetProcAddress32W)(HINSTANCE32, LPCSTR);
  typedef DWORD       (WINAPI *lpfnGetVDMPointer32W) (LPVOID, UINT);
  typedef DWORD       (WINAPI *lpfnCallProc32W)      (FARPROC, DWORD, DWORD);
  typedef WORD        (WINAPI *lpfnWNetGetCaps)      (WORD);
  
  DWORD dwCapBits;
  lpfnLoadLibraryEx32W LoadLibraryEx32W;
  lpfnFreeLibrary32W FreeLibrary32W;
  lpfnGetProcAddress32W GetProcAddress32W;
  lpfnGetVDMPointer32W GetVDMPointer32W;
  lpfnCallProc32W CallProc32W;

  BOOL WFWLoaded();
#endif //defined(_WINDOWS) && !defined(_WIN32)

#ifdef _DOS
  WORD WinVer;
  BYTE bRunningWindows;
  void GetWinInfo();
#endif //ifdef _DOS



////////////////////////////////// Implementation /////////////////////////////

BOOL GetOSVersion(LPOS_VERSION_INFO lpVersionInformation)
{                
  //By Default assume no service pack is installed
  lpVersionInformation->wEmulatedServicePack = 0;
  lpVersionInformation->wUnderlyingServicePack = 0;

  #ifdef UNDER_CE
    OSVERSIONINFO osvi;
    memset(&osvi, 0, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if (!GetVersionEx(&osvi))
      return FALSE;

    lpVersionInformation->dwEmulatedMajorVersion = osvi.dwMajorVersion; 
    lpVersionInformation->dwEmulatedMinorVersion = osvi.dwMinorVersion; 
    lpVersionInformation->dwEmulatedBuildNumber = LOWORD(osvi.dwBuildNumber); //ignore HIWORD
    _tcscpy(lpVersionInformation->szEmulatedCSDVersion, osvi.szCSDVersion);

    //Explicitely map the win32 dwPlatformId to our own values
    if (osvi.dwPlatformId == VER_PLATFORM_WIN32s)
      lpVersionInformation->dwEmulatedPlatformId = PLATFORM_WIN32S;
    else if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
      lpVersionInformation->dwEmulatedPlatformId = PLATFORM_WINDOWS;
    else if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
      lpVersionInformation->dwEmulatedPlatformId = PLATFORM_NT_WORKSTATION; //As a default assume NT Workstation
    else if (osvi.dwPlatformId == VER_PLATFORM_WIN32_CE)
      lpVersionInformation->dwEmulatedPlatformId = PLATFORM_WINDOWS_CE;
    else
      return FALSE;

    //underlying OS is the same
    lpVersionInformation->dwUnderlyingMajorVersion = lpVersionInformation->dwEmulatedMajorVersion; 
    lpVersionInformation->dwUnderlyingMinorVersion = lpVersionInformation->dwEmulatedMinorVersion; 
    lpVersionInformation->dwUnderlyingBuildNumber = lpVersionInformation->dwEmulatedBuildNumber;
    lpVersionInformation->dwUnderlyingPlatformId = lpVersionInformation->dwEmulatedPlatformId;
    _tcscpy(lpVersionInformation->szUnderlyingCSDVersion, lpVersionInformation->szEmulatedCSDVersion);
  #elif _WIN32
    //determine dynamically if GetVersionEx is available, if 
    //not then drop back to using GetVersion

    // Get Kernel handle
    HMODULE hKernel32 = GetModuleHandle(_T("KERNEL32.DLL"));
    if (hKernel32 == NULL)
      return FALSE;

    #ifdef _UNICODE
      lpfnGetVersionEx lpGetVersionEx = (lpfnGetVersionEx) GetProcAddress(hKernel32, "GetVersionExW");
    #else
      lpfnGetVersionEx lpGetVersionEx = (lpfnGetVersionEx) GetProcAddress(hKernel32, "GetVersionExA");
    #endif

    if (lpGetVersionEx)
    {
      OSVERSIONINFO osvi;
      memset(&osvi, 0, sizeof(OSVERSIONINFO));
      osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
      if (!lpGetVersionEx(&osvi))
        return FALSE;

      lpVersionInformation->dwEmulatedMajorVersion = osvi.dwMajorVersion; 
      lpVersionInformation->dwEmulatedMinorVersion = osvi.dwMinorVersion; 
      lpVersionInformation->dwEmulatedBuildNumber = LOWORD(osvi.dwBuildNumber); //ignore HIWORD
      _tcscpy(lpVersionInformation->szEmulatedCSDVersion, osvi.szCSDVersion);
    
      //Explicitely map the win32 dwPlatformId to our own values
      if (osvi.dwPlatformId == VER_PLATFORM_WIN32s)
        lpVersionInformation->dwEmulatedPlatformId = PLATFORM_WIN32S;
      else if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
      {
        lpVersionInformation->dwEmulatedPlatformId = PLATFORM_WINDOWS;

        //Deterine the Win9x Service pack installed
        if (IsWindows95SP1(lpVersionInformation))
        {
          lpVersionInformation->wEmulatedServicePack = 1;
          lpVersionInformation->wUnderlyingServicePack = 1;
        }
        else if (IsWindows95OSR2(lpVersionInformation))
        {
          lpVersionInformation->wEmulatedServicePack = 2;
          lpVersionInformation->wUnderlyingServicePack = 2;
        }
        else if (IsWindows98SP1(lpVersionInformation))
        {
          lpVersionInformation->wEmulatedServicePack = 1;
          lpVersionInformation->wUnderlyingServicePack = 1;
        }
      }
      else if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
      {
        lpVersionInformation->dwEmulatedPlatformId = PLATFORM_NT_WORKSTATION; //As a default assume NT Workstation

        //Determine the NT Service pack
        lpVersionInformation->wEmulatedServicePack = GetNTServicePackFromCSDString(osvi.szCSDVersion);
        lpVersionInformation->wUnderlyingServicePack = lpVersionInformation->wEmulatedServicePack;
      }
      else if (osvi.dwPlatformId == VER_PLATFORM_WIN32_CE)
        lpVersionInformation->dwEmulatedPlatformId = PLATFORM_WINDOWS_CE;
      else
        return FALSE;
    
      //Win32s is not an OS in its own right !
      if (lpVersionInformation->dwEmulatedPlatformId == PLATFORM_WIN32S)
      {                                     
        //Could not find a portable method of determining what Win 16 
        //version from within win32, so just assume Windows 3.10
        lpVersionInformation->dwUnderlyingMajorVersion = 3; 
        lpVersionInformation->dwUnderlyingMinorVersion = 10; 
        lpVersionInformation->dwUnderlyingBuildNumber = 0;
        lpVersionInformation->dwUnderlyingPlatformId = PLATFORM_WINDOWS31;
        _tcscpy(lpVersionInformation->szUnderlyingCSDVersion, _T("Microsoft Windows"));
      }
      else
      { 
        lpVersionInformation->dwUnderlyingMajorVersion = lpVersionInformation->dwEmulatedMajorVersion; 
        lpVersionInformation->dwUnderlyingMinorVersion = lpVersionInformation->dwEmulatedMinorVersion; 
        lpVersionInformation->dwUnderlyingBuildNumber = lpVersionInformation->dwEmulatedBuildNumber;
        lpVersionInformation->dwUnderlyingPlatformId = lpVersionInformation->dwEmulatedPlatformId;
        _tcscpy(lpVersionInformation->szUnderlyingCSDVersion, lpVersionInformation->szEmulatedCSDVersion);

        //Handle the various NT nuances
        if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) 
        {
          //Check to see if we are running on Terminal Server
          if (IsTerminalServicesEnabled()) 
          {
            lpVersionInformation->dwUnderlyingPlatformId = PLATFORM_WINDOWS_TERMINAL_SERVER; 
            //Leave the emulated PlatformId alone as Terminal Server is similiar to Win32s
            //in that most application code does not know that it it really running on it.
          }
          else if (IsEnterpriseServer())
          {
            lpVersionInformation->dwUnderlyingPlatformId = PLATFORM_NT_ENTERPRISE_SERVER; 
            lpVersionInformation->dwEmulatedPlatformId = PLATFORM_NT_ENTERPRISE_SERVER; 
          }
          else
          {
            //Distingush between NT server, PDC, BDC or Workstation, 
            DWORD dwVersion=0;    
            if (WhichNTProduct(dwVersion))
            {
              lpVersionInformation->dwUnderlyingPlatformId = dwVersion; 
              lpVersionInformation->dwEmulatedPlatformId = dwVersion;
            } 
          }
        }
      }
    }
    else
    {
      //Since GetVersionEx is not available we need to fall back on plain
      //old GetVersion. Because GetVersionEx is available on all but v3.1 of NT
      //we can fill in some of the paramters ourselves.
      DWORD dwVersion = GetVersion();

      lpVersionInformation->dwEmulatedMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
      lpVersionInformation->dwEmulatedMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
      lpVersionInformation->dwEmulatedBuildNumber = 0;
      lpVersionInformation->dwEmulatedPlatformId   = PLATFORM_NT_WORKSTATION;   
        _tcscpy(lpVersionInformation->szEmulatedCSDVersion, _T("Microsoft Windows NT"));

      lpVersionInformation->dwUnderlyingMajorVersion = lpVersionInformation->dwEmulatedMajorVersion;
      lpVersionInformation->dwUnderlyingMinorVersion = lpVersionInformation->dwEmulatedMinorVersion;
      lpVersionInformation->dwUnderlyingBuildNumber  = lpVersionInformation->dwEmulatedBuildNumber;
      lpVersionInformation->dwUnderlyingPlatformId   = lpVersionInformation->dwEmulatedPlatformId;   
      _tcscpy(lpVersionInformation->szUnderlyingCSDVersion, lpVersionInformation->szEmulatedCSDVersion);

      //Need to determine the NT Service pack by querying the registry directory.
      lpVersionInformation->wEmulatedServicePack = GetNTServicePackFromRegistry();
      lpVersionInformation->wUnderlyingServicePack = lpVersionInformation->wEmulatedServicePack;

      //Don't need code to check for Terminal Server or Enterprise Server since GetVersionEx is 
      //available on it
      DWORD dwPlatformID;
      if ((WhichNTProduct(dwPlatformID)))
      {
        lpVersionInformation->dwUnderlyingPlatformId = dwPlatformID; 
        lpVersionInformation->dwEmulatedPlatformId   = dwPlatformID;
      }
      else
        return FALSE;
    }
  #else //We must be runing on an emulated or real version of Win16 or Dos
    #ifdef _WINDOWS //Running on some version of Windows                   
      DWORD dwVersion = GetVersion();
      // GetVersion does not differentiate between Windows 3.1 and Windows 3.11
      
      lpVersionInformation->dwEmulatedMajorVersion = LOBYTE(LOWORD(dwVersion)); 
      lpVersionInformation->dwEmulatedMinorVersion = HIBYTE(LOWORD(dwVersion));
      lpVersionInformation->dwEmulatedBuildNumber  = 0; //no build number with Win3.1x
      lpVersionInformation->dwEmulatedPlatformId   = PLATFORM_WINDOWS31;
      _fstrcpy(lpVersionInformation->szEmulatedCSDVersion, "Microsoft Windows");
      
      //GetVersion returns 3.1 even on WFW, need to poke further
      //to find the real difference                      
      if (WFWLoaded())
        lpVersionInformation->dwEmulatedPlatformId = PLATFORM_WINDOWSFW;

      //Call to get the underlying OS here through 16 -> 32 bit Generic Thunk
      BOOL bFoundUnderlyingOS = FALSE;

      // Initialize capability bits for supplied functions
      dwCapBits = WOW_VDMPTR16 | WOW_HWND32;
    
      // Get Kernel handle
      HMODULE hKernel = GetModuleHandle("KERNEL");
      if (hKernel == NULL)
        return FALSE;
    
      // Dynamically link to the functions we want, setting the capability
      // bits as needed
      LoadLibraryEx32W = (lpfnLoadLibraryEx32W) GetProcAddress(hKernel, "LoadLibraryEx32W");
      if (LoadLibraryEx32W)
        dwCapBits |= WOW_LOADLIBRARY;
           
      FreeLibrary32W = (lpfnFreeLibrary32W) GetProcAddress(hKernel, "FreeLibrary32W");
      if (FreeLibrary32W)
        dwCapBits |= WOW_FREELIBRARY;
           
      GetProcAddress32W = (lpfnGetProcAddress32W) GetProcAddress(hKernel, "GetProcAddress32W");
      if (GetProcAddress32W)
        dwCapBits |= WOW_GETPROCADDRESS;
           
      GetVDMPointer32W = (lpfnGetVDMPointer32W) GetProcAddress(hKernel, "GetVDMPointer32W");
      if (GetVDMPointer32W)
        dwCapBits |= WOW_VDMPTR32;
           
      CallProc32W = (lpfnCallProc32W) GetProcAddress(hKernel, "CallProc32W");
      if (CallProc32W)
        dwCapBits |= WOW_CALLPROC;

      //Call thro' the thunk to the Win32 function "GetVersionEx"
      HINSTANCE32 hInstKrnl32 = WOWLoadLibraryEx32("KERNEL32.DLL", NULL, NULL);
      
      //Because we are building the call to the function at runtime, We don't 
      //forget to include the A at the end to call the ASCII version of GetVersionEx
      FARPROC lpfnWin32GetVersionEx = WOWGetProcAddress32(hInstKrnl32, "GetVersionExA");
      if (lpfnWin32GetVersionEx)
      {              
        OSVERSIONINFO osvi;                      
        memset(&osvi, 0, sizeof(OSVERSIONINFO));
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        DWORD dwSuccess = WOWCallProc32(lpfnWin32GetVersionEx, PARAM_01, 1, (LPOSVERSIONINFO) &osvi);
        if (dwSuccess)
        {
          lpVersionInformation->dwUnderlyingMajorVersion = osvi.dwMajorVersion; 
          lpVersionInformation->dwUnderlyingMinorVersion = osvi.dwMinorVersion; 
          lpVersionInformation->dwUnderlyingBuildNumber = LOWORD(osvi.dwBuildNumber); //ignore HIWORD
          _fstrcpy(lpVersionInformation->szUnderlyingCSDVersion, osvi.szCSDVersion);
       
          //Explicitely map the win32 dwPlatformId to our own values
	        if (osvi.dwPlatformId == VER_PLATFORM_WIN32s)
	          lpVersionInformation->dwUnderlyingPlatformId = PLATFORM_WIN32S;
	        else if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
          {
	          lpVersionInformation->dwUnderlyingPlatformId = PLATFORM_WINDOWS;

            //Deterine the Win9x Service pack installed
            if (IsWindows95SP1(lpVersionInformation))
            {
              lpVersionInformation->wEmulatedServicePack = 1;
              lpVersionInformation->wUnderlyingServicePack = 1;
            }
            else if (IsWindows95OSR2(lpVersionInformation))
            {
              lpVersionInformation->wEmulatedServicePack = 2;
              lpVersionInformation->wUnderlyingServicePack = 2;
            }
            else if (IsWindows98SP1(lpVersionInformation))
            {
              lpVersionInformation->wEmulatedServicePack = 1;
              lpVersionInformation->wUnderlyingServicePack = 1;
            }
          }
	        else if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
          {
	          lpVersionInformation->dwUnderlyingPlatformId = PLATFORM_NT_WORKSTATION; //As a default assume NT Workstation

            //Determine the NT Service pack from the OSVI structure string. This saves 
            //having to call the Win32 registry api through a generic thunk
            if (strlen(osvi.szCSDVersion))
            {
              //Parse out the CSDVersion string
              int i=0;      
              while (osvi.szCSDVersion[i] != ('\0') && !isdigit(osvi.szCSDVersion[i]))
                i++;
              lpVersionInformation->wEmulatedServicePack = (WORD) (atoi(&osvi.szCSDVersion[i]));
              lpVersionInformation->wUnderlyingServicePack = lpVersionInformation->wEmulatedServicePack;
            }
          }
	        else if (osvi.dwPlatformId == VER_PLATFORM_WIN32_CE)
	          lpVersionInformation->dwUnderlyingPlatformId = PLATFORM_WINDOWS_CE;
	        else
	          return FALSE;
       
          bFoundUnderlyingOS = TRUE;
        }
      }
      else
      {
        //We failed to get GetVersionEx so try to GetVersion instead. We known that we must be on
        //Windows NT 3.5 or earlier since anything released later by MS had this function.
        FARPROC lpfnWin32GetVersion = WOWGetProcAddress32(hInstKrnl32, "GetVersion");
        if (lpfnWin32GetVersion)
        {              
          DWORD dwVersion = WOWCallProc32(lpfnWin32GetVersion, 0, 0);

          lpVersionInformation->dwUnderlyingMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
          lpVersionInformation->dwUnderlyingMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));
          lpVersionInformation->dwUnderlyingBuildNumber  = 0;
          lpVersionInformation->dwUnderlyingPlatformId   = PLATFORM_NT_WORKSTATION; 
          _fstrcpy(lpVersionInformation->szUnderlyingCSDVersion, "");
   
          bFoundUnderlyingOS = TRUE;
        }
      }
      WOWFreeLibrary32(hInstKrnl32);

      if (!bFoundUnderlyingOS)
      {
        //must be running on a real version of 16 bit Windows whose underlying OS is DOS
        lpVersionInformation->dwUnderlyingMajorVersion = HIBYTE(HIWORD(dwVersion)); 
        lpVersionInformation->dwUnderlyingMinorVersion = LOBYTE(HIWORD(dwVersion)); 
        lpVersionInformation->dwUnderlyingBuildNumber = 0; 
        lpVersionInformation->dwUnderlyingPlatformId = PLATFORM_DOS;
        _fstrcpy(lpVersionInformation->szUnderlyingCSDVersion, "Dos");
      }
    #else //Must be some version of real or emulated Dos
      //Retreive the current version of emulated dos
      BYTE DosMinor;
      BYTE DosMajor;
      _asm
      {
        mov ax, 3306h
        int 21h
        mov byte ptr [DosMajor], bl
        mov byte ptr [DosMinor], bh
      }
      lpVersionInformation->dwEmulatedPlatformId = PLATFORM_DOS;
      lpVersionInformation->dwEmulatedMajorVersion = (DWORD) DosMajor; 
      lpVersionInformation->dwEmulatedMinorVersion = (DWORD) DosMinor;                
      lpVersionInformation->dwEmulatedBuildNumber = 0; //no build number with Dos

      //We can detect if NT is running as it reports Dos v5.5
      if ((lpVersionInformation->dwEmulatedMajorVersion == 5) &&
          (lpVersionInformation->dwEmulatedMinorVersion == 50))    //NT reports Dos v5.5
      {
        _fstrcpy(lpVersionInformation->szUnderlyingCSDVersion, "Microsoft Windows NT");    
        //could not find method of determing version of NT from Dos,
        //so assume 3.50
        lpVersionInformation->dwUnderlyingMajorVersion = 3; 
        lpVersionInformation->dwUnderlyingMinorVersion = 50; 
        lpVersionInformation->dwUnderlyingBuildNumber = 0;  //cannot get access to build number from Dos
        lpVersionInformation->dwUnderlyingPlatformId = PLATFORM_NT_WORKSTATION;
      }            
      else
      {
        //Get the underlying OS here via the int 2FH interface of Windows
        GetWinInfo();
        if (bRunningWindows)
        { 
          if (lpVersionInformation->dwEmulatedMajorVersion >= 7)  //Windows 95/98 marks itself as Dos 7
            lpVersionInformation->dwUnderlyingPlatformId = PLATFORM_WINDOWS;
          else                                                              
          {
            //Could not find method of differentiating between WFW & Win3.1 under Dos,
            //so assume Win3.1                                     
            lpVersionInformation->dwUnderlyingPlatformId = PLATFORM_WINDOWS31;      
          }  
          _fstrcpy(lpVersionInformation->szUnderlyingCSDVersion, "Microsoft Windows");
          lpVersionInformation->dwUnderlyingMajorVersion = (WinVer & 0xFF00) >> 8; 
          lpVersionInformation->dwUnderlyingMinorVersion = WinVer & 0x00FF; 

          if (lpVersionInformation->dwEmulatedMinorVersion == 0)
            lpVersionInformation->dwUnderlyingBuildNumber = 950; //Windows 95 Gold reports Dos v7.0                      
          else if (lpVersionInformation->dwUnderlyingMinorVersion > 0 && 
                   lpVersionInformation->dwUnderlyingMinorVersion < 3) 
          {                                                            
          	//Testing for 95 SP1 has not been done, so the above check
          	//may or may not be work
            lpVersionInformation->dwUnderlyingBuildNumber = 1080; 
          }
          else if (lpVersionInformation->dwUnderlyingMinorVersion == 3)
            lpVersionInformation->dwUnderlyingBuildNumber = 1212; //Windows 95 OSR2 reports Windows 4.03 from 16 bit code
          else if (lpVersionInformation->dwUnderlyingMinorVersion <= 10)
            lpVersionInformation->dwUnderlyingBuildNumber = 1998; //Otherwise must be Windows 98
          else
          {
          	//Need to get the Build number of 98 SE 
          }
        }
        else //must be on a real version of Dos
        {                               
          lpVersionInformation->dwUnderlyingMajorVersion = (DWORD) DosMajor; 
          lpVersionInformation->dwUnderlyingMinorVersion = (DWORD) DosMinor;                
          lpVersionInformation->dwUnderlyingBuildNumber = 0; //no build number with Dos
          lpVersionInformation->dwUnderlyingPlatformId = PLATFORM_DOS;
          _fstrcpy(lpVersionInformation->szUnderlyingCSDVersion, "MS-DOS");
        }
      }  
    #endif  
  #endif

  return TRUE;
}
      
#if defined(_WIN32) && !defined(UNDER_CE)
BOOL WhichNTProduct(DWORD& dwVersion)
{
  HKEY hKey;
  if (RegOpenKey(HKEY_LOCAL_MACHINE, _T("System\\CurrentControlSet\\Control\\ProductOptions"), &hKey) != ERROR_SUCCESS)
    return FALSE;
 
  const int MY_BUFSIZE = 100;
  TCHAR szProductType[MY_BUFSIZE];
  DWORD dwBufLen = MY_BUFSIZE * sizeof(TCHAR);
  if (RegQueryValueEx(hKey, _T("ProductType"), NULL, NULL, (LPBYTE) szProductType, &dwBufLen) != ERROR_SUCCESS)
    return FALSE;

  RegCloseKey(hKey);
        
  BOOL bSuccess = FALSE;

  // check product options, in order of likelihood   
  if (_tcsicmp(_T("WINNT"), szProductType) == 0)
  {
    dwVersion = PLATFORM_NT_WORKSTATION;
    bSuccess = TRUE;
  }  
  else if (_tcsicmp(_T("SERVERNT"), szProductType) == 0)
  {
    dwVersion = PLATFORM_NT_STAND_ALONE_SERVER;
    bSuccess = TRUE;
  }  
  else if (_tcsicmp(_T("LANMANNT"), szProductType) == 0)
  {
    dwVersion = PLATFORM_NT_PRIMARY_DOMAIN_CONTROLLER;
    bSuccess = TRUE;
  }  
  else if (_tcsicmp(_T("LANSECNT"), szProductType) == 0)
  {
    dwVersion = PLATFORM_NT_BACKUP_DOMAIN_CONTROLLER;
    bSuccess = TRUE;
  }  
  
  return bSuccess;
}
#endif                                  
                                   
#if defined(_WINDOWS) && !defined(_WIN32)
UINT WINAPI WOWCreateVDMPointer16(DWORD dwLinBase, DWORD dwLimit)
{
  UINT uSelector, uDSSel;

  if (!(dwCapBits & WOW_VDMPTR16))
    return NULL;

  // Grab our DS (for access rights)
  __asm mov uDSSel, ds

  // Allocate a new selector
  uSelector = AllocSelector(uDSSel);
  if (uSelector)
  {
    // Assign its linear base address and limit
    SetSelectorBase(uSelector, dwLinBase);
    SetSelectorLimit(uSelector, dwLimit);
  }
  return uSelector;
}

UINT WINAPI WOWDeleteVDMPointer16(UINT uSelector)
{
  if (!(dwCapBits & WOW_VDMPTR16))
    return NULL;
  return FreeSelector(uSelector);
}

HWND32 WINAPI WOWHwndToHwnd32(HWND hWnd)
{
  if (!(dwCapBits & WOW_HWND32))
    return NULL;
  
  // OR mask the upper 16 bits
  HWND32 hWnd32 = (HWND32) (WORD) (hWnd);
  return (hWnd32 | 0xffff0000);
}

HINSTANCE32 WINAPI WOWLoadLibraryEx32(LPSTR lpszFile, HFILE32 hFile,
                                               DWORD dwFlags)
{
  if (!(dwCapBits & WOW_LOADLIBRARY))
    return NULL;

  return LoadLibraryEx32W(lpszFile, hFile, dwFlags);
}

BOOL WINAPI WOWFreeLibrary32(HINSTANCE32 hInst32)
{
  if (!(dwCapBits & WOW_FREELIBRARY))
    return NULL;

  return FreeLibrary32W(hInst32);
}

FARPROC WINAPI WOWGetProcAddress32(HINSTANCE32 hInst32,
                                            LPCSTR lpszProc)
{
  if (!(dwCapBits & WOW_GETPROCADDRESS))
    return NULL;
  return GetProcAddress32W(hInst32, lpszProc);
}

DWORD WINAPI WOWGetVDMPointer32(LPVOID lpAddress, UINT fMode)
{
  if (!(dwCapBits & WOW_VDMPTR32))
    return NULL;
  return GetVDMPointer32W(lpAddress, fMode);
}

DWORD FAR CDECL WOWCallProc32(FARPROC lpfnFunction, DWORD dwAddressConvert, DWORD dwParams, ...)
{
  va_list vaList;
  DWORD   dwCount;
  DWORD   dwTemp;

  if (!(dwCapBits & WOW_CALLPROC))
    return NULL;

  // Variable list start
  va_start(vaList,dwParams);

  for(dwCount=0; dwCount < dwParams; dwCount++)
  {
    // Pull each variable off of the stack
    dwTemp=(DWORD)va_arg(vaList,DWORD);

    // Push the DWORD
    __asm push word ptr [dwTemp+2];
    __asm push word ptr [dwTemp];
  }
  // Variable list end
  va_end(vaList);

  // Call Win32.  The pushed variable list precedes the parameters.
  // Appropriate parameters will be popped by this function (based
  // on the value in dwParams)
  return CallProc32W(lpfnFunction, dwAddressConvert, dwParams);
}

BOOL WFWLoaded()
{
  const WORD WNNC_NET_MultiNet         = 0x8000;
  const WORD WNNC_SUBNET_WinWorkgroups = 0x0004;
  const WORD WNNC_NET_TYPE             = 0x0002;
  BOOL rVal;
   
  HINSTANCE hUserInst = LoadLibrary("USER.EXE");
  lpfnWNetGetCaps lpWNetGetCaps = (lpfnWNetGetCaps) GetProcAddress(hUserInst, "WNetGetCaps");
  if (lpWNetGetCaps != NULL)
  {
    // Get the network type
    WORD wNetType = lpWNetGetCaps(WNNC_NET_TYPE);
    if (wNetType & WNNC_NET_MultiNet)
    {
      // a multinet driver is installed
      if (LOBYTE(wNetType) & WNNC_SUBNET_WinWorkgroups) // It is WFW
        rVal = TRUE;
      else // It is not WFW
        rVal = FALSE;
    }
    else
     rVal = FALSE;
  }
  else
    rVal = FALSE;
   
  // Clean up the module instance
  if (hUserInst)
    FreeLibrary(hUserInst);
    
  return rVal;  
}
#endif //defined(_WINDOWS) && !defined(_WIN32)
             
#ifdef _DOS             
void GetWinInfo()
{ 
  BYTE MajorVer;
  BYTE MinorVer;

  //use some inline assembly to determine if Windows if
  //running and what version is active
  _asm
  {
  ; check for Windows 3.1
    mov     ax,160ah                ; WIN31CHECK
    int     2fh                     ; check if running under Win 3.1.
    or      ax,ax
    jz      RunningUnderWin31       ; can check if running in standard
                                    ; or enhanced mode
   
  ; check for Windows 3.0 enhanced mode
    mov     ax,1600h                ; WIN386CHECK
    int     2fh
    test    al,7fh
    jnz     RunningUnderWin30Enh    ; enhanced mode
   
  ; check for 3.0 WINOLDAP
    mov     ax,4680h                ; IS_WINOLDAP_ACTIVE
    int     2fh
    or      ax,ax                   ; running under 3.0 derivative?
    jnz     NotRunningUnderWin
   
  ; rule out MS-DOS 5.0 task switcher
    mov     ax,4b02h                ; detect switcher
    push    bx
    push    es
    push    di
    xor     bx,bx
    mov     di,bx
    mov     es,bx
    int     2fh
    pop     di
    pop     es
    pop     bx
    or      ax,ax
    jz      NotRunningUnderWin      ; MS-DOS 5.0 task switcher found
   
  ; check for standard mode Windows 3.0
    mov     ax,1605h                ; PMODE_START
    int     2fh
    cmp     cx,-1
    jz      RunningUnderWin30Std
   
  ; check for real mode Windows 3.0
    mov     ax,1606h                ; PMODE_STOP
    int     2fh                     ; in case someone is counting
    ; Real mode Windows 3.0 is running
    mov     byte ptr [bRunningWindows], 1
    mov     byte ptr [MajorVer], 3h    
    mov     byte ptr [MinorVer], 0h        
    jmp     ExitLabel
   
  RunningUnderWin30Std:
    ; Standard mode Windows 3.0 is running
    mov     byte ptr [bRunningWindows], 1
    mov     byte ptr [MajorVer], 3h    
    mov     byte ptr [MinorVer], 0h        
    jmp     ExitLabel
   
  RunningUnderWin31:
    ; At this point: CX == 3 means Windows 3.1 enhanced mode
    ;                CX == 2 means Windows 3.1 standard mode
    mov     byte ptr [bRunningWindows], 1
    
    ; Get the version of Windows 
    mov     ax, 1600h   ; Get Enhanced-Mode Windows Installed State
    int     2Fh
    mov     byte ptr [MajorVer], al
    mov     byte ptr [MinorVer], ah
    jmp     ExitLabel
   
  RunningUnderWin30Enh:
    ; Enhanced mode Windows 3.0 is running
    mov     byte ptr [bRunningWindows], 1    
    mov     byte ptr [MajorVer], 3h    
    mov     byte ptr [MinorVer], 0h        
    jmp     ExitLabel
   
  NotRunningUnderWin:                    
    mov     byte ptr [bRunningWindows], 0
    
  ExitLabel:
  }             
  
  WinVer = (WORD) ((MajorVer << 8) + MinorVer);
} 
#endif //_DOS 

BOOL IsWindowsCE(LPOS_VERSION_INFO lpVersionInformation)
{
  return (lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_WINDOWS_CE);
}

BOOL IsWindows95(LPOS_VERSION_INFO lpVersionInformation)
{
  return (lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_WINDOWS &&
          lpVersionInformation->dwUnderlyingMajorVersion == 4 && 
          lpVersionInformation->dwUnderlyingMinorVersion == 0 &&
          lpVersionInformation->dwUnderlyingBuildNumber == 950);
}

BOOL IsWindows95SP1(LPOS_VERSION_INFO lpVersionInformation)
{
  return (lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_WINDOWS &&
          lpVersionInformation->dwUnderlyingMajorVersion == 4 && 
          lpVersionInformation->dwUnderlyingBuildNumber > 950 && 
          lpVersionInformation->dwUnderlyingBuildNumber <= 1080);
}

BOOL IsWindows95OSR2(LPOS_VERSION_INFO lpVersionInformation)
{
  return (lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_WINDOWS &&
          lpVersionInformation->dwUnderlyingMajorVersion == 4 && 
          lpVersionInformation->dwUnderlyingMinorVersion < 10 &&
          lpVersionInformation->dwUnderlyingBuildNumber > 1080);
}

BOOL IsWindows98(LPOS_VERSION_INFO lpVersionInformation)
{
  return (lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_WINDOWS &&
          lpVersionInformation->dwUnderlyingMajorVersion == 4 && 
          lpVersionInformation->dwUnderlyingMinorVersion == 10 &&
          lpVersionInformation->dwUnderlyingBuildNumber == 1998);
}

BOOL IsWindows98SP1(LPOS_VERSION_INFO lpVersionInformation)
{
  return (lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_WINDOWS &&
          lpVersionInformation->dwUnderlyingMajorVersion == 4 && 
          lpVersionInformation->dwUnderlyingMinorVersion == 10 &&
          lpVersionInformation->dwUnderlyingBuildNumber > 1998 &&
          lpVersionInformation->dwUnderlyingBuildNumber < 2183);
}

BOOL IsWindows98SE(LPOS_VERSION_INFO lpVersionInformation)
{
  return (lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_WINDOWS &&
          (lpVersionInformation->dwUnderlyingMajorVersion > 4) || (lpVersionInformation->dwUnderlyingMajorVersion == 4 && 
          lpVersionInformation->dwUnderlyingBuildNumber >= 2183));
}

BOOL IsWindowsNT31(LPOS_VERSION_INFO lpVersionInformation)
{
  return ((lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_NT_STAND_ALONE_SERVER || 
           lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_NT_PRIMARY_DOMAIN_CONTROLLER ||  
           lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_NT_BACKUP_DOMAIN_CONTROLLER ||  
           lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_NT_WORKSTATION) &&
          lpVersionInformation->dwUnderlyingMajorVersion == 3 && 
          lpVersionInformation->dwUnderlyingMinorVersion == 10);
}

BOOL IsWindowsNT35(LPOS_VERSION_INFO lpVersionInformation)
{
  return ((lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_NT_STAND_ALONE_SERVER || 
           lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_NT_PRIMARY_DOMAIN_CONTROLLER ||  
           lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_NT_BACKUP_DOMAIN_CONTROLLER ||  
           lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_NT_WORKSTATION) &&
          lpVersionInformation->dwUnderlyingMajorVersion == 3 && 
          lpVersionInformation->dwUnderlyingMinorVersion == 50);
}

BOOL IsWindowsNT351(LPOS_VERSION_INFO lpVersionInformation)
{
  return ((lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_NT_STAND_ALONE_SERVER || 
           lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_NT_PRIMARY_DOMAIN_CONTROLLER ||  
           lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_NT_BACKUP_DOMAIN_CONTROLLER ||  
           lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_NT_WORKSTATION) &&
          lpVersionInformation->dwUnderlyingMajorVersion == 3 && 
          lpVersionInformation->dwUnderlyingMinorVersion == 51);
}

BOOL IsWindowsNT4(LPOS_VERSION_INFO lpVersionInformation)
{
  return ((lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_NT_STAND_ALONE_SERVER || 
           lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_NT_PRIMARY_DOMAIN_CONTROLLER ||  
           lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_NT_BACKUP_DOMAIN_CONTROLLER ||  
           lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_NT_WORKSTATION) &&
          lpVersionInformation->dwUnderlyingMajorVersion == 4);
}

BOOL IsWindows2000(LPOS_VERSION_INFO lpVersionInformation)
{
  return ((lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_NT_STAND_ALONE_SERVER || 
           lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_NT_PRIMARY_DOMAIN_CONTROLLER ||  
           lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_NT_BACKUP_DOMAIN_CONTROLLER ||  
           lpVersionInformation->dwUnderlyingPlatformId == PLATFORM_NT_WORKSTATION) &&
          lpVersionInformation->dwUnderlyingMajorVersion == 5);
}

#if defined(_WINDOWS) && defined(_WIN32) && !defined(UNDER_CE)
WORD GetNTServicePackFromCSDString(LPCTSTR pszCSDVersion)
{
  WORD wServicePack = 0;
  if (_tcslen(pszCSDVersion))
  {
    //Parse out the CSDVersion string
    int i=0;      
    while (pszCSDVersion[i] != _T('\0') && !_istdigit(pszCSDVersion[i]))
      i++;
    wServicePack = (WORD) (_ttoi(&pszCSDVersion[i]));
  }

  return wServicePack;
}


WORD GetNTServicePackFromRegistry()
{
  //By default assume no service pack
  WORD wServicePack = 0;

  HKEY hCurrentVersion;
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows NT\\CurrentVersion"),
                   0, KEY_READ, &hCurrentVersion) == ERROR_SUCCESS)
  {
    BYTE byData[128];
    DWORD dwType;
    DWORD dwSize = 128;
    if (RegQueryValueEx(hCurrentVersion, _T("CSDVersion"), NULL, &dwType, byData, &dwSize) == ERROR_SUCCESS)
    {
      if (dwType == REG_SZ)
      {
        //Stored as a string on all other versions of NT
        wServicePack = GetNTServicePackFromCSDString((TCHAR*) byData);
      }
      else if (dwType == REG_DWORD)
      {
        //Handle the service pack number being stored as a DWORD which happens on NT 3.1
        wServicePack = HIBYTE((WORD) *((DWORD*) &byData));
      }
    }

    //Don't forget to close the registry key we were using      
    RegCloseKey(hCurrentVersion);
  }

  return wServicePack;
}

BOOL IsTerminalServicesEnabled() 
{
  // The return value
  BOOL bResult = FALSE;

  // Are we running on Windows NT?
  DWORD dwVersion = GetVersion();
  if (!(dwVersion & 0x80000000)) 
  {
    //Yes then check to see if we are running on Terminal Server
    bResult = ValidateProductSuite(_T("Terminal Server"));
  }

  return bResult;
}

BOOL IsEnterpriseServer()
{
  // The return value
  BOOL bResult = FALSE;

  // Are we running on Windows NT?
  DWORD dwVersion = GetVersion();
  if (!(dwVersion & 0x80000000)) 
  {
    //Yes then check to see if we are running on Terminal Server
    bResult = ValidateProductSuite(_T("Enterprise"));
  }

  return bResult;
}

BOOL ValidateProductSuite(LPCTSTR lpszSuiteToValidate) 
{
  // Open the ProductOptions key.
  HKEY hKey = NULL;
  LONG lResult = RegOpenKey(HKEY_LOCAL_MACHINE, _T("System\\CurrentControlSet\\Control\\ProductOptions"), &hKey);
  if (lResult != ERROR_SUCCESS)
    return FALSE;

  // Determine required size of ProductSuite buffer.
  DWORD dwType = 0;
  DWORD dwSize = 0;
  lResult = RegQueryValueEx(hKey, _T("ProductSuite"), NULL, &dwType, NULL, &dwSize);
  if (lResult != ERROR_SUCCESS || !dwSize)
  {
    RegCloseKey(hKey);
    return FALSE;
  }

  // Allocate buffer.
  LPTSTR lpszProductSuites = (LPTSTR) new BYTE[dwSize];

  // Retrieve array of product suite strings.
  lResult = RegQueryValueEx(hKey, _T("ProductSuite"), NULL, &dwType, (LPBYTE) lpszProductSuites, &dwSize);
  if (lResult != ERROR_SUCCESS || dwType != REG_MULTI_SZ)
  {
    //Don't forget to free up the resource we used
    delete [] lpszProductSuites;
    RegCloseKey(hKey);

    return FALSE;
  }

  //All string comparisons will be sub string only and case insensitive
  LPTSTR lpszLocalSuite = new TCHAR[_tcslen(lpszSuiteToValidate)+1];
  _tcscpy(lpszLocalSuite, lpszSuiteToValidate);
  _tcsupr(lpszLocalSuite);

  // Search for suite name in array of strings.
  BOOL bValidated = FALSE;
  LPTSTR lpszSuite = lpszProductSuites;
  while (*lpszSuite) 
  {
    //Ensure the string is upper case
    _tcsupr(lpszSuite);

    //Does the suite match up with the current item?
    if (_tcsstr(lpszSuite, lpszLocalSuite)) 
    {
      bValidated = TRUE;
      break;
    }
    lpszSuite += (lstrlen(lpszSuite) + 1);
  }

  //Don't forget to free up the resource we used
  delete [] lpszLocalSuite;
  delete [] lpszProductSuites;
  RegCloseKey(hKey);

  return bValidated;
}
#endif //#if defined(_WINDOWS) && defined(_WIN32)
