#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

//Deklaracija i inicijalizacija mutexa i kondicionala
pthread_mutex_t mutex =	PTHREAD_MUTEX_INITIALIZER;

//ucesnici
pthread_cond_t agent_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t pusac_sa_duvanom_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t pusac_sa_sibicama_cond =	PTHREAD_COND_INITIALIZER;
pthread_cond_t pusac_sa_papirom_cond =	PTHREAD_COND_INITIALIZER;

//sastojci
pthread_cond_t duvan = PTHREAD_COND_INITIALIZER;
pthread_cond_t papir = PTHREAD_COND_INITIALIZER;
pthread_cond_t sibice = PTHREAD_COND_INITIALIZER;

int ima_duvana = 0;
int ima_papira = 0;
int ima_sibica = 0;

//Globalne promenljive koje odredjuju ko treba da radi (na pocetku radi agent)
int agent_radi = 1;
int pusac_sa_duvanom_radi = 0;
int pusac_sa_sibicama_radi = 0;
int pusac_sa_papirom_radi = 0;

int getRand(int range){
    int randNum = rand() % range;
    return randNum;
}

//FUNKCIJA AGENT
void* agent(void* arg){
    while(1) {
        sleep(1);
        
        pthread_mutex_lock(&mutex);
        
        //Agent ostaje da ceka ako je agent_radi jednak 0
        while(agent_radi == 0)
            pthread_cond_wait(&agent_cond, &mutex);
        
        printf("_____________________________\n");
        
        //Random broj od 0 do 2
        int randNum = getRand(3);
        
        //Broj 0 - Agent je izvukao papir i sibice - pusi pusac koji ima duvan
        if ( randNum == 0 ) {
            agent_radi = 0;
            ima_sibica = 1;
            ima_papira = 1;
            puts("Agent uzima papir i sibice");
            pthread_cond_signal(&papir);
            pthread_cond_signal(&sibice);
        }
        else if ( randNum == 1 ) {
            agent_radi = 0;
            ima_sibica = 1;
            ima_duvana = 1;
            puts("Agent uzima duvan i sibice");
            pthread_cond_signal(&duvan);
            pthread_cond_signal(&sibice);
        }
        else if ( randNum == 2 ) {
            agent_radi = 0;
            ima_duvana = 1;
            ima_papira = 1;
            puts("Agent uzima papir i duvan");
            pthread_cond_signal(&papir);
            pthread_cond_signal(&duvan);
        }
        
        pthread_mutex_unlock(&mutex);
    }

    return 0;
}

void* pusher_papir(void* arg){
    while(1){
        pthread_mutex_lock(&mutex);

        while(ima_papira == 0)
            pthread_cond_wait(&papir, &mutex);
    
        if(ima_sibica == 1) {
            ima_sibica = 0;
            agent_radi = 0;
            pusac_sa_duvanom_radi = 1;
            puts("Pozovi pusaca sa duvanom");
            pthread_cond_signal(&pusac_sa_duvanom_cond);
        }
        if(ima_duvana == 1) {
            ima_duvana = 0;
            agent_radi = 0;
            pusac_sa_sibicama_radi = 1;
            puts("Pozovi pusaca sa sibicama");
            pthread_cond_signal(&pusac_sa_sibicama_cond);
        }
        pthread_mutex_unlock(&mutex);
    }
    
    return 0 ;
}

void* pusher_sibice(void* arg){
    while(1) {
        pthread_mutex_lock(&mutex);
        
        while(ima_sibica == 0)
            pthread_cond_wait(&sibice, &mutex);
    
        if(ima_papira == 1) {
            ima_papira = 0;
            agent_radi = 0;
            pusac_sa_duvanom_radi = 1;
            puts("Pozovi pusaca sa duvanom");
            pthread_cond_signal(&pusac_sa_duvanom_cond);
        }
        if(ima_duvana == 1) {
            ima_duvana = 0;
            agent_radi = 0;
            pusac_sa_papirom_radi = 1;
            puts("Pozovi pusaca sa papirom");
            pthread_cond_signal(&pusac_sa_papirom_cond);
        }
        pthread_mutex_unlock(&mutex);
    }
    
    return 0 ;
}

