#include "StdAfx.h"
#include "OSInfo.h"
#include "CPUInfo.h"
#include <math.h>
#include ".\wssysinfoutil.h"

//#include <mmsystem.h>
//#include <mmreg.h>
//#include "Winspool.h"
#include "WinIoCtl.h"

#define MAX_OF_HARD_DISKS	26
static char HardDiskLetters[MAX_OF_HARD_DISKS][4]={
	"c:\\",	"d:\\",	"e:\\",	"f:\\",	"g:\\",	"h:\\",
	"i:\\",	"j:\\",	"k:\\",	"l:\\",	"m:\\",	"n:\\",
	"o:\\",	"p:\\",	"q:\\",	"r:\\",	"s:\\",	"t:\\",
	"u:\\",	"v:\\",	"w:\\",	"x:\\",	"y:\\",	"z:\\"
};

CwsSysInfoUtil::CwsSysInfoUtil(void)
{
}

CwsSysInfoUtil::~CwsSysInfoUtil(void)
{
}

CString CwsSysInfoUtil::GetOS()
{
	CString Response;
	OS_VERSION_INFO osvi;
	char sText[512]="";
	char sBuf[100]="";
	ZeroMemory(&osvi,sizeof(OS_VERSION_INFO));
	
//	_stprintf(sText, _T("\nOperating System\n"));
	if (GetOSVersion(&osvi)) {                                           
//		_stprintf(sText, _T("Emulated OS: "));
		
		switch (osvi.dwEmulatedPlatformId) {
		case PLATFORM_DOS:               
			{
				_stprintf(sBuf, _T("Dos"));
				break;
			}
		case PLATFORM_WINDOWS31:         
			{
				_stprintf(sBuf, _T("Windows"));
				break;
			}
		case PLATFORM_WINDOWSFW:         
			{
				_stprintf(sBuf, _T("Windows For Workgroups"));
				break; 
			}
		case PLATFORM_WIN32S:
			{
				_stprintf(sBuf, _T("Win32s"));
				break; 
			}
		case PLATFORM_WINDOWS_CE:
			{
				_stprintf(sBuf, _T("Windows CE"));
				break;
			}
		case PLATFORM_WINDOWS:
			{
				if (IsWindows95(&osvi))
					_stprintf(sBuf, _T("Windows 95"));
				else if (IsWindows95SP1(&osvi))
					_stprintf(sBuf, _T("Windows 95 SP1"));
				else if (IsWindows95OSR2(&osvi))
					_stprintf(sBuf, _T("Windows 95 OSR2"));
				else if (IsWindows98(&osvi))
					_stprintf(sBuf, _T("Windows 98"));
				else if (IsWindows98SP1(&osvi))
					_stprintf(sBuf, _T("Windows 98 SP1"));
				else if (IsWindows98SE(&osvi))
					_stprintf(sBuf, _T("Windows 98 Second Edition"));
				else
					_stprintf(sBuf, _T("Windows ??"));
				break;
			}
		case PLATFORM_NT_WORKSTATION:
			{
				if (IsWindows2000(&osvi))
					_stprintf(sBuf, _T("Windows 2000 Professional"));
				else
					_stprintf(sBuf, _T("Windows NT Workstation"));
				break;
			}
		case PLATFORM_NT_PRIMARY_DOMAIN_CONTROLLER:         
			{
				if (IsWindows2000(&osvi))
					_stprintf(sBuf, _T("Windows 2000 Server (Acting as Primary Domain Controller)"));
				else
					_stprintf(sBuf, _T("Windows NT Server (Acting as Primary Domain Controller)"));
				break;
			}
		case PLATFORM_NT_BACKUP_DOMAIN_CONTROLLER:         
			{
				if (IsWindows2000(&osvi))
					_stprintf(sBuf, _T("Windows 2000 Server (Acting as Backup Domain Controller)"));
				else
					_stprintf(sBuf, _T("Windows NT Server (Acting as Backup Domain Controller)"));
				break;
			}
		case PLATFORM_NT_STAND_ALONE_SERVER:
			{
				if (IsWindows2000(&osvi))
					_stprintf(sBuf, _T("Windows 2000 Server (Acting as Standalone Sever)"));
				else
					_stprintf(sBuf, _T("Windows NT Server (Acting as Standalone Sever)"));
				break;
			}
		case PLATFORM_WINDOWS_TERMINAL_SERVER:
			{
				_stprintf(sBuf, _T("Windows NT Terminal Server"));
				break;
			}
		case PLATFORM_NT_ENTERPRISE_SERVER:
			{
				_stprintf(sBuf, _T("Windows NT Enterprise Edition"));
				break;
			}
		default: 
			{
				_stprintf(sBuf, _T("Unknown OS"));
				break;
			}
		}
	}                     
	_tcscat(sText, sBuf);
//	_stprintf(sBuf, _T(" v%d."), osvi.dwEmulatedMajorVersion);
//	_tcscat(sText, sBuf);     
//	_stprintf(sBuf, _T("%02d "), osvi.dwEmulatedMinorVersion);
//	_tcscat(sText, sBuf);                           
	if (osvi.wEmulatedServicePack)       
	{
		_stprintf(sBuf, _T(" Service Pack %d"), osvi.wEmulatedServicePack);
		_tcscat(sText, sBuf);
	}
	if (osvi.dwEmulatedBuildNumber)
	{
		_stprintf(sBuf, _T(" (Build %d)"), osvi.dwEmulatedBuildNumber);
		_tcscat(sText, sBuf);           
	}
//	_tcscat(sText, _T(" \r\n"));
	;

	return sText;
}

