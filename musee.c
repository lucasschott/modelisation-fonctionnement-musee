/**
 * \file musee.c
 */

#include "musee.h"




/*************shm************/




int creer_shm()
{
    key_t key;
    int shmid;

    //supprime le segment de memoire partagé s'il existe
    PRIM(key = ftok("/etc/group", 32445746),"ftok");
    shmid = shmget(key,0,0);
    if(shmid !=-1)
    {
        //delete
        if(shmctl(shmid,IPC_RMID,0)==-1)
        {
            perror("shmctl");
            exit(EXIT_FAILURE);
        }
    }

    //creer la taille necessaire pour n entiers
    int size = sizeof(struct musee);
    shmid = shmget(key,size,IPC_CREAT | 0600);

    //retourner l'identificateur de segment (valeur de retour de shmget)
    return shmid;
}


int acceder_segment(void)
{
    int shmid;
    key_t key;
    PRIM(key = ftok ("/etc/group", 32445746),"ftok");
    PRIM(shmid = shmget(key,0,0),"shmid");
    return shmid;
}


void supprimer_shm(int shmid)
{
    PRIM(shmctl(shmid,IPC_RMID, 0),"shmtcl");
}




/*************semaphore******/




int creer_semaphore (int capacite,int file)
{
    key_t k;
    int semid;

    PRIM(k = ftok("/etc/passwd",32445746),"ftok");
    PRIM(semid = semget(k,8,IPC_CREAT | 0666),"semget");
    //Semaphore file
    PRIM(semctl(semid,FILE_SEM,SETVAL,file),"semctl file");
    //Semaphore visite
    PRIM(semctl(semid,VISITE_SEM,SETVAL,capacite),"semctl visite");
    //Semaphore entree dans le musee
    PRIM(semctl(semid,ENTREE_SEM,SETVAL,0),"semctl entree");
    //Semaphore d'ouverture du musee
    PRIM(semctl(semid,OUVERTURE_SEM,SETVAL,0),"semctl ouverture");
    //Semaphore de volonte d'entrer
    PRIM(semctl(semid,VEUT_ENTRER_SEM,SETVAL,0),"semctl veux entrer");
    //Semaphore de volonte de sortir
    PRIM(semctl(semid,VEUT_SORTIR_SEM,SETVAL,0),"semctl veux sortir");
    //Semaphore de travail du controleur
    PRIM(semctl(semid,TRAVAIL_SEM,SETVAL,0),"semctl travail");

    return semid;
}



int acceder_semaphore(void)
{
    key_t k ;
    int semid ;
    PRIM(k = ftok ("/etc/passwd",32445746),"ftok");
    PRIM(semid = semget(k,0,0),"semget");
    return semid;
}



void P_avec_attente(int id, int sem)
{
    struct sembuf s[1] = {{sem,-1,0}};
    PRIM(semop(id,s,1),"semop");
}



bool P_sans_attente(int id, int sem)
{
    struct sembuf s[1] = {{sem,-1,IPC_NOWAIT}};
    if(semop(id,s,1) == -1)
    {
        return false;
    }
    return true;
}



void V(int id, int sem)
{
    struct sembuf s[1] = {{sem,1,0}};
    PRIM(semop(id,s,1),"semop");
}



void supprimer_semaphore(int semid)
{
    int r ;
    PRIM(r = semctl (semid,0,IPC_RMID,NULL),"semctl IPC_RMID");
}
