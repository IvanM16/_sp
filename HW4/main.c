#include "spbook.h"

int main(int argc, char *argv[]) {
    load_book();

    if (argc >= 2) {
        if (strcmp(argv[1], "--toc") == 0 || strcmp(argv[1], "-t") == 0) {
            render_toc();
        } else if (strcmp(argv[1], "--text") == 0 || strcmp(argv[1], "-e") == 0) {
            const char *out = argc >= 3 ? argv[2] : "spbook.txt";
            export_text(out);
        } else if (strcmp(argv[1], "--html") == 0 || strcmp(argv[1], "-m") == 0) {
            const char *out = argc >= 3 ? argv[2] : "spbook.html";
            export_html(out);
        } else {
            int ch = atoi(argv[1]);
            if (ch >= 1 && ch <= total_chapters) {
                render_chapter(ch - 1);
            } else {
                printf("Chapter not found. Available: 1-%d\n", total_chapters);
            }
        }
    } else {
        interactive_reader();
    }

    return 0;
}
