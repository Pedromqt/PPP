#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#define MAX 100

typedef struct data{
    int ano;
    int mes;
    int dia;
    int hora;
    int minuto;
}data;
    
typedef struct{
    char nome[MAX];
    int numtel;
    char servico[10];
    data data;
}cliente;

typedef struct noLista{
    cliente cliente;
    struct noLista *prox;
}noLista;

typedef noLista*Lista;

Lista start(){
    Lista aux;
    data d;
    d.ano=0;
    d.mes=0;
    d.dia=0;
    d.hora=0;
    d.minuto=0;
    cliente c = {"",0,"",d};
    aux = (Lista) malloc (sizeof (noLista));
    if (aux == NULL) {
        printf("Erro a alocar memória!\n");
        exit(-1);
    }else{
        aux->cliente = c;
        aux->prox = NULL;
    }
    return aux; 
}

int tempo_servico(char*servico){
    if (strcmp(servico,"Manutencao") == 0){
        return 60;
    }else{
        return 30;
    }
}

int cmpDatas(data*atual,data*cliente_atual){
    if(cliente_atual->ano > atual->ano){
        return 1;
    }else if(cliente_atual->ano < atual->ano){
        return -1;
    }else if(cliente_atual->mes > atual->mes){
        return 1;
    }else if(cliente_atual->mes < atual->mes){
        return -1;
    }else if(cliente_atual->dia > atual->dia){
        return 1;
    }else if (cliente_atual->dia < atual->dia){
        return -1;
    }else if(cliente_atual->hora > atual->hora){
        return 1;
    }else if(cliente_atual->hora < atual->hora){
        return -1;
    }else if(cliente_atual->minuto > atual->minuto){
        return 1;
    }else if(cliente_atual->minuto < atual->minuto){
        return -1;
    }else{
        return 0;
    }
} 

void procura(Lista lista, cliente cliente_atual, Lista *ant_reservas, Lista *atual){
    *ant_reservas = lista; 
    *atual = lista->prox;
    while ((*atual) != NULL && cmpDatas(&(*atual)->cliente.data,&cliente_atual.data)== 1){
        *ant_reservas = *atual;
        *atual = (*atual)->prox;
    }
}

int cmpDuracao2(cliente*ant_reservas,cliente*c){
    if(ant_reservas->data.ano==c->data.ano && ant_reservas->data.mes==c->data.mes && ant_reservas->data.dia==c->data.dia){
        if(((c->data.hora*60+c->data.minuto)-(ant_reservas->data.hora*60+ant_reservas->data.minuto))>=tempo_servico(ant_reservas->servico)){
            return 1;
        }else{
            return 0;
        }
    }else{
        return 1;
    }
}

int cmpDuracao1(cliente*ant_reservas,cliente*c,cliente*atual,int duracao_servico){
    if(ant_reservas->data.ano==c->data.ano && ant_reservas->data.mes==c->data.mes && ant_reservas->data.dia==c->data.dia){
        if(((c->data.hora*60+c->data.minuto)-(ant_reservas->data.hora*60+ant_reservas->data.minuto))>=duracao_servico){
            return 1;
        }else{
            return 0;
        }
    }else if(atual->data.ano==c->data.ano && atual->data.mes==c->data.mes && atual->data.dia==c->data.dia){
        if(((atual->data.hora*60+atual->data.minuto)-(c->data.hora*60+c->data.minuto))>=duracao_servico){
            return 1;
        }else{
            return 0;
        }
    }else{
        return 1;
    }
}

void elimina_cliente(Lista lista, cliente c){
    Lista ant, atual;
    procura (lista,c,&ant, &atual);
    if (atual != NULL) {
        ant->prox = atual->prox;
        free (atual);
    }
}