CString CwsSysInfoUtil::GetCPUCount()
{
	CString Response;
	SYSTEM_INFO SysInfo;
	int nProcessors = 0;
	
	// Get the number of processors in the system.
	ZeroMemory (&SysInfo, sizeof (SYSTEM_INFO));
	GetSystemInfo (&SysInfo);
	
	// Number of physical processors in a non-Intel system
	// or in a 32-bit Intel system with Hyper-Threading technology disabled
	nProcessors = SysInfo.dwNumberOfProcessors;
	Response.Format("Number of CPU(s) : %d",nProcessors);
	
	if(Response.GetLength() == 0) {
		Response = "Not Detected";
	}
	return Response;
}

#define CPU_KEY "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"
#define CPU_VAL "ProcessorNameString"
#define CPU_VAL2 "Identifier"
CString CwsSysInfoUtil::GetCPUInfo()
{
	HKEY	hKey;
	if ( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, CPU_KEY, 0, KEY_READ, &hKey ) )
		return "Not Detected\n";

	TCHAR	szBuf[512];
	ZeroMemory( szBuf, sizeof( szBuf ) );

	DWORD	dwType	= REG_SZ,
			dwSize	= sizeof( szBuf );

	if ( ERROR_SUCCESS != RegQueryValueEx( hKey, CPU_VAL, NULL, &dwType, (LPBYTE)szBuf, &dwSize ) )
	{
		if ( ERROR_SUCCESS != RegQueryValueEx( hKey, CPU_VAL2, NULL, &dwType, (LPBYTE)szBuf, &dwSize ) )
		{
            RegCloseKey( hKey );
			return "\nProcessor:\n Not Detected\n";
		}
	} 
	RegCloseKey( hKey );

	CString strCPUInfo = szBuf;
	strCPUInfo.TrimLeft();
	strCPUInfo.TrimRight();

	return strCPUInfo;
}

CString CwsSysInfoUtil::GetCPUName()
{
	CString Response;
	CPUInfo cpu;
	if (cpu.DoesCPUSupportCPUID())
	{
		Response = cpu.GetExtendedProcessorName();
	}
	
	if(Response.GetLength() == 0) {
		Response = "Not Detected";
	}
	Response.TrimLeft();
	Response.TrimRight();

	return Response;
}

