#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "Crack.h"

int main(void) {
    App app = {0};
    strcpy(app.com_file,  "CRACKME.COM");
    app.good_file = 1;
    app.running = 1;
    app.showBg = 1;

    if (!AppInitSDL(&app)) {
        return 1;
    }

    AppCreateButtons(&app);
    AppLoadAssets(&app);

    while (app.running) {
        int mx = 0, my = 0;
        SDL_GetMouseState(&mx, &my);
        app.btn_choose.hovered = ButtonHit(&app.btn_choose, mx, my);
        app.btn_crack.hovered  = ButtonHit(&app.btn_crack,  mx, my);
        app.btn_keygen.hovered = ButtonHit(&app.btn_keygen, mx, my);

        AppHandleEvents(&app);
        AppRender(&app);
    }

    AppDestroy(&app);
    return 0;
}