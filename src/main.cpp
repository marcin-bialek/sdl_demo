#include <SDL.h>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>

#include "safe_queue.h"


static constexpr int WINDOW_WIDTH = 400;
static constexpr int WINDOW_HEIGHT = 300;
static std::atomic<bool> running = true;
 

struct RGBA {
    uint8_t a, b, g, r;
};


static void draw(unsigned int time, RGBA* pixels, int width, int height) {
    constexpr int D = 50;
    RGBA* pixel = pixels;
    int cy = height / 2;
    time = time % 10000;
    int cx = 0;

    if (time <= 5000) {
        cx = D + ((width - 2 * D) * time / 5000); 
    }
    else {
        cx = width - D - ((width - 2 * D) * (time - 5000) / 5000); 
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x, ++pixel) {
            pixel->r = 0;
            pixel->g = 0;
            pixel->b = 0;
            pixel->a = 255;

            if ((x - cx) * (x - cx) + (y - cy) * (y - cy) <= D * D) {
                pixel->r = 255;
            }
        }
    }
}


static void producerLoop(SafeQueue<std::vector<RGBA>>& queue) {
    while (running) {
        auto now = std::chrono::steady_clock::now().time_since_epoch();
        auto t = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
        std::vector<RGBA> buffer(WINDOW_WIDTH * WINDOW_HEIGHT);
        draw(t, buffer.data(), WINDOW_WIDTH, WINDOW_HEIGHT);
        queue.push(std::move(buffer));
        std::this_thread::sleep_for(std::chrono::milliseconds{1000 / 60});
    }
}


int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    auto window = SDL_CreateWindow("siema", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    auto texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
        WINDOW_WIDTH, WINDOW_HEIGHT);

    SafeQueue<std::vector<RGBA>> queue;
    std::thread producer{producerLoop, std::ref(queue)};

    while (running) {
        SDL_Event event{};

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        auto buffer = queue.pop();

        if (buffer.has_value()) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_UpdateTexture(texture, nullptr, reinterpret_cast<void*>((*buffer).data()), WINDOW_WIDTH * sizeof(RGBA));
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);
        }
        else {
            SDL_Delay(1000 / 60);
        }
    }
    
    producer.join();
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

