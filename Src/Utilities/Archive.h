#pragma once
#include "./Utilities/CommonInclude.h"
#include <vector>
#include <string>

class Archive
{
private:
	uint64_t mVersion = 0;
	bool mbIsReadMode = false;
	size_t mPos = 0;
	std::vector<uint8_t> DATA;

	std::string mFilename;
	std::string mDirectory;

	void CreateEmpty();

public:
	Archive();
	Archive(const Archive&) = default;
	Archive(Archive&&) = default;
	Archive(const std::string& filename, bool read_mode = true);
	~Archive() { Close(); }

	Archive& operator=(const Archive&) = default;
	Archive& operator=(Archive&&) = default;

	const uint8_t* GetData() const { return DATA.data(); }
	size_t GetSize() const { return mPos; }
	uint64_t GetVersion() const { return mVersion; }
	bool IsReadMode() const { return mbIsReadMode; }
	void SetReadModeAndResetPos(bool is_read_mode);
	bool IsOpen();
	void Close();
	bool SaveFile(const std::string & filename);
	const std::string& GetSourceDirectory() const;
	const std::string& GetSourceFileName() const;

	//
	// ‘‚«‚İ‘€ì
	//
	inline Archive& operator<<(bool data)
	{
		Write((uint32_t)(data ? 1 : 0));
		return *this;
	}
	inline Archive& operator<<(char data)
	{
		Write((int8_t)data);
		return *this;
	}
	inline Archive& operator<<(unsigned char data)
	{
		Write((uint8_t)data);
		return *this;
	}
	inline Archive& operator<<(int data)
	{
		Write((int64_t)data);
		return *this;
	}
	inline Archive& operator<<(unsigned int data)
	{
		Write((uint64_t)data);
		return *this;
	}
	inline Archive& operator<<(long data)
	{
		Write((int64_t)data);
		return *this;
	}
	inline Archive& operator<<(unsigned long data)
	{
		Write((uint64_t)data);
		return *this;
	}
	inline Archive& operator<<(long long data)
	{
		Write((int64_t)data);
		return *this;
	}
	inline Archive& operator<<(unsigned long long data)
	{
		Write((uint64_t)data);
		return *this;
	}
	inline Archive& operator<<(float data)
	{
		Write(data);
		return *this;
	}
	inline Archive& operator<<(double data)
	{
		Write(data);
		return *this;
	}
	inline Archive& operator<<(const XMFLOAT2& data)
	{
		Write(data);
		return *this;
	}
	inline Archive& operator<<(const XMFLOAT3& data)
	{
		Write(data);
		return *this;
	}
	inline Archive& operator<<(const XMFLOAT4& data)
	{
		Write(data);
		return *this;
	}
	inline Archive& operator<<(const XMFLOAT3X3& data)
	{
		Write(data);
		return *this;
	}
	inline Archive& operator<<(const XMFLOAT4X3& data)
	{
		Write(data);
		return *this;
	}
	inline Archive& operator<<(const XMFLOAT4X4& data)
	{
		Write(data);
		return *this;
	}
	inline Archive& operator<<(const XMUINT2& data)
	{
		Write(data);
		return *this;
	}
	inline Archive& operator<<(const XMUINT3& data)
	{
		Write(data);
		return *this;
	}
	inline Archive& operator<<(const XMUINT4& data)
	{
		Write(data);
		return *this;
	}
	inline Archive& operator<<(const std::string& data)
	{
		uint64_t len = (uint64_t)(data.length() + 1); // +1 for the null-terminator
		Write(len);
		Write(*data.c_str(), len);
		return *this;
	}
	template<typename T>
	inline Archive& operator<<(const std::vector<T>& data)
	{
		(*this) << data.size();
		for (const T& x : data)
		{
			(*this) << x;
		}
		return *this;
	}

