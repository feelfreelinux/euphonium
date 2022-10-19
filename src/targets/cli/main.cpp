#include "Core.h"
#include "FileScriptLoader.h"
#include "PortAudioAudioOutput.h"
#include <EuphoniumLog.h>
#include <optional>
#include "PlatformConstants.h"

const std::string SCRIPTS_PREFIX_PATH = "../../../euphonium/scripts/";

int main(int argc, char *argv[]) {
    initializeEuphoniumLogger();
    bell::createDecoders();

    // auto core = std::make_shared<Core>();
    // auto loader = std::make_shared<FileScriptLoader>();
    // auto output = std::make_shared<PortAudioAudioOutput>();
    // core->selectAudioOutput(output);
    // core->setupBindings();
    // core->loadPlugins(loader);
    // core->handleScriptingThread();
    return 0;
}