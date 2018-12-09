#ifndef __TOOL_BASE64_HPP__
#define __TOOL_BASE64_HPP__

#include <iostream>
#include <string>
#include <string_view>



class tool_Base64 {
public:
	// ÅÐ¶ÏÄÚÈÝÊÇ·ñÎªbase64±àÂë
	static bool is_base64 (const unsigned char *data, int length) {
		if (length % 4) {
			return false;
		} else if (length == 0) {
			return true;
		}
		if (data[length - 1] == '=')
			--length;
		if (data[length - 1] == '=')
			--length;
		for (int i = 0; i < length; ++i) {
			char ch = data[i];
			if (!(isalnum (ch) || (ch == '+') || (ch == '/')))
				return false;
		}
		return true;
	}

	// base64±àÂë
	static std::string base64_encode (unsigned char const* bytes_to_encode, unsigned int in_len) {
		std::string ret;
		int i = 0, j = 0;
		unsigned char char_3[3], char_4[4];
		while (in_len--) {
			char_3[i++] = *(bytes_to_encode++);
			if (i == 3) {
				char_4[0] = (char_3[0] & 0xfc) >> 2;
				char_4[1] = ((char_3[0] & 0x03) << 4) + ((char_3[1] & 0xf0) >> 4);
				char_4[2] = ((char_3[1] & 0x0f) << 2) + ((char_3[2] & 0xc0) >> 6);
				char_4[3] = char_3[2] & 0x3f;

				for (i = 0; i < 4; i++)
					ret += base64_chars[char_4[i]];
				i = 0;
			}
		}
		if (i) {
			for (j = i; j < 3; j++)
				char_3[j] = '\0';
			char_4[0] = (char_3[0] & 0xfc) >> 2;
			char_4[1] = ((char_3[0] & 0x03) << 4) + ((char_3[1] & 0xf0) >> 4);
			char_4[2] = ((char_3[1] & 0x0f) << 2) + ((char_3[2] & 0xc0) >> 6);
			for (j = 0; j < i + 1; j++)
				ret += base64_chars[char_4[j]];
			while ((i++ < 3))
				ret += '=';
		}
		return ret;
	}

	// base64½âÂë
	static std::string base64_decode (std::string_view encoded_string) {
		int in_len = encoded_string.size (), i = 0, j = 0, in_ = 0;
		unsigned char char_4[4], char_3[3];
		std::string ret;
		auto is_base64 = [] (unsigned char c) { return (isalnum (c) || (c == '+') || (c == '/')); };
		while (in_len-- && (encoded_string[in_] != '=') && is_base64 (encoded_string[in_])) {
			char_4[i++] = encoded_string[in_]; in_++;
			if (i == 4) {
				for (i = 0; i < 4; i++)
					char_4[i] = (unsigned char) base64_chars.find (char_4[i]);
				char_3[0] = (char_4[0] << 2) + ((char_4[1] & 0x30) >> 4);
				char_3[1] = ((char_4[1] & 0xf) << 4) + ((char_4[2] & 0x3c) >> 2);
				char_3[2] = ((char_4[2] & 0x3) << 6) + char_4[3];

				for (i = 0; i < 3; i++)
					ret += char_3[i];
				i = 0;
			}
		}
		if (i) {
			for (j = 0; j < i; j++)
				char_4[j] = (unsigned char) base64_chars.find (char_4[j]);
			char_3[0] = (char_4[0] << 2) + ((char_4[1] & 0x30) >> 4);
			char_3[1] = ((char_4[1] & 0xf) << 4) + ((char_4[2] & 0x3c) >> 2);
			for (j = 0; j < i - 1; j++) ret += char_3[j];
		}
		return ret;
	}

private:
	static const std::string base64_chars;
};

inline const std::string tool_Base64::base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#endif //__TOOL_BASE64_HPP__
