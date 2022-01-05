#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

/*LES STRUCTURES*/

struct vecteur
{
    double *V;       //=> tableau avec les datas de iris data (5.1,3.5,1.4,0.2)
    double module;   //=> calculer la norme (normaliser) ||module||=sqrt(x1²+x2²+x3²+x4²)
    char *etiquette; //=> espece qui sont dans le fichier
};

struct node
{
    double *w;  //tableau de 4 valeurs
    double act; //distance (vide au debut)
    char *id;   //l'ID
};

struct bmu
{
    int ligne;
    int colonne;
    struct bmu *next; //BMU suivant
};

struct N_Config
{
    int nb_nodes;      //nombre de noeuds
    int nb_lignes;     //nombre de lignes
    int nb_colonnes;   //nombre de colonnes
    struct bmu *bmu;   //BMU
    struct node **map; //Matrice de noeuds
    int nbBMU;
};

/*-----------------------------FONCTIONS RANDOM----------------------------------*/

/*Retourne une valeur double aleatoire entre 2 valeurs*/

double RandDouble(double min, double max)
{
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

/*Retourne une valeur int aleatoire entre 2 valeurs*/

int Rand_int(int min, int max)
{
    int nRand;
    nRand = (rand() % (max - min + 1)) + min;
    return nRand;
}

/*-----------------------------FONCTIONS PRINCIPALES DE SOM----------------------------------*/

/*Initialisation tu tableau de structure vecteur 150 vecteurs*/

void init(struct vecteur *tableau_de_vecteurs)
{
    FILE *fichier = NULL;
    char chaine[50] = "";         //chaine pour sortir les lignes du fichier Iris.data
    const char *separators = ","; //pour determiner la séparation dans le chaine de caractere
    int i_vecteur = 0;
    int i_tableau = 0;
    fichier = fopen("iris.data", "r");

    if (fichier == NULL)
    {
        printf("Le fichier ne peut pas s'ouvrir\n");
        exit(0);
    }

    if (fichier != NULL)
    {
        while (fgets(chaine, 50, fichier)) //recupere la chaine de caractere depuis iris.data
        {
            tableau_de_vecteurs[i_vecteur].V = (double *)malloc(4 * sizeof(double));
            tableau_de_vecteurs[i_vecteur].etiquette = malloc(20 * sizeof(char));
            char *strToken = strtok(chaine, separators);
            while (strToken != NULL)
            {
                if (i_tableau < 4)
                {
                    tableau_de_vecteurs[i_vecteur].V[i_tableau] = atof(strToken);
                    i_tableau++;
                }
                else
                {
                    strcpy(tableau_de_vecteurs[i_vecteur].etiquette, strToken);
                    tableau_de_vecteurs[i_vecteur].etiquette[strlen(tableau_de_vecteurs[i_vecteur].etiquette) - 1] = '\0';
                }
                strToken = strtok(NULL, separators);
            }
            tableau_de_vecteurs[i_vecteur].module = sqrt(pow(tableau_de_vecteurs[i_vecteur].V[0], 2) + pow(tableau_de_vecteurs[i_vecteur].V[1], 2) + pow(tableau_de_vecteurs[i_vecteur].V[2], 2) + pow(tableau_de_vecteurs[i_vecteur].V[3], 2));
            i_tableau = 0;
            i_vecteur++;
        }
        fclose(fichier);
    }
}

/*Normalise les datas*/

void normaliser(struct vecteur *tableau_de_vecteurs)
{
    for (int i = 0; i < 150; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            tableau_de_vecteurs[i].V[j] = tableau_de_vecteurs[i].V[j] / tableau_de_vecteurs[i].module;
        }
    }
}

/*Melange les datas*/

