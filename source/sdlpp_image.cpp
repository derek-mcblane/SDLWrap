#include "sdlpp_image.h"

#include "sdlpp.h"

#include <string>
#include <string_view>

namespace sdl::image {

[[nodiscard]] SurfaceUniquePtr load_image(const std::string& filename)
{
    SurfaceUniquePtr image{IMG_Load(filename.c_str())};
    if (image == nullptr) {
        throw exception::load_image{};
    }
    return image;
}

[[nodiscard]] static SurfaceUniquePtr load_sized_svg_rw(SDL_RWops *source, int width, int height)
{
    SurfaceUniquePtr image{IMG_LoadSizedSVG_RW(source, width, height)};
    if (image == nullptr) {
        throw exception::load_image{};
    }
    return image;
}

[[nodiscard]] SurfaceUniquePtr load_sized_svg(const std::string& filename, int width, int height)
{
    return load_sized_svg_rw(SDL_RWFromFile(filename.c_str(), "r"), width, height);
}

} // namespace sdl::image
