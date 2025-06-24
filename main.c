#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>

#define LINKBUF_SIZE 100

int recursive = 0;

void showInfos(char *fileName);
void printDir(char *dirName);

void printPermissions(mode_t mode) {
	char perms[10] = "---------";
	char chars[] = {'r','w','x'};
	for (int i = 0; i < 9; i++) {
		if (mode & (1 << (8 - i))) perms[i] = chars[i % 3];
	}
	printf("%s", perms);
}

void printDir(char *dirName) {
	DIR *dir;
	struct dirent *ent;

	dir = opendir(dirName);
	if (!dir) {
		perror("opendir");
		return;
	}
	printf("\n%s:\n", dirName);

	while ((ent = readdir(dir)) != NULL) {
		if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
			continue;

		char *path = malloc(strlen(dirName) + strlen(ent->d_name) + 2);
		if (!path) {
			perror("malloc");
			continue;
		}
		sprintf(path, "%s/%s", dirName, ent->d_name);
		showInfos(path);
		free(path);
	}
	closedir(dir);
}

void showInfos(char *fileName) {
	struct stat statBuffer;
	mode_t m;
	char *date;

	if (lstat(fileName, &statBuffer) != 0) {
		perror("lstat");
		return;
	}

	m = statBuffer.st_mode;

	printf("%lu\t", statBuffer.st_ino);

	printf("%c", S_ISLNK(m) ? 'l' : S_ISDIR(m) ? 'd' : S_ISCHR(m) ? 'c' : 
	       S_ISBLK(m) ? 'b' : S_ISFIFO(m) ? 'p' : S_ISSOCK(m) ? 's' : 
	       S_ISREG(m) ? '-' : '?');

	printPermissions(m);
	printf("\t");

	printf("%lu\t", statBuffer.st_nlink);

	struct passwd *pw = getpwuid(statBuffer.st_uid);
	struct group  *gr = getgrgid(statBuffer.st_gid);
	printf("%s\t%s\t", pw ? pw->pw_name : "unknown", gr ? gr->gr_name : "unknown");

	printf("%ld\t%ld\t", statBuffer.st_size, statBuffer.st_blocks);

	date = ctime(&statBuffer.st_mtime);
	if (date) {
		date[strlen(date) - 1] = '\0';
		printf("%s ", date);
	} else {
		printf("Unknown date ");
	}

	char *lastSlash = strrchr(fileName, '/');
	char *fileBaseName = lastSlash ? lastSlash + 1 : fileName;

	if (fileName[strlen(fileName) - 1] != '.') {
		printf("\x1b[32m");
	} else {
		printf("\x1b[31m");
	}
	printf("%s", fileBaseName);
	printf("\x1b[37m");

	if (S_ISLNK(m)) {
		char linkName[LINKBUF_SIZE + 1] = {0};
		ssize_t len = readlink(fileName, linkName, LINKBUF_SIZE);
		if (len != -1) {
			linkName[len] = '\0';
			printf("\x1b[36m -> %s", linkName);
		} else {
			printf("\x1b[36m -> (lien brisé)");
		}
		printf("\x1b[37m");
	}

	printf("\n");

	if (recursive && S_ISDIR(statBuffer.st_mode) && fileBaseName[0] != '.') {
		printDir(fileName);
	}
}

int main(int argc, char *argv[]) {
	char *fileName = "./";

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-R") == 0) {
			recursive = 1;
		} else {
			fileName = argv[i];
		}
	}

	showInfos(fileName);
	return 0;
}