void suffle(struct vecteur *tableau_de_vecteurs)
{
    srand(time(0)); //set une nouvelle seed pour le random
    struct vecteur vecteur_tmp;
    int num = 0;
    for (int i = 0; i < 150; i++)
    {
        num = (rand() % (149 - 0 + 1)) + 0;     //genere un nombre aléatoire si sera la futur position du vecteur courant
        vecteur_tmp = tableau_de_vecteurs[num]; //changement de place des 2 vecteurs
        tableau_de_vecteurs[num] = tableau_de_vecteurs[i];
        tableau_de_vecteurs[i] = vecteur_tmp;
    }
}

/*Calcule les moyennes en renvoyant un tableau de 4 valeurs qui corresponds au moyenne de chaque colonne*/

double *moyennes_col(struct vecteur *tableau_de_vecteurs)
{
    double *tab = (double *)calloc(4, sizeof(double));
    for (int j = 0; j < 4; j++)
    {
        for (int i = 0; i < 150; i++)
        {
            tab[j] += tableau_de_vecteurs[i].V[j];
        }
        tab[j] = tab[j] / 150;
    }
    return tab;
}

/*Configure le reseau, set les infos dans la structure qui est en parametre, genere une matrice avec allocation memoire de node, et remplis les nodes avec la moyenne des colonnes*/

void config_reseau(struct N_Config *Reseau, double *tab)
{
    Reseau->nb_lignes = 10;                                     //nombre de lignes
    Reseau->nb_colonnes = 6;                                    //nombre de colonnes
    Reseau->nb_nodes = Reseau->nb_lignes * Reseau->nb_colonnes; //set le nombre de node
    Reseau->nbBMU = 0;

    struct node **matrice = malloc(Reseau->nb_lignes * sizeof(struct node *));

    for (int i = 0; i < Reseau->nb_lignes; i++)
    {
        matrice[i] = malloc(Reseau->nb_colonnes * sizeof(struct node));
    }

    for (int i = 0; i < Reseau->nb_lignes; i++)
    {

        for (int j = 0; j < Reseau->nb_colonnes; j++)
        {
            matrice[i][j].w = (double *)malloc(4 * sizeof(double)); //allocation du tableau de valeur
            matrice[i][j].id = malloc(sizeof(char));

            for (int k = 0; k < 4; k++)
            {
                matrice[i][j].w[k] = RandDouble((tab[k] - 0.05), (tab[k] + 0.05)); //genere une valeur aléatoire et la met dans le tableau du node
            }

            matrice[i][j].act = 10; //set la distance
            matrice[i][j].id = "*"; //set l'id
        }
    }

    Reseau->map = matrice; //met la matrice dans la structure du reseau
}

/*Recherche du BMU en fonction du vecteur courant*/

void cherche_BMU(struct N_Config *Reseau, struct vecteur *tableau_de_vecteurs, int id_vecteur)

{
    struct bmu *cell, *tete;
    cell = (struct bmu *)malloc(sizeof(struct bmu));
    tete = cell;
    cell->colonne = 0; //init un BMU initiale en 0,0
    cell->ligne = 0;
    Reseau->bmu = tete;
    Reseau->nbBMU = 1;
    for (int k = 0; k < Reseau->nb_lignes; k++)
    {
        for (int u = 0; u < Reseau->nb_colonnes; u++)
        {
            Reseau->map[k][u].act = 0;
            for (int i = 0; i < 4; i++) //calcule la distance
            {
                Reseau->map[k][u].act += pow(tableau_de_vecteurs[id_vecteur].V[i] - Reseau->map[k][u].w[i], 2);
            }
            Reseau->map[k][u].act = sqrt(Reseau->map[k][u].act);

            if (Reseau->map[k][u].act <= Reseau->map[cell->ligne][cell->colonne].act)
            {
                if ((Reseau->map[k][u].act == Reseau->map[cell->ligne][cell->colonne].act)) //si la distance est egale au bmu actuel, ajouter dans la liste chaine
                {
                    cell->next = (struct bmu *)malloc(sizeof(struct bmu));
                    cell->next->ligne = k;
                    cell->next->colonne = u;
                    cell->next->next = NULL;
                    cell = cell->next;
                    Reseau->nbBMU++;
                }
                if ((Reseau->map[k][u].act < Reseau->map[cell->ligne][cell->colonne].act)) //si plus petit, fait une nouvelle liste chaine
                {
                    cell = (struct bmu *)malloc(sizeof(struct bmu));
                    tete = cell;
                    cell->ligne = k;
                    cell->colonne = u;
                    Reseau->bmu = tete;
                    Reseau->nbBMU = 1;
                }
            }
        }
    }
}

