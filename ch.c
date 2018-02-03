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
#include <ctype.h>
#include <stdbool.h>
#include <sys/wait.h>

//Va chercher le prochain TOKEN dans le strtok
char* getNextValue(char *currentToken){
    return strtok (NULL, " ");
}

//Lit ce que l'usager entre et le split selon l'espace
char* read_input() {
    char* input;
    input = (char *) malloc(200 * sizeof(char));

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

char** getParameters(char** arrayInput, int start, int end){
    if (longueur==0){
        while(arrayInput[longueur]!= 0) {
            longueur = longueur + 1;
        }
    }

    char **temp = (char **) malloc(sizeof(char*)*BUFSIZ);
    int k=0;
    for (int j = start; j<end;j++){
        temp[k] = arrayInput[j];
        k++;
    }
    return temp;
}

void remplaceVariable (char **command)
{ //lit la ligne de commande et remplace les variables identifier par $
    int mot = 0;

    int lettre;

    while(command[mot] != 0) { //tant que c'est pas la fin
        lettre = 0;
        while (command[mot][lettre] != 0) {//tant que c'est pas la fin

            if (command[mot][lettre] == '$') {//si je dois remplacer

                char *copie = malloc((strlen(command[mot]) + 1) * sizeof(char));
                //alloue la memoire pour la copie


                char *variable; //devrait remplacer la variable et le $ par valeur

                if (lettre != 0) {
                    copie = strncpy(copie, command[mot], lettre - 1);
                    //copie tout ce qui est dans le string avant le $
                }
                char *valeur;
                valeur = getenv(&command[mot][lettre + 1]);
                //obtient la valeur de ce que se trouve après $
                // (va jusqu'à la fin de command[mot])

                printf("valeur = %s\n", valeur);
                command[mot] = strcpy(&copie[lettre], valeur);
                // pointe vers la string avec la valeur mis à jour

            }
            lettre++;
        }
        mot++;
    }
}

void execution (char** arrayInput, char** temp)
{

    char cd[3] = "cd";
    if( strcmp(arrayInput[0],cd) == 0){
        //cd n'a qu'un seul argument
        int retour = chdir(temp[1]);
        if(retour == -1)
        {
            printf("%d",errno);
            printf("erreur 1 = %s",strerror(errno));
        }
    } else {
        int value_returned=0;
        pid_t pid = fork();
        if (pid ==0){
            if (temp[0] != NULL){
                //Pour ls et cat et compagnie
                value_returned = execvp(arrayInput[0],temp);
            }else{
                //Pour les déclarations de variables
                value_returned = execlp(arrayInput[0],arrayInput[0],0);
            }

        }else{
            wait(NULL);
        }

        if (value_returned == -1) {
            //TODO : Cette commande bug la quatrieme fois for i in 1 2 3 ; do ls ; done
            printf("erreur 2 = %s",strerror(errno));

        }
    }
}

struct Return {
    bool format;
    int posPointVirg;
};

int findNextSplit(int start, char** command){

    //Loop until reach the first ;
    while (strcmp(command[start], ";") != 0){
        start++;
    }
    return start;
}

struct Return checkFor(char **command){
    char *variable = command[1];
    struct Return return1 = {false,0};

    if (!(isalpha(*variable))){
        printf("For badly made, a variable is necessary");
        return return1;
    }

    if (strcmp(command[2], "in") != 0) {
        printf("For badly made, need the format For x in range;Do ...; Done");
        return return1;
    }

    int posPointVirg = findNextSplit(2, command);

    if (strcmp(command[posPointVirg + 1], "do") != 0) {
        printf("For badly made, need the format For x in range;Do ...; Done");
        return return1;
    }
    struct Return return2 = {true, posPointVirg};
    return return2;
}

bool instructionDone(char** command, int start){
    if (strcmp(command[start], "done") != 0){
        return false;
    }
    return true;
}

int runFor(char **command, int pos){
    //TODO: Rajouter un autre while pour chaque instruction
    char *variable = command[1];
    int start;
    int i = 3;
    int overwrite = 1;
    int end;
    //Boucle pour le range de i
    while (i < pos) {
        start = pos+2;
        setenv(variable, command[i], overwrite);
        //Boucle pour toutes les exécutions séparée par un ;
        while(!instructionDone(command,start)){
            end = findNextSplit(start, command);
            remplaceVariable(command);
            char **parameters = getParameters (command, start, end);
            execution(parameters,parameters);
            start = end+1;
        }
        i++;
    }
    return i;
}

void runAssignation(char **command){
    char *copie = malloc((strlen(command[0])+ 1) * sizeof(char));
    char *variable;
    copie = strcpy(copie,command[0]);
    variable = strtok(copie, "=");
    char *valeur;
    valeur = strtok(NULL,"="); // va chercher la premiere partie
    int overwrite = 1;
    setenv(variable,valeur,overwrite);
}

void lireLigne(char **command,  char *input) {
    bool passer = false;
    int i = 0;
    const char *pour = "for";
    //Check if "for" is the command
    if (strstr(command[0], pour)!= NULL) {
        //Check if for is well made
        struct Return rtn = checkFor(command);
        if (rtn.format){
            i = runFor(command,rtn.posPointVirg);
        }else{
            return;
        }
    }else{

        //Lit la ligne et remplace les $ par leur valeur
        remplaceVariable(command);

        //Vérifie si la ligne est une assignation
        while(command[0][i]!= 0) {
            if(command[0][i] == '='){//si assignation en premier
                runAssignation(command);
                passer=true;
            }
            i++;
        }

        //On a une commande
        if (passer == false){
            char **parameters = getParameters (command, 0, sizeof(command));
            execution(command,parameters);
        }
    }
}



int main(void) {
    printf("Mini-Shell > ");
    char input[200];
    setenv("i","1",1);//pour tester si marche avec $ TODO: enlever le test
    while (strcmp(input, "exit") != 0) {
            char *result = read_input();
            char **command = parse_input(result);
            lireLigne(command,input);
            printf("\r\n Mini-Shell > ");
    }
    fprintf(stdout, "Bye!\n");
    exit(0);
}

    /* char *commandeNonParse;//c'est la commande qui est parse par rapport au ;
    const char pointVirgule[2] = ";";
    commandeNonParse =  strtok(commandesFor,pointVirgule);

    char **commandParse = parse_input(commandeNonParse);*/
    //commandParse est la command qui a ete separer par rapport aux espaces

    // char **parameters1 = getParameters (command1);
    //lireLigne(command1,parameters1,commande);

    /*
    lireLigne(commandParse,commandeNonParse);

    while (commandeNonParse != NULL)
    {
        commandeNonParse = strtok(NULL,pointVirgule);
        if (commandeNonParse != NULL)
        {
            commandParse = parse_input(commandeNonParse);

            //parameters1 = getParameters (command1);
            //lireLigne(command1,parameters1,commande);
            lireLigne(commandParse,commandeNonParse);
        }
    }*/