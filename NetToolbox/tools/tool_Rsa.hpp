#ifndef __TOOL_RSA_HPP__
#define __TOOL_RSA_HPP__

#include <string>
#include <Windows.h>
#include <openssl/pem.h>



class tool_Rsa {
public:
	// 生成公钥私钥对
	static bool generate (int bits, faw::string_t file_pubkey, faw::string_t file_rsapubkey, faw::string_t file_rsaprvkey) {
		bool bExist = false;
		faw::string_t info = _IT (_T ("The following file already exists. Do you want to overwrite it?"));
		if (faw::Directory::exist (file_pubkey)) {
			bExist = true;
			info += _T ("\n");
			info += file_pubkey;
		}
		if (faw::Directory::exist (file_rsapubkey)) {
			bExist = true;
			info += _T ("\n");
			info += file_rsapubkey;
		}
		if (faw::Directory::exist (file_rsaprvkey)) {
			bExist = true;
			info += _T ("\n");
			info += file_rsaprvkey;
		}
		if (bExist) {
			if (IDOK != ::MessageBox (NULL, info.data (), _IT (_T ("Info")).data (), MB_ICONQUESTION | MB_OKCANCEL))
				return false;
		}

		RSA *r = ::RSA_new ();
		BIGNUM *bn = ::BN_new ();
		::BN_set_word (bn, 65537);
		::RSA_generate_key_ex (r, 2048, bn, nullptr);
		bool bSuccess = true;
		//
		std::string _file_pubkey = faw::Encoding::T_to_gb18030 (file_pubkey);
		int a = 0, b = 0, c = 0;
		if (!_file_pubkey.empty ()) {
			BIO *bio = ::BIO_new_file (_file_pubkey.data (), "w+");
			bSuccess &= !!::PEM_write_bio_RSA_PUBKEY (bio, r);
			::BIO_free (bio);
		}
		std::string _file_rsapubkey = faw::Encoding::T_to_gb18030 (file_rsapubkey);
		if (!_file_rsapubkey.empty ()) {
			BIO *bio = ::BIO_new_file (_file_rsapubkey.data (), "w+");
			bSuccess &= !!::PEM_write_bio_RSAPublicKey (bio, r);
			::BIO_free (bio);
		}
		std::string _file_rsaprvkey = faw::Encoding::T_to_gb18030 (file_rsaprvkey);
		if (!_file_rsaprvkey.empty ()) {
			BIO *bio = ::BIO_new_file (_file_rsaprvkey.data (), "w+");
			bSuccess &= !!::PEM_write_bio_RSAPrivateKey (bio, r, nullptr, nullptr, 0, nullptr, nullptr);
			::BIO_free (bio);
		}
		::BN_free (bn);
		::RSA_free (r);
		return bSuccess;
	}
};

#endif //__TOOL_RSA_HPP__
