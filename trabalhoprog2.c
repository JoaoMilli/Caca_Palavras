/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   trabalhoprog2.c
 * Author: joao
 *
 * Created on 9 de Junho de 2019, 20:28
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{		   			
    char matriz[100][100];
    int numCol;
    int numLin;			//Struct para armazenar uma matriz, seu numero de colunas e linhas.	
} tMatriz;

typedef struct{
    int pontuacao;
    int rodada_pontuada;
} tDadosPalavras;		//Struct para armazenar dados das palavras(sera utilizado nas estatisticas).

typedef struct{
    int turno; 
    int tamanho_matriz;
    int n_palavras;
    int aux_n_palavras;
    char palavras[51][17];
    char aux_palavras[51][17]; 
    tMatriz matriz;
    int valpal[51];  //vetor para armazenar um valor que ditara se uma palavra ja foi usada
    int val_leitura;   //inteiro para checar o valor de retorno da leitura de arquivos
    int countpalavras1;
    int countpalavras2;
    int val_jogada[100][100]; //matriz para armazenar valoroes que ditarao se uma jogada ja foi feita
    int tam_palavras[51];
    tDadosPalavras dadospalavras[51];
    int aux_turno;
    char diretorio[1000];
    int ordem_palavra; //inteiro para armazenar a ordem das palavras descobertas(sera utilizado no mapa ordenado)
    tMatriz tabuleiropalavras;
} tConfig;			//Struct da configuracao, sera preenchido e contera a maioria das informacoes sobre o jogo e as jogadas

typedef struct{
    char nome[17];
    int pontuacao;
    int jogada_x;
    int jogada_y;
    int pontosfeitos;
    int pontoscedidos;
} tJogador;		//Struct que contem as informacoes dos jogadores

typedef struct{
    tMatriz tabuleiro;
    tJogador jogador;
    tJogador jogador_1;
    tJogador jogador_2;
    tConfig config;
    int val;
    int valpal[51];
    tMatriz tabuleiropalavras;
} tRetornoJogada; //Struct que retorna o resultado de uma jogada(sera mergida com o tConfig e os tJogadores da main)

tMatriz LeMatriz(){   //Funcao que le elementos do buffer e cria uma matriz
    tMatriz matriz;
    int i,j;
    char cel;
    scanf("%d",&matriz.numLin);
    scanf("%*c");
    matriz.numCol=matriz.numLin;
    for(i=0;i<matriz.numLin;i++){
        for(j=0;j<matriz.numCol; j++){
            scanf("%c",&cel);
            matriz.matriz[i][j]=cel;
        }
        scanf("%*c");
    }
    return matriz;
}

int ContaPalavras(tConfig config,int n){ 	//Funcao que conta quantidade de chars de uma string
    int i,count=0;
    for(i=0;config.palavras[n][i]!='\0';i++){
        count++;
    }
    return count;
}

void Mapaordenado(tConfig config,char* argv[]){ 	//Funcao que gera o mapa ordenado
    FILE*pFile;
    sprintf(config.diretorio,"%s/saida/MapaOrdenado.txt",argv[1]);
    pFile=fopen(config.diretorio,"w");
    int i,j;
    for(i=0;i<config.tabuleiropalavras.numLin;i++){
        for(j=0;j<config.tabuleiropalavras.numCol; j++){
            if((int)config.tabuleiropalavras.matriz[i][j]==45){
                fprintf(pFile," --");
            }
            else{
                fprintf(pFile," %02d",(int)config.tabuleiropalavras.matriz[i][j]);
            }
        }
        fprintf(pFile,"\n");
    }
    fclose(pFile);
}

void Inicializacao(tConfig config, tJogador jogador_1, tJogador jogador_2,char* argv[]){   //Funcao que gera a inicializacao
    FILE*pFile;
    sprintf(config.diretorio,"%s/saida/Inicializacao.txt",argv[1]);
    pFile=fopen(config.diretorio,"w");
    int i,maiorpalavra1=0,maiorpalavra2=0,auxpal1,auxpal2,count=0;
    char l;
    fprintf(pFile,"--Jogador 1--\n");
    fprintf(pFile,"Nome: %s\n",jogador_1.nome);
    fprintf(pFile,"Palavras:\n");
    for(i=0;i<config.n_palavras/2;i++){
        fprintf(pFile,"%s\n",config.palavras[i]);
    }
    for(i=0;i<config.n_palavras/2;i++){
        if(config.tam_palavras[i]>maiorpalavra1){
            maiorpalavra1=config.tam_palavras[i];
            auxpal1=i;
        }
    }
    fprintf(pFile,"Maior Palavra: \n");
    fprintf(pFile,"%s\n",config.palavras[auxpal1]);
    fprintf(pFile,"Letras Iniciais: \n");
    for(l='a';l<='z';l++){
        for(i=0;i<config.n_palavras/2;i++){
            if(config.palavras[i][0]==l){
                count++;
            }
        }
        if(count){
            fprintf(pFile,"%c -> %d\n",l,count);
            count=0;
        }
    }
    fprintf(pFile,"\n--Jogador 2--\n");
    fprintf(pFile,"Nome: %s\n",jogador_2.nome);
    fprintf(pFile,"Palavras:\n");
    for(i=config.n_palavras/2;i<config.n_palavras;i++){
        fprintf(pFile,"%s\n",config.palavras[i]);
    }
    for(i=config.n_palavras/2;i<config.n_palavras;i++){
        if(config.tam_palavras[i]>maiorpalavra2){
            maiorpalavra2=config.tam_palavras[i];
            auxpal2=i;
        }
    }
    fprintf(pFile,"Maior Palavra: \n");
    fprintf(pFile,"%s\n",config.palavras[auxpal2]);
    fprintf(pFile,"Letras Iniciais: \n");
    for(l='a';l<='z';l++){
        for(i=config.n_palavras/2;i<config.n_palavras;i++){
            if(config.palavras[i][0]==l){
                count++;
            }
        }
        if(count){
            fprintf(pFile,"%c -> %d\n",l,count);
            count=0;
        }
    }
    fclose(pFile);
}

