#include "sdlpp_image.h"

#include "sdlpp.h"

#include <string>
#include <string_view>

namespace sdl::image {

SurfaceUniquePtr load_image(const std::string& filename)
{
    SurfaceUniquePtr image{IMG_Load(filename.c_str())};
    if (image == nullptr) {
        throw generic_error{};
    }
    return image;
}

SurfaceUniquePtr load_sized_svg_rw(SDL_RWops* source, const int width, const int height)
{
    SurfaceUniquePtr image{IMG_LoadSizedSVG_RW(source, width, height)};
    if (image == nullptr) {
        throw generic_error{};
    }
    return image;
}

SurfaceUniquePtr load_sized_svg_rw(SDL_RWops* source, const Point<int> size)
{
    return load_sized_svg_rw(source, size.x, size.y);
}

SurfaceUniquePtr load_sized_svg(const std::string& filename, const int width, const int height)
{
    return load_sized_svg_rw(rw_from_file(filename, "r").get(), width, height);
}

SurfaceUniquePtr load_sized_svg(const std::string& filename, const Point<int> size)
{
    return load_sized_svg(filename, size.x, size.y);
}

} // namespace sdl::image
