/* ch.c.
auteur: Maude Sabourin et Jeremy Coulombe
date: 20 Janvier 2018
problèmes connus: a date aucun
  */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int main (void)
{
    printf ("Mini-Shell > ");
    char input[200];
    char path[100], command[100], *parameters[200];

    //Éventuellement, aller chercher dans tout le path avec scanf p-e
    char *envp[] = {(char *) "PATH=/bin",0};

    while (strcmp(input,"exit") != 0){
        read_input();

        parameters[1]=NULL;
        //Copie le /bin/ dans path
        strcpy (path, "/bin/");
        //Concatène le chemin à la commande, par exemple /bin/ls
        strcat (path, command);
        //Exécute la commande
        execve(path, parameters, envp);

        
        //if(fork() != 0){
        //wait(NULL);
        //} else{
        //}
    }
    fprintf (stdout, "Bye!\n");
    exit (0);
}

void read_input(){
    //TODO:Pour l'instant on crée les trucs dans la loop, éventuellement le créer en dehors et écraser la mémoire
    char input[200];
    char command[100], *parameters[200];

    //TODO: Rajouter ça si on a des problèmes de cast
    //fgets(line,sizeof(line),stdin);
    //sscanf (line, "%s", answer);

    //Crédit G Praveen Kumar pour le scanf avec espacement
    scanf(" %[^\n]s", input);
    printf("%s", input);

    //Sépare chaque partie du string
    char* split;
    split = strtok (input, " ");

    //TODO:Cette commande copie juste le premier char, on veut le mot
    command[0] = split[0];

    while (split != NULL){
        //Print chaque mot pour tester
        printf("%s\n", split);
        split = strtok (NULL, " ");
    }

    printf("test commande %s", command);


    if(command[0] == '.') {
        //DO STUFF
    }
    exit(0);
    /*switch(command)
     * {
     *      case "cd":
     *                  int retour = chdir(parametre);
     *                  if(retour == -1)
     *                  {
     *                      printf("%s",errno);
     *                  }
     *     case default: //lit la variable path du système puis va les parcourir
     *                     // potentiellement avec scandir
     *                   char *path = getenv("PATH");
     *                  parse path -> obtient const char *listePath -> tableau des chemins
     *
     *                 for(i = 0, i< sizeof listepath; i++)
     *                 {
     *                      struct dirent **namelist;
     *                      scandir(listePath[i], namelist,Null,alphasort);
     *
                                for(j = 0 , j< sizeof namelist;j++)
                                {
                                    if ( strstr(namelist[j],command) != null )
                                    {
                                         execve(namelist[j], parameters, envp);
                                         break;
                                    }
                                }
     *
     *                  }
     *
     *
     *
     * }*/

    printf("nouvelle commande");
    //scanf( " % 100 %200",command, input );
    scanf( "%200",command );



}
char *parse(char *string, const char *delim)
{
    // inspire par
    // https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
    partie = strtok(string,delim);

    char **tableau[20];

    tableau[0] = partie;
 //   int index= sizeof (partie);
    int index= 1;


    while(partie != Null)
    {
        partie = strtok(Null,delim);
        tableau[index] = partie ;
        //index = index + sizeof(partie);
        index++;
        if (index  >= sizeof (tableau))
        {
            int taille = sizeof (tableau) ;

            char **echange = malloc(taille*2 );
            for (int i = 0; i <  taille ; ++i)
            {
                echange[i] = tableau;
            }
            free(tableau);
            tableau = echange;
        }
    }
}

void read_command (char cmd[], char *par[]){
    char line[1024];
    int count =0, i=0, j=0;
    char *array[100], *pch;

    for (;;){
        int c = fgetc (stdin);
        line[count++] = (char) c;
        if (c=='\n') break;
    }
    if (count==1) return;
    pch= strtok (line, " \n");

    while (pch!=NULL){
        array[i++] = strdup (pch);
        pch = strtok (NULL, " \n");
    }
    //commande
    strcpy (cmd, array[0]);

    //param
    for (j;j<i;j++){
        par[j] = array[j];
    }
    par[i]= NULL;
}
