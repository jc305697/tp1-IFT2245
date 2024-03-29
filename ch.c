/* ch.c.
auteur: Maude Sabourin et Jeremy Coulombe
date: 20 Janvier 2018 */

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

void myFree(char **command, int length) {
    for(int i = 0 ; i < length;i++) {
        if (command[i]!= NULL) {
            free(command[i]);
        }
    }
    free(command);
}

//Copy un tableau de pointeurs du début à la fin et rajoute l'element NULL comme flag de fin
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

//Lit l'entrée de l'utilisation et retourne un token vers le premier mot (la commande)
char* read_input() {
    char* input;
    size_t taille = 0;
    //La taille de l'input est décidée dynamiquement via getline
    getline(&input,&taille,stdin);
    input = strtok(input,"\n");

    //Sépare chaque partie du string selon les espacements
    //Nous supposons une syntaxe avec un espace entre chaque élément
    char *split = strtok(input, " ");
    return split;
}

//Loop dans les tokens du input et crée un array de pointeurs
char** parse_input(char *input) {
    char **parsedArray = (char **) malloc(sizeof(char*)*BUFSIZ);
    int i = 0;
    while (input != NULL) {
        //Alloue de la mémoire pour les mots de la phrase
        parsedArray[i] = malloc(strlen(input) + 1);
        strcpy(parsedArray[i], input);

        //Va chercher le next token pour accéder au prochain mot
        input = getNextValue(input);
        i++;
    }
    parsedArray[i]= 0;
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
    int k=0;
    for (int j = start; j<end;j++){
        temp[k] = arrayInput[j];
        k++;
    }
    temp[k]=0;

    return temp;
}

// Lit la ligne de commande et remplace les variables par leur valeur via les $
void remplaceVariable (char **command) {
    int mot = 0;
    int lettre;

    //On loop dans les mots de la phrase
    while(command[mot] != 0) {
        lettre = 0;
        while (command[mot][lettre] != 0) {
            //Si la lettre est un $, on a une variable
            if (command[mot][lettre] == '$') {
                char * referenceOriginal;
                char *copie = malloc((strlen(command[mot]) + 1) * sizeof(char));

                //Si c'est une variable au sein d'une phrase sans espace
                if (lettre != 0) {
                    //copie tout ce qui est dans le string avant le $
                    copie = strncpy(copie, command[mot], lettre - 1);
                }

                //S'il y a d'autres variables pas séparées par des espaces
                char  *nextvar = malloc((strlen(command[mot]) + 1) * sizeof(char) );
                nextvar = strcpy(nextvar,command[mot]);
                int iterateur = 0;
                int occurence = 0;

                //Borne superieure sur le nombre d'elements
                while (nextvar[iterateur] != 0){
                    if (nextvar[iterateur] == ':') {
                        occurence = occurence + 2;
                    }

                    iterateur++;
                }

                char* varItem = strtok(nextvar, ":");
                char* nextVarItem = strtok(NULL,":");
                //On a la forme $VAR:$VAR:$VAR...
                //s'il reste des string qui avait : comme separateur
                if (nextVarItem != NULL){
                    int j = 1;
                    //tableau contiendra les valeurs des $VAR
                    char **tableauTemp = malloc((occurence + 1) * sizeof(char*));
                    int posTabTemp =0;
                    referenceOriginal = nextvar;
                    int flag = 0;
                    //Tant que existe des token (donc des variables)
                    while (varItem != NULL) {
                        //va chercher la valeur de la string à partir de
                        //la position 1 pour enlever le $
                        char *valeur = getenv(&varItem[j]);
                        tableauTemp[posTabTemp] = valeur;
                        posTabTemp++;

                        if(flag!=0){
                            varItem = strtok (NULL, ":");
                        }
                        else{
                            //si je passe pour la premiere fois
                            varItem = nextVarItem;
                            flag = 1;
                        }

                    }

                    //compteur fin du tableau
                    tableauTemp[posTabTemp]=0;

                    int i = 0;
                    char *stringConcatTemp;
                    size_t longTab = strlen(tableauTemp[0]) + 2;
                    char * stringConcat = malloc((longTab + strlen(""))* sizeof(char));

                    //string vide au début
                    stringConcat[0] = '\0';
                    char *stringTemp;


                    //On loop dans les valeurs des variables (sauf la dernière)
                    //On concatène chaque valeur de variable au :
                    //Et ensuite on remplace dans la commande initiale
                    while (tableauTemp[i]!=0 && tableauTemp[i+1]!=0) {
                        longTab = strlen(tableauTemp[i])+1;

                        char * copie1 = malloc(longTab* sizeof(char) + sizeof(char*));
                        copie1 = strcpy(copie1,tableauTemp[i]);


                        stringConcatTemp = strcat(copie1,":");
                        stringConcat = strcat(stringConcat,stringConcatTemp);

                        longTab=strlen(tableauTemp[i+1]) + strlen(stringConcat);

                        //concaténation prochaine string
                        stringTemp = malloc((longTab + 2) * sizeof(char));

                        //copie string actuelle nouvel espace mémoire
                        stringTemp = strcpy(stringTemp,stringConcat);

                        free(stringConcat);

                        stringConcat = stringTemp;

                        i++;

                        free(copie1);

                    }
                    if ((tableauTemp[i]!=0 && tableauTemp[i+1]==0)){
                        stringConcat = strcat(stringConcat,tableauTemp[i]);
                        i++;
                    }
                    free(command[mot]);
                    command[mot] = stringConcat;
                    free(referenceOriginal);

                    int longueur1 =0;

                    while (tableauTemp[longueur1]!=0){
                        longueur1++;
                    }



                }else{
                    //nextVarItem vaut NULL == pas de : dans l'input

                    //ici on obtient la valeur de ce que se trouve après $
                    char *valeur = getenv(&command[mot][lettre + 1]);
                    referenceOriginal = command[mot];

                    command[mot] = strcpy(&copie[lettre], valeur);

                    free(referenceOriginal);

                }


            }
            lettre++;
        }
        mot++;
    }

}

