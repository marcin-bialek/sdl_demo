#include <SDL.h>
#include <vector>


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


int main(int argc, char* argv[]) {
    constexpr int WINDOW_WIDTH = 400;
    constexpr int WINDOW_HEIGHT = 300;

    SDL_Init(SDL_INIT_VIDEO);
    auto window = SDL_CreateWindow("siema", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    auto texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
        WINDOW_WIDTH, WINDOW_HEIGHT);

    std::vector<RGBA> pixels(WINDOW_WIDTH * WINDOW_HEIGHT * 4);

    for (bool running = true; running;) {
        SDL_Event event{};

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        draw(SDL_GetTicks(), pixels.data(), WINDOW_WIDTH, WINDOW_HEIGHT);
        SDL_UpdateTexture(texture, nullptr, reinterpret_cast<void*>(pixels.data()), WINDOW_WIDTH * sizeof(RGBA));
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }
    
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

