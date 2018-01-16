/**
 * \file directeur.c
 */

#include "musee.h"


int debug_musee;


char msg_usage[] =
"usage: directeur creer <capacite> <file>\n\
        directeur ouvrir|fermer\n\
        directeur supprimer\n";


void exit_usage(void)
{
    fprintf(stderr,"%s",msg_usage);
    exit(EXIT_FAILURE);
}




/****************************OUVRIR MUSEE**************************************/
/******************************************************************************/




int ouvrir_musee(void)
{
    int shmid,semid;
    int attente,i;

    shmid=acceder_segment();
    semid=acceder_semaphore();
    
    //attacher la memoire partage
    struct musee * m;
    m = (struct musee *) shmat(shmid,0,0);
    if(m == (void*) -1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    m->ouvert = true;

    //ouverture du musee
    V(semid,OUVERTURE_SEM);

    while((attente=semctl(semid,OUVERTURE_SEM,GETNCNT,0))>0)
        for(i=0;i<attente;i++)
        {
            V(semid,OUVERTURE_SEM);
            //ouverture du musee
        }

    //PRIM(semctl(semid,OUVERTURE_SEM,SETVAL,attente),"semctl");
    return 1;
}





/*******************************FERMER MUSEE***********************************/
/******************************************************************************/




int fermer_musee(void)
{
    int shmid,semid;

    shmid=acceder_segment();
    semid=acceder_semaphore();
    
    //attacher la memoire partage
    struct musee * m;
    m = (struct musee *) shmat(shmid,0,0);
    if(m == (void*) -1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    m->ouvert = false;


    PRIM(semctl(semid,OUVERTURE_SEM,SETVAL,0),"semctl");

    //réveiller le rontrôleur
    V(semid,TRAVAIL_SEM);

    return 1;
}





/*****************************SUPPRIMER MUSEE**********************************/
/******************************************************************************/




void supprimer_musee(void)
{
    int shmid,semid;

    shmid=acceder_segment();
    semid=acceder_semaphore();

    supprimer_shm(shmid);
    supprimer_semaphore(semid);
}




/*****************************CREER MUSEE**************************************/
/******************************************************************************/




void creer_musee(int capacite,int file)
{

    //creer shm
    int shmid;
    shmid= creer_shm();

    //creer semahpre
    creer_semaphore(capacite,file);

    //attacher la memoire partage
    struct musee * m;
    m = (struct musee *) shmat(shmid,0,0);
    if(m == (void*) -1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    m->ouvert = false;
    m->capacite=capacite;
}




/*****************************MAIN*********************************************/
/******************************************************************************/
int main(int argc, char * argv[])
{
    char * debug_musee_env = getenv("DEBUG_MUSEE");
    if(debug_musee_env == NULL)
        debug_musee = 0;
    else
        debug_musee = atoi(debug_musee_env);

    int capacite,file;

    switch(argc)
    {
        case 2:
            //ouvrir le musee
            if(strncmp(argv[1],"ouvrir",6)==0)
            {
                if(debug_musee>0)printf("le directeur ouvre le musee\n");
                fflush(stdout);
                ouvrir_musee();
            }
            //fermer le musee
            else if(strncmp(argv[1],"fermer",6)==0)
            {
                if(debug_musee>0)printf("le directeur ferme le musee\n");
                fflush(stdout);
                fermer_musee();
            }
            //supprimer le musee
            else if(strncmp(argv[1],"supprimer",9)==0)
            {
                if(debug_musee>0)printf("le directeur supprime le musee\n");
                fflush(stdout);
                supprimer_musee();
            }
            else
            {
                exit_usage();
            }
            break;
        case 4:
            //creer un nouveau musee
            if(strncmp(argv[1],"creer",5)==0)
            {
                if(debug_musee>0)printf("le directeur cree le musee\n");
                fflush(stdout);
                capacite = atoi(argv[2]);
                file = atoi(argv[3]);
                if(capacite <= 0 || file <= 0)
                {
                    exit_usage();
                }
                creer_musee(capacite,file);
            }
            else
            {
                exit_usage();
            }
            break;
        default:
            exit_usage();
    }
    exit(EXIT_SUCCESS);
}
