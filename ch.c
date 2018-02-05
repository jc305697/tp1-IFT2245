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
struct returnRead
{
    char *split;
    char *input;
};
struct returnRead read_input() {
    char* input;
    input = (char *) malloc(200 * sizeof(char));

    //Crédit G Praveen Kumar pour le scanf avec espacement
    scanf(" %[^\n]s", input);
    char *copieInput =malloc(strlen(input) * sizeof(char));
    copieInput = strcpy(copieInput,input);
    //getline(&input,&taille,stdin);//TODO: remplacer scanf par getline
    //Sépare chaque partie du string
    char *split;
    split = strtok(input, " ");
    struct returnRead retour = {split,copieInput};
   // return split;
    return retour;
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

int execution (char** arrayInput, char** temp)
{

    char cd[3] = "cd";
    if( strcmp(arrayInput[0],cd) == 0){
        //cd n'a qu'un seul argument
        int retour = chdir(temp[1]);
        if(retour == -1)
        {
            printf("%d",errno);
            printf("erreur 1 = %s \n",strerror(errno));
        }
        return retour;
    }
    else {
        int value_returned=0;
        pid_t pid = fork();
        if (pid ==0){
            if (temp[0] != NULL){
                //Pour ls et cat et compagnie
                value_returned = execvp(arrayInput[0],temp);
            }
            else{
                //Pour les déclarations de variables
                value_returned = execlp(arrayInput[0],arrayInput[0],0);
            }

        }
        else{
            wait(NULL);
        }

        if (value_returned == -1) {
            //TODO : Cette commande bug la quatrieme fois for i in 1 2 3 ; do ls ; done
            //printf("erreur 2 = %s\n",strerror(errno));
        }
        return value_returned;
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
    int valeur_retour;
    //Boucle pour le range de i


    while (i < pos) {
        start = pos+2;
        setenv(variable, command[i], overwrite);
        //Boucle pour toutes les exécutions séparée par un ;
       /* char **copie = malloc (strlen(*command)* sizeof(char*));
        for(int j =0; j< strlen(*command);j++)
        {
            copie[j] = strcpy(copie[j],command[j]);
        }*/

        while(!instructionDone(command,start)){
            end = findNextSplit(start, command);

            remplaceVariable(command);
            //remplaceVariable(copie);

            char **parameters = getParameters (command, start, end);

             valeur_retour = execution(parameters,parameters);
           /* for(int j =0; j< strlen(*command);j++)
            {
               free( copie[j] );
            }

            free(copie);*/
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
    char *variable;
    copie = strcpy(copie,command[0]);
    variable = strtok(copie, "=");
    char *valeur;
    int valeur_retour;
    valeur = strtok(NULL,"="); // va chercher la premiere partie
    int overwrite = 1;
    valeur_retour =setenv(variable,valeur,overwrite);
    return valeur_retour;
}

int lireLigne(char **command,  char *input)
{

    int i = 0;
    int valeur_retour;
    const char *pour = "for";
    const char *et = "&&";
    const char *ou = "||";
    //Check if "for" is the command
    int mot = 0;
    int lettre = 0;

    while(command[mot] != NULL)
    {


        char *resultat = strstr(command[mot], et);
        char *resultatOu = strstr(command[mot], ou);
        printf(" command[mot]= %s, mot = %d et command length = %zu, resultat= %s,resultatOu= %s\n", command[mot],mot,strlen(*command),resultat,resultatOu);

        if (resultat != NULL) {//s'il y a un &&
            char *premierePartie;
            premierePartie = malloc((resultat - command[mot]) * sizeof(char));//partie avant le &&

            premierePartie = strncpy(premierePartie, command[mot], resultat - command[mot]);
            char *deuximePartie = malloc(strlen(resultat+1)* sizeof(char));//partie apres le &&
            deuximePartie = strcpy(deuximePartie,resultat);

            char *resultat2 = strstr(input, et);//cherche le && dans l'input
            char *premierePartieInput;
            premierePartieInput = malloc((resultat2 - input) * sizeof(char));
            //partie avant le && dans l'input

            premierePartieInput = strncpy(premierePartieInput, input, resultat2 - input);
            char *deuximePartieInput = malloc(strlen(resultat+1)* sizeof(char));
            //partie apres le && dans l'output
            deuximePartieInput = strcpy(deuximePartieInput,resultat2);

            int valeurRetour1;
            int valeurRetour2;
            //
            valeurRetour1 = lireLigne(&premierePartie,premierePartieInput);
            if (valeurRetour1 < 0)//si la premiere partie a une erreur
            {


                printf("%s:%s:%s\n",command[0],command[1],strerror(errno));

                free(premierePartie);
                free(deuximePartie);
                free(premierePartieInput);
                free(deuximePartieInput);

                return valeurRetour1; //je n'execute pas la 2e partie
            }

            valeurRetour2 = lireLigne(&deuximePartie,deuximePartieInput);
            if (valeurRetour2 < 0)
            {

                printf("%s:%s:%s\n",&(resultat + 1)[0],&(resultat + 1)[1],strerror(errno));
                free(premierePartie);
                free(deuximePartie);
                free(premierePartieInput);
                free(deuximePartieInput);
                return valeurRetour2;//je retourne la 2e partie
            }

            return 0;

        }

        if(resultatOu != NULL)//s'il y a un ou
        {
            char *premierePartie; //partie avant le ou
            premierePartie = malloc((resultatOu - command[mot]) * sizeof(char));

            premierePartie = strncpy(premierePartie, command[mot], resultatOu - command[mot]);
            char *deuximePartie = malloc(strlen(resultatOu+1)* sizeof(char));//partie apres le ou
            deuximePartie = strcpy(deuximePartie,resultatOu);

            char *resultatOu2 = strstr(input, ou);//cherche le ou dans l'input
            char *premierePartieInput; //partie avant le ou dans l'input
            premierePartieInput = malloc((resultatOu2 - input) * sizeof(char));

            premierePartieInput = strncpy(premierePartieInput, input, resultatOu2 - input);
            char *deuximePartieInput = malloc(strlen(resultatOu+1)* sizeof(char));//partie apres le ou dans l'input
            deuximePartieInput = strcpy(deuximePartieInput,resultatOu2);

            int valeurRetour1;
            int valeurRetour2;

            valeurRetour1 = lireLigne(&premierePartie,premierePartieInput);
            if (valeurRetour1 >= 0)//si succes dans la premiere partie
            {

                free(premierePartie);
                free(deuximePartie);
                free(premierePartieInput);
                free(deuximePartieInput);
                return valeurRetour1;//pas besoin d'executer la 2e partie
            }

            printf("%s:%s:%s\n",command[0],command[1],strerror(errno));
            //echec premiere partie(commande)

            valeurRetour2 = lireLigne(&deuximePartie,deuximePartieInput);


            if (valeurRetour2 >= 0)//si succes dans la premiere partie
            {
                free(premierePartie);
                free(deuximePartie);
                free(premierePartieInput);
                free(deuximePartieInput);
                return valeurRetour2;//pas besoin d'executer la 2e partie
            }

            return -1;//puisque j'arrive ici si echec dans les 2 parties
        }
        mot++;
    }

    if (strstr(command[0], pour)!= NULL) {
        //Check if for is well made
        struct Return rtn = checkFor(command);
        if (rtn.format){
            valeur_retour = runFor(command,rtn.posPointVirg);
            return valeur_retour;

        }else{
            return -1;
        }
    }

    else{
        bool passer = false;

        //Lit la ligne et remplace les $ par leur valeur
        remplaceVariable(command);

        //Vérifie si la ligne est une assignation
        while(command[0][i]!= 0) {
            if(command[0][i] == '='){//si assignation en premier
                valeur_retour = runAssignation(command);
                passer=true;
                return valeur_retour;
            }
            i++;
        }

        //On a une commande
        if (passer == false){
            char **parameters = getParameters (command, 0, sizeof(command));
            valeur_retour = execution(command,parameters);
            return valeur_retour;
        }
        return -1;
    }

}



int main(void) {
    printf("Mini-Shell > ");
    char input[200];
    //printf("%s",strerror(11));
  //  setenv("i","1",1);//pour tester si marche avec $ TODO: enlever le test
    while (strcmp(input, "exit") != 0) {
            //char *result = read_input();
            struct returnRead retour = read_input();
            char *result = retour.split;
            char *copieInput = retour.input;
            char **command = parse_input(result);
           // printf("input = %s",input);
          //  lireLigne(command,input);
            lireLigne(command,copieInput);
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