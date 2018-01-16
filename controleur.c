/**
 * \file controleur.c
 */

#include "musee.h"



int debug_musee;


char msg_usage[] = "usage: controleur\n";


void exit_usage(void)
{
    fprintf(stderr,"%s",msg_usage);
    exit(EXIT_FAILURE);
}




/****************************CONTROLEUR***************************************/
/*****************************************************************************/




int faire_entrer_visiteur(int semid)
{
    if(P_sans_attente(semid,VEUT_ENTRER_SEM))//si un visiteur veut entrer
    {
        if(debug_musee>1)printf("le controleur dit au visiteur d'entrer\n");
        fflush(stdout);
        P_avec_attente(semid,VISITE_SEM);
        //attendre qu'il y ai de la place dans le musée
        //puis commencer la visite du musée
        if(debug_musee>1)
            printf("le controleur fait sortir le visiteur de la file \n\
et le fait entrer dans le musee\n");
        fflush(stdout);
        V(semid,FILE_SEM);
        //le visiteur sort de la file
        V(semid,ENTREE_SEM);
        //et le controleur laisse passer le visiteur
    }
    return 1;
}




int voir_sortir_visiteur(int semid)
{
    if(P_sans_attente(semid,VEUT_SORTIR_SEM))//si un visiteur veut sortir
    {
        if(debug_musee>1)
            printf("le controleur observe un visiteur sortir du musee\n");
        fflush(stdout);
    }
    return 1;
}




int main(int argc, __attribute__((unused)) char * argv[])
{
    char * debug_musee_env = getenv("DEBUG_MUSEE");
    if(debug_musee_env == NULL)
        debug_musee = 0;
    else
        debug_musee = atoi(debug_musee_env);

    if(argc != 1)
    {
        exit_usage();
    }

    if(debug_musee>0)printf("arrive du controleur devant le musee\n");
    fflush(stdout);
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

    if(debug_musee>0)printf("le controleur attend l'ouverture du musee\n");
    fflush(stdout);

    //attendre l'ouverture du musée
    P_avec_attente(semid,OUVERTURE_SEM);
    if(debug_musee>0)printf("le controleur se met au travail\n");
    fflush(stdout);

    while(m->ouvert)
    {
        if(debug_musee>1)printf("le controleur attend du travail\n");
        fflush(stdout);
        //attendre du travail
        P_avec_attente(semid,TRAVAIL_SEM);
        faire_entrer_visiteur(semid);
        voir_sortir_visiteur(semid);
    }

    if(debug_musee>0)
        printf("le controleur arrete de faire entrer des visiteurs\n");
    if(debug_musee>0)
        printf("il s'occupe de faire sortir les visiteur \
qui sont dans le musee\n");
    fflush(stdout);

    while(semctl(semid,VISITE_SEM,GETVAL,0)<m->capacite)
    {
        if(debug_musee>1)printf("le controleur attend du travail\n");
        fflush(stdout);
        P_avec_attente(semid,TRAVAIL_SEM);
        //attendre du travail
        voir_sortir_visiteur(semid);
    }

    if(debug_musee>0)printf("tous les visiteurs sont sortie du musee\n");
    if(debug_musee>0)printf("le controleur quitte son poste\n");
    fflush(stdout);

    exit(EXIT_SUCCESS);
}
