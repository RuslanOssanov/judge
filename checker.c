#include <stdio.h>

// checker(file){int flagi WA OK TL (ML)}
enum { size_postfix = 3};

void
make_file(char *file)
{
    char ch;
    int i = 0, j;
    do
    {
        ch = getchar();
        file[i++] = ch;
    }
    while (ch != '.');
    for (j = 0; j < size_postfix; j++)
    {
        ch = getchar();
        file[i++] = ch;
    }
}

int
main(void)
{
	char ch_ans, ch_input, *string;
	char file_name[16];
    make_file(file_name);
	FILE *file;
	short flag = 1;
	file = fopen(file_name, "r");
	ch_ans = getchar();
	ch_input = getc(file);
	while (ch_ans != EOF && ch_input !=  EOF && ch_ans != '\0')
	{
		if (ch_ans != ch_input)
		{
			flag = 0;
			break;
		}
		ch_ans = getchar();
		ch_input = getc(file);
        if (ch_input == '\n' && ch_ans == EOF || ch_ans == '\n' && ch_input == EOF || 
                    ch_ans == '\n' && ch_input == '\n')
        {
            fclose(file);
            return 0;
        }
	}
	fclose(file);
	if (flag && (ch_ans == EOF && ch_input == EOF) || (ch_ans == '\0' && (ch_input == EOF || ch_input == '\n')))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
