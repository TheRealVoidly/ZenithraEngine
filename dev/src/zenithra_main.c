#include "zenithra_core.h"
#include "zenithra_debug.h"

void zenithra_test_editor(struct InEngineData *engine_data_str);

int main(int argc, char *argv[]) {
    struct InEngineData *engine_data_str =
        zenithra_init(1200, 700, DEV_MODE | BACKFACE_CULLING, "JetBrainsMonoNerdFont-Regular.ttf", 28);

    zenithra_test_editor(engine_data_str);

    zenithra_destroy(engine_data_str);
    return 0;
}

void zenithra_test_editor(struct InEngineData *engine_data_str) {
    zenithra_load_object(engine_data_str, "cube.obj");

    do {
        // Game logic happens here:

        zenithra_update(engine_data_str);
    } while (!program_should_quit);
}