#pragma once

#include <SDL.h>

#include <gsl/gsl>

#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <thread>

namespace sdl {

class GenericError : virtual public std::runtime_error
{
  public:
    [[nodiscard]] GenericError() : std::runtime_error(SDL_GetError()) {}
};

using Event = SDL_Event;
using EventType = SDL_EventType;
using EventFilterCallback = int (*)(void* userdata, Event* event);

void add_event_watch(EventFilterCallback callback, void* user_data) noexcept;
void set_event_filter(EventFilterCallback callback, void* user_data) noexcept;
bool get_event_filter(EventFilterCallback* callback, void** user_data) noexcept;
bool get_event_filter(EventFilterCallback& callback, void*& user_data) noexcept;
void pump_events() noexcept;
void flush_events(EventType min_event, EventType max_event) noexcept;
void flush_all_events() noexcept;
bool poll_event(Event* event) noexcept;
bool poll_event(Event& event) noexcept;
std::optional<Event> poll_event() noexcept;
void wait_event(Event* event);
void wait_event(Event& event);
Event wait_event();
void wait_event(Event* event, int timeout);
void wait_event(Event& event, int timeout);
Event wait_event(int timeout);

struct RWOpsDeleter
{
    void operator()(SDL_RWops* rw_ops) noexcept
    {
        SDL_RWclose(rw_ops);
    }
};
using RWOpsUniquePtr = std::unique_ptr<SDL_RWops, RWOpsDeleter>;

RWOpsUniquePtr rw_from_file(const char* filename, const char* mode);
RWOpsUniquePtr rw_from_file(const std::string& filename, const std::string& mode);
#ifdef HAVE_STDIO_H
RWOpsUniquePtr rw_from_file(FILE* file);
#endif

enum class RWSeekWhence : int
{
    set = RW_SEEK_SET,
    current = RW_SEEK_CUR,
    end = RW_SEEK_END
};

class RWOps
{
  public:
    RWOps(RWOpsUniquePtr rw_ops = nullptr) : rw_ops_{std::move(rw_ops)} {}
    RWOps(const char* filename, const char* mode) : rw_ops_{rw_from_file(filename, mode)} {}
    RWOps(const std::string& filename, const std::string& mode) : rw_ops_{rw_from_file(filename, mode)} {}
#ifdef HAVE_STDIO_H
    RWOps(FILE* file) : rw_ops_{rw_from_file(file)} {}
#endif

    [[nodiscard]] RWOpsUniquePtr::pointer get_pointer() const noexcept
    {
        return rw_ops_.get();
    }

    void close()
    {
        SDL_RWclose(rw_ops_.release());
    }

    std::size_t read(std::size_t size, std::size_t maxnum, void* data) const
    {
        std::size_t n_read = SDL_RWread(get_pointer(), data, size, maxnum);
        if (n_read == 0) {
            throw GenericError{};
        }
        return n_read;
    }

    [[nodiscard]] std::int64_t seek(int64_t offset, RWSeekWhence whence) const
    {
        std::int64_t seeked_offset = SDL_RWseek(get_pointer(), offset, static_cast<int>(whence));
        if (seeked_offset == -1) {
            throw GenericError{};
        }
        return seeked_offset;
    }

    [[nodiscard]] std::int64_t tell() const
    {
        std::int64_t seeked_offset = SDL_RWtell(get_pointer());
        if (seeked_offset == -1) {
            throw GenericError{};
        }
        return seeked_offset;
    }

    void write(const void* data, std::size_t size, std::size_t n) const
    {
        if (SDL_RWwrite(get_pointer(), data, size, n) < n) {
            throw GenericError{};
        }
    }

