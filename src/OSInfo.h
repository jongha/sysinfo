// OSInfo.h
//

#ifndef __DTWINVER_H__

////////////////////////////////// Includes ///////////////////////////////////


////////////////////////////////// defines ////////////////////////////////////

//values which get stored in [OS_VERSION_INFO].dwEmulatedPlatformId 
//and [OS_VERSION_INFO].dwUnderlyingPlatformId 
const DWORD PLATFORM_WIN32S                       = 0;
const DWORD PLATFORM_WINDOWS                      = 1;
const DWORD PLATFORM_NT_WORKSTATION               = 2; 
const DWORD PLATFORM_WINDOWS31                    = 3;  
const DWORD PLATFORM_WINDOWSFW                    = 4;
const DWORD PLATFORM_DOS                          = 5;
const DWORD PLATFORM_NT_STAND_ALONE_SERVER        = 6;
const DWORD PLATFORM_NT_PRIMARY_DOMAIN_CONTROLLER = 7;
const DWORD PLATFORM_NT_BACKUP_DOMAIN_CONTROLLER  = 8;
const DWORD PLATFORM_WINDOWS_CE                   = 9;
const DWORD PLATFORM_WINDOWS_TERMINAL_SERVER      = 10;
const DWORD PLATFORM_NT_ENTERPRISE_SERVER         = 11;


typedef struct _OS_VERSION_INFO
{
  //What version of OS is being emulated
  DWORD dwEmulatedMajorVersion;
  DWORD dwEmulatedMinorVersion;
  DWORD dwEmulatedBuildNumber;
  DWORD dwEmulatedPlatformId;
#ifdef _WIN32                    
  TCHAR szEmulatedCSDVersion[128];
#else
  char szEmulatedCSDVersion[128];
#endif  
  WORD wEmulatedServicePack;

  //What version of OS is really running                 
  DWORD dwUnderlyingMajorVersion;
  DWORD dwUnderlyingMinorVersion;
  DWORD dwUnderlyingBuildNumber;
  DWORD dwUnderlyingPlatformId;   
#ifdef _WIN32                      
  TCHAR szUnderlyingCSDVersion[128];
#else  
  char szUnderlyingCSDVersion[128];
#endif  
  WORD wUnderlyingServicePack;
} OS_VERSION_INFO, *POS_VERSION_INFO, FAR *LPOS_VERSION_INFO;


/////////////////////////////// Functions /////////////////////////////////////
BOOL GetOSVersion(LPOS_VERSION_INFO lpVersionInformation);

//Please note that the return values for all the following functions 
//are mutually exclusive for example if you are running on 
//95 OSR2 then IsWindows95 will return FALSE
BOOL IsWindowsCE(LPOS_VERSION_INFO lpVersionInformation);
BOOL IsWindows95(LPOS_VERSION_INFO lpVersionInformation);
BOOL IsWindows95SP1(LPOS_VERSION_INFO lpVersionInformation);
BOOL IsWindows95OSR2(LPOS_VERSION_INFO lpVersionInformation);
BOOL IsWindows98(LPOS_VERSION_INFO lpVersionInformation);
BOOL IsWindows98SP1(LPOS_VERSION_INFO lpVersionInformation);
BOOL IsWindows98SE(LPOS_VERSION_INFO lpVersionInformation);
BOOL IsWindowsNT31(LPOS_VERSION_INFO lpVersionInformation);
BOOL IsWindowsNT35(LPOS_VERSION_INFO lpVersionInformation);
BOOL IsWindowsNT351(LPOS_VERSION_INFO lpVersionInformation);
BOOL IsWindowsNT4(LPOS_VERSION_INFO lpVersionInformation);
BOOL IsWindows2000(LPOS_VERSION_INFO lpVersionInformation);


#endif //__DTWINVER_H__