CString CwsSysInfoUtil::GetTotalRAM()
{
	CString Response;
	MEMORYSTATUS memoryStatus;
	ZeroMemory(&memoryStatus,sizeof(MEMORYSTATUS));
	memoryStatus.dwLength = sizeof (MEMORYSTATUS);
	
	::GlobalMemoryStatus (&memoryStatus);
	
	CString Temp;
	Temp.Format("%ld Megabytes Installed Memory",(DWORD) ceil(memoryStatus.dwTotalPhys/1024/1024.0));
	return Temp;
	
/*	Temp.Format("\r\nMemory Available: %ldKB",(DWORD) (memoryStatus.dwAvailPhys/1024));
	Response+=Temp;
	
	Temp.Format("\r\nPrecent of used RAM: %%%ld",memoryStatus.dwMemoryLoad);
	Response+=Temp;

	return Response;
*/
}

BOOL CwsSysInfoUtil::IsDynamicDeviceOnWin9x(char *szRegPath)
{	
	DWORD		dwKeyIndex;
	DWORD		dwSize, dwType;
	char		szHardwareKey[1024];
	char		szEnumName[1024];
	long		lResult, lResult1;
	HKEY		hMainKey, hSubKey;
	LPSTR		lpWin9xSubKey = "CONFIG MANAGER\\ENUM";
	LPSTR		lpWin9xKeyword = "HARDWAREKEY";
	
	// ONLY ONE LOOP FOR ANYTIME BREAK :P //
	do{
		// INIT VARIABLE
		dwKeyIndex = 0;
		lResult = RegOpenKeyEx(HKEY_DYN_DATA, lpWin9xSubKey, 0 , KEY_ENUMERATE_SUB_KEYS|KEY_EXECUTE|KEY_QUERY_VALUE, &hMainKey);		
		if (lResult != ERROR_SUCCESS)break;

		// REAL LOOP FOR FINDING DISPLAY NAME
		while( 1 )
		{
			lResult = RegEnumKey( hMainKey , dwKeyIndex, szEnumName, MAX_PATH + 1);						
			lResult1 = RegOpenKeyEx(hMainKey, szEnumName, 0, KEY_READ, &hSubKey );
			if (lResult == ERROR_NO_MORE_ITEMS || lResult1 != ERROR_SUCCESS )break;						
			
			// CHECK WHETHER HARDWARE KEY IS !!szRegPath!!
			dwType = REG_SZ;dwSize = sizeof(szHardwareKey);memset( szHardwareKey, 0, sizeof(szHardwareKey) );
			lResult = RegQueryValueEx(hSubKey, lpWin9xKeyword, NULL, &dwType, (LPBYTE)szHardwareKey, &dwSize);	
			if( lResult != ERROR_SUCCESS ){
				dwKeyIndex++;
				continue;
			}

			if( _strnicmp( szRegPath, szHardwareKey, strlen(szRegPath) ) ){
				dwKeyIndex++;
				continue;
			}

			// SAME NAME RETURN TRUE
			// TERMINATE REGISTRY KEY
			RegCloseKey( hMainKey );
			RegCloseKey( hSubKey );				
			return TRUE;
		}
	}while( 0 );

	// TERMINATE REGISTRY KEY
	RegCloseKey( hMainKey );
	RegCloseKey( hSubKey );		
	return FALSE;
}