void* pusher_duvan(void* arg){
    while(1){
        pthread_mutex_lock(&mutex);
        while(ima_duvana == 0)
            pthread_cond_wait(&duvan, &mutex);
    
        if(ima_sibica == 1) {
            ima_sibica = 0;
            agent_radi = 0;
            pusac_sa_papirom_radi = 1;
            puts("Pozovi pusaca sa papirom");
            pthread_cond_signal(&pusac_sa_papirom_cond);
        }
        if(ima_papira == 1) {
            ima_papira = 0;
            agent_radi = 0;
            pusac_sa_sibicama_radi = 1;
            puts("Pozovi pusaca sa sibicama");
            pthread_cond_signal(&pusac_sa_sibicama_cond);
        }
        pthread_mutex_unlock(&mutex);
    }
    return 0 ;
}

void* pusac_duvan(void* arg){
    while(1){
        pthread_mutex_lock(&mutex);

        while(pusac_sa_duvanom_radi == 0)
            pthread_cond_wait(&pusac_sa_duvanom_cond, &mutex);

        ima_papira = 0;
        ima_sibica = 0;
        pusac_sa_duvanom_radi = 0;
        agent_radi = 1;
        puts("Pusac sa duvanom: pravi cigarete...");
        pthread_mutex_unlock(&mutex);
        
        puts("Pusac sa duvanom: Pusi...");
    }
    
    return 0;
}

void* pusac_papir(void* arg){
    while(1){
        pthread_mutex_lock(&mutex);

        while(pusac_sa_papirom_radi == 0)
            pthread_cond_wait(&pusac_sa_papirom_cond, &mutex);

        ima_duvana = 0;
        ima_sibica = 0;
        pusac_sa_papirom_radi = 0;
        agent_radi = 1;
        puts("Pusac sa papirom: pravi cigarete...");
        pthread_mutex_unlock(&mutex);
        
        puts("Pusac sa papirom: Pusi...");
    }
    
    return 0;
}

void* pusac_sibice(void* arg){
    while(1){
        pthread_mutex_lock(&mutex);

        while(pusac_sa_sibicama_radi == 0)
            pthread_cond_wait(&pusac_sa_sibicama_cond, &mutex);

        ima_papira = 0;
        ima_duvana = 0;
        pusac_sa_sibicama_radi = 0;
        agent_radi = 1;
        puts("Pusac sa sibicama: pravi cigarete...");
        pthread_mutex_unlock(&mutex);
        
        puts("Pusac sa sibicama: Pusi...");
    }
    
    return 0;
}


int main(int argc, char* argv[]) {
    //Definisanje threadova
    pthread_t agent_t, pusac_sa_duvanom_t, pusac_sa_papirom_t, pusac_sa_sibicama_t, pusher_duvan_t, pusher_papir_t, pusher_sibice_t;
    
    time_t t;
    srand((unsigned) time(&t));
    
    //Inicijalizacija threadova
    if (pthread_create(&agent_t, NULL, agent, NULL) != 0) {
        fprintf (stderr, "Greska prilikom kreiranja niti za agenta\n");
        exit (1);
    }
    
    if (pthread_create(&pusher_duvan_t, NULL, pusher_duvan, NULL) != 0) {
        fprintf (stderr, "Nemoguce je kreirati nit\n");
        exit (1);
    }
    
    if (pthread_create(&pusher_papir_t, NULL, pusher_papir, NULL) != 0) {
        fprintf (stderr, "Nemoguce je kreirati nit\n");
        exit (1);
    }
    
    if (pthread_create(&pusher_sibice_t, NULL, pusher_sibice, NULL) != 0) {
        fprintf (stderr, "Nemoguce je kreirati nit\n");
        exit (1);
    }
    
    if (pthread_create(&pusac_sa_duvanom_t, NULL, pusac_duvan, NULL) != 0) {
        fprintf (stderr, "Nemoguce je kreirati nit\n");
        exit (1);
    }
    
    if (pthread_create(&pusac_sa_papirom_t, NULL, pusac_papir, NULL) != 0) {
        fprintf (stderr, "Nemoguce je kreirati nit\n");
        exit (1);
    }
    
    if (pthread_create(&pusac_sa_sibicama_t, NULL, pusac_sibice, NULL) != 0) {
        fprintf (stderr, "Nemoguce je kreirati nit\n");
        exit (1);
    }
    
    pthread_join(agent_t, NULL);
    pthread_join(pusher_duvan_t, NULL);
    pthread_join(pusher_papir_t, NULL);
    pthread_join(pusher_sibice_t, NULL);
    pthread_join(pusac_sa_duvanom_t, NULL);
    pthread_join(pusac_sa_papirom_t, NULL);
    pthread_join(pusac_sa_sibicama_t, NULL);
}