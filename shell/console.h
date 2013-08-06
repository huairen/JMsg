#ifndef CONSOLE_H_
#define CONSOLE_H_

void console_clear_colors(int ForgC, int BackC);
void console_clear();
void console_clear_line(int line);
void console_goto_xy(int x, int y);
void console_set_text_color(int ForgC);
void console_set_colors(int ForgC, int BackC);
void console_print(char *CharBuffer, int len);
void console_print_at(int x, int y, char *CharBuffer, int len);
void console_cursor_hide();
void console_cursor_show();

#endif