#ifndef SP_BOOK_H
#define SP_BOOK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SECTIONS    64
#define MAX_CHAPTERS    32
#define MAX_LINE_LEN    256
#define TXT_WRAP_W      72

typedef struct {
    char title[128];
    int  num_lines;
    char lines[200][MAX_LINE_LEN];
} Section;

typedef struct {
    int   number;
    char  title[128];
    int   num_sections;
    Section sections[MAX_SECTIONS];
} Chapter;

extern Chapter book[];
extern int total_chapters;

void load_book();
void print_wrapped(const char *text);
void render_chapter(int ch_idx);
void render_section(Chapter *ch, int sec_idx);
void render_toc();
void interactive_reader();
void export_html(const char *filename);
void export_text(const char *filename);

#endif
