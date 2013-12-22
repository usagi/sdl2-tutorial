#include <memory>
#include <chrono>
#include <vector>
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

  void nocheck(const string& message)
  {
    cout << "\x1b[33m[NC]\x1b[39m: " << message << endl;
  }

  std::shared_ptr<nullptr_t> sdl_initialize()
  {
    auto result = SDL_Init(SDL_INIT_EVERYTHING);
    check("SDL / sdl_initialize", result != -1);
    return shared_ptr<nullptr_t>
    ( nullptr
    , [](nullptr_t)
      {
        SDL_Quit();
        nocheck("SDL / quit");
      }
    );
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
  
  void sdl_render_clear(const SDL_Renderer_t& sdl_renderer)
  {
    auto result = SDL_RenderClear(sdl_renderer.get());
    check("SDL / render clear", result == 0);
  }

  void sdl_render_copy
  ( const SDL_Renderer_t& sdl_renderer
  , const SDL_Texture_t& sdl_texture
  )
  {
    auto result = SDL_RenderCopy
    ( sdl_renderer.get()
    , sdl_texture.get()
    , nullptr, nullptr
    );
    check("SDL / render copy", result == 0);
  }

  void sdl_render_present(const SDL_Renderer_t& sdl_renderer)
  {
    SDL_RenderPresent(sdl_renderer.get());
    nocheck("SDL / render present");
  }

  void sdl_delay(const chrono::milliseconds& duration)
  {
    SDL_Delay(duration.count());
    nocheck("SDL / delay");
  }

  vector<SDL_Event> sdl_poll_events()
  {
    vector<SDL_Event> events;
    bool result = false;
    while(true)
    {
      SDL_Event e;
      result = SDL_PollEvent(&e);
      if(result == 0)
        break;
      events.emplace_back(move(e));
    }

    return move(events);
  }

  chrono::microseconds time(const function<void()>& function)
  {
    auto frame_start_time = chrono::high_resolution_clock::now();
    function();
    return chrono::duration_cast<chrono::microseconds>
    (chrono::high_resolution_clock::now() - frame_start_time);
  }

  template<size_t target_fps>
  void adjust_fps(const function<void()>& function)
  {
    auto frame_elapsed_time = time(function);
    auto frame_delta_time
      = chrono::microseconds(1000*1000/target_fps) - frame_elapsed_time;
    nocheck
    ( string("adjust fps / frame delta time: ")
    + to_string(frame_delta_time.count())
    + " [us]"
    );
    if(frame_delta_time.count() > 0)
      sdl_delay(chrono::duration_cast<chrono::milliseconds>(frame_delta_time));
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

  bool is_continue = true;

  auto sdl_main = [&]()
  {
    nocheck("SDL / main: begin");
    sdl_render_clear(sdl_renderer);
    sdl_render_copy(sdl_renderer, sdl_texture);
    sdl_render_present(sdl_renderer);
    for(const auto& sdl_event : sdl_poll_events())
    {
      switch(sdl_event.type)
      {
        case SDL_KEYDOWN:
        {
          auto sdl_key = sdl_event.key.keysym.sym;
          if(sdl_key != SDLK_ESCAPE)
            break;
        }
        case SDL_QUIT:
          is_continue = false;
      }
    }
    nocheck("SDL / main: end");
  };

  do adjust_fps<60>(sdl_main); while(is_continue);
}
catch(const std::exception& e)
{
  std::cerr << "\x1b[31m  exception: " << e.what() << "\x1b[39m\n";
  return 1;
}
