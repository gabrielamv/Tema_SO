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
#include <sys/types.h>
#include <sys/wait.h>
#define BUFFSIZE 4096

int inaltime, latime;
int identificator;
int dimensiune, dimensiune_legatura;
char data[80];
int nrLegaturi;
char drepturi[4][4];

/*Citeste latimea si lungimea unui fisier BMP*/
void citireBMP(char *fisier)
{
    int fd = open(fisier, S_IRUSR);
    if (fd == -1)
    {
        perror("Fisierul nu a fost gasit! Mai cauta.\n");
        exit(0);
    }

    lseek(fd, 18, SEEK_SET);
    read(fd, &latime, 4);
    read(fd, &inaltime, 4);
    close(fd);
}
void convertireBMP(char *fisier){
    int fd = open(fisier, O_RDWR);
    printf("Convertim imaginea %s\n", fisier);
    if (fd == -1)
    {
        perror("Fisierul nu a fost gasit! Mai cauta.\n");
        exit(0);
    }
    int offset;
    int padding;
    int bit_cnt;
    lseek(fd, 10, SEEK_SET);
    read(fd, &offset, 4);
    lseek(fd, 28, SEEK_SET);
    read(fd, &bit_cnt, 2);
    //read(fd, &latime, 4);
    //read(fd, &inaltime, 4);
    padding=(32-(3*latime*bit_cnt)%32)%32;
    padding=padding/8;

    lseek(fd, offset, SEEK_SET);
    int i,j;
    for(i=0; i<inaltime; i++)
    {
        for(j=0; j<latime; j++)
        {
            
            unsigned char p[(3*bit_cnt/8)+1];
            read(fd, p, bit_cnt/8);
            unsigned int r=p[0];
            unsigned int g=p[1];
            unsigned int b=p[2];
            unsigned int gri= 0.299 * r + 0.587 * g + 0.114 * b;

            lseek(fd, -(3*bit_cnt/8), SEEK_CUR);
            memset(p, gri, (3*bit_cnt/8));
            write(fd, p, 3*bit_cnt/8);
            
        }
        lseek(fd, padding, SEEK_CUR);
    }




    close(fd);
}