void Estatisticas(tConfig config, tJogador jogador_1, tJogador jogador_2,char* argv[]){  //Funcao que gera as estatisticas
    FILE*pFile;
    sprintf(config.diretorio,"%s/saida/Estatisticas.txt",argv[1]);
    pFile=fopen(config.diretorio,"w");
    int total;
    fprintf(pFile,"--------------\n");
    fprintf(pFile,"--- PONTOS ---\n");
    fprintf(pFile,"--------------\n\n");
    fprintf(pFile,"%s\n",jogador_1.nome);
    fprintf(pFile,"Feitos: %d\n",jogador_1.pontosfeitos);
    fprintf(pFile,"Recebidos: %d\n",jogador_2.pontoscedidos);
    total=jogador_1.pontosfeitos+jogador_2.pontoscedidos;
    fprintf(pFile,"Total: %d\n",total);
    fprintf(pFile,"\n");
    fprintf(pFile,"%s\n",jogador_2.nome);
    fprintf(pFile,"Feitos: %d\n",jogador_2.pontosfeitos);
    fprintf(pFile,"Recebidos: %d\n",jogador_1.pontoscedidos);
    total=jogador_2.pontosfeitos+jogador_1.pontoscedidos;
    fprintf(pFile,"Total: %d\n",total);
    fprintf(pFile,"\n");
    fprintf(pFile,"---------------------------\n");
    fprintf(pFile,"--- PALAVRAS POR PONTOS ---\n");
    fprintf(pFile,"---------------------------\n\n");
    fprintf(pFile,"|P|     Palavra    | T |\n");
    fprintf(pFile,"|-|----------------|---|\n");
    int i,j,k,c,m,n,auxpontuacao,auxrodadapontuada;
    for(j=0;j<config.aux_n_palavras;j++){
        for(k=j;k<config.aux_n_palavras;k++){
            if(config.dadospalavras[j].pontuacao<config.dadospalavras[k].pontuacao){
                auxpontuacao=config.dadospalavras[j].pontuacao;
                auxrodadapontuada=config.dadospalavras[j].rodada_pontuada;
                strcpy(config.aux_palavras[50],config.aux_palavras[j]);
                for(m=0;m<15;m++){
                    if(config.aux_palavras[50][m]=='\0'){
                        for(n=m;n<15;n++){
                            config.palavras[25][n]='\0';
                        }
                        break;
                    }
                }
                config.dadospalavras[j].pontuacao=config.dadospalavras[k].pontuacao;
                config.dadospalavras[j].rodada_pontuada=config.dadospalavras[k].rodada_pontuada;
                strcpy(config.aux_palavras[j],config.aux_palavras[k]);
                for(m=0;m<15;m++){
                    if(config.aux_palavras[j][m]=='\0'){
                        for(n=m;n<15;n++){
                            config.aux_palavras[j][n]='\0';
                        }
                        break;
                    }
                }
                config.dadospalavras[k].pontuacao=auxpontuacao;
                config.dadospalavras[k].rodada_pontuada=auxrodadapontuada;
                strcpy(config.aux_palavras[k],config.aux_palavras[50]);
                for(m=0;m<15;m++){
                    if(config.aux_palavras[k][m]=='\0'){
                        for(n=m;n<15;n++){
                            config.aux_palavras[k][n]='\0';
                        }
                        break;
                    }
                }
            }
            else if(config.dadospalavras[j].pontuacao==config.dadospalavras[k].pontuacao){
                if(strcmp(config.aux_palavras[j],config.aux_palavras[k])>0){
                    auxpontuacao=config.dadospalavras[j].pontuacao;
                    auxrodadapontuada=config.dadospalavras[j].rodada_pontuada;
                    strcpy(config.aux_palavras[50],config.aux_palavras[j]);
                    for(m=0;m<15;m++){
                        if(config.aux_palavras[50][m]=='\0'){
                            for(n=m;n<15;n++){
                                config.aux_palavras[50][n]='\0';
                            }
                            break;
                        }
                    }
                    config.dadospalavras[j].pontuacao=config.dadospalavras[k].pontuacao;
                    config.dadospalavras[j].rodada_pontuada=config.dadospalavras[k].rodada_pontuada;
                    strcpy(config.aux_palavras[j],config.aux_palavras[k]);
                    for(m=0;m<15;m++){
                        if(config.aux_palavras[j][m]=='\0'){
                            for(n=m;n<15;n++){
                                config.aux_palavras[j][n]='\0';
                            }
                            break;
                        }
                    }
                    config.dadospalavras[k].pontuacao=auxpontuacao;
                    config.dadospalavras[k].rodada_pontuada=auxrodadapontuada;
                    strcpy(config.aux_palavras[k],config.aux_palavras[50]);
                    for(m=0;m<15;m++){
                        if(config.aux_palavras[k][m]=='\0'){
                            for(n=m;n<15;n++){
                                config.aux_palavras[k][n]='\0';
                            }
                            break;
                        }
                    }
                }
            }               
        }
    }
    for(i=0;i<config.aux_n_palavras;i++){
        fprintf(pFile,"|%d|",config.dadospalavras[i].pontuacao);
        for(j=0;j<16;j++){
            if(config.aux_palavras[i][j]=='\0'){
                fprintf(pFile," ");
            }
            else{
                fprintf(pFile,"%c",config.aux_palavras[i][j]);
            }
        }
        fprintf(pFile,"|%03d|\n",config.dadospalavras[i].rodada_pontuada);
    }
    fclose(pFile);
}

tMatriz TabuleiroVazio(tMatriz matriz){  //Funcao que preenche uma matriz dada com '-'
    int i,j;
    for(i=0;i<matriz.numLin;i++){
        for(j=0;j<matriz.numCol; j++){
            matriz.matriz[i][j]='-';
        }
    }
    return matriz;
}

tConfig LeConfig(char* argv[]){    //Funcao para ler o arquivo config.txt
    tConfig config;
    FILE*pFile;
    config.val_leitura=1;
    sprintf(config.diretorio,"%s/config.txt",argv[1]);
    pFile=fopen(config.diretorio,"r");
    if(!pFile){
       config.val_leitura=0;
       return config;
    }
    int aux_palavras,i;
    fscanf(pFile,"%d",&config.turno);
    config.aux_turno=config.turno*2;
    int k,j;
    char cel;
    fscanf(pFile,"%d",&config.matriz.numLin);
    fscanf(pFile,"%*c");
    config.matriz.numCol=config.matriz.numLin;
    for(k=0;k<config.matriz.numLin;k++){
        for(j=0;j<config.matriz.numCol; j++){
            fscanf(pFile,"%c",&cel);
            config.matriz.matriz[k][j]=cel;
        }
        fscanf(pFile,"%*c");
    }
    config.tamanho_matriz=config.matriz.numLin;
    fscanf(pFile,"%d",&config.n_palavras);
    config.countpalavras1=config.n_palavras;
    config.countpalavras2=config.n_palavras;
    config.n_palavras=(config.n_palavras)*2;
    config.aux_n_palavras=config.n_palavras;
    for(i=0;i<config.n_palavras;i++){
        fscanf(pFile,"%s",config.palavras[i]);
    }
    for(i=0;i<config.n_palavras;i++){
        strcpy(config.aux_palavras[i],config.palavras[i]);
    }
    fclose(pFile);
    return config;
}

