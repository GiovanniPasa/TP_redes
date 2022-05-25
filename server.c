#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 1024

void usage(int argc, char **argv) {
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

//gerar randomico
double randfrom(double min, double max) 
{
    double range = (max - min); 
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

int main(int argc, char **argv) {
        int i;
        int total_of_sensors = 0;

        int sensors[4][4];
        //inicialização da matriz
        for(i=0;i<4;i++){
            for(int j=0;j<4;j++){
                sensors[i][j] = 0;
            }
        }
        
        if (argc < 3) {
            usage(argc, argv);
        }

        struct sockaddr_storage storage;
        if (0 != server_sockaddr_init(argv[1], argv[2], &storage)) {
            usage(argc, argv);
        }

        int s;
        s = socket(storage.ss_family, SOCK_STREAM, 0);
        if (s == -1) {
            logexit("socket");
        }

        int enable = 1;
        if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
            logexit("setsockopt");
        }

        struct sockaddr *addr = (struct sockaddr *)(&storage);
        if (0 != bind(s, addr, sizeof(storage))) {
            logexit("bind");
        }

        if (0 != listen(s, 10)) {
            logexit("listen");
        }

        char addrstr[BUFSZ];
        addrtostr(addr, addrstr, BUFSZ);
        printf("bound to %s, waiting connections\n", addrstr);
        int csock;
        char buf[BUFSZ];
        char input[BUFSZ];
        static int sensor_input[4];
        do{
            struct sockaddr_storage cstorage;
            struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
            socklen_t caddrlen = sizeof(cstorage);

            csock = accept(s, caddr, &caddrlen);
            if (csock == -1) {
                logexit("accept");
            }

            char caddrstr[BUFSZ];
            addrtostr(caddr, caddrstr, BUFSZ);
            printf("[log] connection from %s\n", caddrstr);

            
            memset(buf, 0, BUFSZ);
            size_t count = recv(csock, buf, BUFSZ - 1, 0);
            strcpy(input, buf);
            char temp[BUFSZ];
            int equip_id;
            static int num_of_sensors = 0;
            
            //Funçao add
            memset(temp, 0, BUFSZ);
            strncpy(temp, (buf + 0), 10);
            if(!strcmp(temp, "add sensor")){
                num_of_sensors = 0;
                
                
                //checa se foi dado o input de 1 sensor
                memset(temp, 0, BUFSZ);
                strncpy(temp, (buf + 14), 2);
                if(!strcmp(temp,"in")){
                    if(strlen(buf) != 20){
                    }
                    else{
                        memset(temp, 0, BUFSZ);
                        strncpy(temp, (buf + 11), 2);
                        sensor_input[0] = atoi(temp);
                        memset(temp, 0, BUFSZ);
                        strncpy(temp, (buf + 17), 2);
                        equip_id = atoi(temp);
                        if(sensor_input[0] < 1 || sensor_input[0] > 4){
                        sprintf(buf, "invalid sensor\n");
                        }
                        else{
                            num_of_sensors = 1;
                        }
                    }
                }
                
                //checa se foi dado o input de 2 sensores
                memset(temp, 0, BUFSZ);
                strncpy(temp, (buf + 17), 2);
                if(!strcmp(temp,"in")){
                    if(strlen(buf) != 23){
                    }
                    else{
                        memset(temp, 0, BUFSZ);
                        strncpy(temp, (buf + 11), 2);
                        sensor_input[0]= atoi(temp);
                        memset(temp, 0, BUFSZ);
                        strncpy(temp, (buf + 14), 2);
                        sensor_input[1] = atoi(temp);
                        memset(temp, 0, BUFSZ);
                        strncpy(temp, (buf + 20), 2);
                        equip_id = atoi(temp);
                        if(sensor_input[0] < 1 || sensor_input[0] > 4
                        ||sensor_input[1] < 1 ||sensor_input[1] > 4){
                            sprintf(buf, "invalid sensor\n");
                        }
                        else{
                            num_of_sensors = 2;
                        }
                    }
                }
                
                //checa se foi dado o input de 3 sensores
                memset(temp, 0, BUFSZ);
                strncpy(temp, (buf + 20), 2);
                if(!strcmp(temp,"in")){
                     if(strlen(buf) != 26){
                    }
                    else{
                        memset(temp, 0, BUFSZ);
                        strncpy(temp, (buf + 11), 2);
                        sensor_input[0]= atoi(temp);
                        memset(temp, 0, BUFSZ);
                        strncpy(temp, (buf + 14), 2);
                        sensor_input[1] = atoi(temp);
                        memset(temp, 0, BUFSZ);
                        strncpy(temp, (buf + 17), 2);
                        sensor_input[2] = atoi(temp);
                        memset(temp, 0, BUFSZ);
                        strncpy(temp, (buf + 23), 2);
                        equip_id = atoi(temp);
                        if(sensor_input[0] < 1 || sensor_input[0] > 4
                                    ||sensor_input[1] < 1 ||sensor_input[1] > 4 || sensor_input[2] < 0 || sensor_input[2] > 4){
                            sprintf(buf, "invalid sensor\n");
                        }
                        else{
                            num_of_sensors = 3;
                        }
                    }
                    
                }
                //num invalido de equipamento
                if((equip_id < 1 || equip_id > 4) && num_of_sensors){
                    sprintf(buf, "invalid equipment\n"); 
                }
                
                //caso deseja-se adicionar 1 sensor
                else if(num_of_sensors == 1){
                    if((sensors[equip_id - 1][sensor_input[0]-1]) == 1){
                            sprintf(buf, "sensor %02d already exists in %02d\n", sensor_input[0], equip_id); 
                    }
                    else{
                        if(total_of_sensors + 1> 15){
                            sprintf(buf, "limit exceeded\n");
                        }
                        else{
                            sensors[equip_id - 1][sensor_input[0]-1] = 1;
                            total_of_sensors++;
                            sprintf(buf, "sensor %02d added\n", sensor_input[0]); 
                        }
                    } 
                }   
                //caso deseja-se adicionar 2 sensores
                else if(num_of_sensors == 2){
                    int errors[] = {0,0};
                    //checa quantos e quais dos requeridos ja foram adicionados
                    for(i = 0; i<2;i++){
                        int sensor_input[3];
                        if(((sensors[equip_id - 1][(sensor_input[i])-1]) == 1)){
                            errors[i] = 1;
                            
                        }
                        
                    }
                    //caso todos já foram adicionados
                    if(errors[0] && errors[1]){
                        sprintf(buf, "sensor %02d %02d already exists in %02d\n", sensor_input[0], sensor_input[1], equip_id); 
                    }
                    //caso 1 ja foi adicionado
                    else if( errors[0] && !(errors[1])){
                        if((total_of_sensors+1)<= 15){
                            sensors[equip_id - 1][sensor_input[1]-1] = 1;
                            total_of_sensors++;
                            sprintf(buf, "sensor %02d added %02d already exists in %02d\n", sensor_input[1], sensor_input[0], equip_id);
                        }
                        else{
                            sprintf(buf, "limit exceeded\n");
                        }
                    }
                    else if( errors[1] && !(errors[0])){
                        if((total_of_sensors+1)<= 15){
                            sensors[equip_id - 1][sensor_input[0]-1] = 1;
                            total_of_sensors++;
                            sprintf(buf, "sensor %02d added %02d already exists in %02d\n", sensor_input[0], sensor_input[1], equip_id);
                        }
                        else{
                            sprintf(buf, "limit exceeded\n");
                        }
                    }
                    //caso nenhum foi adicionado
                    else if(!(errors[1]) && !(errors[0])){
                        if((total_of_sensors+2)<= 15){
                            total_of_sensors++;
                            total_of_sensors++;
                            sprintf(buf, "sensor %02d %02d added\n", sensor_input[0], sensor_input[1]);
                            sensors[equip_id - 1][sensor_input[0]-1] = 1;
                            sensors[equip_id - 1][sensor_input[1]-1] = 1;
                        }
                        else{
                           sprintf(buf, "limit exceeded\n"); 
                        }

                    }
                    
                }
                //caso deseja-se adicionar 3 sensores
                else if(num_of_sensors == 3){
                    int errors[] = {0,0,0};
                    //checa quantos e quais dos requeridos ja foram adicionados
                    for(i = 0; i<3;i++){
                        if(((sensors[equip_id - 1][sensor_input[i]-1]) == 1)){
                            errors[i] = 1;
                        }
                    }
                    int sum_of_errors = errors[0] + errors[1] + errors[2];
                    //caso os 3 ja estejam instalados
                    if(errors[0] && errors[1] && errors[2]){
                        sprintf(buf, "sensor %02d %02d %02d already exists in %02d\n", sensor_input[0], sensor_input[1], sensor_input[2], equip_id); 
                    }
                    //caso 2 ja estejam instalados
                    else if(sum_of_errors == 2){
                        if((total_of_sensors + 1) > 15){
                            sprintf(buf, "limit exceeded\n"); 
                        }
                        else {
                            int error_index[2];
                            int aux =0;
                            int non_error_index;
                            for(i = 0; i<3;i++){
                                if(errors[i] == 0){
                                    non_error_index = i;
                                }
                                else{
                                    error_index[aux] = i;
                                    aux++;
                                }
                            }
                        sprintf(buf, "sensor %02d added %02d %02d already exists in %02d\n", sensor_input[non_error_index],
                                                        sensor_input[error_index[1]], sensor_input[error_index[0]], equip_id);
                        total_of_sensors++;
                        sensors[equip_id - 1][sensor_input[non_error_index]-1] = 1;
                        }
                    }
                    //caso 1 ja esteja instalado
                    else if(sum_of_errors == 1){
                        if(total_of_sensors + 2 > 15){
                            sprintf(buf, "limit exceeded\n"); 
                        }
                        else {
                            int non_error_index[2];
                            int aux =0;
                            int error_index;
                            for(i = 0; i<3;i++){
                                if(errors[i] == 1){
                                    error_index = i;
                                }
                                else{
                                    non_error_index[aux] = i;
                                    aux++;
                                }
                            }
                        sprintf(buf, "sensor %02d %02d added %02d already exists in %02d\n", sensor_input[non_error_index[1]],
                                                        sensor_input[non_error_index[0]], sensor_input[error_index], equip_id);
                        total_of_sensors++;
                        total_of_sensors++;
                        sensors[equip_id - 1][sensor_input[non_error_index[0]]-1] = 1;
                        sensors[equip_id - 1][sensor_input[non_error_index[1]]-1] = 1;
                        }

                    }
                    //caso nenhum foi instalado ainda
                    else if(sum_of_errors == 0){
                        if((total_of_sensors + 3) > 15){
                            sprintf(buf, "limit exceeded\n");
                        }
                        else{
                            sprintf(buf, "sensor %02d %02d %02d added\n", sensor_input[0],
                                                        sensor_input[1], sensor_input[2]);
                            total_of_sensors++;
                            total_of_sensors++;
                            total_of_sensors++;
                            sensors[equip_id - 1][sensor_input[0]-1] = 1;
                            sensors[equip_id - 1][sensor_input[1]-1] = 1;
                            sensors[equip_id - 1][sensor_input[2]-1] = 1;
                        }
                    }
                }

            }

            //Função remover
            memset(temp, 0, BUFSZ);
            strncpy(temp, (buf + 0), 13);
            if((!strcmp(temp, "remove sensor"))){
                num_of_sensors = 0;
                //checa se foi dado o input de 1 sensor
                memset(temp, 0, BUFSZ);
                strncpy(temp, (buf + 17), 2);
                if(!(strcmp(temp, "in"))){
                    memset(temp, 0, BUFSZ);
                    strncpy(temp, (buf + 14), 2);
                    sensor_input[0] = atoi(temp);
                    memset(temp, 0, BUFSZ);
                    strncpy(temp, (buf + 20), 2);
                    equip_id = atoi(temp);
                     if(sensor_input[0] < 1 || sensor_input[0] > 4){
                       sprintf(buf, "invalid sensor\n");
                    }
                    else{
                        num_of_sensors = 1;
                    }
                }
                //checa se foi dado o input de 2 sensores
                memset(temp, 0, BUFSZ);
                strncpy(temp, (buf + 20), 2);
                if(!(strcmp(temp, "in"))){
                    memset(temp, 0, BUFSZ);
                    strncpy(temp, (buf + 14), 2);
                    sensor_input[0] = atoi(temp);
                    memset(temp, 0, BUFSZ);
                    strncpy(temp, (buf + 17), 2);
                    sensor_input[1] = atoi(temp);
                    strncpy(temp, (buf + 23), 2);
                    equip_id = atoi(temp);
                    if(sensor_input[0] < 1 || sensor_input[0] > 4
                    ||sensor_input[1] < 1 ||sensor_input[1] > 4){
                       sprintf(buf, "invalid sensor\n");
                    }
                    else{
                        num_of_sensors = 2;
                    }
                }
                //checa se foi dado o input de 3 sensores
                memset(temp, 0, BUFSZ);
                strncpy(temp, (buf + 23), 2);
                if(!(strcmp(temp, "in"))){
                    memset(temp, 0, BUFSZ);
                    strncpy(temp, (buf + 14), 2);
                    sensor_input[0] = atoi(temp);
                    memset(temp, 0, BUFSZ);
                    strncpy(temp, (buf + 17), 2);
                    sensor_input[1] = atoi(temp);
                    strncpy(temp, (buf + 20), 2);
                    sensor_input[2] = atoi(temp);
                    num_of_sensors = 2;
                    strncpy(temp, (buf + 26), 2);
                    equip_id = atoi(temp);
                    if(sensor_input[0] < 1 || sensor_input[0] > 4
                    ||sensor_input[1] < 1 ||sensor_input[1] > 4 || sensor_input[2] < 0 || sensor_input[2] > 4){
                       sprintf(buf, "invalid sensor\n");
                    }
                    else{
                        num_of_sensors = 3;
                    }
                }

                //num invalido de equipamento
                if((equip_id < 1 || equip_id > 4) && num_of_sensors){
                    sprintf(buf, "invalid equipment\n"); 
                }

                //tentando remover 1 sensor
                else if(num_of_sensors == 1){
                    
                    if((sensors[equip_id - 1][sensor_input[0]-1]) == 0){
                        sprintf(buf, "sensor %02d does not exist in %02d\n", sensor_input[0], equip_id); 
                    }
                    else{
                        sensors[equip_id - 1][sensor_input[0]-1] = 0;
                        total_of_sensors--;
                        sprintf(buf, "sensor %02d removed\n", sensor_input[0]); 
                    } 
                }
                //tentando remover 2 sensores
                else if(num_of_sensors== 2){
                    //checa quantos e quais dos requeridos ja foram removidos
                    int errors[] = {0,0};
                    for(i = 0; i<2;i++){
                        if(((sensors[equip_id - 1][(sensor_input[i])-1]) == 0)){
                            errors[i] = 1;
                            
                        }
                        
                    }
                    //caso todos ja foram removidos
                    if(errors[0] && errors[1]){
                        sprintf(buf, "sensor %02d %02d does not exist in %02d\n", sensor_input[0], sensor_input[1], equip_id); 
                    }
                    //caso 1 ja foi removido
                    else if( errors[0] && !(errors[1])){
                        sensors[equip_id - 1][sensor_input[1]-1] = 0;
                        total_of_sensors--;
                        sprintf(buf, "sensor %02d removed %02d does not exist in %02d\n", sensor_input[1], sensor_input[0], equip_id);
                    }
                    else if( errors[1] && !(errors[0])){
                        sensors[equip_id - 1][sensor_input[0]-1] = 0;
                        total_of_sensors--;
                        sprintf(buf, "sensor %02d removed %02d does not exist in %02d\n", sensor_input[0], sensor_input[1], equip_id);
                    }
                    //caso nenhum tenha sido removido
                    else if(!(errors[1]) && !(errors[0])){
                        total_of_sensors--;
                        total_of_sensors--;
                        sprintf(buf, "sensor %02d %02d removed\n", sensor_input[0], sensor_input[1]);
                        sensors[equip_id - 1][sensor_input[0]-1] = 0;
                        sensors[equip_id - 1][sensor_input[1]-1] = 0;
                    }
                }
                //tentando remover 3 sensores
                else if(num_of_sensors == 3){
                    int errors[] = {0,0,0};
                    //checa quantos e quais dos requeridos ja foram removidos
                    for(i = 0; i<3;i++){
                        if(((sensors[equip_id - 1][sensor_input[i]-1]) == 0)){
                            errors[i] = 1;
                        }
                    }
                    int sum_of_errors = errors[0] + errors[1] + errors[2];
                    //caso 3 ja estejam removidos
                    if(errors[0] && errors[1] && errors[2]){
                        sprintf(buf, "sensor %02d %02d %02d does not exist in %02d\n", sensor_input[0], sensor_input[1], sensor_input[2], equip_id); 
                    }
                    //caso 2 ja estejam removidos
                    else if(sum_of_errors == 2){
                        int error_index[2];
                        int aux =0;
                        int non_error_index;
                        for(i = 0; i<3;i++){
                            if(errors[i] == 0){
                                non_error_index = i;
                            }
                            else{
                                error_index[aux] = i;
                                aux++;
                            }
                        }
                        sprintf(buf, "sensor %02d removed %02d %02d does not exist in %02d\n", sensor_input[non_error_index],
                                                        sensor_input[error_index[1]], sensor_input[error_index[0]], equip_id);
                        total_of_sensors--;
                        sensors[equip_id - 1][sensor_input[non_error_index]-1] = 0;
                        
                    }
                    //caso 1 ja esteja removido
                    else if(sum_of_errors == 1){
                        int non_error_index[2];
                        int aux =0;
                        int error_index;
                        for(i = 0; i<3;i++){
                            if(errors[i] == 1){
                                error_index = i;
                            }
                            else{
                                non_error_index[aux] = i;
                                aux++;
                            }
                        }
                        sprintf(buf, "sensor %02d %02d removed %02d does not exist in %02d\n", sensor_input[non_error_index[1]],
                                                        sensor_input[non_error_index[0]], sensor_input[error_index], equip_id);
                        total_of_sensors--;
                        total_of_sensors--;
                        sensors[equip_id - 1][sensor_input[non_error_index[0]]-1] = 0;
                        sensors[equip_id - 1][sensor_input[non_error_index[1]]-1] = 0;
                    
                    }
                    else if(sum_of_errors == 0){
                        sprintf(buf, "sensor %02d %02d %02d removed\n", sensor_input[0],
                                                    sensor_input[1], sensor_input[2]);
                        total_of_sensors--;
                        total_of_sensors--;
                        total_of_sensors--;
                        sensors[equip_id - 1][sensor_input[0]-1] = 0;
                        sensors[equip_id - 1][sensor_input[1]-1] = 0;
                        sensors[equip_id - 1][sensor_input[2]-1] = 0;
                    
                    }
                }
            }
            //Função Read
            memset(temp, 0, BUFSZ);
            strncpy(temp, (buf + 0), 4);
            if((!strcmp(temp, "read"))){
                num_of_sensors = 0;
                //checa se foi dado o input de 1 sensor
                memset(temp, 0, BUFSZ);
                strncpy(temp, (buf + 8), 2);
                if(!(strcmp(temp, "in"))){
                    memset(temp, 0, BUFSZ);
                    strncpy(temp, (buf + 5), 2);
                    sensor_input[0] = atoi(temp);
                    memset(temp, 0, BUFSZ);
                    strncpy(temp, (buf + 11), 2);
                    equip_id = atoi(temp);
                     if(sensor_input[0] < 1 || sensor_input[0] > 4){
                       sprintf(buf, "invalid sensor\n");
                    }
                    else{
                        num_of_sensors = 1;
                    }
                }
                //checa se foi dado o input de 2 sensores
                memset(temp, 0, BUFSZ);
                strncpy(temp, (buf + 11), 2);
                if(!(strcmp(temp, "in"))){
                    memset(temp, 0, BUFSZ);
                    strncpy(temp, (buf + 5), 2);
                    sensor_input[0] = atoi(temp);
                    memset(temp, 0, BUFSZ);
                    strncpy(temp, (buf + 8), 2);
                    sensor_input[1] = atoi(temp);
                    strncpy(temp, (buf + 14), 2);
                    equip_id = atoi(temp);
                    if(sensor_input[0] < 1 || sensor_input[0] > 4
                    ||sensor_input[1] < 1 ||sensor_input[1] > 4){
                       sprintf(buf, "invalid sensor\n");
                    }
                    else{
                        num_of_sensors = 2;
                    }
                }
                //checa se foi dado o input de 3 sensores
                memset(temp, 0, BUFSZ);
                strncpy(temp, (buf + 14), 2);
                if(!(strcmp(temp, "in"))){
                    memset(temp, 0, BUFSZ);
                    strncpy(temp, (buf + 5), 2);
                    sensor_input[0] = atoi(temp);
                    memset(temp, 0, BUFSZ);
                    strncpy(temp, (buf + 8), 2);
                    sensor_input[1] = atoi(temp);
                    strncpy(temp, (buf + 11), 2);
                    sensor_input[2] = atoi(temp);
                    strncpy(temp, (buf + 17), 2);
                    equip_id = atoi(temp);
                    if(sensor_input[0] < 1 || sensor_input[0] > 4
                    ||sensor_input[1] < 1 ||sensor_input[1] > 4 || sensor_input[2] < 0 || sensor_input[2] > 4){
                       sprintf(buf, "invalid sensor\n");
                    }
                    else{
                        num_of_sensors = 3;
                    }
                }
                //checa se foi dado o input de 4 sensores
                memset(temp, 0, BUFSZ);
                strncpy(temp, (buf + 17), 2);
                if(!(strcmp(temp, "in"))){
                    memset(temp, 0, BUFSZ);
                    strncpy(temp, (buf + 5), 2);
                    sensor_input[0] = atoi(temp);
                    memset(temp, 0, BUFSZ);
                    strncpy(temp, (buf + 8), 2);
                    sensor_input[1] = atoi(temp);
                    strncpy(temp, (buf + 11), 2);
                    sensor_input[2] = atoi(temp);
                    strncpy(temp, (buf + 14), 2);
                    sensor_input[3] = atoi(temp);
                    strncpy(temp, (buf + 20), 2);
                    equip_id = atoi(temp);
                    if(sensor_input[0] < 1 || sensor_input[0] > 4
                    ||sensor_input[1] < 1 ||sensor_input[1] > 4 || sensor_input[2] < 0 || sensor_input[2] > 4
                    ||sensor_input[3] < 1 ||sensor_input[3] > 4){
                       sprintf(buf, "invalid sensor\n");
                    }
                    else{
                        num_of_sensors = 4;
                    }
                }
                //num invalido de equipamento
                if((equip_id < 1 || equip_id > 4) && num_of_sensors){
                    sprintf(buf, "invalid equipment\n"); 
                }
                //tentando ler 1 sensor
                else if(num_of_sensors == 1){
                    
                    if((sensors[equip_id - 1][sensor_input[0]-1]) == 0){
                        sprintf(buf, "sensor(s) %02d not installed\n", sensor_input[0]); 
                    }
                    else{
                        sprintf(buf, "%.02lf\n",randfrom(0.0, 10.0)); 
                    } 
                }
                //tentando ler 2 sensores
                else if(num_of_sensors== 2){
                    int errors[] = {0,0};
                    for(i = 0; i<2;i++){
                        if(((sensors[equip_id - 1][(sensor_input[i])-1]) == 0)){
                            errors[i] = 1;   
                        }
                    }
                    if(errors[0] && errors[1]){
                        sprintf(buf, "sensor(s) %02d %02d not installed\n", sensor_input[0], sensor_input[1]); 
                    }
                    else if( errors[0] && !(errors[1])){
                        sprintf(buf, "%.02lf and %02d not installed\n", randfrom(0.0, 10.0) , sensor_input[0]);
                    }
                    else if( errors[1] && !(errors[0])){
                        sprintf(buf, "%.02lf and %02d not installed\n", randfrom(0.0, 10.0) , sensor_input[1]);
                    }
                    else if(!(errors[1]) && !(errors[0])){
                        sprintf(buf, "%.02lf %.02lf", randfrom(0.0, 10.0), randfrom(0.0, 10.0));
                    }
                }
                //tentando ler 3 sensores
                else if(num_of_sensors == 3){
                    int errors[] = {0,0,0};
                    //checa quantos e quais sao sensores inexistentes
                    for(i = 0; i<3;i++){
                        if(((sensors[equip_id - 1][sensor_input[i]-1]) == 0)){
                            errors[i] = 1;
                        }
                    }
                    int sum_of_errors = errors[0] + errors[1] + errors[2];
                    //caso 2 não estejam instalados
                    if(errors[0] && errors[1] && errors[2]){
                        sprintf(buf, "sensor(s) %02d %02d %02d not installed\n", sensor_input[0], sensor_input[1], sensor_input[2]); 
                    }
                    //caso 2 não estejam instalados
                    else if(sum_of_errors == 2){
                        int error_index[2];
                        int aux =0;
                        for(i = 0; i<3;i++){
                            if(errors[i] == 0){}
                            else{
                                error_index[aux] = i;
                                aux++;
                            }
                        }
                        sprintf(buf, "%.02lf and %02d %02d not installed\n", randfrom(0.0, 10.0),
                                                    sensor_input[error_index[1]], sensor_input[error_index[0]]); 
                    }
                    //caso 1 nao esteja instalado
                    else if(sum_of_errors == 1){
                        int error_index;
                        for(i = 0; i<3;i++){
                            if(errors[i] == 1){
                                error_index = i;
                            }
                        }
                    sprintf(buf, "%.02lf  %.02lf and %02d not installed\n", randfrom(0.0, 10.0),
                                                    randfrom(0.0, 10.0), sensor_input[error_index]);
                    }
                    //caso todos estejam instalados
                    else if(sum_of_errors == 0){
                        sprintf(buf, "%.02lf  %.02lf %.02lf\n", randfrom(0.0, 10.0),
                                                    randfrom(0.0, 10.0), randfrom(0.0, 10.0));
                    }
                }
                //tentando ler 4 sensores
                else if(num_of_sensors == 4){
                    int errors[] = {0,0,0,0};
                    //checa quantos e quais sao sensores inexistentes
                    for(i = 0; i<4;i++){
                        if(((sensors[equip_id - 1][sensor_input[i]-1]) == 0)){
                            errors[i] = 1;
                        }
                    }
                    int sum_of_errors = errors[0] + errors[1] + errors[2] + errors[3];
                    //caso 4 não estejam instalados
                    if(sum_of_errors == 4){
                        sprintf(buf, "sensor(s) %02d %02d %02d %02d not installed\n", sensor_input[0], sensor_input[1], sensor_input[2], sensor_input[3]); 
                    }
                    //caso 3 não estejam instalados
                    else if(sum_of_errors == 3){
                        int error_index[3];
                        int aux =0;
                        for(i = 0; i<4;i++){
                            if(errors[i] == 0){
                            }
                            else{
                                error_index[aux] = i;
                                aux++;
                            }
                        }
                        sprintf(buf, "%.02lf and %02d %02d %02d not installed\n", randfrom(0.0, 10.0),
                                                sensor_input[error_index[2]], sensor_input[error_index[1]], sensor_input[error_index[0]]);
                    }
                    //caso 2 não estejam instalados
                    else if(sum_of_errors == 2){
                        int error_index[2];
                        int aux =0;
                        for(i = 0; i<4;i++){
                            if(errors[i] == 0){
                            }
                            else{
                                error_index[aux] = i;
                                aux++;
                            }
                        }
                        sprintf(buf, "%.02lf %.02lf and %02d %02d not installed\n", randfrom(0.0, 10.0), randfrom(0.0, 10.0),
                                                    sensor_input[error_index[1]], sensor_input[error_index[0]]); 
                    }
                    //caso 1 não esteja instalado
                    else if(sum_of_errors == 1){
                        int error_index;
                        for(i = 0; i<4;i++){
                            if(errors[i] == 1){
                                error_index = i;
                            }
                        }
                    sprintf(buf, "%.02lf  %.02lf %.02lf and %02d not installed\n", randfrom(0.0, 10.0), randfrom(0.0, 10.0),
                                                    randfrom(0.0, 10.0), sensor_input[error_index]);
                    }
                    //caso todos estejam instalados
                    else if(sum_of_errors == 0){
                        sprintf(buf, "%.02lf %.02lf %.02lf %.02lf\n", randfrom(0.0, 10.0), randfrom(0.0, 10.0),
                                                    randfrom(0.0, 10.0), randfrom(0.0, 10.0));
                    }
                }
            }
            
            //Função list
            memset(temp, 0, BUFSZ);
            strncpy(temp, (buf + 0), 15);
            if((!strcmp(temp, "list sensors in"))){
                memset(temp, 0, BUFSZ);
                strncpy(temp, (buf + 16), 2);
                equip_id = atoi(temp);
                if((equip_id < 1 || equip_id > 4) && num_of_sensors){
                    sprintf(buf, "invalid equipment\n");
                }
                else{
                    int num_of_sensors_that_exists =0;
                    int sensors_that_exists[4];
                    //checa quantos e quais sao sensores existentes
                    for(i = 0; i < 4; i++){
                        if(sensors[equip_id-1][i] == 1){
                            sensors_that_exists[num_of_sensors_that_exists] = i+1;
                            num_of_sensors_that_exists++;
                        }
                    }
                    if(num_of_sensors_that_exists == 0){
                        sprintf(buf,"none\n");
                    }
                    if(num_of_sensors_that_exists ==1){
                        sprintf(buf,"%02d\n", sensors_that_exists[0]);
                    }
                    if(num_of_sensors_that_exists ==2){
                        sprintf(buf,"%02d %02d\n", sensors_that_exists[0], sensors_that_exists[1]);
                    }
                    if(num_of_sensors_that_exists ==3){
                        sprintf(buf,"%02d %02d %02d\n", sensors_that_exists[0], sensors_that_exists[1], sensors_that_exists[2]);
                    }
                    if(num_of_sensors_that_exists ==4){
                        sprintf(buf,"%02d %02d %02d %02d\n", sensors_that_exists[0], sensors_that_exists[1],
                                    sensors_that_exists[2], sensors_that_exists[3]);
                    }
                }
            }

            printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)count, buf);
            //verifica se houve alteracao no buffer
            //se houver o output é valido e a conexao é mantida
            int is_output_valid = (strcmp(buf,input));
            if(is_output_valid){
                count = send(csock, buf, strlen(buf) + 1, 0);
                if (count != strlen(buf) + 1) {
                    logexit("send");
                }
            }
            else{
                sprintf(buf,"disconnect");                          //se o input enviado pelo client, ainda no buffer
                count = send(csock, buf, strlen(buf) + 1, 0);       //nao cumprir a sintaxe de mensagem valida o servidor
                if (count != strlen(buf) + 1) {                     //disconecta o client e continua esperando por novas conexões
                    logexit("send");
                }
            }
            close(csock);

        }while((strcmp(input, "kill\n")));
    
    

    exit(EXIT_SUCCESS);
}