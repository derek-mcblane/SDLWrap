#include "sdlpp.h"

#include <optional>

namespace sdl {

void add_event_watch(EventFilterCallback callback, void* user_data) noexcept
{
    SDL_AddEventWatch(callback, user_data);
}

void set_event_filter(EventFilterCallback callback, void* user_data) noexcept
{
    SDL_SetEventFilter(callback, user_data);
}

bool get_event_filter(EventFilterCallback* callback, void** user_data) noexcept
{
    return static_cast<bool>(SDL_GetEventFilter(callback, user_data));
}

bool get_event_filter(EventFilterCallback& callback, void*& user_data) noexcept
{
    return static_cast<bool>(SDL_GetEventFilter(&callback, &user_data));
}

void pump_events() noexcept
{
    SDL_PumpEvents();
}

void flush_events(EventType min_event, EventType max_event) noexcept
{
    SDL_FlushEvents(min_event, max_event);
}

void flush_all_events() noexcept
{
    SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
}

bool poll_event(Event* event) noexcept
{
    return SDL_PollEvent(event) != 0;
}

bool poll_event(Event& event) noexcept
{
    return SDL_PollEvent(&event) != 0;
}

std::optional<Event> poll_event() noexcept
{
    Event event;
    if (SDL_PollEvent(&event) == 0) {
        return std::nullopt;
    }
    return event;
}

void wait_event(Event* event)
{
    if (SDL_WaitEvent(event) != 0) {
        throw exception::generic_error{};
    }
}

void wait_event(Event& event)
{
    return wait_event(&event);
}

Event wait_event()
{
    Event event;
    wait_event(event);
    return event;
}

void wait_event(Event* event, int timeout)
{
    if (SDL_WaitEventTimeout(event, timeout) != 0) {
        throw exception::generic_error{};
    }
}

void wait_event(Event& event, int timeout)
{
    wait_event(&event, timeout);
}

Event wait_event(int timeout)
{
    Event event;
    wait_event(event);
    return event;
}

Uint32 register_events(int n_events)
{
    if (n_events <= 0) {
        throw exception::generic_error{};
    }
    Uint32 event_id = SDL_RegisterEvents(n_events);
    if (event_id == UINT32_MAX) {
        throw exception::generic_error{};
    }
    return event_id;
}

bool push_event(SDL_Event& event)
{
    int status = SDL_PushEvent(&event);
    if (status < 0) {
        throw exception::generic_error{};
    }
    return status == 1;
}

template <>
bool is_point_in_rectangle<Point<int>, Rectangle<int>>(const Point<int> point, const Rectangle<int> rectangle)
{
    return SDL_PointInRect(&point, &rectangle) == SDL_TRUE;
}

template <>
bool is_point_in_rectangle<Point<float>, Rectangle<float>>(const Point<float> point, const Rectangle<float> rectangle)
{
    return SDL_PointInFRect(&point, &rectangle) == SDL_TRUE;
}

[[nodiscard]] WindowUniquePtr
make_window(const char* title, int x_position, int y_position, int width, int height, Uint32 flags)
{
    WindowUniquePtr window{SDL_CreateWindow(title, x_position, y_position, width, height, flags)};
    if (window == nullptr) {
        throw exception::create_window{};
    }
    return window;
}

[[nodiscard]] WindowUniquePtr make_window(const WindowConfig& config)
{
    return make_window(config.title, config.x_position, config.y_position, config.width, config.height, config.flags);
}

[[nodiscard]] RendererUniquePtr make_renderer(SDL_Window* window, int index, Uint32 flags)
{
    RendererUniquePtr renderer{SDL_CreateRenderer(window, index, flags)};
    if (renderer == nullptr) {
        throw exception::create_renderer{};
    }
    return renderer;
}

[[nodiscard]] RendererUniquePtr make_renderer(SDL_Window* window, const RendererConfig& config)
{
    return make_renderer(window, config.index, config.flags);
}

[[nodiscard]] TextureUniquePtr make_texture_from_surface(SDL_Renderer* renderer, SDL_Surface* surface)
{
    TextureUniquePtr texture{SDL_CreateTextureFromSurface(renderer, surface)};
    if (texture == nullptr) {
        throw exception::texture_from_surface{};
    }
    return texture;
}

[[nodiscard]] TextureUniquePtr Renderer::make_texture_from_surface(SDL_Surface* surface)
{
    return ::sdl::make_texture_from_surface(get_pointer(), surface);
}

[[nodiscard]] SurfaceUniquePtr load_bmp(const std::string& filename)
{
    SurfaceUniquePtr image{SDL_LoadBMP(filename.c_str())};
    if (image == nullptr) {
        throw exception::load_image{};
    }
    return image;
}

[[nodiscard]] SurfaceUniquePtr convert_surface(SurfaceUniquePtr surface, const SDL_PixelFormat* format, Uint32 flags)
{
    SurfaceUniquePtr converted_surface{SDL_ConvertSurface(surface.get(), format, flags)};
    if (converted_surface == nullptr) {
        throw exception::convert_surface{};
    }
    return converted_surface;
}

template <>
void Renderer::draw_point<int>(int point_x, int point_y) const
{
    if (SDL_RenderDrawPoint(get_pointer(), point_x, point_y) != 0) {
        throw exception::generic_error{};
    }
}

template <>
void Renderer::draw_point<float>(float point_x, float point_y) const
{
    if (SDL_RenderDrawPointF(get_pointer(), point_x, point_y) != 0) {
        throw exception::generic_error{};
    }
}

template<>
void Renderer::draw_point<Point<int>>(Point<int> point) const {
    draw_point(point.x, point.y);
}

template<>
void Renderer::draw_point<Point<float>>(Point<float> point) const {
    draw_point(point.x, point.y);
}


void Renderer::draw_line(int x_begin, int y_begin, int x_end, int y_end) const
{
    if (SDL_RenderDrawLine(get_pointer(), x_begin, y_begin, x_end, y_end) != 0) {
        throw exception::generic_error{};
    }
}

void Renderer::draw_line(Point<int> begin, Point<int> end) const
{
    draw_line(begin.x, begin.y, end.x, end.y);
}

template <>
void Renderer::fill_rectangle<Rectangle<int>>(const Rectangle<int>& rectangle)
{
    if (SDL_RenderFillRect(get_pointer(), &rectangle) != 0) {
        throw exception::generic_error{};
    }
}

template <>
void Renderer::fill_rectangle<Rectangle<float>>(const Rectangle<float>& rectangle)
{
    if (SDL_RenderFillRectF(get_pointer(), &rectangle) != 0) {
        throw exception::generic_error{};
    }
}

template <>
void Renderer::fill_rectangles<Rectangle<int>>(std::span<Rectangle<int>> rectangles)
{
    if (SDL_RenderFillRects(get_pointer(), rectangles.data(), gsl::narrow<int>(rectangles.size())) != 0) {
        throw exception::generic_error{};
    }
}

template <>
void Renderer::fill_rectangles<Rectangle<float>>(std::span<Rectangle<float>> rectangles)
{
    if (SDL_RenderFillRectsF(get_pointer(), rectangles.data(), gsl::narrow<int>(rectangles.size())) != 0) {
        throw exception::generic_error{};
    }
}

template <>
void Renderer::copy<int>(SDL_Texture& texture, const Rectangle<int>& source, const Rectangle<int>& destination)
{
    if (SDL_RenderCopy(get_pointer(), &texture, &source, &destination) != 0) {
        throw exception::generic_error{};
    }
}

template <>
void Renderer::copy<float>(SDL_Texture& texture, const Rectangle<int>& source, const Rectangle<float>& destination)
{
    if (SDL_RenderCopyF(get_pointer(), &texture, &source, &destination) != 0) {
        throw exception::generic_error{};
    }
}

} // namespace sdl
