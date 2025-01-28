#include "spdlog/spdlog.h"
#include "Utility/CLIParser.h"
#include "Renderer.h"

int main(int argc, char* argv[]) {
	spdlog::set_level(spdlog::level::trace);
	CLIParser cliParser;
	try {
		cliParser.Parse(argc, argv);
		Scene scene{ cliParser.sceneGeometryPath, cliParser.sceneMaterialPath };
		Renderer renderer{ Integrator::CreateFromName(cliParser.integrator, scene, cliParser.depth) };
		renderer.Render(cliParser.spp, cliParser.withGUI, cliParser.savePath);
	}
	catch (const ArgsException& e) {
		spdlog::error("CLI parser: Unkown argument {}", e.what());
		cliParser.Usage();
	}
	return 0;
}