#pragma once
#pragma once
#include <windows.h>

#include <comdef.h>
#include <wbemidl.h>
#include <atlbase.h>
#include <atlcom.h>
#include <string>
#include <map>
using namespace std;

#define CHECKHR(x) \
	if ( FAILED((x))) break; \

#define CHECKWMIHR(x) \
	if ( (x) != WBEM_S_NO_ERROR ) break; \

using namespace std;
typedef void(__stdcall* AccessValueProc)(wstring strName, VARIANT, DWORD_PTR pParam);
enum WMIClass
{
	Win32_Processor,                        // CPU 处理器
	Win32_PhysicalMemory,                   // 物理内存
	Win32_Keyboard,                         // 键盘
	Win32_PointingDevice,                   // 点输入设备，如鼠标
	Win32_DiskDrive,                        // 硬盘驱动器
	Win32_CDROMDrive,                       // 光盘驱动器
	Win32_BaseBoard,                        // 主板
	Win32_BIOS,                             // BIOS 芯片
	Win32_ParallelPort,                     // 并口
	Win32_SerialPort,                       // 串口
	Win32_SoundDevice,						// 多媒体设置
	Win32_USBController,					// USB 控制器
	Win32_NetworkAdapter,					// 网络适配器
	Win32_NetworkAdapterConfiguration,		// 网络适配器设置
	Win32_Printer,							// 打印机
	Win32_PrinterConfiguration,				// 打印机设置
	Win32_PrintJob,							// 打印机任务
	Win32_TCPIPPrinterPort,                 // 打印机端口
	Win32_POTSModem,						// MODEM
	Win32_POTSModemToSerialPort,			// MODEM 端口
	Win32_DesktopMonitor,					// 显示器
	Win32_VideoController,					// 显卡细节。
	Win32_VideoSettings,                    // 显卡支持的显示模式。
	Win32_TimeZone,							// 时区
	Win32_SystemDriver,						// 驱动程序
	Win32_DiskPartition,					// 磁盘分区
	Win32_LogicalDisk,						// 逻辑磁盘
	Win32_LogicalMemoryConfiguration,		// 逻辑内存配置
	Win32_PageFile,							// 系统页文件信息
	Win32_PageFileSetting,					// 页文件设置
	Win32_BootConfiguration,				// 系统启动配置
	Win32_OperatingSystem,					// 操作系统信息
	Win32_StartupCommand,					// 系统自动启动程序
	Win32_Service,							// 系统安装的服务
	Win32_Group,							// 系统管理组
	Win32_GroupUser,						// 系统组帐号
	Win32_UserAccount,						// 用户帐号
	Win32_Process,							// 系统进程
	Win32_Thread,		                    // 系统线程
	Win32_Share,                            // 共享
	Win32_NetworkClient,                    // 已安装的网络客户端
	Win32_NetworkProtocol					// 已安装的网络协议
};
class CWMIQuery
{
public:
	CWMIQuery();
	CWMIQuery(const wchar_t* strWMINameSpaces);
	virtual ~CWMIQuery();
public:
	static HRESULT Initialize();
	HRESULT ConnectWMI(const wchar_t* strWMINameSpaces = L"");
	HRESULT Query(const wchar_t* strWQL, AccessValueProc pCallBack, DWORD_PTR pUserPtr);
	VOID SetNamespace(const wchar_t* wstrNamespace);
	HRESULT ProcessIWbemClassObject(CComPtr<IWbemClassObject> pclsObj);
	HRESULT ProcessItem(/*CComVariant &vtClass,*/CComBSTR& bstrName, CComVariant& Value, CIMTYPE type, LONG lFlavor);
	HRESULT ProcessUnknownItem(/*CComVariant& vtClass,*/CComBSTR& bstrName, CComVariant& Value, CIMTYPE type, LONG lFlavor);
	VOID SaveItem(CComBSTR bstrName, CComVariant Value, CIMTYPE type, LONG lFlavor);
	map<wstring, map<wstring, wstring>> m_mapValue;
private:
	AccessValueProc m_pAccessValue;
	DWORD_PTR m_dwUserPtr;
	wchar_t m_wstrNamespace[256];
	CComPtr<IWbemLocator> m_pWbemLocator;
	CComPtr<IWbemServices> m_pWbemService;
	bool m_bUnInitialize;
};
