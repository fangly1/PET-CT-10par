#include <stdio.h>
#include <string.h>    
#include <stdlib.h>    
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

//获取指定目录下的所有文件列表
char** GetFileNameList(const char *path, unsigned int* FileCount)  
{  
    int count = 0;  
    char **fileNameList = NULL;  
    struct dirent* ent = NULL;  
    DIR *pDir;  
    char dir[512];  
    struct stat statbuf;  
  
    if(!path || !FileCount)
    {
        return NULL;
    }

    //open directory
    if ((pDir = opendir(path)) == NULL)  
    {  
        printf("Cannot open directory:%s\n", path);  
        return NULL;  
    }  
    //read directory
    while ((ent = readdir(pDir)) != NULL)  
    { //统计当前文件夹下有多少文件（不包括文件夹）  
        //得到读取文件的绝对路径名  
        snprintf(dir, 512, "%s/%s", path, ent->d_name);  
        //得到文件信息  
        lstat(dir, &statbuf);  
        //判断是目录还是文件  
        if (!S_ISDIR(statbuf.st_mode))  
        {  
            count++;  
        }  
    } 
    //close directory
    closedir(pDir);  
   
    //开辟字符指针数组，用于下一步的开辟容纳文件名字符串的空间  
    if ((fileNameList = (char**) malloc(sizeof(char*) * count)) == NULL)  
    {  
        printf("Malloc heap failed!\n");  
        return NULL;  
    }  
  
    //open directory
    if ((pDir = opendir(path)) == NULL)  
    {  
        printf("Cannot open directory:%s\n", path);  
        free((void*)fileNameList);
        return NULL;  
    }  
    //read directory
    int i;  
    for (i = 0; (ent = readdir(pDir)) != NULL && i < count;)  
    {  
        if (strlen(ent->d_name) <= 0)  
        {  
            continue;  
        }  
        //得到读取文件的绝对路径名  
        snprintf(dir, 512, "%s/%s", path, ent->d_name);  
        //得到文件信息  
        lstat(dir, &statbuf);  
        //file or directory 
        if (!S_ISDIR(statbuf.st_mode))  
        {  
            if ((fileNameList[i] = (char*) malloc(strlen(ent->d_name) + 1)) == NULL)  
            {  
                printf("malloc heap failed!\n");  
                //free memory
                for(int j = 0; j < i; j++)
                {
                    free((void*)(fileNameList[j]));
                }
                free((void*)fileNameList);
                closedir(pDir);
                return NULL;  
            }  
            memset(fileNameList[i], 0, strlen(ent->d_name) + 1);  
            strcpy(fileNameList[i], ent->d_name);  
            i++;  
        }  
    } //for  
    //close directory
    closedir(pDir);  
  
    *FileCount = count;

    return fileNameList;  
}

void FileNameListFree(const char **FileList, unsigned int FileCount)
{
    if(!FileList || !FileCount)
    {
        return;
    }

    for(int i=0; i < FileCount; i++)
    {
        if(FileList[i])
        {
            free((void*)(FileList[i]));
        }
    }

    free((void*)FileList);
}

#if 0
int main(int argc, char* argv[])
{
    int count;
    char **FileList;
    if(argc < 2)
    {
        printf("[%s:%d]cmdline param error!\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    FileList = GetFileNameList(argv[1], &count);
    if(!FileList)
    {
        printf("[%s:%d]GetFileNameList error!\r\n", __FUNCTION__, __LINE__);
        return -1;        
    }

    for(int i=0; i<count; i++)
    {
        printf("[%d]:%s\r\n", i, FileList[i]);
    }

    FileNameListFree((const char **)FileList, count);

    return 0;
}
#endif