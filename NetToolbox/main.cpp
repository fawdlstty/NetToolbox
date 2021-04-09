#include "StdAfx.h"
#include "NetToolboxWnd.h"

#include <fstream>
#include <chrono>
#include <thread>
#include <memory>

#include <nlohmann/json.hpp>
using Json = nlohmann::json;

#include <openssl/md5.h>

#include "tools/tool_Process.hpp"
#include "tools/tool_PE.hpp"
#include "tools/tool_Mutex.hpp"
#include "tools/tool_Priv.hpp"
#include "tools/tool_WebRequest.hpp"
#include "tools/tool_WMI.hpp"

#include "Settings.hpp"

// ref: NetToolboxWnd
#if (defined _UNICODE) && (defined _DEBUG) && (defined _WIN64)
#	pragma comment (lib, "../lib/DuiLib_64sd.lib")
#elif (defined _UNICODE) && (defined _DEBUG) && (!defined _WIN64)
#	pragma comment (lib, "../lib/DuiLib_sd.lib")
#elif (defined _UNICODE) && (!defined _DEBUG) && (defined _WIN64)
#	pragma comment (lib, "../lib/DuiLib_64s.lib")
#elif (defined _UNICODE) && (!defined _DEBUG) && (!defined _WIN64)
#	pragma comment (lib, "../lib/DuiLib_s.lib")
#elif (!defined _UNICODE) && (defined _DEBUG) && (defined _WIN64)
#	pragma comment (lib, "../lib/DuiLibA_64sd.lib")
#elif (!defined _UNICODE) && (defined _DEBUG) && (!defined _WIN64)
#	pragma comment (lib, "../lib/DuiLibA_sd.lib")
#elif (!defined _UNICODE) && (!defined _DEBUG) && (defined _WIN64)
#	pragma comment (lib, "../lib/DuiLibA_64s.lib")
#elif (!defined _UNICODE) && (!defined _DEBUG) && (!defined _WIN64)
#	pragma comment (lib, "../lib/DuiLibA_s.lib")
#endif

#pragma comment (lib, "Crypt32.lib")



