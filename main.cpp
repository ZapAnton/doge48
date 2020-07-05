#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>

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

class Cell {
  public:
    Cell(int x, int y, CellType type);
    std::tuple<int, int> get_position();
    CellType get_type() { return this->type; }
    int get_x() const { return this->x; }
    int get_y() const { return this->y; }
    void set_x(int x) { this->x = x; }
    void set_y(int y) { this->y = y; }

  private:
    int x;
    int y;
    CellType type;
};

Cell::Cell(int x, int y, CellType type) : x{x}, y{y}, type{type} {}

std::tuple<int, int> Cell::get_position() {
    return std::make_tuple(this->x, this->y);
}

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
    const int cell_row_count;
    const int cell_height;
    const int cell_width;
    bool running;
    bool field_updated;
    SDL_Window *window;
    SDL_Renderer *renderer;
    std::map<CellType, CellTexture> cell_textures;
    std::vector<Cell> field;

    void load_textures();
    void handle_movement_key(SDL_Keycode key_type);
};

Game::Game()
    : screen_height{800}, screen_width{800}, cell_row_count{4},
      cell_height{screen_height / cell_row_count}, cell_width{screen_width /
                                                              cell_row_count},
      running{false}, field_updated{true}, window{nullptr}, renderer{nullptr} {
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
    std::srand(std::time(nullptr));
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

// TODO: Fix 2 cells movement case
void Game::handle_movement_key(SDL_Keycode key_type) {
    std::function<bool(const Cell &, const Cell &)> sort_algorithm;
    switch (key_type) {
    case SDLK_UP:
        sort_algorithm = [](const Cell &cell1, const Cell &cell2) {
            const int y_position_1 = cell1.get_y();
            const int y_position_2 = cell2.get_y();
            return y_position_1 < y_position_2;
        };
        break;
    case SDLK_DOWN:
        sort_algorithm = [](const Cell &cell1, const Cell &cell2) {
            const int y_position_1 = cell1.get_y();
            const int y_position_2 = cell2.get_y();
            return y_position_1 > y_position_2;
        };
        break;
    case SDLK_RIGHT:
        sort_algorithm = [](const Cell &cell1, const Cell &cell2) {
            const int x_position_1 = cell1.get_x();
            const int x_position_2 = cell2.get_x();
            return x_position_1 < x_position_2;
        };
        break;
    case SDLK_LEFT:
        sort_algorithm = [](const Cell &cell1, const Cell &cell2) {
            const int x_position_1 = cell1.get_x();
            const int x_position_2 = cell2.get_x();
            return x_position_1 > x_position_2;
        };
        break;
    default:
        return;
    };
    std::sort(this->field.begin(), this->field.end(), sort_algorithm);
    for (auto &cell : this->field) {
        std::function<bool(const Cell &)> filter_algorithm;
        switch (key_type) {
        case SDLK_UP:
            filter_algorithm = [&](const Cell &other_cell) {
                return other_cell.get_y() < cell.get_y() &&
                       other_cell.get_x() == cell.get_x();
            };
            break;
        case SDLK_DOWN:
            filter_algorithm = [&](const Cell &other_cell) {
                return other_cell.get_y() > cell.get_y() &&
                       other_cell.get_x() == cell.get_x();
            };
            break;
        case SDLK_LEFT:
            filter_algorithm = [&](const Cell &other_cell) {
                return other_cell.get_x() < cell.get_x() &&
                       other_cell.get_y() == cell.get_y();
            };
            break;
        case SDLK_RIGHT:
            filter_algorithm = [&](const Cell &other_cell) {
                return other_cell.get_x() > cell.get_x() &&
                       other_cell.get_y() == cell.get_y();
            };
            break;
        default:
            return;
        };
        auto cells_in_a_way_count = std::count_if(
            this->field.begin(), this->field.end(), filter_algorithm);
        switch (key_type) {
        case SDLK_UP:
            cell.set_y(cell.get_y() - (cell.get_y() - cells_in_a_way_count));
            break;
        case SDLK_DOWN:
            cell.set_y(cell.get_y() + (this->cell_row_count - 1 - cell.get_y() -
                                       cells_in_a_way_count));
            break;
        case SDLK_LEFT:
            cell.set_x(cell.get_x() - (cell.get_x() - cells_in_a_way_count));
            break;
        case SDLK_RIGHT:
            cell.set_x(cell.get_x() + (this->cell_row_count - 1 - cell.get_x() -
                                       cells_in_a_way_count));
            break;
        default:
            return;
        };
    }
}

void Game::handle_events() {
    SDL_Event event;
    SDL_WaitEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
        this->running = false;
        break;
    case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
            this->running = false;
            break;
        case SDLK_DOWN:
        case SDLK_UP:
        case SDLK_LEFT:
        case SDLK_RIGHT:
            this->field_updated = true;
            this->handle_movement_key(event.key.keysym.sym);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void Game::update() {
    if (this->field.size() ==
        static_cast<std::vector<Cell>::size_type>(
            this->cell_row_count * this->cell_row_count - 1)) {
        this->running = false;
        return;
    }
    if (!this->field_updated) {
        return;
    }
    int generated_x = std::rand() % static_cast<int>(this->cell_row_count);
    int generated_y = std::rand() % static_cast<int>(this->cell_row_count);
    bool cell_is_used =
        std::any_of(this->field.begin(), this->field.end(), [&](Cell &cell) {
            int x_position = 0;
            int y_position = 0;
            std::tie(x_position, y_position) = cell.get_position();
            return x_position == generated_x && y_position == generated_y;
        });
    while (cell_is_used) {
        generated_x = std::rand() % static_cast<int>(this->cell_row_count);
        generated_y = std::rand() % static_cast<int>(this->cell_row_count);
        cell_is_used = std::any_of(
            this->field.begin(), this->field.end(), [&](Cell &cell) {
                int x_position = 0;
                int y_position = 0;
                std::tie(x_position, y_position) = cell.get_position();
                return x_position == generated_x && y_position == generated_y;
            });
    }
    Cell generated_cell(generated_x, generated_y, CellType::One);
    this->field.push_back(generated_cell);
    this->field_updated = false;
}

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
    for (auto &cell : this->field) {
        int cell_x = 0;
        int cell_y = 0;
        std::tie(cell_x, cell_y) = cell.get_position();
        image_location.x = cell_x * this->cell_width;
        image_location.y = cell_y * this->cell_height;
        SDL_RenderCopy(this->renderer,
                       this->cell_textures[cell.get_type()].texture, nullptr,
                       &image_location);
    }
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
