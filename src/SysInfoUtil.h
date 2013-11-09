// SysInfoUtil.h
//

CString GetOS();
CString GetCPUCount();
CString GetCPUInfo();
CString GetCPUName();
CString GetTotalRAM();
CString GetHddDriveInfo();
CString GetCDROMDriveInfo();
CString GetDriveInfo();
CString GetVideoInfo();
CString GetDisplaySettingInfo();
CString GetDeviceInfoWinNTSeries(LPSTR lpNTKeyword, LPSTR lpNTSubKey, LPSTR lpNTService, LPSTR lpNTVerifyName, LPSTR lpNTDesc);
CString GetDeviceInfoWin9XSeries(LPSTR lpWin9xKeyword, LPSTR lpWin9xSubKey, LPSTR lpWin9xService, LPSTR lpWin9xDesc);
BOOL IsDynamicDeviceOnWin9x(char *szRegPath);
CString GetSoundInfo();
CString GetNetworkAdapterInfo();
CString GetMonitorInfo();
CString GetMouseInfo();
CString GetPrinterInfo();
CString GetFDDInfo();
