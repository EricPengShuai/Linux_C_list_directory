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
char rec_fpath[20000][260]; // �洢�ݹ��ļ�·��
char user_fpath[200][260]; // �洢�û������ļ�·��
int rec_top = 0, user_top = 0;
int rec_id = 0, user_id = 0; // �ֱ���Ƶݹ���û������ļ�·��

// ����size����modify_time��ӡ
void limit_print(char *d, long st_size, long mtime, int is_least, int is_huge, int is_modify, long t)
{
    if (is_least && is_huge) //�Ƿ��ļ���С����
    {
        if (st_size >= least && st_size <= huge)
            if (is_modify && t - mtime <= day * 86400) //�Ƿ���ʱ������
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
    struct dirent *entry; //�ļ���Ϣ�ṹ��
    DIR *dp;              //������FILE�����Ŀ¼�й���Ϣ
    struct stat statbuf;  //�ļ���Ϣ

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
            char d[260]; //��ȡĿ¼�ľ���·��
            strcpy(d, dir);
            strcat(d, "/");
            strcat(d, entry->d_name);

            stat(d, &statbuf);
            if (is_rec) //�Ƿ�ݹ��ӡ
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

// �ж��ַ����Ƿ�ȫ������
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
    int flag = 0; // ��ʽ����ֹ����ѡ�����
    int is_rec = 0, is_A = 0, is_least = 0, is_huge = 0, is_modify = 0;
    if (argc >= 2)
    {
        int i = 1;
        while (i < argc) //������Ŀ¼�Լ����ֲ���
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
                    if (strlen(argv[i]) > 2)    // ����"-l20"��������ѡ��
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
                    if (i + 1 == argc || !is_digit(argv[i + 1]))    // ȱ�ٲ������߲�������
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
                    if (strlen(argv[i]) > 2)    // ����"-h20"��������ѡ��
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
                    if (i + 1 == argc || !is_digit(argv[i + 1]))    // ȱ�ٲ������߲�������
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
                    if (strlen(argv[i]) > 2)    // ����"-l20"��������ѡ��
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
                    if (i + 1 == argc || !is_digit(argv[i + 1]))    // ȱ�ٲ������߲�������
                    {
                        printf("'-m' option missing a arg or arg error\n");
                        return 0;
                    }
                    day = atoi(argv[i + 1]);
                    i += 2;
                }
                else if (argv[i][1] == '-')
                {
                    if (strlen(argv[i]) > 2) // -----------------����ѡ��
                    {
                        if(strcmp(argv[i], "--least") == 0) //-------------least
                        {
                            is_least = 1;
                            if (i + 1 == argc || !is_digit(argv[i + 1])) // ȱ�ٲ������߲�������
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
                            if (i + 1 == argc || !is_digit(argv[i + 1])) // ȱ�ٲ������߲�������
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
                            if (i + 1 == argc || !is_digit(argv[i + 1])) // ȱ�ٲ������߲�������
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
            strcpy(user_fpath[user_top++], argv[i]);    // �洢�û����������·��
            i++;
        }
        while (user_id < user_top)  // �����û����������·��
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
        if(strcmp(wd, ".") == 0 && user_top == 0)   // û������·����Ĭ��.
        {
            printf("%s: \n", wd);
            printdir(wd, is_rec, is_A, is_least, is_huge, is_modify, t);
        }
    }
    else
        printdir(wd, is_rec, is_A, is_least, is_huge, is_modify, t);
    return 0;
}