CString CwsSysInfoUtil::GetDeviceInfoWinNTSeries(LPSTR lpNTKeyword, LPSTR lpNTSubKey, LPSTR lpNTService, LPSTR lpNTVerifyName, LPSTR lpNTDesc)
{
	BOOL		bVerify;
	DWORD		dwKeyIndex, dwSubKeyIndex, dwVerifyLoop;
	DWORD		dwSize, dwType;
	char		szClass[1024], szDesc[1024];
	char		szEnumName[1024];
	long		lResult, lResult1;
	HKEY		hMainKey = NULL, hSubKey_1 = NULL, hSubKey_2 = NULL;
//	LPSTR		lpNTKeyword = "CLASS";
//	LPSTR		lpNTSubKey = "SYSTEM\\CURRENTCONTROLSET\\ENUM\\PCI";
//	LPSTR		lpNTService = "MEDIA";
//	LPSTR		lpNTVerifyName = "CONTROL";
//	LPSTR		lpNTDesc = "DEVICEDESC";

	// ONLY ONE LOOP FOR ANYTIME BREAK :P //
	do{
		// INIT VARIABLE
		dwKeyIndex = 0;
		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpNTSubKey, 0 , KEY_ENUMERATE_SUB_KEYS|KEY_EXECUTE|KEY_QUERY_VALUE, &hMainKey);		
		if (lResult != ERROR_SUCCESS)break;

		// REAL LOOP FOR FINDING DEVICE NAME
		while( 1 )
		{
			lResult = RegEnumKey( hMainKey , dwKeyIndex, szEnumName, MAX_PATH + 1);						
			lResult1 = RegOpenKeyEx(hMainKey, szEnumName, 0, KEY_READ, &hSubKey_1);
			if (lResult == ERROR_NO_MORE_ITEMS || lResult1 != ERROR_SUCCESS )break;						
			
			dwSubKeyIndex = 0;
			while(1)
			{
				lResult = RegEnumKey( hSubKey_1, dwSubKeyIndex, szEnumName, MAX_PATH + 1);						
				lResult1 = RegOpenKeyEx(hSubKey_1, szEnumName, 0, KEY_READ, &hSubKey_2);
				if (lResult == ERROR_NO_MORE_ITEMS || lResult1 != ERROR_SUCCESS )break;						
				
				// CHECK WHETHER CLASS IS !!lpNTService!!
				dwType = REG_SZ;dwSize = sizeof(szClass);memset( szClass, 0, sizeof(szClass) );
				lResult = RegQueryValueEx(hSubKey_2, lpNTKeyword, NULL, &dwType, (LPBYTE)szClass, &dwSize);				

				if( lResult != ERROR_SUCCESS ){
					dwSubKeyIndex++;
					continue;
				}

				if( _strnicmp( szClass, lpNTService, strlen(szClass) ) ){
					dwSubKeyIndex++;
					continue;
				}

				// INIT SECOND LOOP
				dwVerifyLoop = 0;
				bVerify = TRUE;
				// SECOND LOOP FOR VERIFYING IT IS USING DEVICE
				while( 1 )
				{
					lResult = RegEnumKey( hSubKey_2, dwVerifyLoop, szEnumName, MAX_PATH + 1);										
					if (lResult == ERROR_NO_MORE_ITEMS  ){
						bVerify = FALSE;
						break;						
					}
					
					if( !_strnicmp( szEnumName, lpNTVerifyName, strlen(szEnumName) ) ){
						bVerify = TRUE;
						break;
					}

					dwVerifyLoop++;
				}

				if( bVerify ){
					//GET lpNTService DEVICE NAME
					dwType = REG_SZ;dwSize = sizeof(szDesc);memset( szDesc, 0, sizeof(szDesc) );
					lResult = RegQueryValueEx(hSubKey_2, lpNTDesc, NULL, &dwType, (LPBYTE)szDesc, &dwSize);	
					if( lResult != ERROR_SUCCESS )break;
					
					return szDesc;
				}

				dwSubKeyIndex++;
			}

			dwKeyIndex++;
		}		
	}while( 0 );

	// TERMINATE REGISTRY KEY
	if (hMainKey) {
		RegCloseKey( hMainKey );
	}
	if (hSubKey_1) {
		RegCloseKey( hSubKey_1 );
	}
	if (hSubKey_2) {
		RegCloseKey( hSubKey_2 );
	}

	return "Not detected";
}

