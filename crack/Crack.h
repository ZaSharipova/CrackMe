#ifndef CRACK_H_
#define CRACK_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

typedef struct {
    SDL_Rect rect;
    const char *label;
    int hovered;
    SDL_Texture *tex;
    int tex_w, tex_h;
} Button;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;

    Button btn_choose;
    Button btn_crack;
    Button btn_keygen;

    SDL_Texture *bg_image;
    SDL_Texture *title_tex;
    int title_w, title_h;

    SDL_Texture *success_image;
    Mix_Music *music;

    int patched;
    int running;

    char keygen_result[32];
    SDL_Texture *keygen_tex;
    int keygen_w, keygen_h;

    int showBg;
} App;

void GenerateKey(char *buf, int buf_size);
void ButtonCreateTexture(Button *btn, SDL_Renderer *renderer, TTF_Font *font);
int  ButtonHit(Button *btn, int x, int y);
void ButtonDraw(Button *btn, SDL_Renderer *renderer);
void ButtonDestroy(Button *btn);
int  PatchCom(const char *path);
int  AppInitSDL(App *app);
void AppCreateButtons(App *app);
void AppLoadAssets(App *app);
int  OpenFileDialog(char *buf, int buf_size);
void AppOnCrack(App *app);
void AppOnChoose(App *app);
void AppOnKeygen(App *app);
void AppHandleEvents(App *app);
void AppRender(App *app);
void AppDestroy(App *app);

#endif //CRACK_H_