/**
 * \file visiteur.c
 */

#include "musee.h"




int debug_musee;



/****************************VISITEUR*****************************************/
/*****************************************************************************/




int main(int argc, char * argv[])
{
    char * debug_musee_env = getenv("DEBUG_MUSEE");
    if(debug_musee_env == NULL)
        debug_musee = 0;
    else
        debug_musee = atoi(debug_musee_env);

    if(debug_musee>0)printf("un visiteur arrive devant le musee\n");
    fflush(stdout);

    if(argc!=2)
    {
        fprintf(stderr,"usage: visiteur <duree de visite>\n");
        exit(EXIT_FAILURE);
    }

    int duree = atoi(argv[1]) * 1000;

    if(duree <= 0)
    {
        fprintf(stderr,"usage: visiteur <duree de visite>\n");
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

    if(!m->ouvert)//si le musée est fermé
    {
        if(debug_musee>1)printf("le visiteur attend l'ouverture du musee");
        //attendre l'ouvertur du musée
        P_avec_attente(semid,OUVERTURE_SEM);
    }


    if(debug_musee>0)printf("le visiteur observe la file d'attente\n");
    fflush(stdout);

    //entrer dans le fils d'attente si elle n'est pas pleine
    if(P_sans_attente(semid,FILE_SEM))
    {
        if(debug_musee>0)
            printf("la file n'est pas trop pleine, \
le visiteur va dans le file d'attente\n");
        fflush(stdout);
        //le visiteur veut entrer dans le musée
        V(semid,VEUT_ENTRER_SEM);
        //il réveil le contrôleur
        V(semid,TRAVAIL_SEM);
        if(debug_musee>0)
            printf("le visiteur veux entrer dans le musee, \
il demande l'authorisation au controleur\n");
        fflush(stdout);
        //il attend l'authorisation d'entrer du controleur
        P_avec_attente(semid,ENTREE_SEM);
        
        if(debug_musee>0)
            printf("le visiteur reste dans le musee %d milisecondes\n",
                    duree/1000);
        fflush(stdout);
        usleep(duree);
        
        //le visiteur veut sortir du musée
        V(semid,VEUT_SORTIR_SEM);
        //il réveil le contrôleur
        V(semid,TRAVAIL_SEM);
        if(debug_musee>1)
            printf("le visiteur veux sortir du musee, \
il passe devant le controleur en sortant\n");
        fflush(stdout);
        //il quitte le musée et libère une place
        V(semid,VISITE_SEM);
    }
    else
    {
        if(debug_musee>0)
            printf("la file est beaucoup trop longue, le visiteur s'enfuit\n");
        fflush(stdout);
        exit(EXIT_FAILURE);
    }

    if(debug_musee>0)
        printf("le visiteur est sortie du musee et \
trouve d'autres occupations\n");

    exit(EXIT_SUCCESS);
}
