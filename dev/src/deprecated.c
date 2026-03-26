typedef struct PointBuffer {
    uint32_t X;
    uint32_t Y;

    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    struct PointBuffer *next;
} POINT_BUF;

/**
 * Draws all the pixels stored in the
 * engine_data_str->RENDERER_BUF
 **/

void zenithra_draw_points(struct InEngineData *engine_data_str) {
    void *pixels; // pixels is a pointer to the framebuffer
    int pitch;    // pitch is the number of bytes per row

    if (SDL_LockTexture(engine_data_str->frame_texture, NULL, &pixels, &pitch) == 0) {
        Uint32 *pixel_ptr = (Uint32 *)pixels;

        POINT_BUF *node = engine_data_str->POINT_BUF;
        while (node) {
            Uint32 combined = ((Uint32)node->r << 24) | ((Uint32)node->g << 16) | ((Uint32)node->b << 8) |
                ((Uint32)node->a);
            pixel_ptr[node->Y * (pitch / 4) + node->X] = combined;

            node = node->next;
        }

        SDL_UnlockTexture(engine_data_str->frame_texture);

        for (int y = 0; y < engine_data_str->renderer_Y; y++) {
            for (int x = 0; x < engine_data_str->renderer_X; x++) {
                pixel_ptr[y * (pitch / 4) + x] = 0xFFFF0000; // ARGB color example (red)
            }
        }
    }
}

/**
 * Creates the renderer buffer
 * Stores pixel coordinate and color of
 * each pixel
 **/

void zenithra_create_point_buffer(struct InEngineData *engine_data_str) {
    int count = engine_data_str->renderer_X * engine_data_str->renderer_Y;

    POINT_BUF *head = NULL;
    POINT_BUF *next_in_line = NULL;

    for (int i = 0; i < count; i++) {
        POINT_BUF *node = zenithra_malloc(engine_data_str, sizeof *node, __FILE__, __LINE__);

        node->X = i % engine_data_str->renderer_X; // Pixel
                                                   // #X left
                                                   // ->
                                                   // right
        node->Y = i / engine_data_str->renderer_X; // Pixel
                                                   // #Y up
                                                   // -> down

        node->r = 0; // Initialize all
                     // pixels as black
        node->g = 0;
        node->b = 0;
        node->a = 0;

        node->next = NULL;

        if (!head) {
            head = node;
        } else {
            next_in_line->next = node;
        }

        next_in_line = node;
    }
    engine_data_str->POINT_BUF = head;

    zenithra_log_msg("Point buffer created "
                     "successfully");
}

/**
 * Destroy the point buffer
 * Probably only when terminating the
 * engine
 **/

void zenithra_destroy_point_buffer(struct InEngineData *engine_data_str) {
    POINT_BUF *node = engine_data_str->POINT_BUF;
    POINT_BUF *next_in_line = node->next;

    zenithra_free(engine_data_str, (void **)&node, sizeof *node);

    while (next_in_line) {
        node = next_in_line;
        next_in_line = node->next;

        zenithra_free(engine_data_str, (void **)&node, sizeof *node);
    }
}

/**
 * Translate to normalized screen
 * coordinated via
 * zenithra_calculate_and_normalize_vertice()
 * and then to pixels coordinates and
 * save into
 * engine_data_str->POINT_RENDERER_BUF
 *
 * @param double Xw, double Yw, double
 * Zw world coordinates of vertice
 **/

void zenithra_save_points_for_rendering(struct InEngineData *engine_data_str,
    double Xw,
    double Yw,
    double Zw) {
    struct TempNormCoords *temp_norm_coords;
    temp_norm_coords = zenithra_normalize_vertice(engine_data_str, Xw, Yw, Zw);

    int screen_X = (int)round(temp_norm_coords->norm_X * engine_data_str->renderer_X); // Map to
                                                                                       // screen
                                                                                       // pixels
    int screen_Y = (int)round(temp_norm_coords->norm_Y * engine_data_str->renderer_Y);

    zenithra_free(engine_data_str, (void **)&temp_norm_coords, sizeof *temp_norm_coords);

    POINT_BUF *node = engine_data_str->POINT_BUF;
    while (node) {
        if (screen_X == node->X && screen_Y == node->Y) {
            node->r = 255;
        }

        node = node->next;
    }
}

/**
 * Function defined only for Linux
 * Handles real-time console input
 **/

int _kbhit() {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds); // File descriptor 0 = stdin
    return select(1, &fds, NULL, NULL, &tv);
}

/**
 * Save object or buffer into SDL framebuffer
 **/

void zenithra_draw_points(struct InEngineData *engine_data_str, SDL_Vertex *vertices, int num) {
    void *pixels; // pixels is a pointer to the framebuffer
    int pitch;    // pitch is the number of bytes per row

    if (SDL_LockTexture(engine_data_str->frame_texture, NULL, &pixels, &pitch) == 0) {
        Uint32 *pixel_ptr = (Uint32 *)pixels;
        int fourth_pitch = pitch / 4;

        for (int i = 0; i < num; i += 3) {
            Uint32 combined = ((Uint32)vertices[i].color.r << 24) | ((Uint32)vertices[i].color.g << 16) |
                ((Uint32)vertices[i].color.b << 8) | ((Uint32)vertices[i].color.a);
            pixel_ptr[(uint32_t)(vertices[i].position.y * engine_data_str->renderer_y) * fourth_pitch +
                (uint32_t)(vertices[i].position.x * engine_data_str->renderer_x)] = combined;
        }
        SDL_UnlockTexture(engine_data_str->frame_texture);
    }
}

/**
 * Called once after drawing every frame to prevent ghost pixels
 **/

void zenithra_clear_texture(struct InEngineData *engine_data_str) {
    SDL_Surface *surface;

    SDL_LockTextureToSurface(engine_data_str->frame_texture, NULL, &surface);

    SDL_FillRect(surface, NULL, SDL_MapRGBA(NULL, 0, 0, 0, 255));

    SDL_UnlockTexture(engine_data_str->frame_texture);
}