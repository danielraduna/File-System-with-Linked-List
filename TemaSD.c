#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"
typedef struct nod {
	void *val;
	struct nod *next;
} nod;

void create_fs(Directory **radacina)
{
	(*radacina) = (Directory *)malloc(sizeof(Directory));
	(*radacina)->name = (char *)malloc(2 * sizeof(char));
	strcpy((*radacina)->name, "/");
	(*radacina)->listFile = NULL;
	(*radacina)->listDir = NULL;
	(*radacina)->parentDir = NULL;
}

void delete_fs(Directory **director)
{
	while ((*director)->parentDir != NULL)
		(*director) = (*director)->parentDir;
	free((*director)->name);
	free((*director));
}

void touch(Directory **director, char *numefisier, char *content)
{
	nod *nou = (nod *)malloc(sizeof(nod));

	nou->val = (File *)malloc(sizeof(File));
	((File *)nou->val)->name =
(char *)malloc((strlen(numefisier) + 1) * sizeof(char));
	((File *)nou->val)->data =
(char *)malloc((strlen(content) + 1) * sizeof(char));


	strcpy(((File *)nou->val)->name, numefisier);
	strcpy(((File *)nou->val)->data, content);

	((File *)nou->val)->size = strlen(content);
	((File *)nou->val)->dir = (*director);

	if ((*director)->listFile == NULL ||
strcmp(((File *)(*director)->listFile->val)->name,
((File *)nou->val)->name) > 0) {
		nou->next = (*director)->listFile;
		(*director)->listFile = nou;

	} else {

		nod *curentfile = (*director)->listFile;

		while (curentfile->next != NULL &&
strcmp(((File *)curentfile->next->val)->name,
((File *)nou->val)->name) < 0)
			curentfile = curentfile->next;
		nou->next = curentfile->next;
		curentfile->next = nou;
	}
}
void mkdir(Directory **director, char *numedir)
{
	nod *nou = (nod *)malloc(sizeof(nod));

	nou->val = (Directory *)malloc(sizeof(Directory));
	((Directory *)nou->val)->name =
(char *)malloc((strlen(numedir) + 1) * sizeof(char));
	strcpy(((Directory *)nou->val)->name, numedir);

	((Directory *)nou->val)->parentDir = (*director);
	((Directory *)nou->val)->listDir = NULL;
	((Directory *)nou->val)->listFile = NULL;
	if ((*director)->listDir == NULL ||
strcmp(((Directory *)(*director)->listDir->val)->name,
((Directory *)nou->val)->name) > 0) {
		nou->next = (*director)->listDir;
		(*director)->listDir = nou;
	} else {

		nod *curentdir = (*director)->listDir;

		while (curentdir->next != NULL &&
strcmp(((Directory *)curentdir->next->val)->name,
((Directory *)nou->val)->name) < 0)
			curentdir = curentdir->next;
		nou->next = curentdir->next;
		curentdir->next = nou;
	}
}

void ls(Directory **director)
{

	nod *curentf = (*director)->listFile;
	nod *curentd = (*director)->listDir;

	while (curentf != NULL) {
		printf("%s ", ((File *)curentf->val)->name);
	curentf = curentf->next;
	}

	if (curentd != NULL)
		while (curentd != NULL) {
			printf("%s ", ((File *)curentd->val)->name);
			curentd = curentd->next;
	}
	printf("\n");
}

void pwd(Directory **director)
{
	if ((*director)->parentDir == NULL)
		printf("%s", (*director)->name);
	else {
		pwd(&(*director)->parentDir);
		if (strcmp(((Directory *)(*director)->parentDir)->name,
"/") == 0)
			printf("%s", (*director)->name);
		else
			printf("/%s", (*director)->name);
	}
}

Directory *cd(Directory **director, char *numedir)
{
	nod *curentdir = (*director)->listDir;

	if (strcmp(numedir, "..") != 0) {
		while (curentdir != NULL) {
			if (strcmp(((Directory *)curentdir->val)->name,
numedir) == 0) {
				return curentdir->val;
			}
			curentdir = curentdir->next;
		}
	} else
		if (strcmp(numedir, "..") == 0)
			return (*director)->parentDir;
	return NULL;
}

void rm(Directory **director, char *numefile)
{
	int bec = 0;

	if ((*director)->listFile != NULL) {
		nod *curentfile = (*director)->listFile;
		nod *prev;

		if (curentfile != NULL &&
strcmp(((File *)curentfile->val)->name, numefile) == 0) {
			bec = 1;
			(*director)->listFile = curentfile->next;
			free(((File *)curentfile->val)->name);
			free(((File *)curentfile->val)->data);
			free((File *)curentfile->val);
			free(curentfile);
		} else {
			while (curentfile->next != NULL &&
strcmp(((File *)curentfile->val)->name, numefile) != 0) {
				prev = curentfile;
				curentfile = curentfile->next;
			}
			if (curentfile != NULL &&
strcmp(((File *)curentfile->val)->name, numefile) == 0) {
				bec = 1;
				prev->next = curentfile->next;
				free(((File *)curentfile->val)->name);
				free(((File *)curentfile->val)->data);
				free((File *)curentfile->val);
				free(curentfile);
			}
		}
	}
	if (!bec)
		printf("Cannot remove '%s': No such file!\n", numefile);
}

