#include "pch.h"

using namespace std::chrono;

int main(int argc, char** argv)
{
    args::ArgumentParser p("r5 vpk extractor");
    args::Positional<std::string> vpkPath(p, "path", "path to vpk file", { "path" });

    // Args Group: Flags
    args::Group agFlags(p, "flags", args::Group::Validators::DontCare, args::Options::Global);
    args::HelpFlag afHelp(agFlags, "help", "help", { 'h', "help" });
    args::Flag afNoPause(agFlags, "nopause", "nopause", { "np", "nopause" });

    // Args Group: Arguments
    args::Group agArguments(p, "arguments", args::Group::Validators::DontCare, args::Options::Global);
    args::ValueFlag<std::string> argOutputPath(agArguments, "", "base path for extractor output", { "output" });
   
    try
    {
        p.ParseCLI(argc, argv);
    }
    catch(args::Help) {
        std::cout << p;
    }
    catch (args::Error& e) {
        std::cerr << e.what() << std::endl << p;
        return 1;
    }
    std::string path = args::get(vpkPath);

    // if no vpk path
    if (path == vpkPath.Name()) {
        std::cout << p;
        return 0;
    }

    g_Logger.DisplayStartupMessage();

    std::string sOutputPath = "vpk\\";

    if (!args::get(argOutputPath).empty())
    {
        sOutputPath = args::get(argOutputPath);
        char lchar = sOutputPath[sOutputPath.size() - 1];
        if (lchar != '\\' && lchar != '/')
            sOutputPath += "\\";
    }

    if (fs::is_directory(path))
    {
        milliseconds msStart = duration_cast<milliseconds>(system_clock::now().time_since_epoch()); //utils::GetTimeInMilliseconds();

        for (const auto& entry : fs::directory_iterator(path))
        {
            std::string epath = entry.path().string();

            if (utils::endsWith(epath, "_dir.vpk"))
            {
                g_Logger.Info("extracting %s\n", epath.c_str());
                Extractor::ExtractAll(VPKDir(epath), sOutputPath);
            }
        }

        milliseconds msEnd = duration_cast<milliseconds>(system_clock::now().time_since_epoch()); //utils::GetTimeInMilliseconds();

        float duration = (msEnd.count() - msStart.count());

        g_Logger.Info("extraction of the directory took %.3f seconds\n", duration / 1000);
    }
    else {

        std::regex archiveRegex("pak000_([0-9]{3})");
        std::smatch regexMatches;
        std::regex_search(path, regexMatches, archiveRegex);
        if (regexMatches.size() != 0) {
            utils::replace(path, regexMatches[0], "pak000_dir");
            utils::replace(path, "client", "englishclient");
        }

        g_Logger.Info("extracting %s\n", path.c_str());

        VPKDir vpk(path);

        Extractor::ExtractAll(vpk, sOutputPath);
    }

    if (!afNoPause)
    {
        g_Logger.Debug("nopause is not set. pausing...\n");
        system("pause");
    }
    return 0;

}