void ImprimeTabela(tConfig config,tJogador jogador_1,tJogador jogador_2){  //Funcao para imprimir a tabela de palavras e pontuacao
    int i, j, k, l, m, n;
    printf("|            Palavras Restantes           |\n");
    printf("|");
    for(i=0;i<16;i++){
        if(jogador_1.nome[i]=='\0'){
            printf(" ");
        }
        else{
            printf("%c",jogador_1.nome[i]);
        }
    }
    printf("(%02d)",jogador_1.pontuacao);
    printf("|");
    for(i=0;i<16;i++){
        if(jogador_2.nome[i]=='\0'){
            printf(" ");
        }
        else{
            printf("%c",jogador_2.nome[i]);
        }
    }
    printf("(%02d)",jogador_2.pontuacao);
    printf("|\n");
    printf("|--------------------|--------------------|\n");
    for(k=0;k<(config.aux_n_palavras/2);k++){
        if(config.palavras[k][0]=='\0'){
            for(l=k;l<(config.aux_n_palavras/2);l++){
                if(config.palavras[l][0]!='\0'){
                    strcpy(config.palavras[k],config.palavras[l]);
                    config.palavras[l][0]='\0';
                    for(m=0;m<15;m++){
                        if(config.palavras[k][m]=='\0'){
                            for(n=m;n<15;n++){
                                config.palavras[k][n]='\0';
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
    for(m=config.aux_n_palavras/2;m<config.aux_n_palavras;m++){
        if(config.palavras[m][0]=='\0'){
            for(n=m;n<(config.aux_n_palavras);n++){
                if(config.palavras[n][0]!='\0'){
                    strcpy(config.palavras[m],config.palavras[n]);
                    config.palavras[n][0]='\0';
                    for(m=0;m<15;m++){
                        if(config.palavras[k][m]=='\0'){
                            for(n=m;n<15;n++){
                                config.palavras[k][n]='\0';
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
    for(i=0,j=(config.aux_n_palavras/2);j<config.aux_n_palavras;i++,j++){
        if(config.palavras[i][0]!='\0' || config.palavras[j][0]!='\0'){
            printf("|");
            if(config.palavras[i][0]!='\0'){
                for(k=0;k<15;k++){
                    if(config.palavras[i][k]=='\0'){
                        printf(" ");
                    }
                    else{
                        printf("%c",config.palavras[i][k]);
                    }
                }
                printf("     ");
            }
            else{
                for(k=0;k<20;k++){
                    printf(" ");
                }
            }
            printf("|");
            if(config.palavras[j][0]!='\0'){
                for(k=0;k<15;k++){
                    if(config.palavras[j][k]=='\0'){
                        printf(" ");
                    }
                    else{
                        printf("%c",config.palavras[j][k]);
                    }
                }
                printf("     ");
            }
            else{
                for(k=0;k<20;k++){
                    printf(" ");
                }
            }
            printf("|\n");
        }
    }
}

void ImprimeLinhaMatriz(tMatriz matriz,int i){    //Funcao para uma linha de uma dada matriz
    int j;
    for(j=0;j<matriz.numCol;j++){
        printf(" %c ",matriz.matriz[i][j]);
    }
    printf("|");
    printf("\n");
}

void ImprimeTabuleiro(tMatriz matriz){		//Funcao para imprimir o tabuleiro
    int i,j,jaux;
    printf("   ");
    for(i=0;i<matriz.numCol;i++){
        printf(" %02d",i);
    }
    printf("\n");
    for(j=0;j<matriz.numLin;j++){
        printf("%02d|",j);
        ImprimeLinhaMatriz(matriz,j);
    }
}

tMatriz TransformaMatriz(tMatriz jogada, tMatriz tabuleiro){  //Funcao que transforma o tabuleiro da partida de acordo com a jogada feira
    int i,j;
    char cel;
    jogada.numCol=tabuleiro.numLin;
    for(i=0;i<tabuleiro.numLin;i++){
        for(j=0;j<tabuleiro.numCol; j++){
            if(tabuleiro.matriz[i][j]=='-'){
                tabuleiro.matriz[i][j]=jogada.matriz[i][j];
            }
            if(jogada.matriz[i][j]>='A' && jogada.matriz[i][j]<='Z'){
                tabuleiro.matriz[i][j]=jogada.matriz[i][j];
            }
        }
    }
    return tabuleiro;
}

//As 8 seguintes funcoes verificam se existem palavras nas 8 diferentes direcoes da coordenada na matriz resposta, retornando o resultado da jogada
tRetornoJogada RetornaJogadaHorizontalPositiva(tJogador jogador_1,tJogador jogador_2,tJogador jogador, tConfig config, tMatriz tabuleiro){
    tRetornoJogada jogada;
    jogada.config=config;
    jogada.jogador=jogador;
    jogada.jogador_1=jogador_1;
    jogada.jogador_2=jogador_2;
    jogada.tabuleiro=tabuleiro;
    int l=0,c=0,y,x,caux;
    x=jogador.jogada_x;
    y=jogador.jogada_y;
    jogada.val=0;
    while(config.aux_palavras[l][c]!='\0'){
        while(config.matriz.matriz[y][x]==config.aux_palavras[l][c]){
            c++;
            x++;
            if(config.aux_palavras[l][c]=='\0'){
                if(!(config.valpal[l])){
                    c=0;
                    x=jogador.jogada_x;
                    y=jogador.jogada_y;
                    jogada.config.val_jogada[y][x]=1;
                    int aux=0;
                    while(config.aux_palavras[l][c]!='\0'){
                        jogada.tabuleiro.matriz[y][x]=config.aux_palavras[l][c];
                        if(jogada.config.tabuleiropalavras.matriz[y][x]=='-'){
                            jogada.config.tabuleiropalavras.matriz[y][x]=jogada.config.ordem_palavra;
                        }
                        if(!(aux)){
                            jogada.tabuleiro.matriz[y][x]=(jogada.tabuleiro.matriz[y][x])-32;
                            aux=1;
                        }
                        c++;
                        x++;
                    }
                    if(l<config.aux_n_palavras/2){
                        if(config.turno%2){
                            jogada.jogador_1.pontuacao+=2;
                            jogada.config.countpalavras1--;
                            jogada.config.dadospalavras[l].pontuacao=2;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno; 
                            jogada.jogador_1.pontosfeitos+=2;
                        }
                        else{
                            jogada.jogador_1.pontuacao++;
                            jogada.config.countpalavras1--;
                            jogada.config.dadospalavras[l].pontuacao=1;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_2.pontoscedidos++;
                        }
                    }
                    else{
                        if(config.turno%2){
                            jogada.jogador_2.pontuacao++;
                            jogada.config.countpalavras2--;
                            jogada.config.dadospalavras[l].pontuacao=1;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_1.pontoscedidos++;
                        }
                        else{
                            jogada.jogador_2.pontuacao+=2;
                            jogada.config.countpalavras2--;
                            jogada.config.dadospalavras[l].pontuacao=2;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_2.pontosfeitos+=2;
                        }
                    }
                    jogada.config.n_palavras--;
                    jogada.config.palavras[l][0]='\0';
                    jogada.val=1;
                    return jogada;
                }
                else return jogada;
            }
        }
        c=0;
        x=jogador.jogada_x;
        l++;
    }
    return jogada;
}

tRetornoJogada RetornaJogadaHorizontalNegativa(tJogador jogador_1,tJogador jogador_2,tJogador jogador, tConfig config, tMatriz tabuleiro){
    tRetornoJogada jogada;
    jogada.config=config;
    jogada.jogador=jogador;
    jogada.jogador_1=jogador_1;
    jogada.jogador_2=jogador_2;
    jogada.tabuleiro=tabuleiro;
    int l=0,c=0,y,x,caux;
    x=jogador.jogada_x;
    y=jogador.jogada_y;
    jogada.val=0;
    while(config.aux_palavras[l][c]!='\0'){
        while(config.matriz.matriz[y][x]==config.aux_palavras[l][c]){
            c++;
            x--;
            if(config.aux_palavras[l][c]=='\0'){
                if(!(config.valpal[l])){
                    c=0;
                    x=jogador.jogada_x;
                    y=jogador.jogada_y;
                    jogada.config.val_jogada[y][x]=1;
                    int aux=0;
                    while(config.aux_palavras[l][c]!='\0'){
                        jogada.tabuleiro.matriz[y][x]=config.aux_palavras[l][c];
                        if(jogada.config.tabuleiropalavras.matriz[y][x]=='-'){
                            jogada.config.tabuleiropalavras.matriz[y][x]=jogada.config.ordem_palavra;
                        }
                        if(!(aux)){
                            jogada.tabuleiro.matriz[y][x]=(jogada.tabuleiro.matriz[y][x])-32;
                            aux=1;
                        }
                        c++;
                        x--;
                    }
                    if(l<config.aux_n_palavras/2){
                        if(config.turno%2){
                            jogada.jogador_1.pontuacao+=2;
                            jogada.config.countpalavras1--;
                            jogada.config.dadospalavras[l].pontuacao=2;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno; 
                            jogada.jogador_1.pontosfeitos+=2;
                        }
                        else{
                            jogada.jogador_1.pontuacao++;
                            jogada.config.countpalavras1--;
                            jogada.config.dadospalavras[l].pontuacao=1;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_2.pontoscedidos++;
                        }
                    }
                    else{
                        if(config.turno%2){
                            jogada.jogador_2.pontuacao++;
                            jogada.config.countpalavras2--;
                            jogada.config.dadospalavras[l].pontuacao=1;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_1.pontoscedidos++;
                        }
                        else{
                            jogada.jogador_2.pontuacao+=2;
                            jogada.config.countpalavras2--;
                            jogada.config.dadospalavras[l].pontuacao=2;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_2.pontosfeitos+=2;
                        }
                    }
                    jogada.config.n_palavras--;
                    jogada.config.palavras[l][0]='\0';
                    jogada.val=1;
                    return jogada;
                }
            }
        }
        c=0;
        x=jogador.jogada_x;
        l++;
    }
    return jogada;
}

tRetornoJogada RetornaJogadaVerticalNegativa(tJogador jogador_1,tJogador jogador_2,tJogador jogador, tConfig config, tMatriz tabuleiro){
    tRetornoJogada jogada;
    jogada.config=config;
    jogada.jogador=jogador;
    jogada.jogador_1=jogador_1;
    jogada.jogador_2=jogador_2;
    jogada.tabuleiro=tabuleiro;
    int l=0,c=0,y,x,caux;
    x=jogador.jogada_x;
    y=jogador.jogada_y;
    jogada.val=0;
    while(config.aux_palavras[l][c]!='\0'){
        while(config.matriz.matriz[y][x]==config.aux_palavras[l][c]){
            c++;
            y++;
            if(config.aux_palavras[l][c]=='\0'){
                if(!(config.valpal[l])){
                    c=0;
                    x=jogador.jogada_x;
                    y=jogador.jogada_y;
                    jogada.config.val_jogada[y][x]=1;
                    int aux=0;
                    while(config.aux_palavras[l][c]!='\0'){
                        jogada.tabuleiro.matriz[y][x]=config.aux_palavras[l][c];
                        if(jogada.config.tabuleiropalavras.matriz[y][x]=='-'){
                            jogada.config.tabuleiropalavras.matriz[y][x]=jogada.config.ordem_palavra;
                        }
                        if(!(aux)){
                            jogada.tabuleiro.matriz[y][x]=(jogada.tabuleiro.matriz[y][x])-32;
                            aux=1;
                        }
                        c++;
                        y++;
                    }
                    if(l<config.aux_n_palavras/2){
                        if(config.turno%2){
                            jogada.jogador_1.pontuacao+=2;
                            jogada.config.countpalavras1--;
                            jogada.config.dadospalavras[l].pontuacao=2;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno; 
                            jogada.jogador_1.pontosfeitos+=2;
                        }
                        else{
                            jogada.jogador_1.pontuacao++;
                            jogada.config.countpalavras1--;
                            jogada.config.dadospalavras[l].pontuacao=1;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_2.pontoscedidos++;
                        }
                    }
                    else{
                        if(config.turno%2){
                            jogada.jogador_2.pontuacao++;
                            jogada.config.countpalavras2--;
                            jogada.config.dadospalavras[l].pontuacao=1;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_1.pontoscedidos++;
                        }
                        else{
                            jogada.jogador_2.pontuacao+=2;
                            jogada.config.countpalavras2--;
                            jogada.config.dadospalavras[l].pontuacao=2;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_2.pontosfeitos+=2;
                        }
                    }
                    jogada.config.n_palavras--;
                    jogada.config.palavras[l][0]='\0';
                    jogada.val=1;
                    return jogada;
                }
            }
        }
        c=0;
        y=jogador.jogada_y;
        l++;
    }
    return jogada;
}

tRetornoJogada RetornaJogadaVerticalPositiva(tJogador jogador_1,tJogador jogador_2,tJogador jogador, tConfig config, tMatriz tabuleiro){
    tRetornoJogada jogada;
    jogada.config=config;
    jogada.jogador=jogador;
    jogada.jogador_1=jogador_1;
    jogada.jogador_2=jogador_2;
    jogada.tabuleiro=tabuleiro;
    int l=0,c=0,y,x,caux;
    x=jogador.jogada_x;
    y=jogador.jogada_y;
    jogada.val=0;
    while(config.aux_palavras[l][c]!='\0'){
        while(config.matriz.matriz[y][x]==config.aux_palavras[l][c]){
            c++;
            y--;
            if(config.aux_palavras[l][c]=='\0'){
                if(!(config.valpal[l])){
                    c=0;
                    x=jogador.jogada_x;
                    y=jogador.jogada_y;
                    jogada.config.val_jogada[y][x]=1;
                    int aux=0;
                    while(config.aux_palavras[l][c]!='\0'){
                        jogada.tabuleiro.matriz[y][x]=config.aux_palavras[l][c];
                        if(jogada.config.tabuleiropalavras.matriz[y][x]=='-'){
                            jogada.config.tabuleiropalavras.matriz[y][x]=jogada.config.ordem_palavra;
                        }
                        if(!(aux)){
                            jogada.tabuleiro.matriz[y][x]=(jogada.tabuleiro.matriz[y][x])-32;
                            aux=1;
                        }
                        c++;
                        y--;
                    }
                    if(l<config.aux_n_palavras/2){
                        if(config.turno%2){
                            jogada.jogador_1.pontuacao+=2;
                            jogada.config.countpalavras1--;
                            jogada.config.dadospalavras[l].pontuacao=2;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno; 
                            jogada.jogador_1.pontosfeitos+=2;
                        }
                        else{
                            jogada.jogador_1.pontuacao++;
                            jogada.config.countpalavras1--;
                            jogada.config.dadospalavras[l].pontuacao=1;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_2.pontoscedidos++;
                        }
                    }
                    else{
                        if(config.turno%2){
                            jogada.jogador_2.pontuacao++;
                            jogada.config.countpalavras2--;
                            jogada.config.dadospalavras[l].pontuacao=1;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_1.pontoscedidos++;
                        }
                        else{
                            jogada.jogador_2.pontuacao+=2;
                            jogada.config.countpalavras2--;
                            jogada.config.dadospalavras[l].pontuacao=2;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_2.pontosfeitos+=2;
                        }
                    }
                    jogada.config.n_palavras--;
                    jogada.config.palavras[l][0]='\0';
                    jogada.val=1;
                    return jogada;
                }
            }
        }
        c=0;
        y=jogador.jogada_y;
        l++;
    }
    return jogada;
}
tRetornoJogada RetornaJogadaSuperiorEsquerda(tJogador jogador_1,tJogador jogador_2,tJogador jogador, tConfig config, tMatriz tabuleiro){
    tRetornoJogada jogada;
    jogada.config=config;
    jogada.jogador=jogador;
    jogada.jogador_1=jogador_1;
    jogada.jogador_2=jogador_2;
    jogada.tabuleiro=tabuleiro;
    int i;
    for(i=0;i<25;i++){
        jogada.valpal[i]=config.valpal[i];
    }
    int l=0,c=0,y,x,caux;
    x=jogador.jogada_x;
    y=jogador.jogada_y;
    jogada.val=0;
    while(config.aux_palavras[l][c]!='\0'){
        while(config.matriz.matriz[y][x]==config.aux_palavras[l][c]){
            c++;
            y--;
            x--;
            if(config.aux_palavras[l][c]=='\0'){
                if(!(config.valpal[l])){
                    c=0;
                    x=jogador.jogada_x;
                    y=jogador.jogada_y;
                    jogada.config.val_jogada[y][x]=1;
                    int aux=0;
                    while(config.aux_palavras[l][c]!='\0'){
                        jogada.tabuleiro.matriz[y][x]=config.aux_palavras[l][c];
                        if(jogada.config.tabuleiropalavras.matriz[y][x]=='-'){
                            jogada.config.tabuleiropalavras.matriz[y][x]=jogada.config.ordem_palavra;
                        }
                        if(!(aux)){
                            jogada.tabuleiro.matriz[y][x]=(jogada.tabuleiro.matriz[y][x])-32;
                            aux=1;
                        }
                        c++;
                        y--;
                        x--;
                    }
                    if(l<config.aux_n_palavras/2){
                        if(config.turno%2){
                            jogada.jogador_1.pontuacao+=2;
                            jogada.config.countpalavras1--;
                            jogada.config.dadospalavras[l].pontuacao=2;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno; 
                            jogada.jogador_1.pontosfeitos+=2;
                        }
                        else{
                            jogada.jogador_1.pontuacao++;
                            jogada.config.countpalavras1--;
                            jogada.config.dadospalavras[l].pontuacao=1;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_2.pontoscedidos++;
                        }
                    }
                    else{
                        if(config.turno%2){
                            jogada.jogador_2.pontuacao++;
                            jogada.config.countpalavras2--;
                            jogada.config.dadospalavras[l].pontuacao=1;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_1.pontoscedidos++;
                        }
                        else{
                            jogada.jogador_2.pontuacao+=2;
                            jogada.config.countpalavras2--;
                            jogada.config.dadospalavras[l].pontuacao=2;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_2.pontosfeitos+=2;
                        }
                    }
                    jogada.config.n_palavras--;
                    jogada.config.palavras[l][0]='\0';
                    jogada.val=1;
                    return jogada;
                }
            }
        }
        c=0;
        y=jogador.jogada_y;
        x=jogador.jogada_x;
        l++;
    }
    return jogada;
}

tRetornoJogada RetornaJogadaSuperiorDireita(tJogador jogador_1,tJogador jogador_2,tJogador jogador, tConfig config, tMatriz tabuleiro){
    tRetornoJogada jogada;
    jogada.config=config;
    jogada.jogador=jogador;
    jogada.jogador_1=jogador_1;
    jogada.jogador_2=jogador_2;
    jogada.tabuleiro=tabuleiro;
    int i;
    for(i=0;i<25;i++){
        jogada.valpal[i]=config.valpal[i];
    }
    int l=0,c=0,y,x,caux;
    x=jogador.jogada_x;
    y=jogador.jogada_y;
    jogada.val=0;
    while(config.aux_palavras[l][c]!='\0'){
        while(config.matriz.matriz[y][x]==config.aux_palavras[l][c]){
            c++;
            y--;
            x++;
            if(config.aux_palavras[l][c]=='\0'){
                if(!(config.valpal[l])){
                    c=0;
                    x=jogador.jogada_x;
                    y=jogador.jogada_y;
                    jogada.config.val_jogada[y][x]=1;
                    int aux=0;
                    while(config.aux_palavras[l][c]!='\0'){
                        jogada.tabuleiro.matriz[y][x]=config.aux_palavras[l][c];
                        if(jogada.config.tabuleiropalavras.matriz[y][x]=='-'){
                            jogada.config.tabuleiropalavras.matriz[y][x]=jogada.config.ordem_palavra;
                        }
                        if(!(aux)){
                            jogada.tabuleiro.matriz[y][x]=(jogada.tabuleiro.matriz[y][x])-32;
                            aux=1;
                        }
                        c++;
                        y--;
                        x++;
                    }
                    if(l<config.aux_n_palavras/2){
                        if(config.turno%2){
                            jogada.jogador_1.pontuacao+=2;
                            jogada.config.countpalavras1--;
                            jogada.config.dadospalavras[l].pontuacao=2;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno; 
                            jogada.jogador_1.pontosfeitos+=2;
                        }
                        else{
                            jogada.jogador_1.pontuacao++;
                            jogada.config.countpalavras1--;
                            jogada.config.dadospalavras[l].pontuacao=1;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_2.pontoscedidos++;
                        }
                    }
                    else{
                        if(config.turno%2){
                            jogada.jogador_2.pontuacao++;
                            jogada.config.countpalavras2--;
                            jogada.config.dadospalavras[l].pontuacao=1;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_1.pontoscedidos++;
                        }
                        else{
                            jogada.jogador_2.pontuacao+=2;
                            jogada.config.countpalavras2--;
                            jogada.config.dadospalavras[l].pontuacao=2;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_2.pontosfeitos+=2;
                        }
                    }
                    jogada.config.n_palavras--;
                    jogada.config.palavras[l][0]='\0';
                    jogada.val=1;
                    return jogada;
                }
            }
        }
        c=0;
        y=jogador.jogada_y;
        x=jogador.jogada_x;
        l++;
    }
    return jogada;
}

tRetornoJogada RetornaJogadaInferiorEsquerda(tJogador jogador_1,tJogador jogador_2,tJogador jogador, tConfig config, tMatriz tabuleiro){
    tRetornoJogada jogada;
    jogada.config=config;
    jogada.jogador=jogador;
    jogada.jogador_1=jogador_1;
    jogada.jogador_2=jogador_2;
    jogada.tabuleiro=tabuleiro;
    int i;
    for(i=0;i<25;i++){
        jogada.valpal[i]=config.valpal[i];
    }
    int l=0,c=0,y,x,caux;
    x=jogador.jogada_x;
    y=jogador.jogada_y;
    jogada.val=0;
    while(config.aux_palavras[l][c]!='\0'){
        while(config.matriz.matriz[y][x]==config.aux_palavras[l][c]){
            c++;
            y++;
            x--;
            if(config.aux_palavras[l][c]=='\0'){
                if(!(config.valpal[l])){
                    c=0;
                    x=jogador.jogada_x;
                    y=jogador.jogada_y;
                    jogada.config.val_jogada[y][x]=1;
                    int aux=0;
                    while(config.aux_palavras[l][c]!='\0'){
                        jogada.tabuleiro.matriz[y][x]=config.aux_palavras[l][c];
                        if(jogada.config.tabuleiropalavras.matriz[y][x]=='-'){
                            jogada.config.tabuleiropalavras.matriz[y][x]=jogada.config.ordem_palavra;
                        }
                        if(!(aux)){
                            jogada.tabuleiro.matriz[y][x]=(jogada.tabuleiro.matriz[y][x])-32;
                            aux=1;
                        }
                        c++;
                        y++;
                        x--;
                    }
                    if(l<config.aux_n_palavras/2){
                        if(config.turno%2){
                            jogada.jogador_1.pontuacao+=2;
                            jogada.config.countpalavras1--;
                            jogada.config.dadospalavras[l].pontuacao=2;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno; 
                            jogada.jogador_1.pontosfeitos+=2;
                        }
                        else{
                            jogada.jogador_1.pontuacao++;
                            jogada.config.countpalavras1--;
                            jogada.config.dadospalavras[l].pontuacao=1;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_2.pontoscedidos++;
                        }
                    }
                    else{
                        if(config.turno%2){
                            jogada.jogador_2.pontuacao++;
                            jogada.config.countpalavras2--;
                            jogada.config.dadospalavras[l].pontuacao=1;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_1.pontoscedidos++;
                        }
                        else{
                            jogada.jogador_2.pontuacao+=2;
                            jogada.config.countpalavras2--;
                            jogada.config.dadospalavras[l].pontuacao=2;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_2.pontosfeitos+=2;
                        }
                    }
                    jogada.config.n_palavras--;
                    jogada.config.palavras[l][0]='\0';
                    jogada.val=1;
                    return jogada;
                }
            }
        }
        c=0;
        y=jogador.jogada_y;
        x=jogador.jogada_x;
        l++;
    }
    return jogada;
}

tRetornoJogada RetornaJogadaInferiorDireita(tJogador jogador_1,tJogador jogador_2,tJogador jogador, tConfig config, tMatriz tabuleiro){
    tRetornoJogada jogada;
    jogada.config=config;
    jogada.jogador=jogador;
    jogada.jogador_1=jogador_1;
    jogada.jogador_2=jogador_2;
    jogada.tabuleiro=tabuleiro;
    int i;
    for(i=0;i<25;i++){
        jogada.valpal[i]=config.valpal[i];
    }
    int l=0,c=0,y,x,caux;
    x=jogador.jogada_x;
    y=jogador.jogada_y;
    jogada.val=0;
    while(config.aux_palavras[l][c]!='\0'){
        while(config.matriz.matriz[y][x]==config.aux_palavras[l][c]){
            c++;
            y++;
            x++;
            if(config.aux_palavras[l][c]=='\0'){
                if(!(config.valpal[l])){
                    c=0;
                    x=jogador.jogada_x;
                    y=jogador.jogada_y;
                    jogada.config.val_jogada[y][x]=1;
                    int aux=0;
                    while(config.aux_palavras[l][c]!='\0'){
                        jogada.tabuleiro.matriz[y][x]=config.aux_palavras[l][c];
                        if(jogada.config.tabuleiropalavras.matriz[y][x]=='-'){
                            jogada.config.tabuleiropalavras.matriz[y][x]=jogada.config.ordem_palavra;
                        }
                        if(!(aux)){
                            jogada.tabuleiro.matriz[y][x]=(jogada.tabuleiro.matriz[y][x])-32;
                            aux=1;
                        }
                        c++;
                        y++;
                        x++;
                    }
                    if(l<config.aux_n_palavras/2){
                        if(config.turno%2){
                            jogada.jogador_1.pontuacao+=2;
                            jogada.config.countpalavras1--;
                            jogada.config.dadospalavras[l].pontuacao=2;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno; 
                            jogada.jogador_1.pontosfeitos+=2;
                        }
                        else{
                            jogada.jogador_1.pontuacao++;
                            jogada.config.countpalavras1--;
                            jogada.config.dadospalavras[l].pontuacao=1;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_2.pontoscedidos++;
                        }
                    }
                    else{
                        if(config.turno%2){
                            jogada.jogador_2.pontuacao++;
                            jogada.config.countpalavras2--;
                            jogada.config.dadospalavras[l].pontuacao=1;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_1.pontoscedidos++;
                        }
                        else{
                            jogada.jogador_2.pontuacao+=2;
                            jogada.config.countpalavras2--;
                            jogada.config.dadospalavras[l].pontuacao=2;
                            jogada.config.dadospalavras[l].rodada_pontuada=config.turno;
                            jogada.jogador_2.pontosfeitos+=2;
                        }
                    }
                    jogada.config.n_palavras--;
                    jogada.config.palavras[l][0]='\0';
                    jogada.val=1;
                    return jogada;
                }
            }
        }
        c=0;
        y=jogador.jogada_y;
        x=jogador.jogada_x;
        l++;
    }
    return jogada;
}

int main(int argc, char* argv[]){
    tConfig config;
    tJogador jogador_1,jogador_2;
    tMatriz tabuleiro_vazio;
    tRetornoJogada jogada;
    if(argc<2){
        printf("ERRO: O diretorio de arquivos de configuracao nao foi informado\n");
        return 0;
    }
    config=LeConfig(argv);
    if(config.val_leitura==0){
        printf("ERRO: Nao foi possivel ler o arquivo de configuracao em %s\n",config.diretorio);
    }
    int x,i;
    for(x=0;x<=config.n_palavras;x++){
        config.tam_palavras[x]=ContaPalavras(config,x);
    }
    config.turno=1;
    config.ordem_palavra=1;
    config.tabuleiropalavras.numCol=config.matriz.numCol;
    config.tabuleiropalavras.numLin=config.matriz.numLin;
    printf("Nome do Jogador 1:\n");
    scanf("%15s",jogador_1.nome);
    printf("Nome do Jogador 2:\n");
    scanf("%15s",jogador_2.nome);
    Inicializacao(config,jogador_1,jogador_2,argv);
    tabuleiro_vazio=TabuleiroVazio(config.matriz);
    config.tabuleiropalavras=TabuleiroVazio(config.tabuleiropalavras);
    while(config.n_palavras>0){
        ImprimeTabela(config,jogador_1,jogador_2);
        printf("\n");
        ImprimeTabuleiro(tabuleiro_vazio);
        printf("\n");
        printf("%s por favor entre com as coordenadas para a sua jogada:\n",jogador_1.nome);
        scanf("%d %d",&jogador_1.jogada_y,&jogador_1.jogada_x);
        if(jogador_1.jogada_x>=config.matriz.numCol || jogador_1.jogada_x<0 || 
           jogador_1.jogada_y>=config.matriz.numLin || jogador_1.jogada_y<0){
            while(1){
                if(jogador_1.jogada_x>=config.matriz.numCol || jogador_1.jogada_x<0 || 
                jogador_1.jogada_y>=config.matriz.numLin || jogador_1.jogada_y<0){
                    ImprimeTabela(config,jogador_1,jogador_2);
                    printf("\n");
                    ImprimeTabuleiro(tabuleiro_vazio);
                    printf("\n");
                    printf("Coordenadas fora do tabuleiro.%s por favor entre com novas coordenadas para a sua jogada:\n",jogador_1.nome);
                }
                scanf("%d %d",&jogador_1.jogada_y,&jogador_1.jogada_x);
                if(jogador_1.jogada_x<config.matriz.numCol && jogador_1.jogada_x>=0 && 
                   jogador_1.jogada_y<config.matriz.numLin && jogador_1.jogada_y>=0){
                    if(config.val_jogada[jogador_1.jogada_x][jogador_1.jogada_y]==0){
                        break;
                    }
                    else{
                        ImprimeTabela(config,jogador_1,jogador_2);
                        printf("\n");
                        ImprimeTabuleiro(tabuleiro_vazio);
                        printf("\n");
                        printf("Coordenadas ja jogadas.%s por favor entre com novas coordenadas para a sua jogada:\n",jogador_1.nome);
                    }
                }
            }
        }
        if(config.val_jogada[jogador_1.jogada_y][jogador_1.jogada_x]==1){
            while(1){
                if(config.val_jogada[jogador_1.jogada_y][jogador_1.jogada_x]==1){
                    ImprimeTabela(config,jogador_1,jogador_2);
                    printf("\n");
                    ImprimeTabuleiro(tabuleiro_vazio);
                    printf("\n");
                    printf("Coordenadas ja jogadas.%s por favor entre com novas coordenadas para a sua jogada:\n",jogador_1.nome);
                }
                scanf("%d %d",&jogador_1.jogada_y,&jogador_1.jogada_x);
                if(config.val_jogada[jogador_1.jogada_y][jogador_1.jogada_x]==0){
                    if(jogador_1.jogada_x<config.matriz.numCol && jogador_1.jogada_x>=0 && 
                       jogador_1.jogada_y<config.matriz.numLin && jogador_1.jogada_y>=0){
                        break;
                    }
                    else{
                        ImprimeTabela(config,jogador_1,jogador_2);
                        printf("\n");
                        ImprimeTabuleiro(tabuleiro_vazio);
                        printf("\n");
                        printf("Coordenadas fora do tabuleiro.%s por favor entre com novas coordenadas para a sua jogada:\n",jogador_1.nome);
                    }
                }
            }
        }
        jogada=RetornaJogadaVerticalNegativa(jogador_1,jogador_2,jogador_1,config,tabuleiro_vazio);
        if(jogada.val){
            config=jogada.config;
            jogador_1=jogada.jogador_1;
            jogador_2=jogada.jogador_2;
            tabuleiro_vazio=TransformaMatriz(jogada.tabuleiro, tabuleiro_vazio);
            config.ordem_palavra++;
        }
        else{
            jogada=RetornaJogadaVerticalPositiva(jogador_1,jogador_2,jogador_1,config,tabuleiro_vazio);
            if(jogada.val){
                config=jogada.config;
                jogador_1=jogada.jogador_1;
                jogador_2=jogada.jogador_2;
                tabuleiro_vazio=TransformaMatriz(jogada.tabuleiro, tabuleiro_vazio);
                config.ordem_palavra++;
            }
            else{
                jogada=RetornaJogadaHorizontalPositiva(jogador_1,jogador_2,jogador_1,config,tabuleiro_vazio);
                if(jogada.val){
                    config=jogada.config;
                    jogador_1=jogada.jogador_1;
                    jogador_2=jogada.jogador_2;
                    tabuleiro_vazio=TransformaMatriz(jogada.tabuleiro, tabuleiro_vazio);
                    config.ordem_palavra++;
                }
                else{
                    jogada=RetornaJogadaHorizontalNegativa(jogador_1,jogador_2,jogador_1,config,tabuleiro_vazio);
                    if(jogada.val){
                        config=jogada.config;
                        jogador_1=jogada.jogador_1;
                        jogador_2=jogada.jogador_2;
                        tabuleiro_vazio=TransformaMatriz(jogada.tabuleiro, tabuleiro_vazio);
                        config.ordem_palavra++;
                    }
                    else{
                        jogada=RetornaJogadaSuperiorEsquerda(jogador_1,jogador_2,jogador_1,config,tabuleiro_vazio);
                        if(jogada.val){
                            config=jogada.config;
                            jogador_1=jogada.jogador_1;
                            jogador_2=jogada.jogador_2;
                            tabuleiro_vazio=TransformaMatriz(jogada.tabuleiro, tabuleiro_vazio);
                            config.ordem_palavra++;
                        }
                        else{
                            jogada=RetornaJogadaSuperiorDireita(jogador_1,jogador_2,jogador_1,config,tabuleiro_vazio);
                            if(jogada.val){
                                config=jogada.config;
                                jogador_1=jogada.jogador_1;
                                jogador_2=jogada.jogador_2;
                                tabuleiro_vazio=TransformaMatriz(jogada.tabuleiro, tabuleiro_vazio);
                                config.ordem_palavra++;
                            }
                            else{
                                jogada=RetornaJogadaInferiorEsquerda(jogador_1,jogador_2,jogador_1,config,tabuleiro_vazio);
                                if(jogada.val){
                                    config=jogada.config;
                                    jogador_1=jogada.jogador_1;
                                    jogador_2=jogada.jogador_2;
                                    tabuleiro_vazio=TransformaMatriz(jogada.tabuleiro, tabuleiro_vazio);
                                    config.ordem_palavra++;
                                }
                                else{
                                    jogada=RetornaJogadaInferiorDireita(jogador_1,jogador_2,jogador_1,config,tabuleiro_vazio);
                                    if(jogada.val){
                                        config=jogada.config;
                                        jogador_1=jogada.jogador_1;
                                        jogador_2=jogada.jogador_2;
                                        tabuleiro_vazio=TransformaMatriz(jogada.tabuleiro, tabuleiro_vazio);
                                        config.ordem_palavra++;
                                    }
                                    else{
                                        tabuleiro_vazio.matriz[jogador_1.jogada_y][jogador_1.jogada_x]=config.matriz.matriz[jogador_1.jogada_y][jogador_1.jogada_x];
                                        tabuleiro_vazio.matriz[jogador_1.jogada_y][jogador_1.jogada_x]=(tabuleiro_vazio.matriz[jogador_1.jogada_y][jogador_1.jogada_x])-32;
                                        config.val_jogada[jogador_1.jogada_y][jogador_1.jogada_x]=1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        config.aux_turno--;
        if(config.countpalavras1==0 || config.countpalavras2==0){
            break;
        }
        else if(config.aux_turno==0){
            break;
        }
        config.turno++;
        ImprimeTabela(config,jogador_1,jogador_2);
        printf("\n");
        ImprimeTabuleiro(tabuleiro_vazio);
        printf("\n");
        printf("%s por favor entre com as coordenadas para a sua jogada:\n",jogador_2.nome);
        scanf("%d %d",&jogador_2.jogada_y,&jogador_2.jogada_x);
        if(jogador_2.jogada_x>=config.matriz.numCol || jogador_2.jogada_x<0 || 
           jogador_2.jogada_y>=config.matriz.numLin || jogador_2.jogada_y<0){
            while(1){
                if(jogador_2.jogada_x>=config.matriz.numCol || jogador_2.jogada_x<0 || 
                jogador_2.jogada_y>=config.matriz.numLin || jogador_2.jogada_y<0){
                    ImprimeTabela(config,jogador_1,jogador_2);
                    printf("\n");
                    ImprimeTabuleiro(tabuleiro_vazio);
                    printf("\n");
                    printf("Coordenadas fora do tabuleiro.%s por favor entre com novas coordenadas para a sua jogada:\n",jogador_2.nome);
                }
                scanf("%d %d",&jogador_2.jogada_y,&jogador_2.jogada_x);
                if(jogador_2.jogada_x<config.matriz.numCol && jogador_2.jogada_x>=0 && 
                   jogador_2.jogada_y<config.matriz.numLin && jogador_2.jogada_y>=0){
                    if(config.val_jogada[jogador_2.jogada_x][jogador_2.jogada_y]==0){
                        break;
                    }
                    else{
                        ImprimeTabela(config,jogador_1,jogador_2);
                        printf("\n");
                        ImprimeTabuleiro(tabuleiro_vazio);
                        printf("\n");
                        ImprimeTabela(config,jogador_1,jogador_2);
                        printf("\n");
                        ImprimeTabuleiro(tabuleiro_vazio);
                        printf("\n");
                        printf("Coordenadas ja jogadas.%s por favor entre com novas coordenadas para a sua jogada:\n",jogador_2.nome);
                    }
                }
            }
        }
        if(config.val_jogada[jogador_2.jogada_y][jogador_2.jogada_x]==1){
            while(1){
                if(config.val_jogada[jogador_2.jogada_y][jogador_2.jogada_x]==1){
                    ImprimeTabela(config,jogador_1,jogador_2);
                    printf("\n");
                    ImprimeTabuleiro(tabuleiro_vazio);
                    printf("\n");
                    printf("Coordenadas ja jogadas.%s por favor entre com novas coordenadas para a sua jogada:\n",jogador_2.nome);
                }
                scanf("%d %d",&jogador_2.jogada_y,&jogador_2.jogada_x);
                if(config.val_jogada[jogador_2.jogada_y][jogador_2.jogada_x]==0){
                    if(jogador_2.jogada_x<config.matriz.numCol && jogador_2.jogada_x>=0 && 
                       jogador_2.jogada_y<config.matriz.numLin && jogador_2.jogada_y>=0){
                        break;
                    }
                    else{
                        ImprimeTabela(config,jogador_1,jogador_2);
                        printf("\n");
                        ImprimeTabuleiro(tabuleiro_vazio);
                        printf("\n");
                        printf("Coordenadas fora do tabuleiro.%s por favor entre com novas coordenadas para a sua jogada:\n",jogador_2.nome);
                    }
                }
            }
        }
        jogada=RetornaJogadaVerticalNegativa(jogador_1,jogador_2,jogador_2,config,tabuleiro_vazio);
        if(jogada.val){
            config=jogada.config;
            jogador_1=jogada.jogador_1;
            jogador_2=jogada.jogador_2;
            tabuleiro_vazio=TransformaMatriz(jogada.tabuleiro, tabuleiro_vazio);
            config.ordem_palavra++;
        }
        else{
            jogada=RetornaJogadaVerticalPositiva(jogador_1,jogador_2,jogador_2,config,tabuleiro_vazio);
            if(jogada.val){
                config=jogada.config;
                jogador_1=jogada.jogador_1;
                jogador_2=jogada.jogador_2;
                tabuleiro_vazio=TransformaMatriz(jogada.tabuleiro, tabuleiro_vazio);
                config.ordem_palavra++;
            }
            else{
                jogada=RetornaJogadaHorizontalPositiva(jogador_1,jogador_2,jogador_2,config,tabuleiro_vazio);
                if(jogada.val){
                    config=jogada.config;
                    jogador_1=jogada.jogador_1;
                    jogador_2=jogada.jogador_2;
                    tabuleiro_vazio=TransformaMatriz(jogada.tabuleiro, tabuleiro_vazio);
                    config.ordem_palavra++;
                }
                else{
                    jogada=RetornaJogadaHorizontalNegativa(jogador_1,jogador_2,jogador_2,config,tabuleiro_vazio);
                    if(jogada.val){
                        config=jogada.config;
                        jogador_1=jogada.jogador_1;
                        jogador_2=jogada.jogador_2;
                        tabuleiro_vazio=TransformaMatriz(jogada.tabuleiro, tabuleiro_vazio);
                        config.ordem_palavra++;
                    }
                    else{
                        jogada=RetornaJogadaSuperiorEsquerda(jogador_1,jogador_2,jogador_2,config,tabuleiro_vazio);
                        if(jogada.val){
                            config=jogada.config;
                            jogador_1=jogada.jogador_1;
                            jogador_2=jogada.jogador_2;
                            tabuleiro_vazio=TransformaMatriz(jogada.tabuleiro, tabuleiro_vazio);
                            config.ordem_palavra++;
                        }
                        else{
                            jogada=RetornaJogadaSuperiorDireita(jogador_1,jogador_2,jogador_2,config,tabuleiro_vazio);
                            if(jogada.val){
                                config=jogada.config;
                                jogador_1=jogada.jogador_1;
                                jogador_2=jogada.jogador_2;
                                tabuleiro_vazio=TransformaMatriz(jogada.tabuleiro, tabuleiro_vazio);
                                config.ordem_palavra++;
                            }
                            else{
                                jogada=RetornaJogadaInferiorEsquerda(jogador_1,jogador_2,jogador_2,config,tabuleiro_vazio);
                                if(jogada.val){
                                    config=jogada.config;
                                    jogador_1=jogada.jogador_1;
                                    jogador_2=jogada.jogador_2;
                                    tabuleiro_vazio=TransformaMatriz(jogada.tabuleiro, tabuleiro_vazio);
                                    config.ordem_palavra++;
                                }
                                else{
                                    jogada=RetornaJogadaInferiorDireita(jogador_1,jogador_2,jogador_2,config,tabuleiro_vazio);
                                    if(jogada.val){
                                        config=jogada.config;
                                        jogador_1=jogada.jogador_1;
                                        jogador_2=jogada.jogador_2;
                                        tabuleiro_vazio=TransformaMatriz(jogada.tabuleiro, tabuleiro_vazio);
                                        config.ordem_palavra++;
                                    }
                                    else{
                                        tabuleiro_vazio.matriz[jogador_2.jogada_y][jogador_2.jogada_x]=config.matriz.matriz[jogador_2.jogada_y][jogador_2.jogada_x];
                                        tabuleiro_vazio.matriz[jogador_2.jogada_y][jogador_2.jogada_x]=(tabuleiro_vazio.matriz[jogador_2.jogada_y][jogador_2.jogada_x])-32;
                                        config.val_jogada[jogador_2.jogada_y][jogador_2.jogada_x]=1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        config.turno++;
        config.aux_turno--;
        if(config.countpalavras1==0 || config.countpalavras2==0){
            break;
        }
        else if(config.aux_turno==0){
            break;
        }
    }
    ImprimeTabela(config,jogador_1,jogador_2);
    printf("\n");
    ImprimeTabuleiro(tabuleiro_vazio);
    printf("\n");
    if(jogador_1.pontuacao>jogador_2.pontuacao){
        printf("%s Ganhou!!!\n",jogador_1.nome);
    }
    else if(jogador_1.pontuacao<jogador_2.pontuacao){
        printf("%s Ganhou!!!\n",jogador_2.nome);
    }
    else{
        printf("Empate\n");
    }
    Estatisticas(config,jogador_1,jogador_2,argv);
    Mapaordenado(config,argv);
    return (EXIT_SUCCESS);
}
