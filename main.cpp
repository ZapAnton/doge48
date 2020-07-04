#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <filesystem>
#include <iostream>
#include <map>
#include <string>

// Type == power of two
enum class CellType {
    One,
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine,
    Ten,
    Eleven,
};

class CellTexture {
  public:
    std::string filename;
    SDL_Texture *texture;
};

class Game {
  public:
    Game();
    ~Game();

    void init();
    void handle_events();
    void update();
    void render();
    void clean();
    bool is_running();

  private:
    const int screen_height;
    const int screen_width;
    const int cell_height;
    const int cell_width;
    bool running;
    SDL_Window *window;
    SDL_Renderer *renderer;
    std::map<CellType, CellTexture> cell_textures;
    void load_textures();
};

Game::Game()
    : screen_height{800}, screen_width{800}, cell_height{screen_height / 4},
      cell_width{screen_width / 4}, running{false}, window{nullptr},
      renderer{nullptr} {
    this->cell_textures[CellType::One] = {"2.png", nullptr};
    this->cell_textures[CellType::Two] = {"4.png", nullptr};
    this->cell_textures[CellType::Three] = {"8.png", nullptr};
    this->cell_textures[CellType::Four] = {"16.png", nullptr};
    this->cell_textures[CellType::Five] = {"32.png", nullptr};
    this->cell_textures[CellType::Six] = {"64.png", nullptr};
    this->cell_textures[CellType::Seven] = {"128.png", nullptr};
    this->cell_textures[CellType::Eight] = {"256.png", nullptr};
    this->cell_textures[CellType::Nine] = {"512.png", nullptr};
    this->cell_textures[CellType::Ten] = {"1024.png", nullptr};
    this->cell_textures[CellType::Eleven] = {"2048.png", nullptr};
}

Game::~Game() { this->clean(); }

void Game::load_textures() {
    auto assets_pics_path = std::filesystem::path() / "assets" / "pics";
    for (auto &[cell_type, texture] : this->cell_textures) {
        auto image_path = assets_pics_path / texture.filename;
        auto image_surface = IMG_Load(image_path.string().c_str());
        if (image_surface == nullptr) {
            std::cerr << "Could not load image " << image_path << ": "
                      << SDL_GetError() << std::endl;
            this->clean();
            this->running = false;
            return;
        }
        auto image_texture =
            SDL_CreateTextureFromSurface(this->renderer, image_surface);
        if (image_texture == nullptr) {
            std::cerr << "Could not create image texture " << image_path << ": "
                      << SDL_GetError() << std::endl;
            ;
            this->clean();
            this->running = false;
            return;
        }
        texture.texture = image_texture;
        SDL_FreeSurface(image_surface);
    }
}

void Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError()
                  << std::endl;
        SDL_Quit();
        this->running = false;
        return;
    }
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        std::cerr << "SDL image could not initialize! SDL_Error: "
                  << SDL_GetError() << std::endl;
        SDL_Quit();
        this->running = false;
        return;
    }
    this->window = SDL_CreateWindow("Doge48", SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED, this->screen_width,
                                    this->screen_width, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "SDL could not create window! SDL_Error: "
                  << SDL_GetError() << std::endl;
        SDL_Quit();
        this->running = false;
        return;
    }
    this->renderer = SDL_CreateRenderer(this->window, -1, 0);
    this->load_textures();
    this->running = true;
}

void Game::handle_events() {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
        this->running = false;
        break;
    case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
            this->running = false;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void Game::update() {}

void Game::render() {
    SDL_SetRenderDrawColor(this->renderer, 250, 214, 114, 255);
    SDL_RenderClear(this->renderer);
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
    for (int i = 0; i < this->screen_width * this->cell_width;
         i += this->cell_width) {
        SDL_RenderDrawLine(this->renderer, i, 0, i, this->screen_height);
    }
    for (int i = 0; i < this->screen_height * this->cell_height;
         i += this->cell_height) {
        SDL_RenderDrawLine(this->renderer, 0, i, this->screen_width, i);
    }
    SDL_Rect image_location{0, 0, this->cell_height, this->cell_width};
    SDL_RenderCopy(this->renderer, this->cell_textures[CellType::One].texture,
                   nullptr, &image_location);
    image_location.x += this->cell_width;
    SDL_RenderCopy(this->renderer, this->cell_textures[CellType::Two].texture,
                   nullptr, &image_location);
    SDL_RenderPresent(this->renderer);
}

void Game::clean() {
    for (auto &[number, texture] : this->cell_textures) {
        SDL_DestroyTexture(texture.texture);
    }
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    IMG_Quit();
    SDL_Quit();
}

bool Game::is_running() { return this->running; }

int main() {
    Game game;
    game.init();
    while (game.is_running()) {
        game.handle_events();
        game.update();
        game.render();
    }
    return 0;
}
