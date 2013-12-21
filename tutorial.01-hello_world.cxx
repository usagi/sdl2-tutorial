#include <memory>
#include <stdexcept>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace
{
  using namespace std;
  
  using SDL_Window_t   = shared_ptr<SDL_Window>;
  using SDL_Renderer_t = shared_ptr<SDL_Renderer>;
  using SDL_Surface_t  = shared_ptr<SDL_Surface>;
  using SDL_Texture_t  = shared_ptr<SDL_Texture>;

  void check(const string& message, const bool result = true)
  {
    if(result)
      cout << "\x1b[32m[OK]\x1b[39m: " << message << endl;
    else
    {
      cerr << "\x1b[31m[NG]\x1b[39m: " << message << "\n";
      throw runtime_error(SDL_GetError());
    }
  }

  std::shared_ptr<nullptr_t> sdl_initialize()
  {
    auto result = SDL_Init(SDL_INIT_EVERYTHING);
    check("SDL / sdl_initialize", result != -1);
    return shared_ptr<nullptr_t>(nullptr, [](nullptr_t){ SDL_Quit(); } );
  }

  SDL_Window_t sdl_create_window
  ( const string& title
  , const int w
  , const int h
  , const int x = SDL_WINDOWPOS_CENTERED
  , const int y = SDL_WINDOWPOS_CENTERED
  , const uint32_t flags = SDL_WINDOW_OPENGL
                         | SDL_WINDOW_RESIZABLE
                         | SDL_WINDOW_INPUT_GRABBED
  )
  {
    SDL_Window_t result
    ( SDL_CreateWindow(title.data(), x, y, w, h, SDL_WINDOW_SHOWN)
    , SDL_DestroyWindow
    );
    check("SDL / create window", bool(result));
    return move(result);
  }

  SDL_Renderer_t sdl_create_renderer(const SDL_Window_t& sdl_window)
  {
    SDL_Renderer_t result
    ( SDL_CreateRenderer
      ( sdl_window.get(), -1
      , SDL_RENDERER_ACCELERATED
      | SDL_RENDERER_PRESENTVSYNC
      )
    , SDL_DestroyRenderer
    );
    check("SDL / create renderer", bool(result));
    return std::move(result);
  }

  SDL_Surface_t
  sdl_load_image(const string& path)
  {
    SDL_Surface_t result
    ( IMG_Load( path.data() )
    , SDL_FreeSurface
    );
    check("SDL / load image", bool(result));
    return move(result);
  }

  SDL_Texture_t
  sdl_create_texture_from_surface
  ( const SDL_Renderer_t& sdl_renderer
  , const SDL_Surface_t& sdl_surface
  )
  {
    
    SDL_Texture_t result
    ( SDL_CreateTextureFromSurface
      ( sdl_renderer.get()
      , sdl_surface.get()
      )
    , SDL_DestroyTexture
    );
    check("SDL / create texture from surface", bool(result));
    return std::move(result);
  }
}

int main()
try
{
  auto sdl = sdl_initialize();
  auto sdl_window   = sdl_create_window("Hello, SDL world!", 512, 512);
  auto sdl_renderer = sdl_create_renderer(sdl_window);
  auto sdl_surface  = sdl_load_image("sample.png");
  auto sdl_texture  = sdl_create_texture_from_surface(sdl_renderer, sdl_surface);
  SDL_RenderClear(sdl_renderer.get());
  SDL_RenderCopy(sdl_renderer.get(), sdl_texture.get(), nullptr, nullptr);
  SDL_RenderPresent(sdl_renderer.get());
  SDL_Delay(3000);
}
catch(const std::exception& e)
{
  std::cerr << "\x1b[31m  exception: " << e.what() << "\x1b[39m\n";
  return 1;
}