void marcar(Lista reservas,Lista prereservas,FILE*ficheiro_reservas,FILE*ficheiro_prereservas,cliente c){
    Lista ptr,ptr_prereservas,ant_reservas,atual_reservas,ant_prereservas,atual_prereservas;
    ptr = (Lista) malloc (sizeof (noLista));
    ptr_prereservas = (Lista) malloc (sizeof (noLista));
    
    int duracao_servico;
    duracao_servico = tempo_servico(c.servico);

    if (ptr != NULL && ptr_prereservas!= NULL) {
        ptr->cliente = c;
        ptr_prereservas->cliente = c;   

        /*COLOCA O CLIENTE NA LISTA DAS RESERVAS*/
        procura(reservas, c, &ant_reservas, &atual_reservas);
        ptr->prox = ant_reservas->prox;
        ant_reservas->prox = ptr;

        /*VERIFICAR SE É POSSIVEL COLOCAR O CLIENTE NA LISTAS DAS RESERVAS E SE NAO FOR, LISTAR NAS PRE-RESERVAS*/  
        if(ant_reservas!=NULL && ptr->prox!=NULL){
            if(cmpDuracao1(&(ant_reservas->cliente),&(ptr->cliente),&(ptr->prox->cliente),duracao_servico)==0){
                printf("Horário Indisponivel! %s %d %02d/%02d/%02d %02d:%02d %s sera colocado nas pre-reservas\n",ptr->cliente.nome,ptr->cliente.numtel,ptr->cliente.data.dia,ptr->cliente.data.mes,ptr->cliente.data.ano,ptr->cliente.data.hora,ptr->cliente.data.minuto,ptr->cliente.servico);
                procura(prereservas,c,&ant_prereservas,&atual_prereservas);
                ptr_prereservas->prox = ant_prereservas->prox;
                ant_prereservas->prox = ptr_prereservas;
                fprintf(ficheiro_prereservas,"%s %d %02d/%02d/%02d %02d:%02d %s\n",ptr->cliente.nome,ptr->cliente.numtel,ptr->cliente.data.dia,ptr->cliente.data.mes,ptr->cliente.data.ano,ptr->cliente.data.hora,ptr->cliente.data.minuto,ptr->cliente.servico);
                elimina_cliente(reservas,ptr->cliente);
                return;
            }
        }else if (ant_reservas!=NULL && ptr->prox==NULL){
            if(cmpDuracao2(&(ant_reservas->cliente),&(ptr->cliente))==0){
                printf("Horário Indisponivel! %s %d %02d/%02d/%02d %02d:%02d %s sera listado nas pre-reservas\n",ptr->cliente.nome,ptr->cliente.numtel,ptr->cliente.data.dia,ptr->cliente.data.mes,ptr->cliente.data.ano,ptr->cliente.data.hora,ptr->cliente.data.minuto,ptr->cliente.servico);
                procura(prereservas,c,&ant_prereservas,&atual_prereservas);
                ptr_prereservas->prox = ant_prereservas->prox;
                ant_prereservas->prox = ptr_prereservas; 
                /*ESCREVER NO FICHEIRO DAS PRE-RESERVAS OS DADOS DO CLIENTE*/
                fprintf(ficheiro_prereservas,"%s|%d %02d/%02d/%02d %02d:%02d %s\n",ptr->cliente.nome,ptr->cliente.numtel,ptr->cliente.data.dia,ptr->cliente.data.mes,ptr->cliente.data.ano,ptr->cliente.data.hora,ptr->cliente.data.minuto,ptr->cliente.servico);
                elimina_cliente(reservas,ptr->cliente);
                return;
            }
        }
        /*ESCREVER NO FICHEIRO DAS RESERVAS OS DADOS DO CLIENTE*/
        fprintf(ficheiro_reservas,"%s|%d %02d/%02d/%02d %02d:%02d %s\n",ptr->cliente.nome,ptr->cliente.numtel,ptr->cliente.data.dia,ptr->cliente.data.mes,ptr->cliente.data.ano,ptr->cliente.data.hora,ptr->cliente.data.minuto,ptr->cliente.servico);
    }   
}

void dados_linha(char*linha,cliente*c){
    
    char*token=strtok(linha,"|");
    strcpy(c->nome,token);

    token=strtok(NULL," ");
    sscanf(token,"%d",&c->numtel);

    token=strtok(NULL," ");
    sscanf(token,"%d/%d/%d",&c->data.dia,&c->data.mes,&c->data.ano);

    token=strtok(NULL," ");
    sscanf(token,"%d:%d",&c->data.hora,&c->data.minuto);
    
    token=strtok(NULL," ");
    sscanf(token,"%s",c->servico);
    
}

void imprimir_crescente(Lista lista){
    Lista ptr=lista->prox;
    while(ptr!=NULL){
        printf("%s %d %02d/%02d/%02d %02d:%02d %s\n",ptr->cliente.nome,ptr->cliente.numtel,ptr->cliente.data.dia,ptr->cliente.data.mes,ptr->cliente.data.ano,ptr->cliente.data.hora,ptr->cliente.data.minuto,ptr->cliente.servico);
        ptr=ptr->prox;
    }   
    
}

