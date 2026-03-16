#include "Crack.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#define CRACKME_EXPECTED_SIZE 10582 
#define WINDOW_TITLE "CrackMe Patcher"
#define WINDOW_W 600
#define WINDOW_H 400

#define COM_FILE "CRACKME.COM"
#define IMAGE_FILE "success.png"
#define SOUND_FILE "success.mp3"
#define FONT_FILE "font.ttf"
#define FONT_SIZE 14

void GenerateKey(char *buf, int buf_size) {
    assert(buf);

    const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int n = strlen(chars);
    srand((unsigned int)time(NULL));

    snprintf(buf, buf_size, "%c%c%c%c%c%c%c%c%c%c", chars[rand() % n],
        chars[rand() % n], chars[rand() % n], chars[rand() % n],
        chars[rand() % n], chars[rand() % n], chars[rand() % n],
        chars[rand() % n], chars[rand() % n], chars[rand() % n]);
}

void ButtonCreateTexture(Button *btn, SDL_Renderer *renderer, TTF_Font *font) {
    assert(btn);
    assert(renderer);
    assert(font);

    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surf = TTF_RenderUTF8_Blended(font, btn->label, color);
    if (!surf) {
        return;
    }

    btn->tex = SDL_CreateTextureFromSurface(renderer, surf);
    btn->tex_w = surf->w;
    btn->tex_h = surf->h;
    SDL_FreeSurface(surf);
}

// проверяю попал ли клик мышкой по кнопке
int ButtonHit(Button *btn, int x, int y) {
    assert(btn);

    return x >= btn->rect.x && x <= btn->rect.x + btn->rect.w &&
        y >= btn->rect.y && y <= btn->rect.y + btn->rect.h;
}

void ButtonDraw(Button *btn, SDL_Renderer *renderer) {
    assert(btn);
    assert(renderer);

    if (btn->hovered) {
        SDL_SetRenderDrawColor(renderer, 180, 0, 180, 255); // тут заливка темнее, потому что hovered
    } else {
        SDL_SetRenderDrawColor(renderer, 220, 0, 220, 255);
    }

    SDL_RenderFillRect(renderer, &btn->rect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
    SDL_RenderDrawRect(renderer, &btn->rect);

    if (btn->tex) {
        SDL_Rect dst = {btn->rect.x + (btn->rect.w - btn->tex_w) / 2,
            btn->rect.y + (btn->rect.h - btn->tex_h) / 2, btn->tex_w, btn->tex_h};
        SDL_RenderCopy(renderer, btn->tex, NULL, &dst);
    }
}

void ButtonDestroy(Button *btn) {
    assert(btn);

    if (btn->tex) {
        SDL_DestroyTexture(btn->tex);
    }

    btn->tex = NULL;
}

// патч COM файла по фиксированному смещению
int PatchCom(const char *path) {
    assert(path);

    const char *file = path;
    FILE *file_out = fopen(file, "r+b");

    if (!file_out) {
        printf("Failed to open file.\n");
        return 0;
    }

    fseek(file_out, 0, SEEK_END);
    long size = ftell(file_out);
    if (size != CRACKME_EXPECTED_SIZE) {
        printf("Wrong file size: %ld (expected %d).\n", size, CRACKME_EXPECTED_SIZE);
        fclose(file_out);
        return 0;
    }

    fseek(file_out, 0x73, SEEK_SET);

    unsigned char value = 0x74;
    fwrite(&value, 1, 1, file_out);

    fclose(file_out);
    printf("Patch is successfully used.\n");

    return 1;
}

int AppInitSDL(App *app) {
    assert(app);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 0;
    }
    if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG)) {
        fprintf(stderr, "IMG_Init: %s\n", IMG_GetError());
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Mix_OpenAudio: %s\n", Mix_GetError());
    }

    Mix_Init(MIX_INIT_MP3);

    if (TTF_Init() < 0) {
        fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
        return 0;
    }

    app->font = TTF_OpenFont(FONT_FILE, FONT_SIZE);
    if (!app->font) {
        fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
        return 0;
    }

    app->window = SDL_CreateWindow(WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_W, WINDOW_H,
        SDL_WINDOW_SHOWN);

    if (!app->window) {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        return 0;
    }

    app->renderer = SDL_CreateRenderer(app->window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!app->renderer) {
        fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
        return 0;
    }

    return 1;
}

