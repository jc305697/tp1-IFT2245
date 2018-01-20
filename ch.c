/* ch.c.
auteur: Maude Sabourin et Jeremy Coulombe
date: 20 Janvier 2018
problèmes connus: a date aucun

  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main (void)
{
    fprintf (stdout, "%% ");

    /* ¡REMPLIR-ICI! : Lire les commandes de l'utilisateur et les exécuter.   */

    printf("entrer votre commande, pour quitter entrer exit");
    char entrer[200];
    scanf( entrer, 200);//200 serait supposer etre la taille d entrer maximmum
    while (strcmp(entrer,"exit") != 0)
    {


        //switch

        printf("nouvelle commande");  //
        scanf(entrer, "%200");
    }

    fprintf (stdout, "Bye!\n");
    exit (0);
}
