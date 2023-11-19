/*
Se va scrie un program în limbajul C ce va prelucra un fișier de intrare ce reprezinta o imagine in format BMP și
va realiza o serie de statistici pe baza acestui fișier.

Programul va primi un parametru și se va apela după cum urmează:
./program <fisier_intrare>

Programul va verifica faptul că a primit un singur argument, precum și tipul acestuia, iar în caz contrar va afișa un mesaj de eroare
”Usage ./program <fisier_intrare>”.

Programul trebuie sa citeasca header-ul fisierului BMP și sa extraga valoarea înălțimii, respectiv a lungimii pentru imaginea data.

Programul va crea un fișier cu numele statistica.txt în care vor fi scrise următoarele informații:

nume fisier: poza.bmp
inaltime: 1920
lungime: 1280
dimensiune: <dimensiune in octeti>
identificatorul utilizatorului: <user id>
timpul ultimei modificari: 28.10.2023
contorul de legaturi: <numar legaturi>
drepturi de acces user: RWX
drepturi de acces grup: R–-
drepturi de acces altii: ---

Se vor folosi doar apeluri sistem pentru lucrul cu fișierele (open, read, write, close, stat,fstat, lstat... etc).
Nu se permite folosirea funcțiilor din biblioteca standard stdio pentru lucrul cu fisiere (fopen, fread, fwrite, fclose... etc).
Se permite folosirea funcției sprintf pentru a obține un string formatat pentru a putea fi scris în fișier folosind apelul sistem "write".
Folosiți corect POSIX API, verificati codurile de retur și tratați scenariile de eroare
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>


#define BUFFSIZE 4096

int inaltime,latime;
int identificator;
int dimensiune;
char data[100];
int nrLegaturi;
char drepturi[4][4];

void citireFisier(char *fisier)
{
    int fd=open(fisier, S_IRUSR);
    if(fd==-1)
    {
        perror("Fisierul nu a fost gasit! Mai cauta.\n");
        exit(0);    }
    
    lseek(fd, 18, SEEK_SET);
    read(fd, &latime, 4);
    read(fd, &inaltime, 4);

    

    close(fd);
}

void citireInfFisier(char *fisier)
{
    struct stat inf;
    int rez=stat(fisier,&inf);
    if(rez==-1)
    {
        perror("Fisierul nu a fost gasit! Mai cauta.\n");
        exit(0);
    }
    identificator=inf.st_uid;
    dimensiune=inf.st_size;
    nrLegaturi=inf.st_nlink;
    struct timespec timp =inf.st_mtimespec;
    time_t raw_time = timp.tv_sec;
    struct tm *timpCorect=localtime(&raw_time);
    sprintf(data, "%02d.%02d.%d %02d:%02d",
    timpCorect->tm_mday,
    timpCorect->tm_mon,
    timpCorect->tm_year,
    timpCorect->tm_hour,
    timpCorect->tm_min);

    //... ne jucam cu inf.st_mode

    if(inf.st_mode&S_IRUSR)
        drepturi[0][0]='R';
    else
        drepturi[0][0]='-';
    if(inf.st_mode&S_IWUSR)
        drepturi[0][1]='W';
    else
        drepturi[0][1]='-';
    if(inf.st_mode&S_IXUSR)
        drepturi[0][2]='E';
    else
        drepturi[0][2]='-';

    if(inf.st_mode&S_IRGRP)
        drepturi[1][0]='R';
    else
        drepturi[1][0]='-';
    if(inf.st_mode&S_IWGRP)
        drepturi[1][1]='W';
    else
        drepturi[1][1]='-';
    if(inf.st_mode&S_IXGRP)
        drepturi[1][2]='E';
    else
        drepturi[1][2]='-';

    if(inf.st_mode&S_IROTH)
        drepturi[2][0]='R';
    else
        drepturi[2][0]='-';
    if(inf.st_mode&S_IWOTH)
        drepturi[2][1]='W';
    else
        drepturi[2][1]='-';
    if(inf.st_mode&S_IXOTH)
        drepturi[2][2]='E';
    else
        drepturi[2][2]='-';


}

void scriereStatistici(char *fisier)
{
    int fd=creat ("statistica.txt", S_IWUSR|S_IRUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    if(fd==-1)
    {
        perror("Eroare scriere statistici.");
    }
    int n;
    char buff[100]; //lungimea liniei

    n=sprintf(buff, "Nume fisier: %s\n", fisier);
    write(fd, buff, n);

    n=sprintf(buff, "Inaltime: %d\n", inaltime);
    write(fd, buff, n);

    n=sprintf(buff, "Latime: %d\n",latime);
    write(fd, buff, n);

    n=sprintf(buff, "Dimensiune: %d\n", dimensiune);
    write(fd, buff, n);

    n=sprintf(buff, "Identificator utilizator: %d\n", identificator);
    write(fd, buff, n);

    n=sprintf(buff, "Timpul: %s\n", data);
    write(fd, buff, n);

    n=sprintf(buff, "Contor legaturi: %d\n", nrLegaturi);
    write(fd, buff, n);

    n=sprintf(buff, "Drepturi acces user: %s\n", drepturi[0]);
    write(fd, buff, n);

    n=sprintf(buff, "Drepturi acces grupuri: %s\n", drepturi[1]);
    write(fd, buff, n);

    n=sprintf(buff, "Drepturi acces pentru altii: %s\n", drepturi[2]);
    write(fd, buff, n);

    close(fd);

    
}

//creat (const char *, mode_t mode);


int main(int argc, char *argv[])
{
    if(argc!=2)
    {
        printf("Usage: %s <fisier_intrare>\n", argv[0]);
        exit(0);
    }
    int l=strlen(argv[1]);
    if(l<5){
        printf("Usage: %s <fisier_intrare>\n", argv[0]);
        exit(0);
    }else if(argv[1][l-1]!='p' || argv[1][l-2]!='m' || argv[1][l-3]!='b' | argv[1][l-4]!='.'){
            printf("Usage: %s <fisier_intrare>\n", argv[0]);
            exit(0);
    }

    citireFisier(argv[1]);
    citireInfFisier(argv[1]);
    scriereStatistici(argv[1]);

    return 0;

}

