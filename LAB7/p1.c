/*
Se va scrie un program in limbajul C ce va prelucra un fisier de intrare si va realiza o serie de statistici pe cel fisier.
Programul va primit 3 parametri si se va apela dupa cum urmeaza:

./program <fisier_intrare> <fisier_statistica> <ch>

Primul argument reprezinta un fisier text de intrare aflat pe disc ce contine in principal text.

Al doilea argument reprezinta fisierul de iesire al programului ce va contine informatii statistice realizate
de program.

Al treilea parametru reprezinta un caracter

Programul trebuie sa parcurga fisierul text dat ca prim parametru si sa realizeze urmatoarele statistici:

numar litere mici
numar litere mari
numar cifre
numar de aparitii ale caracterului <ch> dat ca si al treilea parametru
dimensiune fisier

Un exemplu de fisier de statistica (de iesire) ar putea fi urmatorul:
numar litere mici: 20
numar litere mari: 35
numar cifre: 76
numar aparitii caracter: 12
dimensiune fisier: 100

Se va respecta intocmai structura fisierului de iesire. 

Se vor folosi doar apeluri sistem pentru lucrul cu fisierele
(open, read, write, close, stat, fstat, lstat... etc).
Nu se permite folosirea functiilor din biblioteca standard stdio pentru lucrul cu fisiere(fopen, fread, fwrite, fclose... etc).
Se permite folosirea functiei sprintf pentru a obtine un string formatat pentru a putea fi scris in fisier
folosind apelul sistem "write". 

Pentru a obtine dimensiunea fisierului se vor folosi apeluri sistem din familia stat.

Problema data la laborator SO de Valentin STANGACIU

*/


// 1. de terminat problema asta
// 2. de testat simlink si hardlink pe fisiere
// 3. de citit laboratorul 6 (st_mode)
// 4. citit directoare
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>

#define BUFFSIZE 4096

int litereMari=0;
int litereMici=0;
int nrCifre=0;
int nrAparitii=0;
int dimensiune=0;
char c;


void citireFisier(char *fisier)
{
    int fd=open(fisier, O_RDONLY);
    if(fd==-1)
    {
        perror("Eroare");
    }
    int nr;
    char buff[BUFFSIZE];
    while((nr=read(fd, buff, BUFFSIZE))) 
    {   
        dimensiune=dimensiune+nr;
        for(int i=0; i<nr; i++)
        {
            if(isdigit(buff[i]))
                nrCifre++;
            if(islower(buff[i]))
                litereMici++;
            if(isupper(buff[i]))
                litereMari++;
            if(buff[i]==c)
                nrAparitii++;

        }
    }
    close(fd);
}

void scriereFisier(char *fisier)
{
    int fd=open(fisier, O_WRONLY|O_CREAT);
    if(fd==-1)
    {
        perror("Eroare!");
    }
    char *buff;
    int len;
    len = sprintf(buff, "Nr litere mari: %d\n", litereMari);
    write(fd, buff, len);
    len = sprintf(buff, "Nr litere mici: %d\n", litereMici);
    write(fd, buff, len);
    len = sprintf(buff, "Nr apariții: %d\n", nrAparitii);
    write(fd, buff, len);
    len = sprintf(buff, "Dimensiune: %d\n", dimensiune);
    write(fd, buff, len);
    len = sprintf(buff, "Nr cifre: %d\n", nrCifre);
    write(fd, buff, len);
    close(fd);

}



int main(int argc, char *argv[])
{
    if(argc!=4)
    {
        printf("Usage: %s <fisier_intrare> <fisier_statistica> <ch>\n", argv[0]);
        exit(0);
    }
    c=argv[3][0];
    citireFisier(argv[1]);
    printf("000\n");
    scriereFisier(argv[2]);
    /*printf("Nr litere mari: %d\n", litereMari);
    printf("Nr litere mici: %d\n", litereMici);
    printf("Nr apariții: %d\n", nrAparitii);
    printf("Dimensiune: %d\n", dimensiune);
    printf("Nr cifre: %d\n", nrCifre);*/
    return 0;

}