//Exécute l'appel via les commandes du path ou la commande spécifique cd
int execution (char** arrayInput, char** temp) {
    char cd[3] = "cd";
    int retour;
    if( strcmp(arrayInput[0],cd) == 0){
        //cd n'a qu'un seul argument
        int longeur1 =0;
        while (temp[longeur1]!=0){
            longeur1++;
        }

        if(longeur1==1){
            retour = chdir(getenv("HOME"));
        }
        else{
            retour = chdir(temp[1]);
        }
        if(retour == -1) {
            printf("numero erreur = %d\n",errno);
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

                value_returned = execvp(arrayInput[0],temp);
                exit(value_returned);
            }
        }else{
            //Le parent retourne le code de l'enfant pour valider les erreurs
            wait(&stat);
            return stat;
        }

    }
}

struct Return {
    bool format;
    int posPointVirg;
};


//Loop à partir du start jusqu'à la première
int findNextSplit(int start, char** command, char* val){
    while (strcmp(command[start], val) != 0){
        start++;
    }
    return start;
}

// Verifie la structure du for s'il y en a un
struct Return checkFor(char **command){
    char *variable = command[1];
    struct Return return1 = {false,0};

    if (!(isalpha(*variable))){
        printf("For badly made, a variable is necessary");
        return return1;
    }

    if (strcmp(command[2], "in") != 0) {
        printf("For badly made, need the format For x in range ; Do ... ; Done");
        return return1;
    }

    int posPointVirg = findNextSplit(2, command, ";");

    if (strcmp(command[posPointVirg + 1], "do") != 0) {
        printf("For badly made, need the format For x in range ; Do ... ; Done");
        return return1;
    }
    struct Return return2 = {true, posPointVirg};
    return return2;
}

// Verifie la fin d'une loop "for" */
bool instructionDone(char** command, int start){
    if (strcmp(command[start], "done") != 0){
        return false;
    }
    return true;
}

// Méthode de contrôle qui ne fait qu'appeler l'exécution de la commande
int exec (char** copie, int start, int end){
    int valeur_retour;
    char **parameters = getParameters (copie, 0, end-start);

    valeur_retour = execution(parameters,parameters);

    return valeur_retour;
}


