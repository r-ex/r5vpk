#include "pch.h"

using namespace std::chrono;

void _extractAll(VPKDir vpk, std::string baseOutput = "vpk\\") {

	// create parameters for decompression
	lzham_decompress_params decompParams{};
	decompParams.m_dict_size_log2 = 20;
	decompParams.m_decompress_flags = lzham_decompress_flags::LZHAM_DECOMP_FLAG_OUTPUT_UNBUFFERED;
	decompParams.m_struct_size = sizeof(lzham_decompress_params);

	for (int i = 0; i < vpk.archives.size(); i++) {
		fs::path vpkPath(vpk.path);

		std::string sParentPath = ".";

		// this fixes an issue when the exe is in the same folder as the vpks
		// since it will fail to find the archive vpks due to adding "\" to the start of the path
		// but with no parent path
		if (vpkPath.parent_path().u8string() != "")
			sParentPath = vpkPath.parent_path().u8string();

		std::string path = sParentPath + "\\" + vpk.archives[i];
		std::ifstream ifArchiveStream(path, std::ios_base::binary);

		for (auto& block : vpk.entblocks) {
			// skip all blocks that aren't in the current archive
			// there is definitely a better way of doing this but
			// this works so it doesn't matter right now
			if (block.aIndex != i)
				continue;
			else {
				fs::path p(baseOutput + block.path);

				std::string fPath = p.u8string();				// get string from path

				fs::create_directories(p.parent_path());		// create all directories in path

				std::ofstream ofOutputFileStream(fPath, std::ios_base::binary | std::ios_base::out);

				if (!ofOutputFileStream.is_open())
				{
					g_Logger.Debug("Output file was not open when checked at line %i:\n", __LINE__);

					g_Logger.Error("%s didn't open properly. Skipping file\n", fPath.c_str());
					continue;
				}

				ofOutputFileStream.clear(); // make sure file is empty before writing

				for (auto& ent : block.ents) {

					char* compressedDataBuf = new char[ent.cSize];

					memset(compressedDataBuf, 0, ent.cSize);

					ifArchiveStream.seekg(ent.offset); // seek to entry offset in archive
					ifArchiveStream.read(compressedDataBuf, ent.cSize); // read compressed data from archive

					lzham_uint8* lzOut = new lzham_uint8[ent.uSize];

					if (ent.bIsCompressed)
					{
						g_Logger.Debug("[ENTRY] =======================================\n");
						g_Logger.Debug("[ENTRY] block name:\n", block.path.c_str());
						g_Logger.Debug("[ENTRY] sizes:\n");
						g_Logger.Debug("[ENTRY]       uncompressed: %lld\n", ent.uSize);
						g_Logger.Debug("[ENTRY]       compressed:   %lld\n", ent.cSize);

						lzham_uint32 adler32 = 0;
						lzham_decompress_status_t lzStatus = lzham_decompress_memory(&decompParams, lzOut, (size_t*)&ent.uSize, (lzham_uint8*)compressedDataBuf, ent.cSize, &adler32);
						if (lzStatus != lzham_decompress_status_t::LZHAM_DECOMP_STATUS_SUCCESS)
						{
							g_Logger.Error("failed decompression for an entry within block %s (archive %i)\n", block.path.c_str(), i);
							g_Logger.Error("lzStatus %i\n", lzStatus);
						}
						else {
							// if successfully decompressed, write to file
							ofOutputFileStream.write((char*)lzOut, ent.uSize);
						}
					}
					else {
						// if not compressed, write raw data into output file
						ofOutputFileStream.write(compressedDataBuf, ent.cSize);
					}

					delete[] lzOut, compressedDataBuf;
				}
				ofOutputFileStream.close();
			}
		}
		ifArchiveStream.close();
	}
}

float Extractor::ExtractAll(VPKDir vpk, std::string baseOutput="") {

	milliseconds msStart = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
		);

	std::thread th(&_extractAll, vpk, baseOutput);

	th.join();

	milliseconds msEnd = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
		);

	float duration = msEnd.count() - msStart.count();

	g_Logger.Info("|-> extraction took %.3f seconds\n", duration / 1000);
	//printf("[r5vpk] |-> extraction took %.3f seconds\n", duration / 1000);

	return duration;
}