/*Citeste informatiile despre un fisier*/
int citireInfoFisier(char *fisier)
{
    struct stat inf;
    int rez = stat(fisier, &inf);
    if (rez == -1)
    {
        perror("Fisierul nu a fost gasit! Mai cauta.\n");
        exit(0);
    }

    int tip = -1;

    if (S_ISREG(inf.st_mode))
    {
        tip = 0;
        int l = strlen(fisier);
        if (fisier[l - 4] == '.' && fisier[l - 3] == 'b' && fisier[l - 2] == 'm' && fisier[l - 1] == 'p')
            tip = 1;
    }else
    if (S_ISDIR(inf.st_mode)){
        tip = 3;
    }
    if (S_ISLNK(inf.st_mode)){
        tip = 2;
    }


    identificator = inf.st_uid;
    dimensiune = inf.st_size;
    nrLegaturi = inf.st_nlink;
    time_t raw_time = inf.st_mtime;
    struct tm *timpCorect = localtime(&raw_time);
    sprintf(data, "%02d.%02d.%d %02d:%02d",
            timpCorect->tm_mday,
            timpCorect->tm_mon,
            timpCorect->tm_year,
            timpCorect->tm_hour,
            timpCorect->tm_min);

    if (inf.st_mode & S_IRUSR)
        drepturi[0][0] = 'R';
    else
        drepturi[0][0] = '-';
    if (inf.st_mode & S_IWUSR)
        drepturi[0][1] = 'W';
    else
        drepturi[0][1] = '-';
    if (inf.st_mode & S_IXUSR)
        drepturi[0][2] = 'E';
    else
        drepturi[0][2] = '-';

    if (inf.st_mode & S_IRGRP)
        drepturi[1][0] = 'R';
    else
        drepturi[1][0] = '-';
    if (inf.st_mode & S_IWGRP)
        drepturi[1][1] = 'W';
    else
        drepturi[1][1] = '-';
    if (inf.st_mode & S_IXGRP)
        drepturi[1][2] = 'E';
    else
        drepturi[1][2] = '-';

    if (inf.st_mode & S_IROTH)
        drepturi[2][0] = 'R';
    else
        drepturi[2][0] = '-';
    if (inf.st_mode & S_IWOTH)
        drepturi[2][1] = 'W';
    else
        drepturi[2][1] = '-';
    if (inf.st_mode & S_IXOTH)
        drepturi[2][2] = 'E';
    else
        drepturi[2][2] = '-';

    if (tip == 2)
    {
        rez = lstat(fisier, &inf);
        if (rez == -1)
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
int scriereStatistici(char *cale, char *fisier, int tip)
{
    int nr_linii = 0;
    int fd = open(cale, O_CREAT|O_RDWR|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    if (fd == -1)
    {
        perror("Eroare scriere statistici.");
    }
    int n;
    char buff[100]; // lungimea liniei

    if (tip == 0 || tip == 1)
        n = sprintf(buff, "Nume fisier: %s\n", fisier);
    else if (tip == 2)
        n = sprintf(buff, "Nume legatura: %s\n", fisier);
    else
        n = sprintf(buff, "Nume director: %s\n", fisier);
    write(fd, buff, n);
    nr_linii++;

    if (tip == 1)
    {
        n = sprintf(buff, "Inaltime: %d\n", inaltime);
        write(fd, buff, n);
        nr_linii++;
        n = sprintf(buff, "Latime: %d\n", latime);
        write(fd, buff, n);
        nr_linii++;
    }

    if (tip == 0 || tip == 1)
    {
        n = sprintf(buff, "Dimensiune: %d\n", dimensiune);
        write(fd, buff, n);
        nr_linii++;
    }
    if (tip == 2)
    {
        n = sprintf(buff, "Dimensiune: %d\n", dimensiune_legatura);
        write(fd, buff, n);
        nr_linii++;
        n = sprintf(buff, "Dimensiune fisier: %d\n", dimensiune);
        write(fd, buff, n);
        nr_linii++;
    }

    if (tip != 2)
    {
        n = sprintf(buff, "Identificator utilizator: %d\n", identificator);
        write(fd, buff, n);
        nr_linii++;
    }

    if (tip == 0 || tip == 1)
    {
        n = sprintf(buff, "Timpul: %s\n", data);
        write(fd, buff, n);
        nr_linii++;

        n = sprintf(buff, "Contor legaturi: %d\n", nrLegaturi);
        write(fd, buff, n);
        nr_linii++;
    }

    n = sprintf(buff, "Drepturi acces user: %s\n", drepturi[0]);
    write(fd, buff, n);
    nr_linii++;

    n = sprintf(buff, "Drepturi acces grupuri: %s\n", drepturi[1]);
    write(fd, buff, n);
    nr_linii++;

    n = sprintf(buff, "Drepturi acces pentru altii: %s\n", drepturi[2]);
    write(fd, buff, n);
    nr_linii++;

    /// write(fd, "--------------------------------------------------\n", 51);

    close(fd);
    return nr_linii;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <director_intrare> <director_iesire>\n", argv[0]);
        exit(0);
    }
    int l = strlen(argv[1]);
    if (l < 5)
    {
        printf("Usage: %s <director_intrare> <director_iesire>\n", argv[0]);
        exit(0);
    }

    char *director = argv[1];

    // parcurgem directorul
    // pentru fiecare intrare, verificam daca e fisier normal, bmp, leg sim, sau dir
    // in fiecare caz, in functie de tip, apelam diverse functii

    DIR *dir = opendir(director);
    if (dir == NULL)
    {
        perror("Eroare deschidere director\n");
        exit(0);
    }
    struct dirent *intrare;
    int nr_copii=0;
    while ((intrare = readdir(dir)) != NULL)
    {
        char denumire[200];
        strcpy(denumire, director);
        strcat(denumire, "/");
        strcat(denumire, intrare->d_name);
        int tip = citireInfoFisier(denumire);
        // printf("Fisier %s tip:%d\n", denumire, tip);
        if (tip == 1)
        { // fisier bmp
            citireBMP(denumire);
            int pid;
            if ((pid = fork()) < 0)
            {
                perror("Eroare");
                exit(1);
            }
            if (pid == 0)
            {//copil

                
                convertireBMP(denumire);
                exit(0);
            }
            //parinte
            nr_copii++;
        }
        int pid;
        if ((pid = fork()) < 0)
        {
            perror("Eroare");
            exit(1);
        }
        if (pid == 0)
        {
            /* codul fiului - doar codul fiului poate ajunge aici (doar in fiu fork() returneaza 0)*/

            char cale[100];
            strcpy(cale, argv[2]);
            strcat(cale, "/");
            strcat(cale, intrare->d_name);
            strcat(cale, "_statistica.txt");

            int nr = scriereStatistici(cale, denumire, tip);

            exit(0); // apel necesar pentru a se opri codul fiului astfel incat acesta sa nu execute si codul parintelui
        }
        /* codul parintelui */
        nr_copii++;
    }
    if (closedir(dir) == -1)
    {
        perror("Eroare inchidere director\n");
        exit(0);
    }
    while(nr_copii)
    {
        int status;
        int pid = wait(&status);
        char mesaj[100];
        int n = sprintf(mesaj, "S-a încheiat procesul cu pid-ul %d și codul %d.\n", pid, status);
        write(1,mesaj,n);
        nr_copii--;
    }

    return 0;
}
