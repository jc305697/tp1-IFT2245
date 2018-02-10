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
#include <sys/stat.h>


//Va chercher le prochain TOKEN dans le strtok
char* getNextValue(char *currentToken){
    return strtok (NULL, " ");
}

void myFree(char **command, int length)
{
    for(int i = 0 ; i < length;i++) {
        if (command[i]!= NULL) {
            free(command[i]);
        }
    }
    free(command);
}

char** copy(char **command,int start,int end) {
    int longueur = end - start +1;
    char **copie = malloc(longueur * sizeof(char *));
    int i = 0;
    for (int j = start; j < end; j++) {
        copie[i] = malloc((strlen(command[j]) + 1) * sizeof(char));
        strcpy(copie[i], command[j]);
        i++;
    }
    copie[i] = malloc(sizeof(NULL));
    copie[i] = NULL;

    return copie;
}

char* read_input() {
    //char* input =NULL;
    char* input;
    size_t taille = 0;
    //input = (char *) malloc(200 * sizeof(char));

    //Crédit G Praveen Kumar pour le scanf avec espacement
    //scanf(" %[^\n]s", input);
    getline(&input,&taille,stdin);
    input = strtok(input,"\n");
    // getline(&input,&taille,stdin);//TODO: remplacer scanf par getline
    //Sépare chaque partie du string
    char *split = strtok(input, " ");
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

    char **temp = (char **) malloc(sizeof(char*)*(BUFSIZ ));
    // char **temp = (char **) malloc(sizeof(char*)*(strlen(*arrayInput) +1));
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

    while(command[mot] != 0) { //tant que c'est pas la fin de command
        lettre = 0;
        while (command[mot][lettre] != 0) {//tant que c'est pas la fin du mot


            if (command[mot][lettre] == '$') {//si je dois remplacer la variable par sa valeur
                char * referenceOriginal;
                char *copie = malloc((strlen(command[mot]) + 1) * sizeof(char));
                if (lettre != 0) {
                    copie = strncpy(copie, command[mot], lettre - 1);
                    //copie tout ce qui est dans le string avant le $
                }
                //S'il y a d'autres variables pas séparées par des espaces
                //char** nextvar = copy(command, mot, mot+1);//copie le mot
                char  *nextvar = malloc((strlen(command[mot]) + 1) * sizeof(char) );
                strcpy(nextvar,command[mot]);
                //TODO JÉRÉMY : Sais-tu comment je peux accéder au mot? ici quand je mets *nextchar ça me dit ya un token même si yen a pas (avec for j in 1 2 3 ; do echo $i $j ; done)

                int iterateur = 0;
                int occurence = 0;
                while (nextvar[iterateur] != 0){
                    if (nextvar[iterateur] == ':'){
                        occurence = occurence + 2;
                    }
                    iterateur++;
                }

                char* varItem = strtok(nextvar, ":");
                char* temp = varItem;
                varItem = strtok (NULL, ":");
                //On a la forme $VAR:$VAR:$VAR...
                if (varItem != NULL){//s'il reste des string qui avait : comme separateur
                    //int i = 0;
                    int j = 1;
                    char **tableauTemp = malloc((occurence + 1) * sizeof(char*));
                    int posTabTemp =0;
                    referenceOriginal = nextvar;
                    //Tant que existe des token (donc des variables)
                    while (varItem != NULL) {
                        //Va chercher la valeur au mot i à la première lettre (évite le $)
                        //   char *valeur = getenv(&nextvar[i][j]);//&nextvar[i][j] est le pointeur vers la string de la variable
                        char *valeur = getenv(&varItem[j]);
                        // referenceOriginal = nextvar[i];

                        tableauTemp[posTabTemp] = valeur;
                        posTabTemp ++;
                        //==============================================================================================
                        //TODO JÉRÉMY : PEUX-TU FIX CECI, JE SAIS PAS TROP OÙ LE COPIER..
                        //En ce moment VALEUR contient la vrai valeur et il faudrait le mettre dans un tableau temporaire
                        //Et seulement à la fin utiliser strconcat pour tout remettre ensemble sous format var:var:var
                        //==============================================================================================

                        //  strcpy(&copie[j], valeur);
                        // free(referenceOriginal); voir quand je peux free
                        //  free(valeur);
                        //i++;
                        varItem = strtok (NULL, ":");
                    }

                    int i = 0;
                    char *stringConcatTemp;
                    char * stringConcat = malloc((strlen(tableauTemp[0]) + 2 + strlen("")) * sizeof(char));
                    //stringConcat = strcpy(stringConcat,"");
                    stringConcat[0] = '\0';
                    char *stringTemp;
                    while (tableauTemp[i]!=0 && tableauTemp[i+1]!=0) {
                        stringConcatTemp = strcat(tableauTemp[i],":");

                        stringConcat = strcat(stringConcat,stringConcatTemp);

                        stringTemp = malloc((strlen(tableauTemp[i+1] + strlen(stringConcat) + 2)) * sizeof(char));
                        //prend plus d'espace memoire pour pouvoir concatener la prochaine string
                        stringTemp = strcpy(stringTemp,stringConcat);
                        //copie la string actuelle dans le nouvel espace memoire
                        free(stringConcat);
                        //libere ancien espace memoire et donne le nouvel espace memoire
                        stringConcat =stringTemp;

                        free(stringConcatTemp);
                        i++;
                    }
                    if ((tableauTemp[i]!=0 && tableauTemp[i+1]==0)){
                        stringConcat = strcat(stringConcat,tableauTemp[i]);
                        i++;
                    }
                    free(command[mot]);
                    command[mot] = stringConcat;
                    free(referenceOriginal);

                }else{
                    char *valeur = getenv(&command[mot][lettre + 1]);
                    //obtient la valeur de ce que se trouve après $ (va jusqu'à la fin de command[mot])
                    referenceOriginal = command[mot];

                    command[mot] = strcpy(&copie[lettre], valeur);

                    free(referenceOriginal);

                    // pointe vers la string avec la valeur mis à jour
                }

            }
            lettre++;
        }
        mot++;
    }
}

