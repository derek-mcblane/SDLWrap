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

namespace InitFlags
{
    static constexpr int None = 0;
    static constexpr int JPG = IMG_INIT_JPG;
    static constexpr int PNG = IMG_INIT_PNG;
    static constexpr int TIF = IMG_INIT_TIF;
    static constexpr int WebP = IMG_INIT_WEBP;
    static constexpr int JXL = IMG_INIT_JXL;
    static constexpr int AVIF = IMG_INIT_AVIF;
};

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
[[nodiscard]] SurfaceUniquePtr load_sized_svg(const std::string& filename, int width, int height);

} // namespace sdl::image
