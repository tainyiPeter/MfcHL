#include "Aes.h"

#pragma comment(lib, "Bcrypt.lib")


// 生成随机字节（用于密钥和 IV）
std::vector<BYTE> Aes::generate_random_bytes(size_t length)
{
	std::vector<BYTE> buffer(length);
	BCryptGenRandom(
		nullptr,
		buffer.data(),
		(ULONG)buffer.size(),
		BCRYPT_USE_SYSTEM_PREFERRED_RNG
	);
	return buffer;
}

// AES-CBC 加密（返回加密后的数据）
std::vector<BYTE> Aes::encrypt_aes_cbc(
	const std::vector<BYTE>& plaintext,
	const std::vector<BYTE>& key,
	const std::vector<BYTE>& iv
) 
{
	BCRYPT_ALG_HANDLE hAlgorithm = nullptr;
	BCRYPT_KEY_HANDLE hKey = nullptr;
	NTSTATUS status = 0;
	DWORD cipherBlockLength = 0;
	DWORD resultLength = 0;

	// 1. 打开 AES 算法提供程序
	status = BCryptOpenAlgorithmProvider(
		&hAlgorithm,
		BCRYPT_AES_ALGORITHM,
		nullptr,
		0
	);
	if (status != 0) throw std::runtime_error("BCryptOpenAlgorithmProvider failed");

	// 2. 设置 CBC 模式
	status = BCryptSetProperty(
		hAlgorithm,
		BCRYPT_CHAINING_MODE,
		(PBYTE)BCRYPT_CHAIN_MODE_CBC,
		sizeof(BCRYPT_CHAIN_MODE_CBC),
		0
	);
	if (status != 0) throw std::runtime_error("BCryptSetProperty failed");

	// 3. 生成对称密钥
	status = BCryptGenerateSymmetricKey(
		hAlgorithm,
		&hKey,
		nullptr,
		0,
		(PBYTE)key.data(),
		(ULONG)key.size(),
		0
	);
	if (status != 0) throw std::runtime_error("BCryptGenerateSymmetricKey failed");

	// 4. 获取块长度（AES 块大小为 16 字节）
	ULONG blockLength = 0;
	DWORD cbData = 0;
	status = BCryptGetProperty(
		hAlgorithm,
		BCRYPT_BLOCK_LENGTH,
		(PBYTE)&blockLength,
		sizeof(blockLength),
		&cbData,
		0
	);
	if (status != 0) throw std::runtime_error("BCryptGetProperty failed");

	// 5. 加密
	std::vector<BYTE> ciphertext(plaintext.size() + blockLength); // 预留填充空间
	status = BCryptEncrypt(
		hKey,
		(PBYTE)plaintext.data(),
		(ULONG)plaintext.size(),
		nullptr,
		(PBYTE)iv.data(),
		(ULONG)iv.size(),
		ciphertext.data(),
		(ULONG)ciphertext.size(),
		&resultLength,
		BCRYPT_BLOCK_PADDING
	);
	if (status != 0) throw std::runtime_error("BCryptEncrypt failed");

	ciphertext.resize(resultLength); // 调整到实际加密长度

	// 6. 清理资源
	BCryptDestroyKey(hKey);
	BCryptCloseAlgorithmProvider(hAlgorithm, 0);

	return ciphertext;
}

// AES-CBC 解密（返回解密后的数据）
std::vector<BYTE> Aes::decrypt_aes_cbc(
	const std::vector<BYTE>& ciphertext,
	const std::vector<BYTE>& key,
	const std::vector<BYTE>& iv
) 
{
	BCRYPT_ALG_HANDLE hAlgorithm = nullptr;
	BCRYPT_KEY_HANDLE hKey = nullptr;
	NTSTATUS status = 0;
	DWORD resultLength = 0;

	// 1. 打开 AES 算法提供程序
	status = BCryptOpenAlgorithmProvider(
		&hAlgorithm,
		BCRYPT_AES_ALGORITHM,
		nullptr,
		0
	);
	if (status != 0) throw std::runtime_error("BCryptOpenAlgorithmProvider failed");

	// 2. 设置 CBC 模式
	status = BCryptSetProperty(
		hAlgorithm,
		BCRYPT_CHAINING_MODE,
		(PBYTE)BCRYPT_CHAIN_MODE_CBC,
		sizeof(BCRYPT_CHAIN_MODE_CBC),
		0
	);
	if (status != 0) throw std::runtime_error("BCryptSetProperty failed");

	// 3. 生成对称密钥
	status = BCryptGenerateSymmetricKey(
		hAlgorithm,
		&hKey,
		nullptr,
		0,
		(PBYTE)key.data(),
		(ULONG)key.size(),
		0
	);
	if (status != 0) throw std::runtime_error("BCryptGenerateSymmetricKey failed");

	// 4. 解密
	std::vector<BYTE> plaintext(ciphertext.size());
	status = BCryptDecrypt(
		hKey,
		(PBYTE)ciphertext.data(),
		(ULONG)ciphertext.size(),
		nullptr,
		(PBYTE)iv.data(),
		(ULONG)iv.size(),
		plaintext.data(),
		(ULONG)plaintext.size(),
		&resultLength,
		BCRYPT_BLOCK_PADDING
	);
	if (status != 0) throw std::runtime_error("BCryptDecrypt failed");

	plaintext.resize(resultLength); // 调整到实际解密长度

	// 5. 清理资源
	BCryptDestroyKey(hKey);
	BCryptCloseAlgorithmProvider(hAlgorithm, 0);

	return plaintext;
}