void AppCreateButtons(App *app) {
    assert(app);

    int btn_w = 230;
    int btn_h = 45;
    int btn_x = 310 + (290 - btn_w) / 2;

    app->btn_choose = (Button){
        .rect = {btn_x, 120, btn_w, btn_h},
        .label = "CHOOSE YOUR FILE",
        .hovered = 0, .tex = NULL
    };

    app->btn_crack = (Button){
        .rect = {btn_x, 210, btn_w, btn_h},
        .label = "CRACK",
        .hovered = 0, .tex = NULL
    };

    app->btn_keygen = (Button){
        .rect = {btn_x, 300, btn_w, btn_h},
        .label = "GENERATE A KEY",
        .hovered = 0, .tex = NULL
    };

    ButtonCreateTexture(&app->btn_choose, app->renderer, app->font);
    ButtonCreateTexture(&app->btn_crack,  app->renderer, app->font);
    ButtonCreateTexture(&app->btn_keygen, app->renderer, app->font);
}

void AppLoadAssets(App *app) {
    assert(app);

    SDL_Surface *surf = IMG_Load("bg.png");
    if (surf) {
        app->bg_image = SDL_CreateTextureFromSurface(app->renderer, surf);
        SDL_FreeSurface(surf);
    }

    TTF_Font *big_font = TTF_OpenFont(FONT_FILE, 20);
    if (big_font) {
        SDL_Color white = {255, 255, 255, 255};
        surf = TTF_RenderUTF8_Blended(big_font, "PATCH BY ZARINA", white);

        if (surf) {
            app->title_tex = SDL_CreateTextureFromSurface(app->renderer, surf);
            app->title_w = surf->w;
            app->title_h = surf->h;
            SDL_FreeSurface(surf);
        }

        TTF_CloseFont(big_font);
    }
}

int OpenFileDialog(char *buf, int buf_size) {
    assert(buf);

    FILE *file_out = popen("osascript -e 'POSIX path of (choose file with prompt \"Select .COM file\")'", "r");
    if (!file_out) {
        return 0;
    }

    if (!fgets(buf, buf_size, file_out)) {
        pclose(file_out);
        return 0;
    }
    pclose(file_out);

    int len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    }

    return (strlen(buf) > 0);
}

void AppOnCrack(App *app) {
    assert(app);

    app->patched = PatchCom(COM_FILE);
    if (!app->patched) {
        return;
    }

    app->showBg = 0; 
    SDL_Surface *surf = IMG_Load(IMAGE_FILE);
    if (surf) {
        app->success_image = SDL_CreateTextureFromSurface(app->renderer, surf);
        SDL_FreeSurface(surf);
    }

    app->music = Mix_LoadMUS(SOUND_FILE);
    if (app->music) {
        Mix_PlayMusic(app->music, 1);
    }
}

void AppOnChoose(App *app) {
    assert(app);

    char path[1024] = {0};
    if (!OpenFileDialog(path, sizeof(path))) return;

    printf("Chosen file: %s\n", path);
    app->patched = PatchCom(path);
    if (!app->patched) {
        return;
    }

    SDL_Surface *surf = IMG_Load(IMAGE_FILE);
    if (surf) {
        if (app->success_image) {
            SDL_DestroyTexture(app->success_image);
        }

        app->success_image = SDL_CreateTextureFromSurface(app->renderer, surf);
        SDL_FreeSurface(surf);
    }

    if (!app->music) {
        app->music = Mix_LoadMUS(SOUND_FILE);
        if (app->music) {
            Mix_PlayMusic(app->music, 1);
        }
    }
}