/*Fonction qui sort un BMU aléatoire dans une liste chainée*/

struct bmu *bmu_aleatoire(struct N_Config *Reseau)
{
    struct bmu *tmp = Reseau->bmu;
    for (int i = 1; i < Rand_int(1,Reseau->nbBMU); i++) //for qui s'arrete au nombre random généré avec le nombre de bmu
    {
        tmp = tmp->next;
    }
    tmp->next = NULL;
    return tmp;
}

/*Fonction qui fait la propagation du voisinage*/

void voisinage(struct N_Config *Reseau, struct vecteur Vecteur, double alpha, int degre)
{

    struct bmu *bmuSelectionne;

    bmuSelectionne = bmu_aleatoire(Reseau);

    /*Set un id pour le BMU en fonction du vecteur courant*/

    if (strcmp(Vecteur.etiquette, "Iris-setosa") == 0)
    {
        Reseau->map[bmuSelectionne->ligne][bmuSelectionne->colonne].id = "S";
    }
    if (strcmp(Vecteur.etiquette, "Iris-versicolor") == 0)
    {
        Reseau->map[bmuSelectionne->ligne][bmuSelectionne->colonne].id = "E";
    }
    if (strcmp(Vecteur.etiquette, "Iris-virginica") == 0)
    {
        Reseau->map[bmuSelectionne->ligne][bmuSelectionne->colonne].id = "I";
    }

    for (int i = bmuSelectionne->ligne - degre; i <= bmuSelectionne->ligne + degre; i++) //parcours le voisinage au tour du BMU
    {
        for (int j = bmuSelectionne->colonne - degre; j <= bmuSelectionne->colonne + degre; j++)
        {
            if (((i >= 0) & (i <= Reseau->nb_lignes - 1)) & ((j >= 0) & (j <= Reseau->nb_colonnes - 1))) //Si les id rentre dans a matrice
            {
                for (int u = 0; u < 4; u++) //change la valeur des poids
                {
                    Reseau->map[i][j].w[u] = Reseau->map[i][j].w[u] + alpha * (Vecteur.V[u] - Reseau->map[i][j].w[u]);
                }
            }
        }
    }
}

void apprentissage(struct N_Config *Reseau, struct vecteur *tableau_de_vecteurs)

{
    int Ttotal = 500;
    double alphaInit = 0.7;
    double alpha = 0;
    int i = 0;
    int degre = 3;
    int val_chang_deg = Ttotal / degre;
    int interation = 0;

    while (i < 2000)
    {
        if ((interation == val_chang_deg) & (degre > 1))//baisse la taille du degre
        {
            degre = degre - 1;
        }
        if (i == 500)//change de phase
        {
            interation = 0;
            Ttotal = 1500;
            alphaInit = 0.07;
        }
        alpha = alphaInit * (1 - (double)interation / Ttotal);
        suffle(tableau_de_vecteurs);
        for (int j = 0; j < 150; j++) //parcoures tableau de vecteurs
        {
            cherche_BMU(Reseau, tableau_de_vecteurs, j); //cherche le bmu avec le vecteur[j]
            voisinage(Reseau, tableau_de_vecteurs[j], alpha, degre);
        }
        i++;
        interation++;
    }
}

/*-----------------------------FONCTIONS DE LIBERATION DE LA MEMOIRE----------------------------------*/

/*Libere la mémoire alloué pour le tableau de vecteurs*/