void imprimir_decrescente(Lista lista){
    Lista ptr = lista->prox;
    Lista pilha = NULL;

    while (ptr != NULL) {
        Lista novoNo = (Lista)malloc(sizeof(noLista));
        if (novoNo != NULL) {
            novoNo->cliente = ptr->cliente;
            novoNo->prox = pilha;
            pilha = novoNo;
            ptr = ptr->prox;
        }else{
            printf("Erro a alocar memoria\n");
            exit(-1);
        }
    }
    while (pilha != NULL) {
        printf("%s %d %02d/%02d/%02d %02d:%02d %s\n", pilha->cliente.nome, pilha->cliente.numtel, pilha->cliente.data.dia, pilha->cliente.data.mes, pilha->cliente.data.ano, pilha->cliente.data.hora, pilha->cliente.data.minuto, pilha->cliente.servico);
        Lista temp = pilha;
        pilha = pilha->prox;
        free(temp);
    }}

int verificacao_dados(cliente c){
    int cont=0;
    int temp_num=c.numtel;
    int flag;
    /*Verificar se o mes que o cliente insere existe e se o dia desse mes tambem existe*/
    int array[12][2]={{1,31},{2,28},{3,31},{4,30},{5,31},{6,30},{7,31},{8,31},{9,30},{10,31},{11,30},{12,31}};
    if(c.data.mes<=12 && c.data.mes>0 && c.data.ano>0){
        for(int i=0;i<12;i++){
            if(c.data.mes==array[i][0]){
                if(c.data.dia<=array[i][1] && c.data.dia>0){
                    flag=1;
                    break;
                }else{
                    flag=0;
                    break;
                }
            }
        }
    }else{
        flag=0;
    }
    while(temp_num!=0){
        temp_num/=10;
        cont++;
    }
    int duracao_servico=tempo_servico(c.servico);
    if(cont==9 && (c.data.hora>=8 && c.data.hora*60+c.data.minuto+duracao_servico<=18*60) && flag==1){
        return 1;
    }else{
        return 0;
    }
}

void elimina_ficheiro(const char*ficheiro_txt,cliente c1){
    FILE*ficheiro;
    FILE*temp_ficheiro;
    char linha[MAX];
    ficheiro=fopen(ficheiro_txt,"r");
    temp_ficheiro=fopen("temp.txt","w");
    
    if(ficheiro!=NULL && temp_ficheiro!=NULL){
        fseek(ficheiro, 0, SEEK_SET);
        while(fgets(linha,MAX,ficheiro)!=NULL){
            cliente c2;
            dados_linha(linha,&c2);
            if(c2.data.ano==c1.data.ano && c2.data.mes==c1.data.mes && c2.data.dia==c1.data.dia && c2.data.hora==c1.data.hora && c2.data.minuto==c1.data.minuto){
                continue;
            }else{
                fprintf(temp_ficheiro,"%s|%d %02d/%02d/%02d %02d:%02d %s\n",c2.nome,c2.numtel,c2.data.dia,c2.data.mes,c2.data.ano,c2.data.hora,c2.data.minuto,c2.servico); 
            }
        }
    }
    fclose(ficheiro);
    fclose(temp_ficheiro);
    remove(ficheiro_txt);
    rename("temp.txt",ficheiro_txt);
}

void imprimir_cliente(Lista lista,cliente c){
    Lista ptr=lista->prox;
    while(ptr!=NULL){
        if(strcmp(ptr->cliente.nome,c.nome)==0){
            printf("%s %d %02d/%02d/%02d %02d:%02d %s\n",ptr->cliente.nome,ptr->cliente.numtel,ptr->cliente.data.dia,ptr->cliente.data.mes,ptr->cliente.data.ano,ptr->cliente.data.hora,ptr->cliente.data.minuto,ptr->cliente.servico);
            ptr=ptr->prox;
        }
    }
}

int menu_cliente(void){
    int res;
    printf("O que deseja fazer?\n");
    printf("1-Ver as minhas reservas.\n");
    printf("2-Ver as minhas pre-reservas");
    scanf("%d",&res);
    getchar();
    return res;
}

int menu_trabalhador(void){
    int res;
    printf("O que deseja fazer?\n");
    printf("1-Eliminar cliente das reservas\n");
    printf("2-Eliminar cliente das pre-reservas\n");
    printf("3-Mostrar os clientes listados nas reservas\n");
    printf("4-Mostrar os clientes listados nas pre-reservas\n");
    printf("5-Executar tarefa!\n");
    scanf("%d",&res);
    getchar();/*Elimina o ultimo carater do buffer de entrada*/
    return res;
}

