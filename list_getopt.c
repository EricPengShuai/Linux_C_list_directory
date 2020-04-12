#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <getopt.h>

long least = 0;
long huge = 0;
int day = 0;
char rec_fpath[20000][260]; // 存储递归文件路径
char user_fpath[200][260];  // 存储用户输入文件路径
int rec_top = 0, user_top = 0;
int rec_id = 0, user_id = 0;    // 分别控制递归和用户输入文件路径

void limit_print(char *d, long st_size, long mtime, int is_least, int is_huge, int is_modify, long t)
{
    if (is_least && is_huge) //是否文件大小限制
    {
        if (st_size >= least && st_size <= huge)
            if (is_modify && t - mtime <= day * 86400) //是否有时间限制
                printf("%15ld \t %s\n", st_size, d);
            else if (is_modify == 0)
                printf("%15ld \t %s\n", st_size, d);
            else
                ;
        else
            ;
    }
    else if (is_least)
    {
        if (st_size >= least)
            if (is_modify && t - mtime <= day * 86400)
                printf("%15ld \t %s\n", st_size, d);
            else if (is_modify == 0)
                printf("%15ld \t %s\n", st_size, d);
            else
                ;
        else
            ;
    }
    else if (is_huge)
    {
        if (st_size <= huge)
            if (is_modify && t - mtime <= day * 86400)
                printf("%15ld \t %s\n", st_size, d);
            else if (is_modify == 0)
                printf("%15ld \t %s\n", st_size, d);
            else
                ;
        else
            ;
    }
    else
        printf("%15ld \t %s\n", st_size, d);
}

void printdir(char *dir, int is_rec, int is_A, int is_least, int is_huge, int is_modify, long t)
{
    struct dirent *entry; //文件信息结构体
    DIR *dp;              //类似于FILE，存放目录有关信息
    struct stat statbuf;  //文件信息

    if ((dp = opendir(dir)) == NULL)
    {
        printf("Can't open directory %s\n", dir);
        return;
    }
    else
    {
        while ((entry = readdir(dp)) != NULL)
        {
            if (entry->d_name[0] == '.' && is_A == 0)
                continue;
            char d[260]; //获取目录的绝对路径
            strcpy(d, dir);
            strcat(d, "/");
            strcat(d, entry->d_name);

            stat(d, &statbuf);
            if (is_rec) //是否递归打印
                if (S_ISDIR(statbuf.st_mode) && entry->d_name[strlen(entry->d_name) - 1] != '.')
                    strcpy(rec_fpath[rec_top++], d);

            /* if (is_least && is_huge) //是否文件大小限制
            {
                if (statbuf.st_size >= least && statbuf.st_size <= huge)
                    if (is_modify && t - statbuf.st_mtime <= day * 86400) //是否有时间限制
                        printf("%15ld \t %s\n", statbuf.st_size, d);
                    else if (is_modify == 0)
                        printf("%15ld \t %s\n", statbuf.st_size, d);
                    else
                        ;
                else
                    ;
            }
            else if (is_least)
            {
                if (statbuf.st_size >= least)
                    if (is_modify && t - statbuf.st_mtime <= day * 86400)
                        printf("%15ld \t %s\n", statbuf.st_size, d);
                    else if (is_modify == 0)
                        printf("%15ld \t %s\n", statbuf.st_size, d);
                    else
                        ;
                else
                    ;
            }
            else if (is_huge)
            {
                if (statbuf.st_size <= huge)
                    if (is_modify && t - statbuf.st_mtime <= day * 86400)
                        printf("%15ld \t %s\n", statbuf.st_size, d);
                    else if (is_modify == 0)
                        printf("%15ld \t %s\n", statbuf.st_size, d);
                    else
                        ;
                else
                    ;
            }
            else
                printf("%15ld \t %s\n", statbuf.st_size, d); */
            limit_print(d, statbuf.st_size, statbuf.st_mtime, is_least, is_huge, is_modify, t);
        }
        while (rec_id < rec_top)
        {
            if (!is_least && !is_huge)
                printf("%s:\n", rec_fpath[rec_id]);
            printdir(rec_fpath[rec_id++], is_rec, is_A, is_least, is_huge, is_modify, t);
        }
    }
    closedir(dp);
    return;
}

// 判断字符串是否全是数字
int is_digit(char *str)
{
    return (strspn(str, "0123456789") == strlen(str));
}

int main(int argc, char *argv[])
{
    time_t t = time(0);
    struct stat st;
    char *wd = ".";
    int is_rec = 0, is_A = 0, is_least = 0, is_huge = 0, is_modify = 0;
    
    int opt;
    int option_id = 0;  // argv参数下标
    char *optstring = "ral:h:m:";
    static struct option long_options[] =
    {
        {"recursive", no_argument, NULL, 'r'},
        {"all", no_argument, NULL, 'a'},
        {"least", required_argument, NULL, 'l'},
        {"huge", required_argument, NULL, 'h'},
        {"modify", required_argument, NULL, 'm'},
        {0, 0, 0 ,0}
    };
    
    while((opt = getopt_long(argc, argv, optstring, long_options, &option_id)) != -1)
    {
        // printf("opt = %c\n", opt);
        // printf("optarg = %s\n", optarg);
        // printf("optind = %d\n", optind);
        // printf("argv[optind-1] = %s\n", argv[optind - 1]);
        // printf("option_idnex = %d\n\n", option_id);
        if (opt == 'r')
        {
            is_rec = 1;
        }
        else if (opt == 'a')
        {
            is_A = 1;
        }
        else if (opt == 'l')
        {
            is_least = 1;
            least = atoi(optarg);
            if (!is_digit(optarg))  //命令错误...-lk
            {
                printf("'-l/--least' option missing a arg or arg error\n");
                is_least = 0;
                return 0;
            }
        }
        else if (opt == 'h')
        {
            is_huge = 1;
            huge = atoi(optarg);
            if (!is_digit(optarg)) //命令错误...-hk
            {
                printf("'-h/--huge' option missing a arg or arg error\n");
                return 0;
            }
        }
        else if (opt == 'm')
        {
            is_modify = 1;
            day = atoi(optarg);
            if (!is_digit(optarg)) //命令错误...-mk
            {
                printf("'-m/--modify' option missing a arg or arg error\n");
                return 0;
            }
        }
        else    //无法识别的option
        {
            printf("Invalid option for list\n");
            return 0;
        }
    }

    //printf("%d %d %d\n", optind, user_id, user_top);
    while(optind < argc)    //处理用户输入路径名
    {
        strcpy(user_fpath[user_top++], argv[optind]);
        stat(argv[optind], &st);
        if (!S_ISDIR(st.st_mode))
        {
            if (st.st_size == 0)
                printf("Failed! %s not exist.\n", argv[optind]);
            else
                // printf("%15ld \t %s\n", st.st_size, argv[optind]);
                limit_print(argv[optind], st.st_size, st.st_mtime, is_least, is_huge, is_modify, t);
        }
        else
        {
            printf("%s:\n", argv[optind]);
            printdir(argv[optind], is_rec, is_A, is_least, is_huge, is_modify, t);
        }
        optind ++;
    }
    //printf("%d %d %d\n", optind, user_id, user_top);

    if (strcmp(wd, ".") == 0 && user_top == 0)  // 默认输入
    {
        printf("%s: \n", wd);
        printdir(wd, is_rec, is_A, is_least, is_huge, is_modify, t);
    }
    return 0;
}