void rmdir(Directory **director, char *numedir)
{
	nod *curentdir = (*director)->listDir;
	nod *dirsters = NULL;
	nod *curentfile;
	nod *filesters;
	Directory *dir;

	if (curentdir != NULL &&
strcmp(((Directory *)curentdir->val)->name, numedir) == 0) {
		(*director)->listDir = curentdir->next;
		dirsters = curentdir;
	} else
		while (curentdir->next
!= NULL) {
			if (strcmp(((Directory *)curentdir->next->val)
->name, numedir) == 0) {
				dirsters = curentdir->next;
				curentdir->next = curentdir->next->next;
				break;
			}
		    curentdir = curentdir->next;
		}
	if (dirsters == NULL) {
		printf("Cannot remove '%s': No such directory!\n",
			   numedir);
	} else {
		dir = dirsters->val;
		curentfile = dir->listFile;

		while (curentfile != NULL) {
			filesters = curentfile;
			curentfile = curentfile->next;
			rm(&dir, ((File *)filesters->val)->name);
		}

		curentdir = dir->listDir;
		nod *curentdir_next;

		while (curentdir != NULL) {
			curentdir_next = curentdir->next;
			rmdir(&dir, ((Directory *)curentdir->val)->name);
			curentdir = curentdir_next;
		}
		free(dir->name);
		free(dir);
		free(dirsters);
	}
}

void tree(Directory *director, int spatii)
{
	int i;
	nod *curentfile = director->listFile;
	nod *curentdir = director->listDir;

	while (curentfile != NULL) {
		for (i = 0; i < spatii; i++)
			printf("    ");
		printf("%s\n", ((File *)curentfile->val)->name);
		curentfile = curentfile->next;
	}

	while (curentdir != NULL) {
		for (i = 0; i < spatii; i++)
			printf("    ");
		printf("%s\n", ((Directory *)curentdir->val)->name);
		tree(curentdir->val, spatii+1);
		curentdir = curentdir->next;
	}
}

int main(void)
{
	Directory *curentdir;
	char *comanda = (char *)malloc(100 * sizeof(char));

	scanf("%s", comanda);
	while (strcmp(comanda, "delete") != 0) {
		if (strcmp(comanda, "create") == 0)
			create_fs(&curentdir);
		else
			if (strcmp(comanda, "touch") == 0) {
				char *numefisier =
(char *)malloc(100 * sizeof(char));
				char *content =
(char *)malloc(100 * sizeof(char));

				scanf("%s", numefisier);
				scanf("%s", content);
				touch(&curentdir, numefisier, content);
				free(numefisier);
				free(content);
			} else
				if (strcmp(comanda, "mkdir") == 0) {
					char *numedir =
(char *)malloc(100 * sizeof(char));

					scanf("%s", numedir);
					mkdir(&curentdir, numedir);
					free(numedir);
			} else
				if (strcmp(comanda, "delete") == 0) {
					delete_fs(&curentdir);
					break;
			} else
				if (strcmp(comanda, "ls") == 0)
					ls(&curentdir);
			else
				if (strcmp(comanda, "pwd") == 0) {
					pwd(&curentdir);
					printf("\n");
			} else
				if (strcmp(comanda, "cd") == 0) {
					char *numedir =
(char *)malloc(100 * sizeof(char));

					scanf("%s", numedir);
					if (cd(&curentdir, numedir) != NULL)
						curentdir =
cd(&curentdir, numedir);
					else {
						printf("Cannot move to ");
						printf("'%s'", numedir);
						printf(": No such ");
						printf("directory!\n"); }
					free(numedir);
			} else
				if (strcmp(comanda, "rm") == 0) {
					char *numefisier =
(char *)malloc(100 * sizeof(char));

					scanf("%s", numefisier);
					rm(&curentdir, numefisier);
					free(numefisier);
			} else
				if (strcmp(comanda, "tree") == 0) {
					int spatii = 1;

					printf("%s\n", curentdir->name);
					tree(curentdir, spatii);
			} else
				if (strcmp(comanda, "rmdir")  == 0) {
					char *numedir =
(char *)malloc(100 * sizeof(char));

					scanf("%s", numedir);
					rmdir(&curentdir, numedir);
					free(numedir);
			}
	scanf("%s", comanda);

	}
	free(comanda);
	free(curentdir->name);
	free(curentdir);
}