CString CwsSysInfoUtil::GetDeviceInfoWin9XSeries(LPSTR lpWin9xKeyword, LPSTR lpWin9xSubKey, LPSTR lpWin9xService, LPSTR lpWin9xDesc)
{
	HKEY		hMainKey, hSubKey_1, hSubKey_2;
	long		lResult, lResult1;
	DWORD		dwKeyIndex, dwSubKeyIndex ;
	DWORD		dwSize, dwType;
//	LPSTR		lpWin9xKeyword	= "CLASS";
//	LPSTR		lpWin9xSubKey	= "ENUM\\PCI";
//	LPSTR		lpWin9xService	= "MEDIA";
//	LPSTR		lpWin9xDesc		= "DEVICEDESC";
	char		szRegPath[1024], szSubRegPath[1024], szEnumName[1024], szClass[1024], szDesc[1024];

	memset( szRegPath, 0, sizeof(szRegPath) );
	memset( szSubRegPath, 0, sizeof(szRegPath) );
	memset( szEnumName, 0, sizeof(szRegPath) );
	memset( szClass, 0, sizeof(szRegPath) );
	memset( szDesc, 0, sizeof(szRegPath) );

	// ONLY ONE LOOP FOR ANYTIME BREAK :P //
	do{
		// INIT VARIABLE
		dwKeyIndex = 0;
		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpWin9xSubKey, 0 , KEY_ENUMERATE_SUB_KEYS|KEY_EXECUTE|KEY_QUERY_VALUE, &hMainKey);		
		if (lResult != ERROR_SUCCESS)break;

		// REAL LOOP FOR FINDING DEVICE NAME
		while( 1 )
		{
			// INIT PATH BUFF			
			CString strTmp = lpWin9xSubKey;
			int nFind = strTmp.ReverseFind('\\');
			strTmp = strTmp.Mid(nFind+1);
			strcpy( szRegPath, strTmp );

			lResult = RegEnumKey( hMainKey , dwKeyIndex, szEnumName, MAX_PATH + 1);						
			lResult1 = RegOpenKeyEx(hMainKey, szEnumName, 0, KEY_READ, &hSubKey_1);
			if (lResult == ERROR_NO_MORE_ITEMS || lResult1 != ERROR_SUCCESS )break;						

			// ADD PATH
			wsprintf( szRegPath, "%s\\%s", szRegPath, szEnumName );

			memset( szSubRegPath, 0, sizeof(szSubRegPath) );
			dwSubKeyIndex = 0;
			while(1)
			{
				lResult = RegEnumKey( hSubKey_1, dwSubKeyIndex, szEnumName, MAX_PATH + 1);						
				lResult1 = RegOpenKeyEx(hSubKey_1, szEnumName, 0, KEY_READ, &hSubKey_2);
				if (lResult == ERROR_NO_MORE_ITEMS || lResult1 != ERROR_SUCCESS )break;						

				// ADD PATH
				wsprintf( szSubRegPath, "%s\\%s", szRegPath, szEnumName );

				// CHECK WHETHER CLASS IS !!lpWin9xService!!
				dwType = REG_SZ;dwSize = sizeof(szClass);memset( szClass, 0, sizeof(szClass) );
				lResult = RegQueryValueEx(hSubKey_2, lpWin9xKeyword, NULL, &dwType, (LPBYTE)szClass, &dwSize);	
				if( lResult != ERROR_SUCCESS ){
					dwSubKeyIndex++;
					continue;
				}

				if( _strnicmp( szClass, lpWin9xService, strlen(szClass) ) )break;				

				if( !IsDynamicDeviceOnWin9x( szSubRegPath ) ){
					dwSubKeyIndex++;
					continue;
				}

				//GET lpWin9xService DEVICE NAME
				dwType = REG_SZ;dwSize = sizeof(szDesc);memset( szDesc, 0, sizeof(szDesc) );
				lResult = RegQueryValueEx(hSubKey_2, lpWin9xDesc, NULL, &dwType, (LPBYTE)szDesc, &dwSize);	
				if( lResult != ERROR_SUCCESS )break;
								
				return szDesc;
				dwSubKeyIndex++;
			}

			dwKeyIndex++;
		}

	}while(0);
	
	// TERMINATE REGISTRY KEY
	RegCloseKey( hMainKey );
	RegCloseKey( hSubKey_1 );
	RegCloseKey( hSubKey_2 );

	return "Not detected";
}

