////////////////////////////////////////////////////////////////////////////////
//
// Class Name:  tool_Gzip
// Description: Gzip压缩工具类
// Class URI:   https://github.com/fawdlstty/NetToolbox
// Author:      Fawdlstty
// Author URI:  https://www.fawdlstty.com/
// License:     此文件单独授权 以MIT方式开源共享
// Last Update: Dec 19, 2018
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __TOOL_GZIP_HPP__
#define __TOOL_GZIP_HPP__

#include <zlib.h>
#include <string>
#include <string_view>

#define CHUNK 16384
#define windowBits 15
#define GZIP_ENCODING 16



class tool_Gzip {
public:
	// gzip压缩数据
	static std::string compress (std::string_view data, int level = 9) {
		std::string ret = "";
		unsigned char out[CHUNK];
		z_stream strm;
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		if (deflateInit2 (&strm, level, Z_DEFLATED, windowBits | GZIP_ENCODING, 8, Z_DEFAULT_STRATEGY) != Z_OK)
			return ret;
		strm.next_in = (unsigned char*) data.data ();
		strm.avail_in = (uInt) data.length ();
		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;
			if (deflate (&strm, Z_FINISH) == Z_STREAM_ERROR)
				return ret;
			size_t have = CHUNK - strm.avail_out;
			ret.append ((char*) out, have);
		} while (strm.avail_out == 0);
		if (deflateEnd (&strm) != Z_OK)
			return ret;
		return ret;
	}

	// gzip解压数据
	static std::string decompress (std::string_view data) {
		std::string ret = "";
		z_stream strm;
		unsigned char out[CHUNK];
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		strm.avail_in = 0;
		strm.next_in = Z_NULL;
		if (inflateInit2 (&strm, 16 + MAX_WBITS) != Z_OK)
			return ret;
		strm.avail_in = (uInt) data.length ();
		strm.next_in = (unsigned char*) data.data ();
		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;
			int iret = inflate (&strm, Z_NO_FLUSH);
			switch (iret) {
			case Z_NEED_DICT:
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				inflateEnd (&strm);
				return ret;
			}
			size_t have = CHUNK - strm.avail_out;
			ret.append ((char*) out, have);
		} while (strm.avail_out == 0);
		if (inflateEnd (&strm) != Z_OK)
			return ret;
		return ret;
	}
};

#endif //__TOOL_GZIP_HPP__
