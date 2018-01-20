/* ch.c.
auteur: Maude Sabourin et Jeremy Coulombe
date: 20 Janvier 2018
problèmes connus: a date aucun

  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main (void)
{
    fprintf (stdout, "%% ");

    /* ¡REMPLIR-ICI! : Lire les commandes de l'utilisateur et les exécuter.   */

    printf("entrer votre commande, pour quitter entrer exit");
    char entrer[200];
    //char commande[100];
    scanf( entrer, 200);//200 serait supposer etre la taille d entrer maximmum
   // while (strcmp(commande,"exit") != 0)
    while (strcmp(entrer,"exit") != 0)

    {
        if(entrer[0] == '.')
        {

        }

        /*switch(commande)
         * {
         *      case "cd":
         *                  int retour = chdir(parametre);
         *                  if(retour == -1)
         *                  {
         *                      printf("%s",errno);
         *                  }
         *     case default: //lit la variable path du système puis va les parcourir
         *                     // potentiellement avec scandir
         *
         *
         * }*/

        printf("nouvelle commande");  //
//        scanf( " % 100 %200",commande,entrer );
        scanf( "%200",entrer );
    }

    fprintf (stdout, "Bye!\n");
    exit (0);
}