	//
	// “Ç‚İ‚İ‘€ì
	//
	inline Archive& operator >> (bool& data)
	{
		uint32_t temp;
		Read(temp);
		data = (temp == 1);
		return *this;
	}
	inline Archive& operator >> (char& data)
	{
		int8_t temp;
		Read(temp);
		data = (char)temp;
		return *this;
	}
	inline Archive& operator >> (unsigned char& data)
	{
		uint8_t temp;
		Read(temp);
		data = (unsigned char)temp;
		return *this;
	}
	inline Archive& operator >> (int& data)
	{
		int64_t temp;
		Read(temp);
		data = (int)temp;
		return *this;
	}
	inline Archive& operator >> (unsigned int& data)
	{
		uint64_t temp;
		Read(temp);
		data = (unsigned int)temp;
		return *this;
	}
	inline Archive& operator >> (long& data)
	{
		int64_t temp;
		Read(temp);
		data = (long)temp;
		return *this;
	}
	inline Archive& operator >> (unsigned long& data)
	{
		uint64_t temp;
		Read(temp);
		data = (unsigned long)temp;
		return *this;
	}
	inline Archive& operator >> (long long& data)
	{
		int64_t temp;
		Read(temp);
		data = (long long)temp;
		return *this;
	}
	inline Archive& operator >> (unsigned long long& data)
	{
		uint64_t temp;
		Read(temp);
		data = (unsigned long long)temp;
		return *this;
	}
	inline Archive& operator >> (float& data)
	{
		Read(data);
		return *this;
	}
	inline Archive& operator >> (double& data)
	{
		Read(data);
		return *this;
	}
	inline Archive& operator >> (XMFLOAT2& data)
	{
		Read(data);
		return *this;
	}
	inline Archive& operator >> (XMFLOAT3& data)
	{
		Read(data);
		return *this;
	}
	inline Archive& operator >> (XMFLOAT4& data)
	{
		Read(data);
		return *this;
	}
	inline Archive& operator >> (XMFLOAT3X3& data)
	{
		Read(data);
		return *this;
	}
	inline Archive& operator >> (XMFLOAT4X3& data)
	{
		Read(data);
		return *this;
	}
	inline Archive& operator >> (XMFLOAT4X4& data)
	{
		Read(data);
		return *this;
	}
	inline Archive& operator >> (XMUINT2& data)
	{
		Read(data);
		return *this;
	}
	inline Archive& operator >> (XMUINT3& data)
	{
		Read(data);
		return *this;
	}
	inline Archive& operator >> (XMUINT4& data)
	{
		Read(data);
		return *this;
	}
	inline Archive& operator >> (std::string& data)
	{
		uint64_t len;
		Read(len);
		char* str = new char[(size_t)len];
		memset(str, '\0', (size_t)(sizeof(char) * len));
		Read(*str, len);
		data = std::string(str);
		delete[] str;
		return *this;
	}
	template<typename T>
	inline Archive& operator >> (std::vector<T>& data)
	{
		size_t count;
		(*this) >> count;
		data.resize(count);
		for (size_t i = 0; i < count; ++i)
		{
			(*this) >> data[i];
		}
		return *this;
	}



private:

	// ƒƒ‚ƒŠ‘€ì‚É‚æ‚é‘‚«‚İ
	template<typename T>
	inline void Write(const T& data, uint64_t count = 1)
	{
		size_t size = (size_t)(sizeof(data) * count);
		size_t right = mPos + size;
		if (right > DATA.size())
		{
			DATA.resize(right * 2);
		}
		memcpy(reinterpret_cast<void*>((uint64_t)DATA.data() + (uint64_t)mPos), &data, size);
		mPos = right;
	}

	// ƒƒ‚ƒŠ‘€ì‚É‚æ‚é“Ç‚İ‚İ
	template<typename T>
	inline void Read(T& data, uint64_t count = 1)
	{
		memcpy(&data, reinterpret_cast<void*>((uint64_t)DATA.data() + (uint64_t)mPos), (size_t)(sizeof(data) * count));
		mPos += (size_t)(sizeof(data) * count);
	}

};