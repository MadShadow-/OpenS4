
#include "Gfx.hpp"

#include <filesystem>
#include <map>
#include <string>

#include "Image.hpp"
#include "IndexList.hpp"
#include "Palette.hpp"

namespace OpenS4::Import {

std::shared_ptr<IGraphics> read_gfx_without_jil(std::string path) {
    auto make_reader = [](std::string path, std::string extension) {
        return std::make_shared<OpenS4::Import::Reader>(path + extension);
    };

    auto gfx_reader = make_reader(path, ".gfx");
    auto gil_reader = make_reader(path, ".gil");
    auto pal_reader = make_reader(path, ".pa6");
    auto pil_reader = make_reader(path, ".pil");

    if (pal_reader->size() == 0) {
        pal_reader = make_reader(path, ".p26");
        pil_reader = make_reader(path, ".pi2");
    }

    std::shared_ptr<GraphicsIndexList> gil =
        std::make_shared<GraphicsIndexList>(gil_reader);
    std::shared_ptr<PaletteIndexList> pil =
        std::make_shared<PaletteIndexList>(pil_reader);
    std::shared_ptr<PaletteCollection> pal =
        std::make_shared<PaletteCollection>(pal_reader, pil);
    std::shared_ptr<IGraphics> gfx =
        std::make_shared<GraphicsLazy>(gfx_reader, gil, pal);

    return gfx;
}

std::shared_ptr<IGraphics> read_gfx_with_jil(std::string path) {
    auto make_reader = [](std::string path, std::string extension) {
        return std::make_shared<Reader>(path + extension);
    };

    auto gfx_reader = make_reader(path, ".gfx");
    auto gil_reader = make_reader(path, ".gil");
    auto pal_reader = make_reader(path, ".p26");
    auto pil_reader = make_reader(path, ".pi2");

    auto jil_reader = make_reader(path, ".jil");
    auto dil_reader = make_reader(path, ".dil");

    std::shared_ptr<GraphicsIndexList> gil =
        std::make_shared<GraphicsIndexList>(gil_reader);
    std::shared_ptr<PaletteIndexList> pil =
        std::make_shared<PaletteIndexList>(pil_reader);
    std::shared_ptr<PaletteCollection> pal =
        std::make_shared<PaletteCollection>(pal_reader, pil);

    std::shared_ptr<JobIndexList> jil =
        std::make_shared<JobIndexList>(jil_reader);
    std::shared_ptr<DirectionIndexList> dil =
        std::make_shared<DirectionIndexList>(dil_reader);

    std::shared_ptr<IGraphics> gfx =
        std::make_shared<GraphicsLazy>(gfx_reader, gil, pal, jil, dil);

    return gfx;
}

std::shared_ptr<BackgroundGraphics> read_gh6(std::string path) {
    auto make_reader = [](std::string path, std::string extension) {
        return std::make_shared<Reader>(path + extension);
    };

    auto gh_reader = make_reader(path, ".gh6");

    std::shared_ptr<BackgroundGraphics> gh =
        std::make_shared<BackgroundGraphics>(gh_reader);

    return gh;
}

GraphicsRegistry getGraphicsRegistry(std::string path) {
    GraphicsRegistry registry;

    for (int i = 0; i <= 50; i++) {
        std::string n_path = path + "/" + std::to_string(i);

        if (std::filesystem::exists(n_path + ".gfx")) {
            auto gfx = std::filesystem::exists(n_path + ".jil")
                           ? read_gfx_with_jil(n_path)
                           : read_gfx_without_jil(n_path);
            registry.addGFX(i, gfx);
        } else if (std::filesystem::exists(n_path + ".gh5")) {
            auto gh = read_gh6(n_path);
            registry.addGH(i, gh);
        }
    }

    return registry;
}
}  // namespace OpenS4::Graphics

#ifdef _WIN32
#include <Windows.h>
namespace OpenS4::Import {
std::string getGraphicsPathByRegistry() {
    char buffer[256] = {0};
    DWORD buffer_size = 256;
    auto rc = RegGetValueA(
        HKEY_LOCAL_MACHINE,
        "SOFTWARE\\WOW6432Node\\Ubisoft\\Launcher\\Installs\\11785",
        "InstallDir", RRF_RT_REG_SZ, nullptr, buffer, &buffer_size);

    GraphicsRegistry registry;

    if (rc != ERROR_SUCCESS) {
        return "";
    }

    std::string path = buffer + std::string("Gfx");

    return path;
}
}  // namespace OpenS4::Graphics
#else
namespace OpenS4::GraphicsReader {
std::string getGraphicsPathByRegistry() { return ""; }
}  // namespace OpenS4::GraphicsReader
#endif  // _WIN32
