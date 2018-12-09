#ifndef __TOOL_SERIAL_PORT_HPP__
#define __TOOL_SERIAL_PORT_HPP__

#include <string>
#include <vector>
#include <map>
#include <thread>
#include <functional>
#include <chrono>
#include <tuple>
#include <algorithm>

#include <Windows.h>
#include <devguid.h>
#include <SetupAPI.h>

#include "../3rdparty/serial/serial.h"



class tool_SerialPort {
public:
	static std::vector<std::string> get_list () {
		std::vector<std::string> v;
		HDEVINFO hDevInfo = ::SetupDiGetClassDevsA (&GUID_DEVCLASS_PORTS, NULL, NULL, 0);
		if (hDevInfo != INVALID_HANDLE_VALUE) {
			SP_DEVINFO_DATA dd = { sizeof (SP_DEVINFO_DATA) };
			for (DWORD dw = 0; ::SetupDiEnumDeviceInfo (hDevInfo, dw, &dd); dw++) {
				char tbuf[MAX_PATH] = { 0 };
				if (::SetupDiGetDeviceRegistryPropertyA (hDevInfo, &dd, SPDRP_FRIENDLYNAME, NULL, (PBYTE) tbuf, sizeof (tbuf), NULL))
					v.push_back (tbuf);
			}
			::SetupDiDestroyDeviceInfoList (hDevInfo);
		}
		std::sort (v.begin (), v.end (), [] (std::string s1, std::string s2) {
			size_t p1 = s1.find ('('), p2 = s2.find ('(');
			return atoi (&s1[p1 + 4]) < atoi (&s2[p2 + 4]);
		});
		return v;
	}

	tool_SerialPort () {
		m_thread = std::thread (&tool_SerialPort::thread_func, this);
	}
	virtual ~tool_SerialPort () {
		m_exit = true;
		m_thread.join ();
		close ();
	}

	bool open (std::string name, uint32_t baud_rate = 9600, uint8_t byte_size = 8, std::string parity = "none", std::string stopbits = "1") {
		static std::map<std::string, serial::parity_t> map_parity = { { "none", serial::parity_none }, { "odd", serial::parity_odd }, { "even", serial::parity_even }, { "mark", serial::parity_mark }, { "space", serial::parity_space } };
		static std::map<std::string, serial::stopbits_t> map_stopbits = { { "1", serial::stopbits_one }, { "1.5", serial::stopbits_one_point_five }, { "2", serial::stopbits_two } };
		static serial::Timeout timeout = serial::Timeout::simpleTimeout (100);

		close ();
		m_serial.setTimeout (timeout);
		m_serial.setPort (name);
		m_serial.setBaudrate (baud_rate);
		m_serial.setBytesize ((serial::bytesize_t) byte_size);
		m_serial.setParity (map_parity[parity]);
		m_serial.setStopbits (map_stopbits[stopbits]);
		m_serial.open ();
		return (m_is_open = m_serial.isOpen ());
	}
	void close () {
		if (m_serial.isOpen ())
			m_serial.close ();
		m_is_open = false;
	}
	size_t write (std::string data) {
		if (!m_serial.isOpen ())
			return 0;
		return m_serial.write (data);
	}

	void set_on_event (std::function<void (std::string)> on_receive, std::function<void ()> on_close) {
		m_on_receive = on_receive;
		m_on_close = on_close;
	}
	bool is_open () { return m_serial.isOpen (); }
	std::string get_name () { return m_serial.getPort (); }

protected:
	void thread_func () {
		while (!m_exit) {
			if (m_serial.isOpen ()) {
				size_t sz_old = 0, sz = m_serial.available ();
				if (sz > 0) {
					while (sz > sz_old) {
						if (m_exit)
							return;
						std::this_thread::sleep_for (std::chrono::milliseconds (100));
						sz_old = sz;
						sz = m_serial.available ();
					}
					if (m_on_receive)
						m_on_receive (m_serial.read (sz));
				}
			} else if (m_is_open) {
				if (m_on_close)
					m_on_close ();
				m_is_open = false;
			}
			std::this_thread::sleep_for (std::chrono::milliseconds (100));
			//::Sleep (4000);
			//m_on_receive ("来自串口的消息。。。");
		}
	}

protected:
	serial::Serial m_serial;
	bool m_is_open = false;
	bool m_exit = false;
	std::function<void (std::string)> m_on_receive = nullptr;
	std::function<void ()> m_on_close = nullptr;
	std::thread m_thread;
};

#endif //__TOOL_SERIAL_PORT_HPP__