// 获取文件md5
std::string _get_file_md5 (LPCTSTR path) {
	HANDLE hFile = ::CreateFile (path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return "";
	DWORD dwsz_high = 0;
	DWORD dwsz = ::GetFileSize (hFile, &dwsz_high);
	::SetFilePointer (hFile, 0, nullptr, FILE_BEGIN);
	int64_t file_length = dwsz | (((int64_t) dwsz_high) * 0x100000000);
	constexpr size_t sz_1M = 1024 * 1024;
	int8_t *buf = new int8_t[sz_1M];
	size_t i, block_count = (size_t) (file_length / sz_1M);
	size_t last_size = (size_t) (file_length - (sz_1M * (int64_t) block_count));
	//
	// 文件Hash
	//
	MD5_CTX _md5;
	::MD5_Init (&_md5);
	//
	for (i = 0; i < block_count; ++i) {
		::ReadFile (hFile, buf, sz_1M, &dwsz, nullptr);
		::MD5_Update (&_md5, buf, sz_1M);
	}
	if (last_size > 0) {
		::ReadFile (hFile, buf, (DWORD) last_size, &dwsz, nullptr);
		::MD5_Update (&_md5, buf, last_size);
	}
	//
	unsigned char buf_md5[16] = { 0 };
	::MD5_Final (buf_md5, &_md5);
	//
	std::string str_md5 = "";
	for (i = 0; i < 64; ++i) {
		if (i < sizeof (buf_md5)) str_md5 += tool_StringA::byte_to_str (buf_md5[i]);
	}
	//
	delete[] buf;
	::CloseHandle (hFile);
	return str_md5;
}

// 应用程序初始化类
class ProgramGuard {
public:
	ProgramGuard () {
		WSAData wd;
		if (::WSAStartup (MAKEWORD (2, 2), &wd)) {
			::MessageBox (NULL, _T ("WSAStartup失败，程序将退出！"), International::translate (_T ("Info")).data (), MB_ICONHAND);
		} else if (FAILED (::CoInitializeEx (NULL, COINIT_APARTMENTTHREADED))) {
			::MessageBox (NULL, _T ("COM+初始化失败，程序将退出！"), International::translate (_T ("Info")).data (), MB_ICONHAND);
			::WSACleanup ();
		} else if (FAILED (::CoInitializeSecurity (NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL))) {
			::MessageBox (NULL, _T ("COM+安全信息设置失败，程序将退出！"), International::translate (_T ("Info")).data (), MB_ICONHAND);
			::CoUninitialize ();
			::WSACleanup ();
		} else {
			//::curl_global_init (NULL);
			tool_Priv::adjust_debug ();

			is_succeed = true;
		}
	}
	~ProgramGuard () {
		if (is_succeed) {
			//::curl_global_cleanup ();
			::CoUninitialize ();
			::WSACleanup ();
		}
	}
	bool		is_succeed = false;
};

int WINAPI _tWinMain (HINSTANCE hInstance, HINSTANCE, LPTSTR, int nCmdShow) {
	// 初始化软件环境
	ProgramGuard pg;
	if (!pg.is_succeed)
		return 0;
	faw::string_t path = faw::Directory::get_current_path ().str ();
	faw::string_t _src = path + _T ("NetToolbox.exe"), _srcd = path + _T ("res.dll");
	auto ver_src = tool_PE::get_version (_src.data ());

	// 加载配置文件
	Settings::init ();

	// 判断版本 & 更新控制
#ifndef _DEBUG
	// 比较版本
	auto _cmp_ver = [] (std::tuple<size_t, size_t, size_t, size_t> a, std::tuple<size_t, size_t, size_t, size_t> b) -> int {
		auto[a1, a2, a3, a4] = a;
		auto[b1, b2, b3, b4] = b;
		if (a1 > b1) { return 1; } else if (b1 > a1) { return -1; }
		if (a2 > b2) { return 1; } else if (b2 > a2) { return -1; }
		if (a3 > b3) { return 1; } else if (b3 > a3) { return -1; }
		if (a4 > b4) { return 1; } else if (b4 > a4) { return -1; }
		return 0;
	};

	// 解析版本
	auto _parse_ver = [] (std::string s) -> std::tuple<size_t, size_t, size_t, size_t> {
		std::vector<std::string> v;
		tool_StringA::split (s, v, '.');
		while (v.size () < 4)
			v.push_back ("0");
		return { stoi (v[0]), stoi (v[1]), stoi (v[2]), stoi (v[3]) };
	};

	// 更新控制
	faw::string_t _new = path + _T ("NetToolbox.new.exe"), _newd = path + _T ("res.new.dll"), _oldd = path + _T ("res.dll");
	decltype (ver_src) ver_new = { 0, 0, 0, 0 };
	bool exist_new = faw::Directory::exist (_new.data ());
	if (!exist_new) {
		// 检查新版
		std::thread ([=] () {
			try {
				std::string url_base = "https://nettoolbox.fawdlstty.com/";
				std::string data = tool_WebRequest::get (url_base + "info.json");
				Json o = Json::parse (data);
				std::string ver_srv = o["version"];
				if (_parse_ver (ver_srv) > ver_src) {
					tool_Mutex m (_T ("nettoolbox_checknew"));
					if (!m.try_lock ())
						return;
					for (auto &item : o["files"]) {
						std::string _file = item["name"];
						std::string _md5 = item["md5"];
						std::string _local_file = path.stra () + (_file == "NetToolbox.exe" ? "NetToolbox.new.exe" : _file);
						if (_get_file_md5 (faw::Encoding::gb18030_to_T (_local_file).data ()) != _md5) {
							std::string _data = tool_WebRequest::get (url_base + _file);
							if (!_data.empty ()) {
								MD5_CTX ctx_md5;
								::MD5_Init (&ctx_md5);
								::MD5_Update (&ctx_md5, &_data[0], _data.size ());
								unsigned char buf_md5[16] = { 0 };
								::MD5_Final (buf_md5, &ctx_md5);
								std::string str_md5 = "";
								for (size_t i = 0; i < sizeof (buf_md5); ++i)
									str_md5 += tool_StringA::byte_to_str (buf_md5[i]);
								if (tool_StringA::is_equal_nocase (str_md5, _md5)) {
									// 修复release下可能直接创建文件的问题
									[_local_file, _data] () {
										std::ofstream ofs (_local_file, std::ios::trunc | std::ios::binary);
										ofs.write (_data.data (), _data.size ());
										ofs.close ();
									} ();
								} else {
									if (faw::Directory::exist (_new))
										::DeleteFile (_new.data ());
									if (faw::Directory::exist (_newd))
										::DeleteFile (_newd.data ());
									if (IDOK == ::MessageBox (NULL, _T ("更新失败，是否尝试手动更新？"), _T ("易大师网络工具箱"), MB_ICONQUESTION | MB_OKCANCEL))
										::ShellExecute (NULL, _T ("open"), _T ("https://nettoolbox.fawdlstty.com/NetToolbox.7z"), NULL, NULL, SW_SHOW);
									return;
								}
							}
						}
					}
					::MessageBox (NULL, _T ("更新已完成，软件将在下次启动时完成更新。"), _T ("易大师网络工具箱"), MB_ICONINFORMATION);
				}
			} catch (...) {
			}
		}).detach ();
	} else {
		ver_new = tool_PE::get_version (_new.stra ().data ());
		if (faw::Directory::get_filename (faw::Directory::get_current_file ()) == _T ("NetToolbox.exe")) {
			// 检查版本号是否等于新文件，如果相等，则等待新进程退出并删除新文件，否则创建新进程并结束自身
			if (_cmp_ver (ver_src, ver_new) == 0) {
				while (tool_Process::process_exist (_T ("NetToolbox.new.exe")))
					std::this_thread::sleep_for (std::chrono::milliseconds (100));
				if (faw::Directory::exist (_new))
					::DeleteFile (_new.data ());
				if (faw::Directory::exist (_newd))
					::DeleteFile (_newd.data ());
				if (faw::Directory::exist (_oldd))
					::DeleteFile (_oldd.data ());
			} else {
				tool_Process::create_process (_new.str_view ());
				return 0;
			}
		} else {
			// 检查版本号是否等于主程序，如果相等，则退出自身，否则等主程序退出并将自身复制给主程序
			if (_cmp_ver (ver_src, ver_new) != 0) {
				while (tool_Process::process_exist (_T ("NetToolbox.exe")))
					std::this_thread::sleep_for (std::chrono::milliseconds (100));
				::CopyFile (_new.data (), _src.data (), FALSE);
				if (faw::Directory::exist (_newd.data ()))
					::CopyFile (_newd.data (), _srcd.data (), FALSE);
			}
			tool_Process::create_process (_src.str_view ());
			return 0;
		}
	}
#endif

	// 初始化路径
	CPaintManagerUI::SetInstance (hInstance);
#ifdef _DEBUG
	CPaintManagerUI::SetResourcePath (_T ("..\\res"));
#else
	CPaintManagerUI::SetResourceType (UILIB_ZIPRESOURCE);
	HRSRC hResource = ::FindResource (hInstance, MAKEINTRESOURCE (IDR_ZIPRES1), _T ("ZIPRES"));
	if (hResource != NULL) {
		DWORD dwSize = 0;
		HGLOBAL hGlobal = ::LoadResource (hInstance, hResource);
		if (hGlobal) {
			DWORD dwSize = ::SizeofResource (hInstance, hResource);
			if (dwSize) {
				CPaintManagerUI::SetResourceZip ((LPBYTE)::LockResource (hGlobal), dwSize);
				CResourceManager::GetInstance ()->LoadResource (_T ("res.xml"), _T (""));
			}
		}
		::FreeResource (hResource);
	}
#endif

	// 计算编译器版本
	auto[v1, v2, v3, v4] = ver_src;
	std::vector<LPCTSTR> publish { _T ("Alpha"), _T ("Beta"), _T ("Gamma"), _T ("RC"), _T ("GA") };
	faw::string_t str_publish = _T ("Community");
	//_T ("Community"), _T ("Personal"), _T ("Professional"), _T ("Enterprise"), _T ("Ultimate")
	faw::string_t _caption2 = fmt::format (_T ("{:04}.{:02}.{:02}　{}"), v1, v2, v3, str_publish);

	// 创建窗口
	NetToolboxWnd wnd (_caption2);
	wnd.Create (NULL, _T ("易大师网络工具箱"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	wnd.CenterWindow ();
	wnd.ShowModal ();
	return 0;
}