  private:
    RWOpsUniquePtr rw_ops_;
};

template <typename T>
struct select_point;

template <>
struct select_point<int>
{
    using type = SDL_Point;
};

template <>
struct select_point<float>
{
    using type = SDL_FPoint;
};

template <typename T>
using Point = typename select_point<T>::type;

template <typename T>
concept SDLPointT = std::disjunction_v<std::is_same<T, SDL_Point>, std::is_same<T, SDL_FPoint>>;

template <typename Point>
struct point_dimension;

template <typename Point>
using point_dimension_type = typename point_dimension<Point>::type;

template <>
struct point_dimension<SDL_Point>
{
    using type = int;
};

template <>
struct point_dimension<SDL_FPoint>
{
    using type = float;
};

namespace point_operators {

template <SDLPointT T>
bool operator==(const T lhs, const T rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

template <SDLPointT T>
bool operator!=(const T lhs, const T rhs)
{
    return lhs.x != rhs.x || lhs.y != rhs.y;
}

template <SDLPointT T>
T operator+(const T lhs, const T rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

template <SDLPointT T>

T& operator+=(T& lhs, const T rhs)
{
    return lhs = lhs + rhs;
}

template <SDLPointT T>
T operator-(const T lhs, const T rhs)
{
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

template <SDLPointT T>
T& operator-=(T& lhs, const T rhs)
{
    return lhs = lhs - rhs;
}

template <SDLPointT T, typename U>
T operator*(const U lhs, const T rhs)
{
    return {lhs * rhs.x, lhs * rhs.y};
}

template <SDLPointT T, typename U>
T operator*(const T lhs, const U rhs)
{
    return {lhs.x * rhs, lhs.y * rhs};
}

template <SDLPointT T, typename U>
T& operator*=(T& lhs, const U rhs)
{
    return lhs = lhs * rhs;
}

template <SDLPointT T, typename U>
T operator/(const T lhs, const U rhs)
{
    return {lhs.x / rhs, lhs.y / rhs};
}

template <SDLPointT T, typename U>
T& operator/=(T& lhs, const U rhs)
{
    return lhs = lhs / rhs;
}

} // namespace point_operators

template <typename T>
struct select_rectangle;

template <>
struct select_rectangle<int>
{
    using type = SDL_Rect;
};

template <>
struct select_rectangle<float>
{
    using type = SDL_FRect;
};

template <typename T>
using Rectangle = typename select_rectangle<T>::type;

template <typename Rectangle>
struct rectangle_dimension;

template <typename Rectangle>
using rectangle_dimension_type = typename rectangle_dimension<Rectangle>::type;

template <>
struct rectangle_dimension<SDL_Rect>
{
    using type = int;
};

template <>
struct rectangle_dimension<SDL_FRect>
{
    using type = float;
};

namespace rectangle_operators {

template <typename RectangleT>
RectangleT operator+(const RectangleT lhs, const Point<typename rectangle_dimension<RectangleT>::type> rhs)
{
    return RectangleT{lhs.x + rhs.x, lhs.y + rhs.y, lhs.w, lhs.h};
}

template <typename RectangleT>
RectangleT operator+(const Point<typename rectangle_dimension<RectangleT>::type> lhs, const RectangleT rhs)
{
    return rhs + lhs;
}

template <typename RectangleT>
RectangleT& operator+=(RectangleT& lhs, const Point<typename rectangle_dimension<RectangleT>::type> rhs)
{
    return lhs = lhs + rhs;
}

} // namespace rectangle_operators

template <typename PointT>
Rectangle<point_dimension_type<PointT>> make_rectangle(const PointT origin, const PointT size)
{
    return {origin.x, origin.y, size.x, size.y};
}

template <typename PointT, typename RectangleT>
bool is_point_in_rectangle(PointT point, RectangleT rectangle);

using Color = SDL_Color;

struct RendererConfig
{
    int index;
    Uint32 flags;
};

struct WindowConfig
{
    const char* title;
    int x_position;
    int y_position;
    int width;
    int height;
    Uint32 flags;
};

namespace InitFlags {
static constexpr Uint32 none = 0;
static constexpr Uint32 timer = SDL_INIT_TIMER;
static constexpr Uint32 audio = SDL_INIT_AUDIO;
static constexpr Uint32 video = SDL_INIT_VIDEO;
static constexpr Uint32 joystick = SDL_INIT_JOYSTICK;
static constexpr Uint32 haptic = SDL_INIT_HAPTIC;
static constexpr Uint32 game_controller = SDL_INIT_GAMECONTROLLER;
static constexpr Uint32 events = SDL_INIT_EVENTS;
static constexpr Uint32 sensor = SDL_INIT_SENSOR;
static constexpr Uint32 no_parachute = SDL_INIT_NOPARACHUTE;
static constexpr Uint32 everything =
    timer | audio | video | joystick | haptic | game_controller | events | sensor | no_parachute;
} // namespace InitFlags

inline void initialize(Uint32 flags)
{
    if (SDL_Init(flags) < 0) {
        throw GenericError{};
    }
}

inline void quit()
{
    SDL_Quit();
}

struct WindowDeleter
{
    void operator()(SDL_Window* window) noexcept
    {
        SDL_DestroyWindow(window);
    }
};

struct RendererDeleter
{
    void operator()(SDL_Renderer* window) noexcept
    {
        SDL_DestroyRenderer(window);
    }
};

struct TextureDeleter
{
    void operator()(SDL_Texture* surface) noexcept
    {
        SDL_DestroyTexture(surface);
    }
};

struct SurfaceDeleter
{
    void operator()(SDL_Surface* surface) noexcept
    {
        SDL_FreeSurface(surface);
    }
};

using WindowUniquePtr = std::unique_ptr<SDL_Window, WindowDeleter>;
using RendererUniquePtr = std::unique_ptr<SDL_Renderer, RendererDeleter>;
using TextureUniquePtr = std::unique_ptr<SDL_Texture, TextureDeleter>;
using SurfaceUniquePtr = std::unique_ptr<SDL_Surface, SurfaceDeleter>;

[[nodiscard]] WindowUniquePtr
make_window(const char* title, int x_position, int y_position, int width, int height, Uint32 flags);
[[nodiscard]] WindowUniquePtr make_window(const WindowConfig& config);

[[nodiscard]] RendererUniquePtr make_renderer(SDL_Window* window, int index, Uint32 flags);
[[nodiscard]] RendererUniquePtr make_renderer(SDL_Window* window, const RendererConfig& config);

[[nodiscard]] TextureUniquePtr make_texture(SDL_Renderer* renderer, Uint32 format, int access, int width, int height);
[[nodiscard]] TextureUniquePtr make_texture_from_surface(SDL_Renderer* renderer, SDL_Surface* surface);

[[nodiscard]] SurfaceUniquePtr load_bmp(const std::string& filename);

[[nodiscard]] SurfaceUniquePtr
convert_surface(SurfaceUniquePtr surface, const SDL_PixelFormat* format, Uint32 flags = 0);

class Texture
{
  public:
    struct Properties
    {
        Uint32 format;
        int access;
        int width;
        int height;

        void set_size(const Point<int> size)
        {
            width = size.x;
            height = size.y;
        }
    };

    Texture(TextureUniquePtr texture = nullptr) noexcept : texture_{std::move(texture)} {}

    [[nodiscard]] TextureUniquePtr::pointer get_pointer() const noexcept
    {
        return texture_.get();
    }

    [[nodiscard]] Uint32 format() const noexcept
    {
        Uint32 format;
        SDL_QueryTexture(get_pointer(), &format, nullptr, nullptr, nullptr);
        return format;
    }

    [[nodiscard]] int access() const noexcept
    {
        int access;
        SDL_QueryTexture(get_pointer(), nullptr, &access, nullptr, nullptr);
        return access;
    }

    [[nodiscard]] int width() const noexcept
    {
        int width;
        SDL_QueryTexture(get_pointer(), nullptr, nullptr, &width, nullptr);
        return width;
    }

    [[nodiscard]] int height() const noexcept
    {
        int height;
        SDL_QueryTexture(get_pointer(), nullptr, nullptr, nullptr, &height);
        return height;
    }

    [[nodiscard]] Properties properties() const noexcept
    {
        Properties properties;
        SDL_QueryTexture(get_pointer(), &properties.format, &properties.access, &properties.width, &properties.height);
        return properties;
    }

    [[nodiscard]] Point<int> size() const noexcept
    {
        Point<int> size;
        SDL_QueryTexture(get_pointer(), nullptr, nullptr, &size.x, &size.y);
        return size;
    }

    operator SDL_Texture&() const noexcept
    {
        return *get_pointer();
    }

  private:
    TextureUniquePtr texture_;
};

class Renderer
{
  public:
    Renderer(RendererUniquePtr renderer = nullptr) : renderer_{std::move(renderer)} {}
    Renderer(SDL_Window* window, int index, Uint32 flags) : renderer_{make_renderer(window, index, flags)} {}
    Renderer(SDL_Window* window, const RendererConfig& config) : renderer_{make_renderer(window, config)} {}

    [[nodiscard]] RendererUniquePtr::pointer get_pointer() const noexcept
    {
        return renderer_.get();
    }

    void set_scale(float scale_x, float scale_y) const
    {
        if (SDL_RenderSetScale(get_pointer(), scale_x, scale_y) != 0) {
            throw GenericError{};
        }
    }

    void set_draw_blend_mode(SDL_BlendMode mode) const
    {
        if (SDL_SetRenderDrawBlendMode(get_pointer(), mode) != 0) {
            throw GenericError{};
        }
    }

    [[nodiscard]] SDL_BlendMode get_draw_blend_mode() const
    {
        SDL_BlendMode mode;
        if (SDL_GetRenderDrawBlendMode(get_pointer(), &mode) != 0) {
            throw GenericError{};
        }
        return mode;
    }

    void set_viewport(const sdl::Rectangle<int>& rectangle) const
    {
        if (SDL_RenderSetViewport(get_pointer(), &rectangle) != 0) {
            throw GenericError{};
        }
    }

    [[nodiscard]] sdl::Rectangle<int> get_viewport() const
    {
        sdl::Rectangle<int> rectangle;
        SDL_RenderGetViewport(get_pointer(), &rectangle);
        return rectangle;
    }

    void set_render_target(SDL_Texture* texture) const
    {
        if (SDL_SetRenderTarget(get_pointer(), texture) != 0) {
            throw GenericError{};
        }
    }

    [[nodiscard]] SDL_Texture* get_render_target() const
    {
        return SDL_GetRenderTarget(get_pointer());
    }

    void set_draw_color(const Color& color) const
    {
        if (SDL_SetRenderDrawColor(get_pointer(), color.r, color.g, color.b, color.a) != 0) {
            throw GenericError{};
        }
    }

    void clear() const
    {
        if (SDL_RenderClear(get_pointer()) != 0) {
            throw GenericError{};
        }
    }

    void present() const noexcept
    {
        SDL_RenderPresent(get_pointer());
    }

    template <typename T>
    void draw_point(T point_x, T point_y) const;
    template <typename Point>
    void draw_point(Point point) const;

    void draw_line(int x_begin, int y_begin, int x_end, int y_end) const;
    void draw_line(Point<int> begin, Point<int> end) const;

    template <typename Rectangle>
    void fill_rectangle(const Rectangle& rectangle);
    template <typename Rectangle>
    void fill_rectangles(std::span<Rectangle> rectangles);

    template <typename DestinationRectangle>
    void copy(SDL_Texture& texture, const Rectangle<int>& source, const DestinationRectangle& destination);

    [[nodiscard]] TextureUniquePtr make_texture(Uint32 format, int access, int width, int height) const;
    [[nodiscard]] TextureUniquePtr make_texture(const Texture::Properties& properties) const;
    [[nodiscard]] TextureUniquePtr make_texture_from_surface(SDL_Surface* surface) const;

  private:
    RendererUniquePtr renderer_;
};

class Window
{
  public:
    Window(WindowUniquePtr window = nullptr) : window_(std::move(window)) {}
    Window(const char* title, int x_position, int y_position, int width, int height, Uint32 flags)
        : Window(make_window(title, x_position, y_position, width, height, flags))
    {}
    Window(const WindowConfig& config)
        : Window(config.title, config.x_position, config.y_position, config.width, config.height, config.flags)
    {}

    [[nodiscard]] WindowUniquePtr::pointer get_pointer() const noexcept
    {
        return window_.get();
    }

    [[nodiscard]] std::tuple<int, int> size() const noexcept
    {
        std::tuple<int, int> size;
        SDL_GetWindowSize(get_pointer(), &std::get<0>(size), &std::get<1>(size));
        return size;
    }

    [[nodiscard]] int width() const noexcept
    {
        return std::get<0>(size());
    }

    [[nodiscard]] int height() const noexcept
    {
        return std::get<1>(size());
    }

    [[nodiscard]] bool shown() const noexcept
    {
        return (SDL_GetWindowFlags(get_pointer()) & SDL_WINDOW_SHOWN) != 0U;
    }

    [[nodiscard]] bool hidden() const noexcept
    {
        return (SDL_GetWindowFlags(get_pointer()) & SDL_WINDOW_HIDDEN) != 0U;
    }

  private:
    WindowUniquePtr window_;
};

} // namespace sdl