CString CwsSysInfoUtil::GetHddDriveInfo()
{
	int index=0;
	float fTotalSize=0;
	HANDLE hDevice;
	CString Result;
	CString HardDisk;
	DWORD junk;
	DISK_GEOMETRY pdg;
	BOOL bWin9X = FALSE;
	if (GetVersion() >= 0x80000000) {
		bWin9X = TRUE;
	}

	HardDisk.Format("\\\\.\\PhysicalDrive%d",index);

	hDevice=CreateFile(HardDisk,0,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);

	if (hDevice==INVALID_HANDLE_VALUE)
		Result=="Error: Can not Query Hard Disk Drives.";

	while (hDevice!=INVALID_HANDLE_VALUE)
	{
		BOOL bResult=DeviceIoControl(hDevice,IOCTL_DISK_GET_DRIVE_GEOMETRY,
									NULL,0,
									&pdg,sizeof(DISK_GEOMETRY),
									&junk,
									(LPOVERLAPPED) NULL);
		if (bResult)
		{
			CString Temp;
			Temp.Format("Hard Disk Drive %d : ",index+1);
			Result+=Temp;

			double DiskSize=pdg.Cylinders.QuadPart * pdg.TracksPerCylinder * 
				pdg.SectorsPerTrack * pdg.BytesPerSector/1024/1024/1024.0;
			
			Temp.Format("%.2f GB\n", DiskSize);
			Result+=Temp;
//			fTotalSize += atof(Temp);
		}

		CloseHandle(hDevice);

		index++;
		HardDisk.Format("\\\\.\\PhysicalDrive%d",index);
		hDevice=CreateFile(HardDisk,0,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
	}

	double dTotalSize = 0, dTotalFree = 0;
	ULARGE_INTEGER AvailableToCaller, Disk, Free;
	for (int iCounter=0; iCounter < MAX_OF_HARD_DISKS; iCounter++)
	{
		if (GetDriveType(HardDiskLetters[iCounter])==DRIVE_FIXED)
		{
			if (GetDiskFreeSpaceEx(HardDiskLetters[iCounter],&AvailableToCaller,&Disk,&Free))
			{
				CString Temp;
//				Temp.Format("*** Hard Disk: (%s) ***\r\n",HardDiskLetters[iCounter]);
//				Result+=Temp;

//				Temp.Format("Total size: %I64d (MB)\r\n",Disk.QuadPart/1024/1024);
//				Result+=Temp;

//				ULONGLONG Used=Disk.QuadPart-Free.QuadPart;
//				Temp.Format("Used: %I64d (MB)\r\n",Used/1024/1024);
//				Result+=Temp;

//				Temp.Format("Free: %I64d (MB)\r\n\r\n",Free.QuadPart/1024/1024);
//				Result+=Temp;

				dTotalSize += (INT64)(Disk.QuadPart);
				dTotalFree += (INT64)(Free.QuadPart);

				if (bWin9X) {
					Temp.Format("Hard Disk Drive %d : %.2f GB\n", iCounter+1, dTotalSize/1024/1024/1024.0);
					Result += Temp;
				}
			}
		}
	}

	dTotalSize = dTotalSize/1024/1024/1024.0;
	dTotalFree = dTotalFree/1024/1024/1024.0;

//	CString Temp;
//	Temp.Format("Number of Hard Disk Drives: %d\n",index);
	CString strTotalSize, strTotalFree;
	strTotalSize.Format("%.2f Gigabytes Usable Hard Drive Capacity\n", dTotalSize/*fTotalSize*/);
	strTotalFree.Format("%.2f Gigabytes Hard Drive Free Space\n", dTotalFree);
	Result=strTotalSize+strTotalFree+Result;

	return Result + "\n";
}

CString CwsSysInfoUtil::GetCDROMDriveInfo()
{
	CString strDriveInfo;
	if (GetVersion() < 0x80000000) {
		// 윈NT 계열
		strDriveInfo += GetDeviceInfoWinNTSeries("SERVICE", "SYSTEM\\CURRENTCONTROLSET\\ENUM\\IDE", "CDROM", "CONTROL", "FRIENDLYNAME") + "\n";
		strDriveInfo += GetDeviceInfoWinNTSeries("SERVICE", "SYSTEM\\CURRENTCONTROLSET\\ENUM\\SCSI", "CDROM", "CONTROL", "FRIENDLYNAME") + "\n";
	} else {
		// 윈9X 계열
		strDriveInfo += GetDeviceInfoWin9XSeries("CLASS", "ENUM\\SCSI", "CDROM", "DEVICEDESC") + "\n";
	}

	strDriveInfo.TrimLeft();
//	strDriveInfo.TrimRight();

	return strDriveInfo;
}

CString CwsSysInfoUtil::GetDriveInfo()
{
	CString strDriveInfo;
	strDriveInfo = GetHddDriveInfo();
	strDriveInfo += GetCDROMDriveInfo();

	if(strDriveInfo.GetLength() == 0) {
		strDriveInfo = "Not Detected";
	}
	return strDriveInfo;
}

typedef BOOL (__stdcall *EnumDisplayDevicesEx)(PVOID,DWORD,PVOID,DWORD);
CString CwsSysInfoUtil::GetVideoInfo()
{
	EnumDisplayDevicesEx pEnumDisplayDevices;

    HINSTANCE  hInstUser32;
    DISPLAY_DEVICE DispDev; 
	DEVMODE DispDevMode;
    
    hInstUser32 = LoadLibrary("User32.DLL");
    if (!hInstUser32) return "";
    
    pEnumDisplayDevices = (EnumDisplayDevicesEx)GetProcAddress(hInstUser32,"EnumDisplayDevicesA");
    if (!pEnumDisplayDevices) {
        FreeLibrary(hInstUser32);
        return "";
    }

    ZeroMemory(&DispDev, sizeof(DISPLAY_DEVICE));
    DispDev.cb = sizeof(DISPLAY_DEVICE); 
    
	(*pEnumDisplayDevices)(NULL, 0, &DispDev, 0);

    FreeLibrary(hInstUser32);

	CString strRet = DispDev.DeviceString;
	if(strRet.GetLength() == 0) {
		strRet = "Not Detected";
	}

	return strRet;
}

CString CwsSysInfoUtil::GetDisplaySettingInfo()
{
	DEVMODE m_DevMode;
	ZeroMemory(&m_DevMode,sizeof(m_DevMode));
	::EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &m_DevMode);

	CString strRet;
	strRet.Format("%d x %d (%d bit) (%dHz)", m_DevMode.dmPelsWidth, m_DevMode.dmPelsHeight, m_DevMode.dmBitsPerPel, m_DevMode.dmDisplayFrequency);

	return strRet;
}

