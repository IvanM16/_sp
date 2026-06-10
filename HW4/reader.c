#include "spbook.h"

void print_wrapped(const char *text) {
    int len = (int)strlen(text);
    int start = 0;
    while (start < len) {
        int end = start + TXT_WRAP_W;
        if (end >= len) {
            printf("%s\n", text + start);
            break;
        }
        int break_at = end;
        while (break_at > start && text[break_at] != ' ') break_at--;
        if (break_at == start) break_at = end;
        int n = break_at - start + (text[break_at] == ' ' ? 0 : 0);
        if (n > 0) printf("%.*s\n", break_at - start, text + start);
        start = break_at;
        while (start < len && text[start] == ' ') start++;
    }
}

static void print_header(const char *title) {
    printf("\n");
    printf("============================================\n");
    printf("  %s\n", title);
    printf("============================================\n\n");
}

static void print_subheader(const char *title) {
    printf("\n--------------------------------------------\n");
    printf("  %s\n", title);
    printf("--------------------------------------------\n\n");
}

void render_chapter(int ch_idx) {
    if (ch_idx < 0 || ch_idx >= total_chapters) return;
    Chapter *ch = &book[ch_idx];

    char buf[256];
    sprintf(buf, "Chapter %d  %c  %s", ch->number,
            (ch->number + '0'), ch->title);
    print_header(buf);

    for (int s = 0; s < ch->num_sections; s++) {
        Section *sec = &ch->sections[s];
        print_subheader(sec->title);
        for (int l = 0; l < sec->num_lines; l++) {
            if (sec->lines[l][0] == '\0')
                printf("\n");
            else
                print_wrapped(sec->lines[l]);
        }
    }
}

void render_section(Chapter *ch, int sec_idx) {
    if (!ch || sec_idx < 0 || sec_idx >= ch->num_sections) return;
    Section *sec = &ch->sections[sec_idx];
    print_subheader(sec->title);
    for (int l = 0; l < sec->num_lines; l++) {
        if (sec->lines[l][0] == '\0')
            printf("\n");
        else
            print_wrapped(sec->lines[l]);
    }
}

void render_toc() {
    print_header("Table of Contents");
    for (int i = 0; i < total_chapters; i++) {
        Chapter *ch = &book[i];
        printf("  [%d]  Chapter %d: %s\n", i + 1, ch->number, ch->title);
        for (int s = 0; s < ch->num_sections; s++) {
            printf("            %s\n", ch->sections[s].title);
        }
        printf("\n");
    }
    printf("  [t]  Table of Contents (this page)\n");
    printf("  [e]  Export book to text file\n");
    printf("  [h]  Export book to HTML file\n");
    printf("  [q]  Quit\n\n");
}

void interactive_reader() {
    char input[256];
    int page_size = 0;
    int current_line = 0;

    printf("\n\n");
    printf("  +--------------------------------------------------+\n");
    printf("  |                                                  |\n");
    printf("  |    S P - B O O K                                |\n");
    printf("  |    System Programming: From Source to Hardware   |\n");
    printf("  |                                                  |\n");
    printf("  |    An interactive book generated as a C program  |\n");
    printf("  |                                                  |\n");
    printf("  +--------------------------------------------------+\n");

    printf("\n  Commands:\n");
    printf("    <number>      Open chapter by number  (e.g. 3)\n");
    printf("    <ch>.<sec>    Open specific section   (e.g. 4.2)\n");
    printf("    t             Show Table of Contents\n");
    printf("    e             Export to text  (spbook.txt)\n");
    printf("    h             Export to HTML  (spbook.html)\n");
    printf("    q             Quit\n\n");

    printf("  Type 't' to see the Table of Contents, or a chapter\n");
    printf("  number (1-%d) to start reading.\n\n", total_chapters);

    while (1) {
        printf("  spbook> ");
        fflush(stdout);
        if (!fgets(input, sizeof(input), stdin)) break;

        int len = (int)strlen(input);
        while (len > 0 && (input[len-1] == '\n' || input[len-1] == '\r'))
            input[--len] = '\0';

        if (len == 0) continue;

        if (strcmp(input, "q") == 0 || strcmp(input, "Q") == 0) {
            printf("\n  Goodbye!\n\n");
            break;
        }
        if (strcmp(input, "t") == 0 || strcmp(input, "T") == 0) {
            render_toc();
            continue;
        }
        if (strcmp(input, "e") == 0 || strcmp(input, "E") == 0) {
            export_text("spbook.txt");
            continue;
        }
        if (strcmp(input, "h") == 0 || strcmp(input, "H") == 0) {
            export_html("spbook.html");
            continue;
        }

        int ch_num = 0, sec_num = -1;
        if (sscanf(input, "%d.%d", &ch_num, &sec_num) == 2) {
            if (ch_num >= 1 && ch_num <= total_chapters) {
                Chapter *ch = &book[ch_num - 1];
                if (sec_num >= 1 && sec_num <= ch->num_sections) {
                    char buf[256];
                    sprintf(buf, "Chapter %d: %s  |  %s",
                            ch->number, ch->title,
                            ch->sections[sec_num - 1].title);
                    print_header(buf);
                    render_section(ch, sec_num - 1);
                } else {
                    printf("\n  Section %d not found in Chapter %d.\n", sec_num, ch_num);
                }
            } else {
                printf("\n  Chapter %d not found.\n", ch_num);
            }
            continue;
        }

        if (sscanf(input, "%d", &ch_num) == 1) {
            if (ch_num >= 1 && ch_num <= total_chapters) {
                render_chapter(ch_num - 1);
            } else {
                printf("\n  Chapter %d not found.  Available: 1-%d\n",
                       ch_num, total_chapters);
            }
            continue;
        }

        printf("\n  Unknown command.  Try: <chap>, <chap>.<sec>, t, e, h, q\n");
    }
}

