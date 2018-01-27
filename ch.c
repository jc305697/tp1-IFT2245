/* ch.c.
auteur: Maude Sabourin et Jeremy Coulombe
date: 20 Janvier 2018
problèmes connus: a date aucun
  */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

char **parse(char *string, const char *delim)
{
    // inspire par
    // https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
    char *partie = strtok(string,delim);

    char **tableau = malloc(20 * sizeof(char*));

    tableau[0] = partie;
    //   int index= sizeof (partie);
    int index= 1;


    while(partie != NULL)
    {
        partie = strtok(NULL,delim);
        tableau[index] = partie ;
        //index = index + sizeof(partie);
        index++;
        if (index  >= sizeof (tableau))
        {
            int taille = sizeof (tableau) ;

            char **echange = malloc(taille * 2 );
            for (int i = 0; i <  taille ; ++i)
            {
                echange[i] = tableau[i];
            }
            free(tableau);
            char **tableau = malloc(sizeof(echange));

            for (int i = 0; i <  taille ; ++i)
            {
                strcpy(tableau[i],echange[i]);
            }

            free(echange);
        }
    }
    return tableau;

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
    printf("input = %s\n", input);

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



    //copie input puisque strtok modifie la string
    int longueur = 0;

    while(input[longueur]!= 0)
    {
        longueur = longueur + 1;
    }

    char *ligneCommande = malloc( longueur * sizeof(char));

    ligneCommande = strcpy(ligneCommande, input);

    //gere les assignations de variables
    const char egal[2] = "=";
    char **ligneSep = parse(ligneCommande,egal);
    int mot = 0;

    while(ligneSep[mot]!=NULL)
    {
        const char *variable;
        int caractere = 0;
        while (ligneSep[mot][caractere] != 0)
        {//veut le dernier caractere
            variable = &ligneSep[mot][caractere];
            caractere = caractere + 1;
        }

        if(ligneSep[mot+1]!=NULL)//si on est pas au dernier mot
        {
            const char *valeur = &ligneSep[mot+1][0]; //va chercher la valeur
            int overwrite = 1;
            int retour3 = setenv(variable,valeur,overwrite);
            if (retour3 == -1)
            {
                printf("%d",errno);
            }
        }

    }

    //gere les appels aux variable avec $
    free(ligneSep);
    const char dollar[2] = "$";
    free(ligneCommande);

    char *copieLigneCommande = malloc( longueur * sizeof(char));

    copieLigneCommande = strcpy(copieLigneCommande, command);

    char **ligneCommSep;
    ligneCommSep = parse(copieLigneCommande,dollar);
    free(copieLigneCommande);

    int i = 1;
    const char *variable;
    char *valeur;
    while (ligneCommSep[i] != NULL)
    {
        variable = &ligneCommSep[i][0];
        valeur = getenv(variable);
        //a completer probleme est remplacer la variable par sa valeur
    }



    //switch(command)
    //{
        char cd[3] = "cd";
        if( strcmp(command,cd) == 0)
        {
           /* int longueur1 = 0;

            while(input[longueur1]!= 0)
            {
                longueur1 = longueur1 + 1;
            }
            const char *parametre = malloc(longueur1* sizeof(char));
            parametre = strcpy(parametre,parameters);
            int retour = chdir(parametre);
            */
            int retour = chdir(parameters);
            if(retour == -1)
            {
                printf("%d",errno);
            }
        }
        else
        {
            //case default: //lit la variable path du système puis va les parcourir
            // potentiellement avec scandir
            const char s[2] = " ";

            char **ligneSep = parse(ligneCommande, s);

            int retour2 = execvp(ligneSep[0], parameters);

            if (retour2 == -1) {
                printf("%d", errno);
            }
        }


    exit(0);

    // printf("nouvelle commande");
    //scanf( " % 100 %200",command, input );
    // scanf( "%200",command );



}



int main (void)
{
    printf ("Mini-Shell > ");
    char input[200];
    char path[100], command[100], *parameters[200];

    //Éventuellement, aller chercher dans tout le path avec scanf p-e
    char *envp[] = {(char *) "PATH=/bin",0};
    //    char *envp[] = {getenv("PATH"),0};
    while (strcmp(input,"exit") != 0){
        read_input();

        parameters[1]=NULL;
        //Copie le /bin/ dans path
        strcpy (path, "/bin/");

        //Concatène le chemin à la commande, par exemple /bin/ls
        strcat (path, command);
        //Exécute la commande
        execve(path, parameters, envp);
        //execvp(command,parameters);


        //if(fork() != 0){
        //wait(NULL);
        //} else{
        //}
    }
    fprintf (stdout, "Bye!\n");
    exit (0);
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
