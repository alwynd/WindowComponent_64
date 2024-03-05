#include "windowComponent_64.h"

FileStream::FileStream()
{
	ZeroMemory(this, sizeof(FileStream));
	file = new wifstream();
}

/** Opens the file. */
bool FileStream::openFile(LPWSTR p_FileName)
{
	m_FileName = p_FileName;
	fileSize = determineFileSize(m_FileName);

	if (fileSize < 1) return false;

	file->open(m_FileName, ios::in | ios::binary);	//open file
	if (file->bad() || file->fail())					//check good
	{
		return false;
	}

	file->seekg(0, ios::beg);						//Position to start

	return true;
}


/** 
	Reads from start to end into buffer. 
	The buffer is expected to be of correct size.
**/
bool FileStream::readFile(LPWSTR buffer, INT64 start, INT64 end)
{
	if (fileSize < 1) return false;
	if (!file->is_open()) return false;

	if (start < 0) start = 0;
	if (start > fileSize) start = fileSize;

	if (end < 0) end = 0;
	if (end > fileSize) end = fileSize;

	if (start > end) start = end;

	readLen = end-start;
	if (readLen < 1) return false;

	file->seekg(start);
	file->read(buffer, readLen);

	return (!file->bad() && !file->fail());

}

/** Closes the file. */
bool FileStream::closeFile()
{
	if (file->is_open())
		file->close();
	return true;
}


FileStream::~FileStream()
{
	closeFile();
	SAFE_DEL(file);
	ZeroMemory(this, sizeof(FileStream));
}