CString CwsSysInfoUtil::GetSoundInfo()
{
	CString strInfo;
	if (GetVersion() < 0x80000000) {
		// 윈NT 계열
		strInfo = GetDeviceInfoWinNTSeries("CLASS", "SYSTEM\\CURRENTCONTROLSET\\ENUM\\PCI", "MEDIA", "CONTROL", "DEVICEDESC");
	}else{
		// 윈9X 계열
		strInfo = GetDeviceInfoWin9XSeries("CLASS", "ENUM\\PCI", "MEDIA", "DEVICEDESC");
	}

	strInfo.TrimLeft();
	strInfo.TrimRight();

	return strInfo;
}

CString CwsSysInfoUtil::GetNetworkAdapterInfo()
{
	CString strInfo;
	if (GetVersion() < 0x80000000) {
		// 윈NT 계열
		strInfo = GetDeviceInfoWinNTSeries("CLASS", "SYSTEM\\CURRENTCONTROLSET\\ENUM\\PCI", "NET", "CONTROL", "DEVICEDESC");
	}else{
		// 윈9X 계열
		strInfo = GetDeviceInfoWin9XSeries("CLASS", "ENUM\\PCI", "NET", "DEVICEDESC");
	}

	strInfo.TrimLeft();
	strInfo.TrimRight();

	return strInfo;
}

