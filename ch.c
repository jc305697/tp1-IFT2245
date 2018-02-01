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
#include <stdbool.h>

//Prend un pointeur et l'assigne dans un tableau de pointeurs
char **parse(char **string, const char *delim)
{
    // inspire par
    // https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
    //TODO: VERIFIER SI MARCHE AVEC STRING CHAR**
    char *partie = strtok(string[0],delim);

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

//Va chercher le prochain TOKEN dans le strtok
char* getNextValue(char *currentToken){
    return strtok (NULL, " ");
}

//Lit ce que l'usager entre et le split selon l'espace
char* read_input() {
    char* input;
    size_t taille;
    input = (char *) malloc(200 * sizeof(char));
    //TODO: Rajouter ça si on a des problèmes de cast
    //fgets(line,sizeof(line),stdin);
    //sscanf (line, "%s", answer);

    //Crédit G Praveen Kumar pour le scanf avec espacement
    scanf(" %[^\n]s", input);
    //getline(&input,&taille,stdin);//TODO: remplacer scanf par getline
    //Sépare chaque partie du string
    char *split;
    split = strtok(input, " ");
    return split;
}

//Prend un pointeur et crée un array de pointeurs pour chaque mot
char** parse_input(char *input) {
    char **parsedArray = (char **) malloc(sizeof(char*)*BUFSIZ);
    int i = 0;
    while (input != NULL) {
        parsedArray[i] = malloc(strlen(input) + 1);
        strcpy(parsedArray[i], input);
        input = getNextValue(input);
        i++;
    }
    return parsedArray;
}


int longueur = 0;
//Code jérémy variables
/*
void manager_vars(char** parsedArray,char** parameters){
    //gere les assignations de variables
    const char egal[2] = "=";
    char **ligneSep = parse(parsedArray,egal);
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
        //TODO:a completer probleme est remplacer la variable par sa valeur
    }
}
*/
char** getParameters(char** arrayInput){
    if (longueur==0){
        while(arrayInput[longueur]!= 0)
        {
            longueur = longueur + 1;
        }
    }

    int i = 0;
    while(arrayInput[i]!=NULL){
        i++;
    }

    const char ch = '=';
    if (i<=1){
        char* ret = strchr(arrayInput[0],ch);

        if (ret!=NULL){
            //manager_vars();
        }
    }
    char **temp = (char **) malloc(sizeof(char*)*BUFSIZ);

    for (int i = 0; i<sizeof(arrayInput);i++){
        temp[i] = arrayInput[i];
    }
    return temp;
}

void execution (char** arrayInput, char** temp)
{

    char cd[3] = "cd";
    if( strcmp(arrayInput[0],cd) == 0){
        //cd n'a qu'un seul argument
        int retour = chdir(temp[0]);
        if(retour == -1)
        {
            printf("%d",errno);
            printf("%s",strerror(errno));
        }
    } else {
        int value_returned;
        if (temp[0] != NULL){
            //Pour ls et cat et compagnie
            value_returned = execvp(arrayInput[0],temp);
        }else{
            //Pour les déclarations de variables
            value_returned = execlp(arrayInput[0],arrayInput[0],0);
        }

        /*
        const char s[2] = " ";
        char **ligneSep = parse(ligneCommande, s);

        int retour2 = execvp(ligneSep[0], parameters);
        */
        if (value_returned == -1) {
            printf("wtf");
            printf("%d", errno);
        }
    }
}

void lireLigne(char **command, char **parameters)
{
    bool passer = false;
    int i = 0;
    const char *pour = "for";
    //pour = malloc(4 * sizeof(char));
  //  pour[0] = 'f';
    if (strstr(command[0], pour)!= NULL)
    {
        //faire gestion si commande est un for
    }



    while(command[0][i]!= 0)
    {
        if(command[0][i] == '=')//si assignation en premier
        {
            passer = true;
            char *variable;
            variable = strtok(command[0], "=");
            printf("variable = %s\n",variable);
            char *valeur;
            valeur = strtok(NULL,"="); // va chercher la premiere partie
            printf("valeur = %s\n",valeur);//va chercher la seconde partie
            int overwrite = 1;
            setenv(variable,valeur,overwrite);

            printf("valeur de variable, %s\n",getenv(variable));
            break;
        }
        i++;
    }
    if (passer == false)//si ce n'etait pas une assignation donc = commande
    {
        execution(command,parameters);
    }

}

int main(void) {
    printf("Mini-Shell > ");
    char input[200];
    while (strcmp(input, "exit") != 0) {
        char *result = read_input();
        printf("result = %s \n",result);
        char **command = parse_input(result);

        char **parameters = getParameters (command);
        lireLigne(command,parameters);
        //execution(command,parameters);
        exit(0); //TODO: enlever pour faire une boucle

        //if(fork() != 0){
        //wait(NULL);
        //} else{
        //}
    }
    fprintf(stdout, "Bye!\n");
    exit(0);
}





void read_command(char cmd[], char *par[]) {
    char line[1024];
    int count = 0, i = 0, j = 0;
    char *array[100], *pch;

    for (;;) {
        int c = fgetc(stdin);
        line[count++] = (char) c;
        if (c == '\n') break;
    }
    if (count == 1) return;
    pch = strtok(line, " \n");

    while (pch != NULL) {
        array[i++] = strdup(pch);
        pch = strtok(NULL, " \n");
    }
    //commande
    strcpy(cmd, array[0]);

    //param
    for (j; j < i; j++) {
        par[j] = array[j];
    }
    par[i] = NULL;
}
