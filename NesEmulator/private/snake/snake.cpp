#include <iostream>
#include <SDL3/SDL.h>

#include "../public/snake/snake.h"

// 自定义 deleter，用于管理 SDL 资源
struct SDL_WindowDeleter
{
    void operator()(SDL_Window* window) const
    {
        SDL_DestroyWindow(window);
    }
};

struct SDL_RendererDeleter
{
    void operator()(SDL_Renderer* renderer) const
    {
        SDL_DestroyRenderer(renderer);
    }
};

using WindowPtr = std::unique_ptr<SDL_Window, SDL_WindowDeleter>;
using RendererPtr = std::unique_ptr<SDL_Renderer, SDL_RendererDeleter>;

static constexpr int TILE_SIZE = 32; // 每个格子 32x32 像素
static constexpr int SCALE_SIZE = 10; // 10x10 网格
static constexpr int WINDOW_WIDTH = TILE_SIZE * SCALE_SIZE; // 320
static constexpr int WINDOW_HEIGHT = TILE_SIZE * SCALE_SIZE; // 320


void test_snake()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << '\n';
        return;
    }

    WindowPtr window(SDL_CreateWindow(
        "Snake game",
        WINDOW_WIDTH, // w
        WINDOW_HEIGHT, // h
        SDL_WINDOW_RESIZABLE // flags
    ));
    if (!window)
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return;
    }

    RendererPtr renderer(SDL_CreateRenderer(window.get(), nullptr));
    if (!renderer)
    {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        return;
    }
    if (!SDL_SetRenderScale(renderer.get(), SCALE_SIZE, SCALE_SIZE))
    {
        std::cerr << "SDL_SetRenderScale Error: " << SDL_GetError() << std::endl;
        // 可以根据需要决定是否继续运行
    }
    bool running = true;
    SDL_Event event;

    while (running)
    {
        // 处理事件: 使用 SDL_PollEvent 而非 SDL_PumpEvents + 手动遍历
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }

        // 清屏：SDL3 中不再使用 SDL_SetRenderDrawColor 和 SDL_RenderClear，而是统一 RenderViewport
        // 这里是设置清除颜色（RGBA 各分量）
        if (!SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255))
        {
            std::cerr << "SDL_SetRenderDrawColor Error: " << SDL_GetError() << std::endl;
        }
        if (!SDL_RenderClear(renderer.get()))
        {
            std::cerr << "SDL_RenderClear Error: " << SDL_GetError() << std::endl;
        }

        /********** 这里是你的游戏逻辑和绘图代码 **********/
        // 例如：
        // SDL_SetRenderDrawColor(renderer.get(), 255, 255, 255, 255);
        // SDL_RenderFillRect(renderer.get(), &some_rect);
        /************************************************/

        // 显示渲染结果
        SDL_RenderPresent(renderer.get());
    }
}