void liberer_le_tableau(struct vecteur *tableau_de_vecteurs)
{
    for (int i = 0; i < 150; i++)
    {
        free(tableau_de_vecteurs[i].V);
        free(tableau_de_vecteurs[i].etiquette);
    }
    printf("Memoire utilise par le tableau de vecteurs libere\n");
}

/*Libere l'espace memoire utilisé par la matrice de node*/

void liberer_la_matrice(struct N_Config *Reseau)
{
    for (int i = 0; i < Reseau->nb_lignes; i++)
    {
        for (int j = 0; j < Reseau->nb_colonnes; j++)
        {
            free(Reseau->map[i][j].w);
        }
        free(Reseau->map[i]);
    }

    free(Reseau->map);

    printf("Memoire utilise par le reseau de nodes libere\n");
}

/*-----------------------------FONCTIONS D'AFFICHAGE----------------------------------*/

/*Affichage du tableau de vecteurs*/

void affiche_tableau_vecteur(struct vecteur *tableau_de_vecteurs)
{
    for (int i = 0; i < 150; i++)
    {
        printf("Vecteur %d | Module : %f | Espece : %s | Tableau ", i, tableau_de_vecteurs[i].module, tableau_de_vecteurs[i].etiquette);
        for (int j = 0; j < 4; j++)
        {
            printf("%f | ", tableau_de_vecteurs[i].V[j]);
        }
        printf("\n");
    }
}

/*Fonction d'affichage des bmu*/

void affiche_bmu(struct N_Config *Reseau)
{
    struct bmu *actuel = Reseau->bmu;
    while (actuel != NULL)
    {
        printf("BMU | ID : %s | Ligne : %d | Colonne : %d | Distance : %f \n", Reseau->map[actuel->ligne][actuel->colonne].id, actuel->ligne, actuel->colonne, Reseau->map[actuel->ligne][actuel->colonne].act);
        actuel = actuel->next;
    }
}

/*Affichage de la matrice de node*/

void afficheMatriceNode(struct N_Config *Reseau)
{
    printf("\033[0;31m");
    printf("I: Iris-virginica  ");
    printf("\033[0;32m");
    printf("E: Iris-versicolor  ");
    printf("\033[0;33m");
    printf("S: Iris-setosa\n");
    printf("\033[0m");
    for (int i = 0; i < Reseau->nb_lignes; i++)
    {
        for (int j = 0; j < Reseau->nb_colonnes; j++)
        {
            if (!strncmp(Reseau->map[i][j].id, "I", 1))
            {
                printf("\033[0;31m");
            }

            if (!strncmp(Reseau->map[i][j].id, "E", 1))
            {
                printf("\033[0;32m");
            }

            if (!strncmp(Reseau->map[i][j].id, "S", 1))
            {
                printf("\033[0;33m");
            }

            printf("%s", Reseau->map[i][j].id);
            printf("\033[0m");
            printf("\t| ");
        }
        printf("\n");
    }
}

/*-----------------------------MAIN----------------------------------*/

int main()

{

    struct vecteur tableau_de_vecteurs[150];

    double *tabMoyenne = (double *)malloc(4 * sizeof(double)); // Init en mémoire le tableau de moyenne

    struct N_Config *mon_reseau = malloc(sizeof(struct N_Config)); // Init en mémoire le reseau

    init(tableau_de_vecteurs);       //Recupere les valeurs de iris.data
    normaliser(tableau_de_vecteurs); //Normalise ces valeurs
    suffle(tableau_de_vecteurs);     //Melanger ces valeurs

    tabMoyenne = moyennes_col(tableau_de_vecteurs); //Fait la moyenne des colonnes

    config_reseau(mon_reseau, tabMoyenne); //Configure le reseau de node

    apprentissage(mon_reseau, tableau_de_vecteurs); //Fonction d'apprentissage

    afficheMatriceNode(mon_reseau);

    liberer_la_matrice(mon_reseau);
    liberer_le_tableau(tableau_de_vecteurs);

    return 0;
}