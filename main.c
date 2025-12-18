#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

// Macro for mapping ctrl+key (ctrl+a=1, ctrl+b=2, etc)
#define CTRL_KEY(k) ((k) & 0x1f)

struct termios orig_termios;

/*** terminal ***/
void die(const char *s)
{
    perror(s);
    exit(1);
}

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCIFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    /*
    ECHO: printing to screen
    ICANON: canonical mode which processes input line by line
    ISIG: Ctrl-C and Ctrl-Z
    IXON: Ctrl-S and Ctrl-Q
    IEXTEN: Ctrl-V
    ICRNL: Input Carriage Return New Line
    OPOST: \r and \r\n
    BRKINT: something to do with break condition
    INPCK: parity checking
    ISTRIP: strips the 8th bit of each input byte
    CS8: not a flag. used to set the character size to 8 bits
    */

    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_cc[VMIN] = 0;  // sets minimum number of bytes needed for read()
    raw.c_cc[VTIME] = 1; // sets time to wait for read() to return
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

char editorReadKey()
{
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
    {
        if (nread == -1 && errno != EAGAIN)
            die("read");
    }
    return c;
}

/*** input ***/
void editorProcessKeypress()
{
    char c = editorReadKey();
    switch (c)
    {
    case CTRL_KEY('q'):
        exit(0);
        break;
    }
}

int main()
{
    enableRawMode();
    while (1)
    {
        editorProcessKeypress();
    }
    return 0;
}