CString CwsSysInfoUtil::GetMonitorInfo()
{
	CString strInfo;
	if (GetVersion() < 0x80000000) {
		// 윈NT 계열
		strInfo = GetDeviceInfoWinNTSeries("CLASS", "SYSTEM\\CURRENTCONTROLSET\\ENUM\\DISPLAY", "MONITOR", "CONTROL", "DEVICEDESC");
	}else{
		// 윈9X 계열
		strInfo = GetDeviceInfoWin9XSeries("CLASS", "ENUM\\MONITOR", "MONITOR", "DEVICEDESC");
	}

	strInfo.TrimLeft();
	strInfo.TrimRight();

	return strInfo;
}

CString CwsSysInfoUtil::GetMouseInfo()
{
	// Check if mouse is present or not.
	if (::GetSystemMetrics (SM_MOUSEPRESENT) == 0) return "";

	int nBtnCnt = GetSystemMetrics(SM_CMOUSEBUTTONS);

	CString strMouseInfo;
	strMouseInfo.Format("%d button mouse", nBtnCnt);

	return strMouseInfo;
}

CString CwsSysInfoUtil::GetPrinterInfo()
{
	char buffer[1025] = "";
	GetProfileString("windows", "device", NULL, buffer, 1024);

	CString defaultPrinter = buffer, port = buffer;
	int position = defaultPrinter.Find(',');
	defaultPrinter = defaultPrinter.Left(position); 
	position = port.Find(',', position + 1);
	port = port.Right(port.GetLength() - position - 1);

	if(defaultPrinter.GetLength() == 0) {
		defaultPrinter = "Not Dectedted";
	}
	
	return defaultPrinter;
	
/*	CString printerInfo = defaultPrinter + " on " + port;

	// get default printer handle
	HANDLE hPrinter = NULL;
	BOOL result = OpenPrinter(
		(LPSTR)defaultPrinter.operator const char*(),
		&hPrinter,  // pointer to printer handle
		NULL);

	if(result == 0){
		ClosePrinter(hPrinter);
		return "";
	}

	// get default printer's driver information
	DWORD returnedByte;
	buffer[0] = '\0';
	result = GetPrinterDriver(
		hPrinter,     // printer object
		NULL, // address of environment
		6,         // structure level
		(unsigned char*)buffer,   // address of structure array
		1024,          // size, in bytes, of array
		&returnedByte);    // address of variable with number of bytes 
                       // retrieved (or required)
 
	if(result == 0){
		ClosePrinter(hPrinter);
		return "";
	}

	DRIVER_INFO_3 *driveInfo = (DRIVER_INFO_3*)buffer;
	TRACE("Driver Name        : %s\n", driveInfo->pName);
	TRACE("Driver Version     : %ui\n", driveInfo->cVersion);
	TRACE("Driver Environment : %s\n", driveInfo->pEnvironment);
	TRACE("Driver path        : %s\n", driveInfo->pDriverPath);
	TRACE("Default Data Type  : %s\n", driveInfo->pDefaultDataType);

	ClosePrinter(hPrinter);
*/
}

CString CwsSysInfoUtil::GetFDDInfo()
{
    CString strFDDInfo,CDDriveName;

    char DrivesName[500];
    ::GetLogicalDriveStrings(500, DrivesName);

    char *pDriveName = DrivesName;
    while (*pDriveName)
    {
        UINT nDriveType = ::GetDriveType(pDriveName);
        if (nDriveType == DRIVE_REMOVABLE) {
            CDDriveName.Format("%s", pDriveName);
            break;
        }
        pDriveName += strlen(pDriveName)+1;
    }
    if (CDDriveName != ""){
		strFDDInfo = "Yes";
    }else{
        strFDDInfo = "No";
    }

	return strFDDInfo;
}
