#pragma once

#include "sdlpp.h"

#include "SDL_image.h"

#include <stdexcept>

namespace sdl::image {

namespace exception {

class generic_error : virtual public std::runtime_error
{
  public:
    [[nodiscard]] generic_error() : std::runtime_error(IMG_GetError()) {}

    [[nodiscard]] generic_error(const generic_error& other) noexcept = default;
    generic_error& operator=(const generic_error& other) noexcept = default;

    [[nodiscard]] generic_error(generic_error&& other) noexcept = default;
    generic_error& operator=(generic_error&& other) noexcept = default;

    ~generic_error() noexcept override = default;

    [[nodiscard]] static const char* error() noexcept
    {
        return IMG_GetError();
    }
};

class initialize_extension : virtual public std::runtime_error
{
  public:
    [[nodiscard]] initialize_extension() : std::runtime_error(IMG_GetError()) {}
};

class load_image : virtual public std::runtime_error
{
  public:
    [[nodiscard]] load_image() : std::runtime_error(IMG_GetError()) {}
};

} // namespace exception

enum class InitFlags
{
    none = 0,
    jpg = IMG_INIT_JPG,
    png = IMG_INIT_PNG,
    tif = IMG_INIT_TIF,
    webp = IMG_INIT_WEBP,
    jxl = IMG_INIT_JXL,
    avif = IMG_INIT_AVIF,
};

inline void initialize(int flags)
{
    if ((IMG_Init(flags) & flags) == 0) {
        throw exception::initialize_extension{};
    }
}

inline void initialize(InitFlags flags)
{
    initialize(static_cast<int>(flags));
}

inline void quit()
{
    IMG_Quit();
}

[[nodiscard]] SurfaceUniquePtr load_image(const std::string& filename);
[[nodiscard]] SurfaceUniquePtr load_sized_svg(const std::string& filename, int width, int height);

} // namespace sdl::image
