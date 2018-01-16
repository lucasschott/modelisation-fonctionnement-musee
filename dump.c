/**
 * \file dump.c
 */

#include "musee.h"

int main(int argc, __attribute__((unused)) char * arvg[])
{
    if(argc != 1)
    {
        fprintf(stdout,"usage: dump\n");
        exit(EXIT_FAILURE);
    }

    int shmid,semid;

    shmid = acceder_segment();
    semid = acceder_semaphore();

    struct musee * m;
    m = (struct musee *) shmat(shmid,0,0);
    if(m==(void*) -1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    printf("semaphore de file : %d (places dispolible dans la file)\n",
            semctl(semid,FILE_SEM,GETVAL,0));
    printf("semaphore de visite : %d (places disponible dans le musee)\n",
            semctl(semid,VISITE_SEM,GETVAL,0));
    printf("semaphore d'entree : %d (1:passage ouvert)\n",
            semctl(semid,ENTREE_SEM,GETVAL,0));
    printf("semaphore d'ouverture : %d (>0:le musee est ouvert)\n",
            semctl(semid,OUVERTURE_SEM,GETVAL,0));
    printf("semaphore de volonte d'entrer : %d (1:une visiteur veut entrer)\n",
            semctl(semid,VEUT_ENTRER_SEM,GETVAL,0));
    printf("semaphore de volonte de sortir : %d (1:un visiteur veut sortir)\n",
            semctl(semid,VEUT_SORTIR_SEM,GETVAL,0));
    printf("semaphore de travail : %d (1:le controleur a du travail)\n",
            semctl(semid,TRAVAIL_SEM,GETVAL,0));


    exit(EXIT_SUCCESS);
}
