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

//Va chercher le prochain TOKEN dans le strtok
char* getNextValue(char *currentToken){
    return strtok (NULL, " ");
}

//Lit ce que l'usager entre et le split selon l'espace
char* read_input() {
    char input[200];

    //TODO: Rajouter ça si on a des problèmes de cast
    //fgets(line,sizeof(line),stdin);
    //sscanf (line, "%s", answer);

    //Crédit G Praveen Kumar pour le scanf avec espacement
    scanf(" %[^\n]s", input);

    //Sépare chaque partie du string
    char *split;
    split = strtok(input, " ");
    return split;
}

//Prend un pointeur et crée un array de pointeurs pour chaque mot
char** parse_input(char *input) {
    printf("avant malloc");
    printf(input);
    char **parsedArray = (char **) malloc(sizeof(char*)*BUFSIZ);
    printf("apres malloc");
    printf(input);
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
void manager_vars(){
    //TODO: Trouver comment on va intégrer ce code avec le array de pointeurs plutot que des array direct
    char command[10] = "";
    char input[10] = "";
    char parameters[10] = "";
    if(command[0] == '.') {
        //DO STUFF
    }

    //copie input puisque strtok modifie la string

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
}


void execution (char** arrayInput)
{
    if (longueur==0){
        while(arrayInput[longueur]!= 0)
        {
            longueur = longueur + 1;
        }
    }
    //const char *parametre = malloc(longueur* sizeof(char));
    //TODO: Envoyer tous les parametres pas juste le premier
    char* temp[100];
    for (int i = 1; i<sizeof(arrayInput);i++){
        temp[i-1] = arrayInput[i];
    }

    char cd[3] = "cd";
    if( strcmp(arrayInput[0],cd) == 0){
        //cd n'a qu'un seul argument
        int retour = chdir(temp[0]);
        if(retour == -1)
        {
            printf("%d",errno);
        }
    } else {
        //TODO: Changer pour envoyer un vecteur d'arguments
        int value_returned = execvp(arrayInput[0], temp);
        /*
        const char s[2] = " ";
        char **ligneSep = parse(ligneCommande, s);

        int retour2 = execvp(ligneSep[0], parameters);
        */
        if (value_returned == -1) {
            printf("%d", errno);
        }
    }
}


int main(void) {
    printf("Mini-Shell > ");
    char input[200];
    while (strcmp(input, "exit") != 0) {
        char *result = read_input();
        char **parsed_array = parse_input(result);
        execution (parsed_array);


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
