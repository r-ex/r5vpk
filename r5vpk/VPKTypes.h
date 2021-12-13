#pragma once
#include "pch.h"

constexpr unsigned int VPK_DIR_MAGIC = 1437209140;

class VPKEntry
{
public:
	unsigned int entFlags; // entry flags
	unsigned short texFlags; // texture flags (only used if the entry is a vtf)
	uint64_t offset; // offset in archive
	uint64_t cSize; // compressed size of entry
	uint64_t uSize; // uncompressed size of entry
	bool bIsCompressed;

	VPKEntry(BinaryIO* reader) {
		this->entFlags = reader->read<unsigned int>();
		this->texFlags = reader->read<unsigned short>();
		this->offset = reader->read<uint64_t>();
		this->cSize = reader->read<uint64_t>();
		this->uSize = reader->read<uint64_t>();
		this->bIsCompressed = (this->cSize != this->uSize);
	}

	VPKEntry(unsigned int nEntFlags, unsigned short nTexFlags, uint64_t nOffset, uint64_t nCompressedSize, uint64_t nUncompressedSize)
	{
		this->entFlags = nEntFlags;
		this->texFlags = nTexFlags;
		this->offset = nOffset;
		this->cSize = nCompressedSize;
		this->uSize = nUncompressedSize;
		this->bIsCompressed = nCompressedSize != nUncompressedSize;
	}

	void Write(BinaryIO* writer)
	{
		writer->write(this->entFlags);
		writer->write(this->texFlags);
		writer->write(this->offset);
		writer->write(this->cSize);
		writer->write(this->uSize);
	}
};

class VPKEntryBlock
{
public:
	unsigned int crc; // crc32 for the uncompressed block
	unsigned __int16 pb; // preload bytes
	unsigned __int16 aIndex; // index of the archive that contains this block

	std::vector<VPKEntry> ents; // vector of all the block's entries (entries have a size limit of 1 MiB, so anything over is split into separate entries within the same block)
	std::string path; // path to block within vpk

	VPKEntryBlock(BinaryIO* reader, std::string path) {

		std::replace(path.begin(), path.end(), '/', '\\'); // try to replace a forward slash with the windows backslash path separator

		this->path = path; // set path of block
		this->crc = reader->read<uint32_t>(); // read crc
		this->pb = reader->read<uint16_t>(); // read pb
		this->aIndex = reader->read<uint16_t>(); // read archive index

		// put all file entries into the vector
		do {
			VPKEntry entry(reader);
			this->ents.push_back(entry);
		} while (reader->read<uint16_t>() != 65535);
	}

	VPKEntryBlock(uint32_t crc, uint16_t nPreloadBytes, uint16_t nArchiveIndex)
	{
		this->crc = crc;
		this->pb = nPreloadBytes;
		this->aIndex = nArchiveIndex;
	}

	void AddEntry(VPKEntry entry)
	{
		this->ents.push_back(entry);
	}

	void Write(BinaryIO* writer)
	{
		writer->write(this->crc);
		writer->write(this->pb);
		writer->write(this->aIndex);

		if (this->ents.size() == 0)
		{
			g_Logger.Warning("Entry block does not contain any entries");
		}

		for (int i = 0; i < this->ents.size(); ++i)
		{
			this->ents[i].Write(writer);
		}

		uint16_t nEndMarker = 65535;
		writer->write(nEndMarker);
	}
};
class VPKDir
{
public:
	uint32_t magic; // file magic
	uint16_t majorVer; // vpk major version
	uint16_t minorVer; // vpk minor version
	uint32_t tSize; // tree size
	uint32_t fdSize; // 
	std::vector<VPKEntryBlock> entblocks; // vector of entry blocks
	uint16_t hArchiveIndex = 0; // highest archive index (archive count-1)
	std::vector<std::string> archives; // vector of archive file names
	std::string path; // path to vpkDir file

	VPKDir(std::string path) {
		BinaryIO reader;
		reader.open(path, BinaryIOMode::BinaryIOMode_Read);
		this->magic = reader.read<uint32_t>();
		if (this->magic != VPK_DIR_MAGIC) {
			std::cout << "file is not a supported VPK directory" << std::endl;
			std::exit(1);
		}
		this->majorVer = reader.read<uint16_t>();
		this->minorVer = reader.read<uint16_t>();
		this->tSize = reader.read<uint32_t>();
		this->fdSize = reader.read<uint32_t>();
		this->entblocks = getEntryBlocks(&reader);
		this->path = path; // set path to VPK dir file
		for (auto block : this->entblocks) {
			if (block.aIndex > this->hArchiveIndex) {
				this->hArchiveIndex = block.aIndex;
			}
		}
		for (int i = 0; i < this->hArchiveIndex + 1; i++) {
			std::string archivePath = this->getArchivePath(path, i);

#if _DEBUG
			std::cout << archivePath << std::endl;
#endif
			this->archives.push_back(archivePath);
		}
	}

	std::string getArchivePath(std::string dirPath, int aIndex) {
		std::string unlocalized = utils::stripLocalization(dirPath);
		std::ostringstream os;
		os << std::setw(3) << std::setfill('0') << aIndex;
		std::string archiveId = "pak000_" + os.str();
		utils::replace(unlocalized, "pak000_dir", archiveId);
		return unlocalized;
	}

	std::string getBlockPath(std::string extension, std::string path, std::string name) {
		if (path == " ") path = "";
		if (!path.empty()) path += "\\";
		return path + name + "." + extension;
	}

	std::vector<VPKEntryBlock> getEntryBlocks(BinaryIO* reader) {
		std::string extension, path, name;
		std::vector<VPKEntryBlock> blocks;
		while (!(extension = reader->readString()).empty()) {
			while (!(path = reader->readString()).empty()) {
				while (!(name = reader->readString()).empty()) {
					std::string fPath = getBlockPath(extension, path, name);
					blocks.push_back(VPKEntryBlock(reader, fPath));
				}
			}
		}
		return blocks;
	}

};

