#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

long least = 0;
long huge = 0;
int day = 0;
char rec_fpath[20000][260]; // 存储递归文件路径
char user_fpath[200][260]; // 存储用户输入文件路径
int rec_top = 0, user_top = 0;
int rec_id = 0, user_id = 0; // 分别控制递归和用户输入文件路径

// 限制size或者modify_time打印
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
    char dest[50] = "";
    int flag = 0; // 显式地终止命令选项分析
    int is_rec = 0, is_A = 0, is_least = 0, is_huge = 0, is_modify = 0;
    if (argc >= 2)
    {
        int i = 1;
        while (i < argc) //处理多个目录以及各种参数
        {
            if (argv[i][0] == '-' && flag == 0)
            {
                if (argv[i][1] == 'r')
                {
                    if(strlen(argv[i]) > 2)
                    {
                        printf("'-r' option error\n");
                        return 0;
                    }
                    is_rec = 1;
                    i++;
                }
                else if (argv[i][1] == 'a')
                {
                    if (strlen(argv[i]) > 2)
                    {
                        printf("'-a' option error\n");
                        return 0;
                    }
                    is_A = 1;
                    i++;
                }
                else if (argv[i][1] == 'l')
                {
                    is_least = 1;
                    if (strlen(argv[i]) > 2)    // 处理"-l20"这类命令选项
                    {
                        strcpy(dest, &(argv[i][2]));
                        if (!is_digit(dest))
                        {
                            printf("'-l' option have an error arg\n");
                            return 0;
                        }
                        else
                        {
                            least = atoi(dest);
                            i += 1;
                            continue;
                        }
                    }
                    if (i + 1 == argc || !is_digit(argv[i + 1]))    // 缺少参数或者参数出错
                    {
                        printf("'-l' option missing a arg or arg error\n");
                        return 0;
                    }
                    least = atoi(argv[i + 1]);
                    i += 2;
                }
                else if (argv[i][1] == 'h')
                {
                    is_huge = 1;
                    if (strlen(argv[i]) > 2)    // 处理"-h20"这类命令选项
                    {
                        strcpy(dest, &(argv[i][2]));
                        if (!is_digit(dest))
                        {
                            printf("'-h' option have an error arg\n");
                            return 0;
                        }
                        else
                        {
                            huge = atoi(dest);
                            i += 1;
                            continue;
                        }
                    }
                    if (i + 1 == argc || !is_digit(argv[i + 1]))    // 缺少参数或者参数出错
                    {
                        printf("'-h' option missing a arg or arg error\n");
                        return 0;
                    }
                    huge = atoi(argv[i + 1]);
                    i += 2;
                }
                else if (argv[i][1] == 'm')
                {
                    is_modify = 1;
                    if (strlen(argv[i]) > 2)    // 处理"-l20"这类命令选项
                    {
                        strcpy(dest, &(argv[i][2]));
                        if (!is_digit(dest))
                        {
                            printf("'-m' option have an error arg\n");
                            return 0;
                        }
                        else
                        {
                            day = atoi(dest);
                            i += 1;
                            continue;
                        }
                    }
                    if (i + 1 == argc || !is_digit(argv[i + 1]))    // 缺少参数或者参数出错
                    {
                        printf("'-m' option missing a arg or arg error\n");
                        return 0;
                    }
                    day = atoi(argv[i + 1]);
                    i += 2;
                }
                else if (argv[i][1] == '-')
                {
                    if (strlen(argv[i]) > 2) // -----------------处理长选项
                    {
                        if(strcmp(argv[i], "--least") == 0) //-------------least
                        {
                            is_least = 1;
                            if (i + 1 == argc || !is_digit(argv[i + 1])) // 缺少参数或者参数出错
                            {
                                printf("'--least' option missing a arg or arg error\n");
                                return 0;
                            }
                            least = atoi(argv[i + 1]);
                            i += 2;
                            continue;
                        }
                        else if (strstr(argv[i], "--least=") != NULL)   //-------------least=
                        {
                            is_least = 1;
                            strcpy(dest, &(argv[i][8]));
                            if (!is_digit(dest))
                            {
                                printf("'--least' option have an error arg\n");
                                return 0;
                            }
                            else
                            {
                                least = atoi(dest);
                                i += 1;
                                continue;
                            }
                        }
                        else if (strcmp(argv[i], "--huge") == 0)   //--------------huge
                        {
                            is_huge = 1;
                            if (i + 1 == argc || !is_digit(argv[i + 1])) // 缺少参数或者参数出错
                            {
                                printf("'--huge' option missing a arg or arg error\n");
                                return 0;
                            }
                            huge = atoi(argv[i + 1]);
                            i += 2;
                            continue;
                        }
                        else if (strstr(argv[i], "--huge=") != NULL)    //------------huge=
                        {
                            is_huge = 1;
                            strcpy(dest, &(argv[i][7]));
                            if (!is_digit(dest))
                            {
                                printf("'--huge' option have an error arg\n");
                                return 0;
                            }
                            else
                            {
                                huge = atoi(dest);
                                i += 1;
                                continue;
                            }
                        }
                        else if (strcmp(argv[i], "--modify") == 0)   //--------------modify
                        {
                            is_modify = 1;
                            if (i + 1 == argc || !is_digit(argv[i + 1])) // 缺少参数或者参数出错
                            {
                                printf("'--modify' option missing a arg or arg error\n");
                                return 0;
                            }
                            day = atoi(argv[i + 1]);
                            i += 2;
                            continue;
                        }
                        else if (strstr(argv[i], "--modify=") != NULL)   //---------modify=
                        {
                            is_modify = 1;
                            strcpy(dest, &(argv[i][9]));
                            if (!is_digit(dest))
                            {
                                printf("'--modify' option have an error arg\n");
                                return 0;
                            }
                            else
                            {
                                day = atoi(dest);
                                i += 1;
                                continue;
                            }
                        }
                        else if (strcmp(argv[i], "--recursive") == 0) //--------------recursive
                        {
                            is_rec = 1;
                            i ++;
                            continue;
                        }
                        else if (strcmp(argv[i], "--all") == 0) //--------------all
                        {
                            is_A = 1;
                            i ++;
                            continue;
                        }
                        else
                        {
                            printf("list: invalid option '%s'\n", argv[i]);
                            return 0;
                        }
                    }
                    flag = 1;
                    i++;
                }
				else
				{
					printf("list: invalid option '%s'\n", argv[i]);
					return 0;
                }
				continue;
            }
            strcpy(user_fpath[user_top++], argv[i]);    // 存储用户输入的所有路径
            i++;
        }
        while (user_id < user_top)  // 遍历用户输入的所有路径
        {
            wd = user_fpath[user_id];
            stat(wd, &st); 
			if (!S_ISDIR(st.st_mode))
            {
                if (st.st_size == 0)
                    printf("Failed! %s not exist.\n", wd);
                else
                    limit_print(argv[optind], st.st_size, st.st_mtime, is_least, is_huge, is_modify, t);
            }
            else
            {
                printf("%s:\n", wd);
                printdir(wd, is_rec, is_A, is_least, is_huge, is_modify, t);
            }
            user_id ++;
        }
        if(strcmp(wd, ".") == 0 && user_top == 0)   // 没有输入路径就默认.
        {
            printf("%s: \n", wd);
            printdir(wd, is_rec, is_A, is_least, is_huge, is_modify, t);
        }
    }
    else
        printdir(wd, is_rec, is_A, is_least, is_huge, is_modify, t);
    return 0;
}
