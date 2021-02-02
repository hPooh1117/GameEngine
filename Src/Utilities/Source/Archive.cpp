#include "..\Archive.h"

#include "./Utilities/Log.h"
#include "./Utilities/Util.h"
#include <fstream>
#include <sstream>

uint64_t gArchiveVersion = 0;

Archive::Archive()
{
	CreateEmpty();
}

Archive::Archive(const std::string& filename, bool read_mode):mFilename(filename), mbIsReadMode(read_mode)
{
	if (!filename.empty())
	{
		mDirectory = FileOp::GetDirectoryFromPath(filename);
		if (read_mode)
		{
			if (FileOp::FileRead(filename, DATA))
			{
				(*this) >> mVersion;
				if (mVersion < gArchiveVersion)
				{
					Log::Error("The archive version (%d) is no longer supported!", mVersion);
					Close();
				}
				if (mVersion > gArchiveVersion)
				{
					Log::Error("The archive version (%d) is higher than the program's (%d)!", mVersion, gArchiveVersion);
					Close();
				}
			}
		}
		else
		{
			CreateEmpty();
		}
	}

}

void Archive::CreateEmpty()
{
	mbIsReadMode = false;
	mPos = 0;

	mVersion = gArchiveVersion;
	DATA.resize(128); // starting size
	(*this) << mVersion;
}

void Archive::SetReadModeAndResetPos(bool is_read_mode)
{
	mbIsReadMode = is_read_mode;
	mPos = 0;

	if (mbIsReadMode)
	{
		(*this) >> mVersion;
	}
	else
	{
		(*this) << mVersion;
	}
}

bool Archive::IsOpen()
{
	return !DATA.empty();
}

void Archive::Close()
{
	if (!mbIsReadMode && !mFilename.empty())
	{
		SaveFile(mFilename);
	}
	DATA.clear();
}

bool Archive::SaveFile(const std::string& filename)
{
	return FileOp::FileWrite(filename, DATA.data(), mPos);
}

const std::string& Archive::GetSourceDirectory() const
{
	return mDirectory;
}

const std::string& Archive::GetSourceFileName() const
{
	return mFilename;
}