void export_text(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        printf("\n  Error: Cannot write to '%s'\n", filename);
        return;
    }

    fprintf(f, "SP-BOOK: System Programming -- From Source to Hardware\n");
    fprintf(f, "=======================================================\n\n");

    for (int i = 0; i < total_chapters; i++) {
        Chapter *ch = &book[i];
        fprintf(f, "Chapter %d: %s\n", ch->number, ch->title);
        fprintf(f, "----------------------------------------\n\n");
        for (int s = 0; s < ch->num_sections; s++) {
            Section *sec = &ch->sections[s];
            fprintf(f, "%s\n\n", sec->title);
            for (int l = 0; l < sec->num_lines; l++) {
                if (sec->lines[l][0] == '\0')
                    fprintf(f, "\n");
                else
                    fprintf(f, "%s\n", sec->lines[l]);
            }
            fprintf(f, "\n");
        }
    }
    fclose(f);
    printf("\n  Book exported to '%s'\n", filename);
}

void export_html(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        printf("\n  Error: Cannot write to '%s'\n", filename);
        return;
    }

    fprintf(f, "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n");
    fprintf(f, "<meta charset=\"UTF-8\">\n");
    fprintf(f, "<title>SP-Book: System Programming</title>\n");
    fprintf(f, "<style>\n");
    fprintf(f, "  body { font-family: 'Segoe UI', Tahoma, sans-serif; "
                "max-width: 800px; margin: 40px auto; "
                "padding: 0 20px; line-height: 1.7; "
                "color: #1a1a1a; background: #fafafa; }\n");
    fprintf(f, "  h1 { color: #2c3e50; border-bottom: 3px solid #3498db; "
                "padding-bottom: 10px; }\n");
    fprintf(f, "  h2 { color: #34495e; margin-top: 40px; "
                "border-bottom: 1px solid #ddd; padding-bottom: 5px; }\n");
    fprintf(f, "  h3 { color: #7f8c8d; margin-top: 30px; }\n");
    fprintf(f, "  pre { background: #2c3e50; color: #ecf0f1; "
                "padding: 15px; border-radius: 6px; overflow-x: auto; }\n");
    fprintf(f, "  .toc { background: #eee; padding: 20px; "
                "border-radius: 8px; margin: 20px 0; }\n");
    fprintf(f, "  .toc a { color: #2980b9; text-decoration: none; }\n");
    fprintf(f, "</style>\n</head>\n<body>\n\n");

    fprintf(f, "<h1>SP-Book: System Programming</h1>\n");
    fprintf(f, "<p><em>From Source Code to Hardware -- "
                "A Journey Through the Machine</em></p>\n\n");

    fprintf(f, "<div class=\"toc\">\n<h2>Table of Contents</h2>\n<ol>\n");
    for (int i = 0; i < total_chapters; i++) {
        Chapter *ch = &book[i];
        fprintf(f, "  <li><a href=\"#ch%d\">Chapter %d: %s</a></li>\n",
                ch->number, ch->number, ch->title);
    }
    fprintf(f, "</ol>\n</div>\n\n");

    for (int i = 0; i < total_chapters; i++) {
        Chapter *ch = &book[i];
        fprintf(f, "<h2 id=\"ch%d\">Chapter %d: %s</h2>\n",
                ch->number, ch->number, ch->title);
        for (int s = 0; s < ch->num_sections; s++) {
            Section *sec = &ch->sections[s];
            fprintf(f, "<h3>%s</h3>\n", sec->title);
            for (int l = 0; l < sec->num_lines; l++) {
                if (sec->lines[l][0] == '\0')
                    fprintf(f, "<br>\n");
                else
                    fprintf(f, "<p>%s</p>\n", sec->lines[l]);
            }
        }
        fprintf(f, "\n");
    }

    fprintf(f, "\n<hr>\n<p><em>Generated by SP-Book -- "
                "a C program that IS its own documentation.</em></p>\n");
    fprintf(f, "</body>\n</html>\n");
    fclose(f);
    printf("\n  Book exported to '%s'\n", filename);
}