int execution (char** arrayInput, char** temp) {
    char cd[3] = "cd";
    //Commande particuliere
    if( strcmp(arrayInput[0],cd) == 0){
        //cd n'a qu'un seul argument
        int retour = chdir(temp[1]);
        if(retour == -1) {
            printf("numero erreur = %d",errno);
            printf("erreur 1 = %s \n",strerror(errno));
        }
        return retour;
    } else {
        //Toutes les autres commandes
        int value_returned = 0;
        pid_t pid = fork();
        int stat;
        if (pid == 0){
            //child
            if (temp[0] != NULL){
                //Pour ls et cat et compagnie
                value_returned = execvp(arrayInput[0],temp);
                exit(value_returned);
            }
        }else{
            wait(&stat);
            return stat;
        }

    }
}

struct Return {
    bool format;
    int posPointVirg;
};

int findNextSplit(int start, char** command, char* val){
    //Loop until reach the first ;
    while (strcmp(command[start], val) != 0){
        start++;
    }
    return start;
}

/* Verifie la structure du for s'il y en a un */
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

    int posPointVirg = findNextSplit(2, command, ";");

    if (strcmp(command[posPointVirg + 1], "do") != 0) {
        printf("For badly made, need the format For x in range;Do ...; Done");
        return return1;
    }
    struct Return return2 = {true, posPointVirg};
    return return2;
}

/* Verifie la fin d'une loop "for" */
bool instructionDone(char** command, int start){
    if (strcmp(command[start], "done") != 0){
        return false;
    }
    return true;
}

int exec (char** copie, int start, int end){
    int valeur_retour;
    char **parameters = getParameters (copie, 0, end-start);
    valeur_retour = execution(parameters,parameters);
    //myFree(copie, end-start);free déjà via pfirst psecond?
    return valeur_retour;
}


int runFor(char **command, int pos){
    char *variable = command[1];
    int start;
    int i = 3;
    int overwrite = 1;
    int end;
    int valeur_retour;

    while (i < pos) {
        if (strstr(command[i],"$")){
            char *valeur = getenv(&command[i][1]);
            command[i] = valeur;
        }
        start = pos+2;

        setenv(variable, command[i], overwrite);

        char** copie;
        while(!instructionDone(command,start)){
            end = findNextSplit(start, command, ";");
            if (strcmp(command[end+1],"do")==0){
                //On va chercher le prochain ; et on ajoute un pour le done
                end = findNextSplit(end+2,command,";")+2;
                copie = copy(command,start,end);
                lireLigne(copie,0,end-start);
            }
            copie = copy(command,start,end);
            remplaceVariable(copie);

            valeur_retour = exec(copie,start,end);

            if (valeur_retour < 0)
            {
                printf("erreur numero %d dans for \n",valeur_retour);
                return valeur_retour;
            }
            start = end+1;
        }
        i++;
    }
    return valeur_retour;
}


