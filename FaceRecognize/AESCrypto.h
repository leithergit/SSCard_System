#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#ifdef _WIN32
#include <windows.h>
#endif

class AESCrypto {
private:
	// SHA1PRNG 实现 - 两次 SHA1
	std::string nextBytes(int numBytes, std::string state) {
		std::string output;

		while (output.size() < (size_t)numBytes) {
			unsigned char hash[SHA_DIGEST_LENGTH];

			// 第一次 SHA1
			SHA1(reinterpret_cast<const unsigned char*>(state.data()),
				state.size(), hash);
			state.assign(reinterpret_cast<char*>(hash), SHA_DIGEST_LENGTH);

			// 第二次 SHA1
			SHA1(reinterpret_cast<const unsigned char*>(state.data()),
				state.size(), hash);
			state.assign(reinterpret_cast<char*>(hash), SHA_DIGEST_LENGTH);

			output += state;
		}

		return output.substr(0, numBytes);
	}

	void printHex(const std::string& label, const unsigned char* data, size_t len) {
		std::cout << label;
		for (size_t i = 0; i < len; i++) {
			printf("%02X", data[i]);
		}
		std::cout << std::endl;
	}

	std::string toHex(const std::vector<unsigned char>& data) {
		std::stringstream ss;
		ss << std::hex << std::setfill('0') << std::uppercase;
		for (unsigned char byte : data) {
			ss << std::setw(2) << static_cast<int>(byte);
		}
		return ss.str();
	}

	std::string toHex(const std::string& data) {
		std::stringstream ss;
		ss << std::hex << std::setfill('0') << std::uppercase;
		for (unsigned char byte : data) {
			ss << std::setw(2) << static_cast<int>(byte);
		}
		return ss.str();
	}

	std::vector<unsigned char> fromHex(const std::string& hex) {
		std::vector<unsigned char> bytes;
		for (size_t i = 0; i < hex.length(); i += 2) {
			std::string byteString = hex.substr(i, 2);
			unsigned char byte = static_cast<unsigned char>(
				std::strtol(byteString.c_str(), nullptr, 16));
			bytes.push_back(byte);
		}
		return bytes;
	}

public:
	// AES 加密 - 输入字符串，输出十六进制
	std::string Encrypt(const std::string& aeskey, const std::string& plaintext) {
		if (aeskey.length() != 24) {
			throw std::runtime_error("Key must be 24 chars");
		}

		// 使用 SHA1PRNG 生成 16 字节密钥
		std::string keyStr = nextBytes(16, aeskey);

		// 初始化 AES ECB 加密
		EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
		if (!ctx) throw std::runtime_error("Failed to create context");

		const unsigned char* key = reinterpret_cast<const unsigned char*>(keyStr.data());

		if (EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), nullptr, key, nullptr) != 1) {
			EVP_CIPHER_CTX_free(ctx);
			throw std::runtime_error("Failed to init encryption");
		}

		EVP_CIPHER_CTX_set_padding(ctx, 1);

		// 加密
		std::vector<unsigned char> ciphertext(plaintext.length() + AES_BLOCK_SIZE);
		int len = 0, ciphertext_len = 0;

		if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len,
			reinterpret_cast<const unsigned char*>(plaintext.c_str()),
			plaintext.length()) != 1) {
			EVP_CIPHER_CTX_free(ctx);
			throw std::runtime_error("Encryption failed");
		}
		ciphertext_len = len;

		if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
			EVP_CIPHER_CTX_free(ctx);
			throw std::runtime_error("Encryption final failed");
		}
		ciphertext_len += len;
		EVP_CIPHER_CTX_free(ctx);

		ciphertext.resize(ciphertext_len);
		return toHex(ciphertext);
	}

	// AES 解密 - 输入十六进制，输出字符串
	std::string Decrypt(const std::string& aeskey, const std::string& hexCiphertext) {
		if (aeskey.length() != 24) {
			throw std::runtime_error("Key must be 24 chars");
		}

		// 使用 SHA1PRNG 生成相同的密钥
		std::string keyStr = nextBytes(16, aeskey);

		// 转换十六进制密文为字节
		std::vector<unsigned char> ciphertext = fromHex(hexCiphertext);

		// 初始化 AES ECB 解密
		EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
		if (!ctx) throw std::runtime_error("Failed to create context");

		const unsigned char* key = reinterpret_cast<const unsigned char*>(keyStr.data());

		if (EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), nullptr, key, nullptr) != 1) {
			EVP_CIPHER_CTX_free(ctx);
			throw std::runtime_error("Failed to init decryption");
		}

		EVP_CIPHER_CTX_set_padding(ctx, 1);

		// 解密
		std::vector<unsigned char> plaintext(ciphertext.size() + AES_BLOCK_SIZE);
		int len = 0, plaintext_len = 0;

		if (EVP_DecryptUpdate(ctx, plaintext.data(), &len,
			ciphertext.data(), ciphertext.size()) != 1) {
			EVP_CIPHER_CTX_free(ctx);
			throw std::runtime_error("Decryption failed");
		}
		plaintext_len = len;

		if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
			EVP_CIPHER_CTX_free(ctx);
			throw std::runtime_error("Decryption final failed");
		}
		plaintext_len += len;
		EVP_CIPHER_CTX_free(ctx);

		plaintext.resize(plaintext_len);
		return std::string(reinterpret_cast<char*>(plaintext.data()), plaintext_len);
	}
};