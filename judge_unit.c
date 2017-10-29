#include <stdio.h>
#include <wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


enum { size_user = 10, size_goal = 4, size_path = 6, size_4_run = 4, size_test = 4, size_name_catalog = 6};


void
parse_user_goal(char *ans_1, char *ans_2, char *str)
{
    int i = 0, j = 0;
    while (str[i++] != '/')
        ;
    while (str[i] != '/')
    {
        ans_1[j++] = str[i++];
    }
    ans_1[j++] = '_';
    ans_1[j] = '\0';
    i++;
    j = 0;
    while (str[i] != '.')
    {
        ans_2[j++] = str[i++];
    }
    ans_2[j] = '\0';
}

char *
read_file(char *str, FILE *file)
{
    char tmp_ch;
    int i = 0;
    while ((tmp_ch = fgetc(file)) != EOF )
    {
        str = (char *)realloc(str, sizeof(char) * (i + 1));
        str[i++] = tmp_ch;
    }
    str[i] = '\0';
    return str;
}

void
merge_str(char *arg1, char *arg2, char *ans)
{
    int len1 = strlen(arg1);
    int len2 = strlen(arg2);
    int len_ans, i;
    for (len_ans = 0; len_ans < len1; len_ans++)
    {
        ans[len_ans] = arg1[len_ans];
    }
    for (i = 0; i < len2; i++)
    {
        ans[len_ans++] = arg2[i];
    }
    ans[len_ans] = '\0';
}


void
create_file_str(char *goal, int num_test, char *ans,  char *flag)
{
    char num_str[size_test];
    sprintf(ans, "test/%s/%03d", goal, num_test);
    if (!strcmp(flag, ".dat"))
    {
        merge_str(ans, ".dat", ans);
    }
    else
    {
        merge_str(ans, ".ans", ans);
    }
}

int
compute_result(int count_test, char *goal, char *path_4_run)
{
    int num_test, result = 0;
    for (num_test = 1; num_test <= count_test; num_test++)
    {
        char file_dat[size_name_catalog + size_goal + size_test];
        create_file_str(goal, num_test, file_dat, ".dat");
        FILE *ans = fopen(file_dat, "r");
        char *ans_str;
        ans_str = NULL;
        ans_str = read_file(ans_str, ans);
        fclose(ans);
        int status_checker, pid_file;
        int file_prog[2];
        pipe(file_prog);
        if ((pid_file = fork()) == 0)
        {
            dup2(file_prog[0], 0);
            close(file_prog[1]);
            int file_checker[2], pid_check;
            pipe(file_checker);
            if ((pid_check = fork()) != 0)
            {
                waitpid(pid_check, NULL, 0);
                dup2(file_checker[0], 0);
                close(file_checker[1]);
                execlp("./checker", "./checker", NULL);
            }
            else
            {
                dup2(file_checker[1], 1);
                char file_ans[strlen(file_dat)];
                create_file_str(goal, num_test, file_ans, ".ans");
                write(file_checker[1], file_ans, sizeof(file_ans));
                close(file_checker[0]);
                execlp(path_4_run, path_4_run, NULL);
            }
        }
        else
        {
            close(file_prog[0]);
            write(file_prog[1], ans_str, sizeof(ans_str)); // запись в трубку
            waitpid(pid_file, &status_checker, 0);
            close(file_prog[1]);
        }
        if (status_checker == 0)
        {
            result++;
        }
    }
    return result;
}

void
clear_dir_var(char *str)
{
    if (!fork())
    {
        execlp("rm", "rm", str, NULL);
    }
    else
    {
        wait(NULL);   
    }
}

int
main(int argc, char *argv[])
{
    char  user[size_user], goal[size_goal];
    int count_test = atoi(argv[2]);
    char user_goal[size_user + size_goal];
    parse_user_goal(user, goal, argv[1]);

    merge_str(user, goal, user_goal);

    char path[size_path + size_goal + size_user];
    merge_str("var/", user_goal, path);

    char path_4_run[strlen(path) + size_4_run];
    merge_str("./", path, path_4_run);

    int status_gcc;
    FILE *somestream = fopen("log", "w");
    int fd = fileno(somestream);
    if (!fork())
    {
        dup2(fd, 2);
        execlp("gcc", "gcc", argv[1], "-o", path, NULL);
        perror("execlp");
        return 0;
    }
    wait(&status_gcc);
    if (status_gcc != 0)
    {
        return 0;
    }

    int result = compute_result(count_test, goal, path_4_run);
    clear_dir_var(path);

    return result;
}