int runFor(char **command, int pos){
    char *variable = command[1];
    int start;
    int i = 3;
    int overwrite = 1;
    int end;
    int valeur_retour;

    //Permet de looper dans le range donné
    while (i < pos) {
        //Si la loop contient des variables, on get la première pour start la loop
        if (strstr(command[i],"$")){
            char *valeur = getenv(&command[i][1]);
            command[i] = valeur;
        }

        start = pos+2; //skip le do et l'espace

        //Affecte la valeur de l'env

        setenv(variable, command[i], overwrite);

        char** copie;
        //Loop jusqu'au premier done trouvé
        while(!instructionDone(command,start)){
            end = findNextSplit(start, command, ";");

            //Vérifie si une prochaine loop est imbriquée après le do
            if (strcmp(command[end+1],"do")==0){

                //Skip le premier done pour l'imbrication
                end = findNextSplit(end+2,command,";")+2;

                //Copie uniquement le for central et l'exécute
                copie = copy(command,start,end);
                lireLigne(copie,0,end-start);
            }

            //Copie et exécute le for externe
            copie = copy(command,start,end);
            remplaceVariable(copie);

            valeur_retour = exec(copie,start,end);

            if (valeur_retour < 0) {
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
    char *var_name;

    //Évite la modification commande initiale
    copie = strcpy(copie,command[0]);
    var_name = strtok(copie, "=");

    int valeur_retour;
    //Valeur à donner à la variable
    char* valeur = strtok(NULL,"=");
    int overwrite = 1;

    //Assignation de la valeur dans l'environnement
    valeur_retour = setenv(var_name,valeur,overwrite);
    free(copie);

    return valeur_retour; //-1 si erreur
}

//S'occupe de parser la ligne selon les éléments
int lireLigne(char **command, int start, int end) {
    int i = 0;
    int valeur_retour;
    const char *pour = "for";

    //Regarde si la commande commence par un for
    if (strstr(command[start], pour)!= NULL) {
        //REgarde si le for est bien construit
        struct Return rtn = checkFor(command);
        if (rtn.format){
            //Exécute le for
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
            if(command[start][i] == '='){
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

//Retourne le type de concat et sa position via une loop dans la commande
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

//Loop dans l'array jusqu'à la valeur NULL qui indique la fin de la phrase
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
    int mot = 0;
    struct concat res;
    //Retourne la position et le type du prochain && ou ||
    res = getNextConcat(command,mot);

    //S'il existe une prochaine partie
    if (res.isNext){
        int start = 0;
        int end = res.pos;//position du && ou ||

        //Copie la section avant le && ou ||
        char** pfirst = copy(command, start, end);

        //Valeur de début et de fin
        int starts = res.pos+1;
        int ends = getLastWord(command,starts);

        //Copie la section après le && et ||
        char** psecond = copy(command, starts, ends);

        //Exécuter la première partie
        int valeurRetour1 = lireLigne(pfirst,0,end);

        //Si on a un && et que le child existe anormalement (!=0 sauf 65280 qui est 255)
        if (valeurRetour1 != 0 && valeurRetour1!= 65280 && res.type==0){
            //Désalloque la mémoire et quitte
            myFree(pfirst, end-start);
            myFree(psecond, ends-starts);
            return;
            //Si on a un || et que le child existe normalement, on sort
        }else if ((valeurRetour1==0 || valeurRetour1==65280) && res.type==1){
            return;
        }

        //Si on a un autre && ou ||, on resplit récursivement
        if (getNextConcat(psecond,0).isNext){
            splitParts(psecond);
            return;
        }

        //Exécute la deuxième partie
        int valeurRetour2 = lireLigne(psecond,0,ends-end-1);
        if (valeurRetour2 != 0) {
            myFree(pfirst, end-start);
            myFree(psecond, ends-starts);
            return;
        }


    }else{
        //S'il n'y a pas de && ou ||, on exécute la commande normalement
        int end = getLastWord(command,0);
        lireLigne(command,0,end);
    }

}


int main(void) {
    printf("Mini-Shell > ");
    char* res = read_input();
    while (strcmp(res, "exit") != 0) {
        //Parse l'entree par espace
        if(strcmp(res,"")!=0){
            char **command = parse_input(res);
            //Split l'input selon les && et ||
            splitParts(command);
            printf("\r\n Mini-Shell > ");
            res = read_input();
        } else{
            printf("\r\n Mini-Shell > ");
            res = read_input();
        }

    }
    fprintf(stdout, "Bye!\n");

    exit(0);
}
