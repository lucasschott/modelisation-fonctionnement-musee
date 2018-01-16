/**
 * \file musee.h
 */

#include <errno.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <time.h>
#include <unistd.h>
#include <sys/sem.h>
#include <stdbool.h>
#include <string.h>

#define FILE_SEM 0
#define VISITE_SEM 1
#define ENTREE_SEM 2
#define OUVERTURE_SEM 3
#define VEUT_ENTRER_SEM 4
#define VEUT_SORTIR_SEM 5
#define TRAVAIL_SEM 6


struct musee
{
    bool ouvert;
    int capacite;
};

#define PRIM(r, msg) do{if((r)==-1){perror(msg);exit(EXIT_FAILURE);}}while(0)

void fail(char * msg);



/*************shm************/




int creer_shm();
int acceder_segment(void);
void supprimer_shm(int shmid);




/*************semaphore******/




int creer_semaphore (int capacite,int file);
int acceder_semaphore(void);


void P_avec_attente(int semid, int sem);
bool P_sans_attente(int semid, int sem);
void V(int semid, int sem);


void supprimer_semaphore(int semid);


