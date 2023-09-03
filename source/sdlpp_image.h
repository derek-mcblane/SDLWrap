#pragma once

#include "sdlpp.h"

#include "SDL_image.h"

#include <stdexcept>

namespace sdl::image {

class generic_error : virtual public std::runtime_error
{
  public:
    [[nodiscard]] generic_error() : std::runtime_error(IMG_GetError()) {}
};

namespace InitFlags {
static constexpr int none = 0;
static constexpr int jpg = IMG_INIT_JPG;
static constexpr int png = IMG_INIT_PNG;
static constexpr int tif = IMG_INIT_TIF;
static constexpr int webp = IMG_INIT_WEBP;
static constexpr int jxl = IMG_INIT_JXL;
static constexpr int avif = IMG_INIT_AVIF;
}; // namespace InitFlags

inline void initialize(int flags)
{
    if ((IMG_Init(flags) & flags) == 0) {
        throw generic_error{};
    }
}

inline void quit()
{
    IMG_Quit();
}

[[nodiscard]] SurfaceUniquePtr load_image(const std::string& filename);
[[nodiscard]] SurfaceUniquePtr load_sized_svg_rw(SDL_RWops* source, int width, int height);
[[nodiscard]] SurfaceUniquePtr load_sized_svg_rw(SDL_RWops* source, Point<int> size);
[[nodiscard]] SurfaceUniquePtr load_sized_svg(const std::string& filename, int width, int height);
[[nodiscard]] SurfaceUniquePtr load_sized_svg(const std::string& filename, Point<int> size);

} // namespace sdl::image
