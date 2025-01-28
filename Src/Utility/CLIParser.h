#include <string>
#include "spdlog/spdlog.h"
#include "../Exception/ArgsException.h"

struct CLIParser {
	void Parse(int argc, char* argv[]) {
		int i{1};
		while (i < argc) {
			if (std::string{ "--scene" }.compare(argv[i]) == 0) {
				sceneGeometryPath = std::string{ argv[++i] };
				sceneMaterialPath = std::string{ argv[++i] };
			}
			else if (std::string{ "--save" }.compare(argv[i]) == 0) {
				savePath = std::string{ argv[++i] };
			}
			else if (std::string{ "--depth" }.compare(argv[i]) == 0) {
				depth = std::stoi(argv[++i]);
			}
			else if (std::string{ "--spp" }.compare(argv[i]) == 0) {
				spp = std::stoi(argv[++i]);
			}
			else if (std::string{ "--gui" }.compare(argv[i]) == 0) {
				withGUI = true;
			}
			else if (std::string{ "--integrator" }.compare(argv[i]) == 0) {
				integrator = std::string{ argv[++i] };
			}
			else {
				throw ArgsException{argv[i]};
			}
			++i;
		}
	}

	void Usage() {
		spdlog::trace("CLI parser: --scene --save --depth --spp --gui");
	}

	std::string sceneGeometryPath;
	std::string sceneMaterialPath;
	std::string integrator{ "IndirectIlluminationIntegrator" };
	std::string savePath{ "../Renders/image.jpeg" };
	int spp = 64;
	int depth = 4;
	bool withGUI = false;
};