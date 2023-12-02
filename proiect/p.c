/*

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
#include <dirent.h>
#include <fcntl.h>
#define BUFFSIZE 4096

int inaltime,latime;
int identificator;
int dimensiune, dimensiune_legatura;
char data[100];
int nrLegaturi;
char drepturi[4][4];

/*Citeste latimea si lungimea unui fisier BMP*/
void citireBMP(char *fisier)
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

/*Citeste informatiile despre un fisier*/
int citireInfoFisier(char *fisier)
{
    struct stat inf;
    int rez=stat(fisier,&inf);
    if(rez==-1)
    {
        perror("Fisierul nu a fost gasit! Mai cauta.\n");
        exit(0);
    }
    
    int tip = -1;
    if(inf.st_mode | S_IFDIR)
        tip = 3;
    if(inf.st_mode | S_IFREG){
        tip=0;
        int l = strlen(fisier);
        if(fisier[l-4]=='.' || fisier[l-3]=='b' || fisier[l-2]=='m' || fisier[l-1]=='p')
            tip=1; 
    }

    if(inf.st_mode | S_IFLNK)
        tip = 2;


    identificator=inf.st_uid;
    dimensiune=inf.st_size;
    nrLegaturi=inf.st_nlink;
    time_t raw_time = inf.st_mtime;
    struct tm *timpCorect=localtime(&raw_time);
    sprintf(data, "%02d.%02d.%d %02d:%02d",
    timpCorect->tm_mday,
    timpCorect->tm_mon,
    timpCorect->tm_year,
    timpCorect->tm_hour,
    timpCorect->tm_min);

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
    
    if(tip == 2){
        rez=lstat(fisier,&inf);
        if(rez==-1)
        {
            perror("Fisierul nu a fost gasit! Mai cauta.\n");
            exit(0);
        }
        dimensiune_legatura = inf.st_size;
    }
    return tip;
}


/*adauga in fisierul statistica.txt informatiile despre fisier/director
TIP: 0=normal, 1=bmp, 2=leg simbolica, 3=director*/
void scriereStatistici(char *fisier, int tip)
{
    int fd=open ("statistica.txt", O_WRONLY| O_APPEND);
    if(fd==-1)
    {
        perror("Eroare scriere statistici.");
    }
    int n;
    char buff[100]; //lungimea liniei

    if(tip == 0 || tip==1)
        n=sprintf(buff, "Nume fisier: %s\n", fisier);
    else if(tip == 2)
        n=sprintf(buff, "Nume legatura: %s\n", fisier);
    else
        n=sprintf(buff, "Nume director: %s\n", fisier);
    write(fd, buff, n);

    if(tip == 1){
        n=sprintf(buff, "Inaltime: %d\n", inaltime);
        write(fd, buff, n);
        n=sprintf(buff, "Latime: %d\n",latime);
        write(fd, buff, n);
    }

    if(tip ==0 || tip == 1){
        n=sprintf(buff, "Dimensiune: %d\n", dimensiune);
        write(fd, buff, n);
    }
    if(tip ==2){
        n=sprintf(buff, "Dimensiune: %d\n", dimensiune_legatura);
        write(fd, buff, n);
        n=sprintf(buff, "Dimensiune fisier: %d\n", dimensiune);
        write(fd, buff, n);
    }

    if(tip != 2){
        n=sprintf(buff, "Identificator utilizator: %d\n", identificator);
        write(fd, buff, n);
    }

    if(tip == 0 || tip == 1){
        n=sprintf(buff, "Timpul: %s\n", data);
        write(fd, buff, n);

        n=sprintf(buff, "Contor legaturi: %d\n", nrLegaturi);
        write(fd, buff, n);
    }

    n=sprintf(buff, "Drepturi acces user: %s\n", drepturi[0]);
    write(fd, buff, n);

    n=sprintf(buff, "Drepturi acces grupuri: %s\n", drepturi[1]);
    write(fd, buff, n);

    n=sprintf(buff, "Drepturi acces pentru altii: %s\n", drepturi[2]);
    write(fd, buff, n);

    write(fd, "--------------------------------------------------\n", 51);

    close(fd);
}

int main(int argc, char *argv[])
{
    if(argc!=2)
    {
        printf("Usage: %s <director_intrare>\n", argv[0]);
        exit(0);
    }
    int l=strlen(argv[1]);
    if(l<5){
        printf("Usage: %s <director_intrare>\n", argv[0]);
        exit(0);
    }
    int fd=creat ("statistica.txt", S_IWUSR|S_IRUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    if(fd==-1)
    {
        perror("Eroare creare fisier statistici.");
    }
    close(fd);

    char *director = argv[1];

    //parcurgem directorul
    //pentru fiecare intrare, verificam daca e fisier normal, bmp, leg sim, sau dir
    //in fiecare caz, in functie de tip, apelam diverse functii

    DIR *dir = opendir(director);
    if(dir == NULL){
        perror("Eroare deschidere director\n");
        exit(0);
    }
    struct dirent *intrare;
    while((intrare = readdir(dir)) != NULL){
        char denumire[200];
        strcpy(denumire, director);
        strcat(denumire, "/");
        strcat(denumire, intrare->d_name);
        int tip = citireInfoFisier(denumire);
        if(tip == 1){//fisier bmp
            citireBMP(denumire);
        }
        scriereStatistici(denumire, tip);
    }
    if(closedir(dir) == -1){
        perror("Eroare inchidere director\n");
        exit(0);
    }
    return 0;

}

