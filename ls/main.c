#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#define COLOR_RESET "\033[0m"
#define COLOR_BLUE "\033[34m"
#define COLOR_GREEN "\033[32m"
#define COLOR_CYAN "\033[36m"


void display_file_info(const char *path, const char *name, struct stat *file_stat, int detailed) {
    char link_target[1024]; 
    ssize_t link_len;

    if (detailed) {
       
        printf((S_ISDIR(file_stat->st_mode)) ? "d" : (S_ISLNK(file_stat->st_mode)) ? "l" : "-");
        printf((file_stat->st_mode & S_IRUSR) ? "r" : "-");
        printf((file_stat->st_mode & S_IWUSR) ? "w" : "-");
        printf((file_stat->st_mode & S_IXUSR) ? "x" : "-");
        printf((file_stat->st_mode & S_IRGRP) ? "r" : "-");
        printf((file_stat->st_mode & S_IWGRP) ? "w" : "-");
        printf((file_stat->st_mode & S_IXGRP) ? "x" : "-");
        printf((file_stat->st_mode & S_IROTH) ? "r" : "-");
        printf((file_stat->st_mode & S_IWOTH) ? "w" : "-");
        printf((file_stat->st_mode & S_IXOTH) ? "x" : "-");

        
        printf(" %-5ld", file_stat->st_nlink);


        struct passwd *pw = getpwuid(file_stat->st_uid);
        if (pw != NULL) {
            printf(" %-10s", pw->pw_name);
        } else {
            printf(" %-10d", file_stat->st_uid);
        }

        struct group *gr = getgrgid(file_stat->st_gid);
        if (gr != NULL) {
            printf(" %-10s", gr->gr_name);
        } else {
            printf(" %-10d", file_stat->st_gid);
        }

        printf(" %8ld", file_stat->st_size);

        char time_buff[20];
        strftime(time_buff, sizeof(time_buff), "%b %d %H:%M", localtime(&file_stat->st_mtime));
        printf(" %10s", time_buff);
    }


    if (S_ISDIR(file_stat->st_mode)) {
        printf(" %s%s%s\n", COLOR_BLUE, name, COLOR_RESET);
    }
    else if ((file_stat->st_mode & S_IXUSR) && !S_ISLNK(file_stat->st_mode)) {
        printf(" %s%s%s\n", COLOR_GREEN, name, COLOR_RESET);
    }
    else if ((file_stat->st_mode & S_IXUSR) && S_ISLNK(file_stat->st_mode)) {
        // Читаем, на что указывает символическая ссылка
        link_len = readlink(path, link_target, sizeof(link_target) - 1);
        if (link_len != -1) {
            link_target[link_len] = '\0';
            printf(" %s%s%s -> %s\n", COLOR_CYAN, name, COLOR_RESET, link_target);
        } else {
            printf(" %s%s%s\n", COLOR_CYAN, name, COLOR_RESET);
        }
    } else if (file_stat->st_mode & S_IXUSR) {
        // читаю, на что указывает символическая ссылка
        link_len = readlink(path, link_target, sizeof(link_target) - 1);
        if (link_len != -1) {
            link_target[link_len] = '\0'; 
            printf(" %s%s%s -> %s\n", COLOR_CYAN, name, COLOR_RESET, link_target);
        } else {
            printf(" %s%s%s\n", COLOR_CYAN, name, COLOR_RESET);
        }
    } else {
        printf(" %s\n", name);
    }
}

int sort_files(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

int main(int argc, char *argv[]) {
    int opt;
    int show_detailed = 0, show_all = 0;

    while ((opt = getopt(argc, argv, "la")) != -1) {
        switch (opt) {
            case 'l':
                show_detailed = 1; 
                break;
            case 'a':
                show_all = 1; 
                break;
            default:
                fprintf(stderr, "Использование: %s [-l] [-a] [директория]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    const char *directory = (optind < argc) ? argv[optind] : ".";

    
    DIR *dir = opendir(directory);
    if (!dir) {
        perror("Ошибка при открытии директории");
        return EXIT_FAILURE;
    }

    struct dirent *entry;
    char **file_list = NULL;
    size_t num_files = 0;
    long total_blocks = 0;

    
    while ((entry = readdir(dir)) != NULL) {
        if (!show_all && entry->d_name[0] == '.') {
            continue; 
        }

        
        file_list = realloc(file_list, sizeof(char *) * (num_files + 1));
        if (!file_list) {
            perror("Ошибка выделения памяти");
            closedir(dir);
            return EXIT_FAILURE;
        }
        file_list[num_files] = strdup(entry->d_name);
        num_files++;

        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);
        struct stat file_stat;
        if (lstat(path, &file_stat) == -1) {
            perror("Ошибка получения информации о файле");
            free(file_list[num_files - 1]);
            num_files--;
            continue;
        }
        total_blocks += file_stat.st_blocks;
    }
    closedir(dir);

    
    qsort(file_list, num_files, sizeof(char *), sort_files);

    printf("total %ld\n", total_blocks);

    for (size_t i = 0; i < num_files; i++) {
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", directory, file_list[i]);

        struct stat file_stat;
        if (lstat(path, &file_stat) == -1) {
            perror("Ошибка получения информации о файле");
            free(file_list[i]);
            continue;
        }

        display_file_info(path, file_list[i], &file_stat, show_detailed);
        free(file_list[i]);
    }
    free(file_list);

    return 0;
}