int runAssignation(char **command){
    char *copie = malloc((strlen(command[0])+ 1) * sizeof(char));
    char *variable; //Contiendra le nom de la variable
    //Copie pour ne pas modifier la commande initiale
    copie = strcpy(copie,command[0]);
    variable = strtok(copie, "=");
    char *valeur;
    int valeur_retour;
    valeur = strtok(NULL,"="); //Valeur à donner à la variable
    int overwrite = 1;
    valeur_retour = setenv(variable,valeur,overwrite);//Assignation de la valeur dans l'environnement
    free(copie);
    return valeur_retour; //-1 si erreur
}

int lireLigne(char **command, int start, int end) {
    int i = 0;
    int valeur_retour;
    const char *pour = "for";

    //Check if "for" is the command
    if (strstr(command[start], pour)!= NULL) {
        //Check if for is well made
        struct Return rtn = checkFor(command);
        if (rtn.format){
            valeur_retour = runFor(command,rtn.posPointVirg);
            return valeur_retour;

        }else{
            return -1;
        }
    }else{
        //Lit la ligne et remplace les $ par leur valeur
        remplaceVariable(command);

        //Vérifie si la ligne est une assignation
        while(command[start][i]!= 0) {
            if(command[start][i] == '='){//si assignation en premier
                valeur_retour = runAssignation(command);
                return valeur_retour;
            }
            i++;
        }

        //On a une commande
        return exec(command,start,end);
    }

}

struct concat {
    bool isNext;
    int type; //0 for &&, 1 for ||
    int pos;
};
/*
 * retourne la position du prochain && ou || isNext indique s'il y en a un
 * autre. type laquelle des 2 et pos indique la position.
 */
struct concat getNextConcat(char** command, int start){
    const char *et = "&&";
    const char *ou = "||";
    int mot = start;
    while (command[mot] != NULL){
        if (strstr(command[mot],et) != NULL){
            struct concat ct = {true, 0, mot};
            return ct;
        }
        if (strstr(command[mot],ou) != NULL){
            struct concat ct = {true, 1, mot};
            return ct;
        }
        mot ++;
    }
    struct concat ct = {false,-1,-1};
    return ct;
}

int getLastWord(char** command,int start){
    int temp = start;
    while (command[temp] != NULL){
        temp++;
    }
    return temp;
}

void splitParts(char** command){
    int i = 0;
    int valeur_retour;
    //const char *pour = "for";
    //const char *et = "&&";
    // const char *ou = "||";
    int mot = 0; //à enlever
    struct concat res;
    res = getNextConcat(command,mot);

    if (res.isNext){//si il y a un && ou un ||
        int start = 0;
        int end = res.pos;//position du &&
        char** pfirst = copy(command, start, end);//copie avant le &&

        int starts = res.pos+1;
        int ends = getLastWord(command,starts);
        char** psecond = copy(command, starts, ends);

        //End toujours 1 de plus que le final
        int valeurRetour1 = lireLigne(pfirst,0,end);
        //65280 is 255 in hexa, child exited normally
        if (valeurRetour1 != 0 && valeurRetour1!= 65280 && res.type==0){
            myFree(pfirst, end-start);
            myFree(psecond, ends-starts);
            return;
        }else if (valeurRetour1==0 && res.type==1){
            return;
        }

        if (getNextConcat(psecond,0).isNext){
            splitParts(psecond);
            return;
        }

        int valeurRetour2 = lireLigne(psecond,0,ends-end-1);
        if (valeurRetour2 != 0) {
            myFree(pfirst, end-start);
            myFree(psecond, ends-starts);
            return;//je retourne la 2e partie
        }


    }else{
        int end = getLastWord(command,0);
        lireLigne(command,0,end);
    }

}

int main(void) {
    printf("Mini-Shell > ");
    // char input[200];
    char* res = read_input();
    // printf(" valeur de input = %s\n",res);
    while (strcmp(res, "exit") != 0) {
        char **command = parse_input(res);
        splitParts(command);
        //lireLigne(command);
        //  free(command);
        // free(res);
        printf("\r\n Mini-Shell > ");
        res = read_input();
    }
    fprintf(stdout, "Bye!\n");

    exit(0);
}