void AppOnKeygen(App *app) {
    assert(app);

    GenerateKey(app->keygen_result, sizeof(app->keygen_result));
    printf("Generated key: %s\n", app->keygen_result);

    if (app->keygen_tex) {
        SDL_DestroyTexture(app->keygen_tex);
        app->keygen_tex = NULL;
    }

    SDL_Color white = {255, 255, 255, 255};
    // SDL_Surface *surf = TTF_RenderUTF8_Blended(app->font, app->keygen_result, white);
    // if (surf) {
    //     app->keygen_tex = SDL_CreateTextureFromSurface(app->renderer, surf);
    //     app->keygen_w = surf->w;
    //     app->keygen_h = surf->h;
    //     SDL_FreeSurface(surf);
    // }
}

void AppHandleEvents(App *app) {
    assert(app);

    SDL_Event e = {};
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) app->running = 0;
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
            app->running = 0;
        }

        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            if (ButtonHit(&app->btn_crack, e.button.x, e.button.y)) {
                AppOnCrack(app);
            }

            if (ButtonHit(&app->btn_choose, e.button.x, e.button.y)) {
                AppOnChoose(app);
            }

            if (ButtonHit(&app->btn_keygen, e.button.x, e.button.y)) {
                AppOnKeygen(app);
            }
        }
    }
}

void AppRender(App *app) {
    assert(app);

    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);

    SDL_Rect left = {0, 0, 300, WINDOW_H};
    if (app->bg_image && app->showBg) {
        SDL_RenderCopy(app->renderer, app->bg_image, NULL, &left);
        
    } else if (app->showBg) {
        SDL_SetRenderDrawColor(app->renderer, 30, 10, 40, 255);
        SDL_RenderFillRect(app->renderer, &left);
    }

    SDL_Rect right = {300, 0, 300, WINDOW_H};
    SDL_SetRenderDrawColor(app->renderer, 220, 0, 220, 255);
    SDL_RenderFillRect(app->renderer, &right);

    if (app->title_tex) {
        SDL_Rect dst = {300 + (300 - app->title_w) / 2, 30, app->title_w, app->title_h};
        SDL_RenderCopy(app->renderer, app->title_tex, NULL, &dst);
    }

    if (app->keygen_tex) {
        SDL_Rect dst = {300 + (300 - app->keygen_w) / 2, 360, app->keygen_w, app->keygen_h};
        SDL_RenderCopy(app->renderer, app->keygen_tex, NULL, &dst);
    }

    ButtonDraw(&app->btn_choose, app->renderer);
    ButtonDraw(&app->btn_crack,  app->renderer);
    ButtonDraw(&app->btn_keygen, app->renderer);

    if (app->success_image) {
        SDL_Rect dst = {50, 50, 200, 200};
        SDL_RenderCopy(app->renderer, app->success_image, NULL, &dst);
    }

    SDL_RenderPresent(app->renderer);
}

void AppDestroy(App *app) {
    assert(app);

    ButtonDestroy(&app->btn_choose);
    ButtonDestroy(&app->btn_crack);
    ButtonDestroy(&app->btn_keygen);

    if (app->title_tex) {
        SDL_DestroyTexture(app->title_tex);
    }

    if (app->bg_image) {
        SDL_DestroyTexture(app->bg_image);
    }

    if (app->success_image) {
        SDL_DestroyTexture(app->success_image);
    }

    if (app->music) { 
        Mix_HaltMusic(); 
        Mix_FreeMusic(app->music); 
    }

    if (app->keygen_tex) {
        SDL_DestroyTexture(app->keygen_tex);
    }

    TTF_CloseFont(app->font);
    SDL_DestroyRenderer(app->renderer);
    SDL_DestroyWindow(app->window);
    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}