void apagar_dados_aplicacao(Lista reservas, Lista prereservas, char*ficheiro_reservas, char* ficheiro_prereservas){
    Lista ptr=reservas,ptr2=prereservas;
    while (ptr != NULL) {
        Lista prox = ptr->prox;
        free(ptr);
        ptr = prox;
    }
    while (ptr2 != NULL) {
        Lista prox = ptr2->prox;
        free(ptr2);
        ptr2 = prox;
    }
    remove(ficheiro_reservas);
    remove(ficheiro_prereservas);

}

void inicializar_aplicacao(void){
    int cmpDatas(data*d1,data*d2);
    FILE *ficheiro_marcacoes;
    FILE *ficheiro_reservas;
    FILE *ficheiro_prereservas;

    ficheiro_marcacoes=fopen("marcacoes.txt","r");
    ficheiro_reservas=fopen("reservas.txt","w");
    ficheiro_prereservas=fopen("pre-reservas.txt","w");

    char linha[MAX];
    Lista reservas = start();
    Lista prereservas = start();

    while (fgets(linha,MAX, ficheiro_marcacoes) != NULL) {
        cliente c;
        dados_linha(linha,&c); 
        if (verificacao_dados(c)==1){
            marcar(reservas,prereservas,ficheiro_reservas,ficheiro_prereservas,c);
        }else{
            printf("O cliente %s apresenta dados invalidos, logo nao sera agendado.\n",c.nome);
        }
    }
    fclose(ficheiro_marcacoes);
    fclose(ficheiro_reservas);
    fclose(ficheiro_prereservas);
    int flag=0;
    int flag1=0;
    int flag2=0;
    int flag4=0;
    cliente cli;
    char str[MAX];
    while(flag==0){
        printf("1 - Trabalhador da aplicacao.\n");
        printf("2 - Cliente.\n");
        scanf("%d",&flag1);
        getchar();
        if(flag1==1){  
            printf("Insira a palavra-passe!\n");
            scanf("%s",str);
            getchar();
            if(strcmp(str,"trabalhar123")==0){
                flag=1;
                flag2=1;
            }else{
                printf("Palavra-passe errada, tente novamente!\n");
            }
        }else if(flag1==2){
            printf("Insira o seu nome:\n");
            scanf("%s",cli.nome);
            getchar();
            flag=1;
            flag4=1;
        }else{
            printf("Opcao invalida!\n");
        }
    }
    while(flag4==1){
        switch(menu_cliente()){
            case 1:
                imprimir_cliente(reservas,cli);
            break;
            case 2:
                imprimir_cliente(prereservas,cli);
            break;
            default:
                printf("Opcao invalida!\n");
            break;
        }
        printf("Pretende continuar a operar?\n");
        printf("| 1-Sim | Outra-Nao |");
        scanf("%d",&flag4);
        getchar();
    }
    
    
    while(flag2==1){
        switch(menu_trabalhador()){
            char data_str[MAX];
            cliente c;
            int res2;
            case 1:
                if(reservas->prox!=NULL){
                    printf("Insira o nome do cliente:\n");
                    scanf("%s",c.nome);
                    getchar();
                    printf("Insira a data da reserva deste cliente\n");
                    fgets(data_str,MAX,stdin);
                    sscanf(data_str,"%d/%d/%d %d:%d %s",&c.data.dia,&c.data.mes,&c.data.ano,&c.data.hora,&c.data.minuto,c.servico);
                    elimina_cliente(reservas,c);
                    printf("Foi desmarcada com sucesso a %s de %s em %d/%02d/%d %02d:%02d!\n",c.servico,c.nome,c.data.dia,c.data.mes,c.data.ano,c.data.hora,c.data.minuto);
                    elimina_ficheiro("reservas.txt",c);
                    Lista pre_reservas=prereservas->prox;
                    printf("Sera verificado se existe alguem das pre-reservas que possa ser colocado nas reservas!\n");
                    /*Caso nao seja possivel colocar o cliente das pre reservas, este voltara novamente a ser colocado nas pre-reservas*/
                    while(pre_reservas!=NULL){
                        elimina_ficheiro("pre-reservas.txt", pre_reservas->cliente);
                        marcar(reservas, prereservas, ficheiro_reservas, ficheiro_prereservas, pre_reservas->cliente);
                        elimina_cliente(prereservas,c);
                        pre_reservas=pre_reservas->prox;
                    }
                }else{
                    printf("Nao existem reservas!\n");
                }
            break;
            case 2:
                if(prereservas->prox!=NULL){
                    printf("Insira o nome do cliente:\n");
                    scanf("%s",c.nome);
                    getchar();
                    printf("Insira a data da pre-reserva deste cliente\n");
                    fgets(data_str,MAX,stdin);
                    sscanf(data_str,"%d/%d/%d %d:%d",&c.data.dia,&c.data.mes,&c.data.ano,&c.data.hora,&c.data.minuto);
                    elimina_cliente(prereservas,c);
                    printf("Foi desmarcada com sucesso a pre-reserva de %s em %d/%d/%d %d:%d!\n",c.nome,c.data.dia,c.data.mes,c.data.ano,c.data.hora,c.data.minuto);
                    elimina_ficheiro("reservas.txt",c);
                }else{
                    printf("Nao existem pre-reservas!\n");
                }
            break;
            case 3:
                if(reservas->prox!=NULL){
                    int i=1;
                    while(i){
                        printf("1-Imprimir a lista das reservas por ordem crescente da data.\n");
                        printf("2-Imprimir a lista das reservas por ordem decrescente da data.\n");
                        scanf("%d",&res2);
                        getchar();
                        if(res2==1){
                            imprimir_crescente(reservas);
                            i=0;
                        }else if(res2==2){
                            imprimir_decrescente(reservas);
                            i=0;
                        }else{
                            printf("Opcao invalida!\n");
                        }
                    }   
                }else{
                    printf("Nao existem reservas!\n");
                }
            break;
            case 4:
                if(prereservas->prox!=NULL){
                    int j=1;
                    while(j){
                        printf("1-Imprimir a lista das pre-reservas por ordem crescente da data.\n");
                        printf("2-Imprimir a lista das pre-reservas por ordem decrescente da data.\n");
                        scanf("%d",&res2);
                        getchar();
                        if(res2==1){
                            imprimir_crescente(prereservas);
                            j=0;
                        }else if(res2==2){
                            imprimir_decrescente(prereservas);
                            j=0;
                        }else{
                            printf("Opcao invalida!\n");
                        }
                    }
                }else{
                    printf("Nao existem pre-reservas!\n");
                }
            break;
            case 5:
                if(reservas->prox!=NULL){
                    strcpy(c.nome,reservas->prox->cliente.nome);
                    c.data.ano=reservas->prox->cliente.data.ano;
                    c.data.mes=reservas->prox->cliente.data.mes;
                    c.data.dia=reservas->prox->cliente.data.dia;
                    c.data.hora=reservas->prox->cliente.data.hora;
                    c.data.minuto=reservas->prox->cliente.data.minuto;
                    elimina_cliente(reservas,c);
                    printf("Foi executada com sucesso, a reserva de %s em %02d/%02d/%02d %02d:%02d\n",c.nome,c.data.ano,c.data.mes,c.data.ano,c.data.hora,c.data.minuto);
                    elimina_ficheiro("reservas.txt",c);
                }else{
                    printf("Nao existe reservas para executar!\n");
                }
            break;
            default:
                printf("Opcao invalida!\n");
            break;
        }
        printf("Deseja fazer mais operacoes?\n");
        printf("| 1-Sim | Outra-Nao |\n");
        scanf("%d",&flag2);
        getchar();
    }
    int limpar=0;
    printf("Deseja apagar todos os dados da aplicacao?\n");
    printf("| 1-Sim | Outra-Nao |");
    scanf("%d",&limpar);
    getchar();
    if(limpar==1){
        apagar_dados_aplicacao(reservas,prereservas,"reservas.txt","pre-reservas.txt");
        printf("Aplicacao formatada com sucesso!\n");
    }
}

void inicializar_funcoes(void){
    Lista start();
    int tempo_servico(char*servico);
    int cmpDatas(data*atual,data*cliente_atual);
    void procura(Lista lista, cliente cliente_atual, Lista *ant_reservas, Lista *atual);
    int cmpDuracao2(cliente*ant_reservas,cliente*c);
    int cmpDuracao1(cliente*ant_reservas,cliente*c,cliente*atual,int duracao_servico);
    void elimina_cliente(Lista lista, cliente c);
    void marcar(Lista reservas,Lista prereservas,FILE*ficheiro_reservas,FILE*ficheiro_prereservas,cliente c);
    void dados_linha(char*linha,cliente*c);
    void imprimir_crescente(Lista lista);
    void imprimir_decrescente(Lista lista);
    int verificacao_dados(cliente c);
    void elimina_ficheiro(const char*ficheiro_txt,cliente c1);
    void imprimir_cliente(Lista lista,cliente c);
    int menu_cliente(void);
    int menu_trabalhador(void);
    void apagar_dados_aplicacao(Lista reservas, Lista prereservas, char*ficheiro_reservas, char* ficheiro_prereservas);
    void inicializar_aplicacao(void);
}
int main(void){
    inicializar_funcoes();
    inicializar_aplicacao();
}