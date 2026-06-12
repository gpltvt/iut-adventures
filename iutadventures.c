/* IUT ADVENTURES
Autrice: PLAETEVOET Gaëlle
Jeu textuel à choix multiples - Projet BUT1 (2025-2026)
COMPILATION: gcc iutadventures.c -o iutadventures
EXÉCUTION: .\iutadventures.exe */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
    #include <windows.h>
#endif

/* Liste de toutes les scènes possibles du jeu. 
Les valeurs NOTOK correspondent aux scène pénalisantes. */
typedef enum
{
    SCENE_1,
    SCENE_2,
    SCENE_3,
    SCENE_4,
    SCENE_4_NOTOK,
    SCENE_5,
    SCENE_5_NOTOK,
    SCENE_6,
    SCENE_7,
    SCENE_8,
    SCENE_8_NOTOK,
    SCENE_9,
    SCENE_9_NOTOK,
    SCENE_10,
    SCENE_10_NOTOK,
    SCENE_11,
    SCENE_11_NOTOK,
    SCENE_12,
    SCENE_12_NOTOK,
    SCENE_13,
    SCENE_13_NOTOK,
    SCENE_14,
    SCENE_14_NOTOK,
    SCENE_15,
    SCENE_15_NOTOK,
    SCENE_16,
    SCENE_16_NOTOK,
    SCENE_17,
    SCENE_17_NOTOK,
    SCENE_18,
    SCENE_18_NOTOK,
    SCENE_19,
    SCENE_19_NOTOK,
    SCENE_20,
    SCENE_20_NOTOK,
    SCENE_21,
    SCENE_21_NOTOK,
    SCENE_22,
    SCENE_22_NOTOK,
    SCENE_23,
    SCENE_24,
    SCENE_24_NOTOK,
    SCENE_25,
    SCENE_25_NOTOK,
    SCENE_26,
    SCENE_26_NOTOK,
    SCENE_27,
    SCENE_27_NOTOK,
    SCENE_28,
    SCENE_28_NOTOK,
    SCENE_29,
    SCENE_29_NOTOK,
    GAME_OVER1,
    GAME_OVER2,
    GAME_OVER3,
    GAME_OVER4,
    GAME_OVER5,
    GAME_OVER6,
    GAME_OVER7,
    GAME_OVER8,
    VICTORY,
    QUIT
} SceneType;

// Personnage jouable
typedef struct
{
    int id;
    char folder[50];
    char nom[50];
} Personnage;

// Définition d'une transition
typedef struct
{
    SceneType scene_actuelle;
    int choix;
    SceneType scene_suivante;
    int fin_jeu;
} Transition;

// Textes de l'interface selon la langue choisie
typedef struct
{
    const char *welcome_title;
    const char *press_enter;
    const char *select_title;
    const char *choice_prompt;
    const char *unknown_choice;
    const char *goodbye;
    const char *thanks;
    const char *selected_msg;
} GameTexts;

// Textes de l'interface en français
const GameTexts text_fr = {
    "      BIENVENUE DANS LE JEU",
    "Appuie sur Entrée pour commencer...",
    "    SELECTION DU PERSONNAGE",
    "Ton choix (ou 0 pour quitter et m pour la map): ",
    "Choix non reconnu...",
    "Au revoir !",
    "      MERCI D'AVOIR JOUE !",
    "Tu as choisi : "};

// Textes de l'interface en anglais
const GameTexts text_en = {
    "      WELCOME TO THE GAME",
    "Press Enter to start...",
    "    CHARACTER SELECTION",
    "Your choice (or 0 to quit or m for the map): ",
    "Unknown choice...",
    "Goodbye!",
    "      THANKS FOR PLAYING!",
    "You chose: "};

// Barre de progression selon la scène actuelle et le total de scènes du personnage
void display_progress(int current, int total, const char *lang)
{
    if (current > total)
        current = total;
    float percent = ((float)current / (float)total) * 100;

    const char *label = (strcmp(lang, "fr") == 0) ? "Progression" : "Progress";

    printf("\n%s : [", label);
    int pos = (int)(percent / 5); 
    for (int i = 0; i < 20; i++)
    {
        if (i < pos)
            printf("=");
        else
            printf(" ");
    }
    printf("] %.0f%%\n", percent);
}

/* Déclenchement aléatoire d'une question surprise
En cas de mauvaise réponse, le joueur est renvoyé à la scène 1*/
SceneType trigger_random_event(const char *lang, SceneType current)
{
    int chance = rand() % 20;

    if (chance == 1)
    {
        char input[100];
        int reponse = 0;

        printf("\n************************************************************\n");
        if (strcmp(lang, "fr") == 0)
        {
            printf("EVENEMENT ALEATOIRE : Un prof de com' vous attrape !\n");
            printf("\"Vite ! Lequel de ces élèments n'est PAS un obstacle à la communication ?\"\n");
            printf("1. Le bruit environnant\n2. L'empathie\n3. Le manque de feedback\n");
            printf("Votre réponse : ");
        }
        else
        {
            printf("RANDOM EVENT: A communication teacher stops you!\n");
            printf("\"Quick! Which of these is NOT a barrier to communication?\"\n");
            printf("1. Physical noise\n2. Empathy\n3. Lack of feedback\n");
            printf("Your answer: ");
        }

        if (fgets(input, sizeof(input), stdin) != NULL)
        {
            sscanf(input, "%d", &reponse);
        }

        if (reponse != 2)
        { 
            if (strcmp(lang, "fr") == 0)
                printf("\n\"Faux ! Vous êtes tout embrouille, vous vous perdez dans les couloirs...\"\n");
            else
                printf("\n\"Wrong! You are all confused, you get lost in the hallways...\"\n");

            SceneType punition[] = {SCENE_1};
            return SCENE_1;
        }
        else
        {
            if (strcmp(lang, "fr") == 0)
                printf("\n\"Exact ! Continuez votre chemin.\"\n");
            else
                printf("\n\"Correct! Keep going on your way.\"\n");
        }
        printf("Appuie sur Entree pour continuer...");
        fgets(input, sizeof(input), stdin);
    }
    return current; 
}

/* Charge et affiche le contenu d'un fichier texte correspondant à une scène.
Efface le terminal avant l'affichage. */
void load_scene(const char *folder, const char *filename)
{
    char filepath[256];

#ifdef _WIN32
    sprintf(filepath, "%s\\%s", folder, filename);
    system("cls");
#else
    sprintf(filepath, "%s/%s", folder, filename);
    system("clear");
#endif

    FILE *in_file = fopen(filepath, "r");
    if (in_file == NULL)
    {
        printf("ERREUR: Fichier introuvable -> %s\n", filepath);
        printf("Appuie sur Entrée/Press Enter...");
        getchar();
        return;
    }

    char line[500];
    while (fgets(line, sizeof(line), in_file) != NULL)
    {
        printf("%s", line);
    }
    printf("\n");
    fclose(in_file);
}

// Retourne le nom du fichier .txt associé à une scène
const char *get_scene_filename(SceneType scene)
{
    static const char *noms[] = {
        [SCENE_1]        = "scene1.txt",
        [SCENE_2]        = "scene2.txt",
        [SCENE_3]        = "scene3.txt",
        [SCENE_4]        = "scene4.txt",
        [SCENE_4_NOTOK]  = "scene4notok.txt",
        [SCENE_5]        = "scene5.txt",
        [SCENE_5_NOTOK]  = "scene5notok.txt",
        [SCENE_6]        = "scene6.txt",
        [SCENE_7]        = "scene7.txt",
        [SCENE_8]        = "scene8.txt",
        [SCENE_8_NOTOK]  = "scene8notok.txt",
        [SCENE_9]        = "scene9.txt",
        [SCENE_9_NOTOK]  = "scene9notok.txt",
        [SCENE_10]       = "scene10.txt",
        [SCENE_10_NOTOK] = "scene10notok.txt",
        [SCENE_11]       = "scene11.txt",
        [SCENE_11_NOTOK] = "scene11notok.txt",
        [SCENE_12]       = "scene12.txt",
        [SCENE_12_NOTOK] = "scene12notok.txt",
        [SCENE_13]       = "scene13.txt",
        [SCENE_13_NOTOK] = "scene13notok.txt",
        [SCENE_14]       = "scene14.txt",
        [SCENE_14_NOTOK] = "scene14notok.txt",
        [SCENE_15]       = "scene15.txt",
        [SCENE_15_NOTOK] = "scene15notok.txt",
        [SCENE_16]       = "scene16.txt",
        [SCENE_16_NOTOK] = "scene16notok.txt",
        [SCENE_17]       = "scene17.txt",
        [SCENE_17_NOTOK] = "scene17notok.txt",
        [SCENE_18]       = "scene18.txt",
        [SCENE_18_NOTOK] = "scene18notok.txt",
        [SCENE_19]       = "scene19.txt",
        [SCENE_19_NOTOK] = "scene19notok.txt",
        [SCENE_20]       = "scene20.txt",
        [SCENE_20_NOTOK] = "scene20notok.txt",
        [SCENE_21]       = "scene21.txt",
        [SCENE_21_NOTOK] = "scene21notok.txt",
        [SCENE_22]       = "scene22.txt",
        [SCENE_22_NOTOK] = "scene22notok.txt",
        [SCENE_23]       = "scene23.txt",
        [SCENE_24]       = "scene24.txt",
        [SCENE_24_NOTOK] = "scene24notok.txt",
        [SCENE_25]       = "scene25.txt",
        [SCENE_25_NOTOK] = "scene25notok.txt",
        [SCENE_26]       = "scene26.txt",
        [SCENE_26_NOTOK] = "scene26notok.txt",
        [SCENE_27]       = "scene27.txt",
        [SCENE_27_NOTOK] = "scene27notok.txt",
        [SCENE_28]       = "scene28.txt",
        [SCENE_28_NOTOK] = "scene28notok.txt",
        [SCENE_29]       = "scene29.txt",
        [SCENE_29_NOTOK] = "scene29notok.txt",
        [GAME_OVER1]     = "gameover1.txt",
        [GAME_OVER2]     = "gameover2.txt",
        [GAME_OVER3]     = "gameover3.txt",
        [GAME_OVER4]     = "gameover4.txt",
        [GAME_OVER5]     = "gameover5.txt",
        [GAME_OVER6]     = "gameover6.txt",
        [GAME_OVER7]     = "gameover7.txt",
        [GAME_OVER8]     = "gameover8.txt",
        [VICTORY]        = "victory.txt",
    };

    if (scene >= 0 && scene < QUIT)
        return noms[scene];

    return NULL;
}

// Affiche le menu de sélection de langue et stocke le code choisi (fr/en)
void select_language(char *lang_code)
{
    int choix = 0;
    char buffer[100];

    while (choix != 1 && choix != 2)
    {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        printf("      LANGUAGE / LANGUE\n");
        printf("1. Francais\n");
        printf("2. English\n\n");
        printf("Choice: ");

        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            if (sscanf(buffer, "%d", &choix) != 1)
            {
                choix = 0;
            }
        }
    }
    if (choix == 1)
        strcpy(lang_code, "fr");
    else
        strcpy(lang_code, "en");
}

// Affiche le menu de sélection du personnage et retourne le personnage choisi.
Personnage select_personnage(const char *lang, const GameTexts *txt)
{
    Personnage perso;
    char choix;
    char menu_path[100];
    char buffer[100];
    int valid = 0;

#ifdef _WIN32
    sprintf(menu_path, "menu\\%s", lang);
#else
    sprintf(menu_path, "menu/%s", lang);
#endif

    do
    {
        printf("%s\n", txt->select_title);

        load_scene(menu_path, "menupersonnage.txt");

        printf("\n[?] ");

        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            choix = buffer[0];
        }
        else
        {
            choix = 'z';
        }

        switch (choix)
        {
        case 'm':
        case 'M':
            perso.id = 1;
            strcpy(perso.folder, "melec");
            strcpy(perso.nom, "Mme MELEC");
            valid = 1;
            break;
        case 'd':
        case 'D':
            perso.id = 2;
            strcpy(perso.folder, "ladauphine");
            strcpy(perso.nom, "Mme La DAUPHINE");
            valid = 1;
            break;
        case 't':
        case 'T':
            perso.id = 3;
            strcpy(perso.folder, "captain");
            strcpy(perso.nom, "Mr CAPTAIN");
            valid = 1;
            break;
        case 'l':
        case 'L':
            perso.id = 4;
            strcpy(perso.folder, "lettres");
            strcpy(perso.nom, "Mme LETTRES");
            valid = 1;
            break;
        case 'a':
        case 'A':
            perso.id = 5;
            strcpy(perso.folder, "athe");
            strcpy(perso.nom, "Mr ATHE");
            valid = 1;
            break;
        case 'c':
        case 'C':
            perso.id = 6;
            strcpy(perso.folder, "cocot");
            strcpy(perso.nom, "Mr COCOT");
            valid = 1;
            break;
        default:
            printf("\n>>> %s <<<\n", txt->unknown_choice);
            printf("Appuie sur Entrée pour reessayer...");
            fgets(buffer, sizeof(buffer), stdin);

#ifdef _WIN32
            system("cls");
#else
            system("clear");
#endif
            valid = 0;
            break;
        }
    } while (!valid);

    printf("\n%s %s\n", txt->selected_msg, perso.nom);
    printf("%s", txt->press_enter);
    fgets(buffer, sizeof(buffer), stdin);

    return perso;
}

/* Parcourt du tableau de transitions pour trouver la scène suivante */
SceneType get_next_scene(SceneType current, int choix, Transition *transitions, int nb_transitions, int *fin)
{
    for (int i = 0; i < nb_transitions; i++)
    {
        if (transitions[i].scene_actuelle == current && transitions[i].choix == choix)
        {
            *fin = transitions[i].fin_jeu;
            return transitions[i].scene_suivante;
        }
    }
    *fin = 0;
    return current;
}

// Initialisation
int main(int argc, char *argv[])
{
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
    srand(time(NULL));

    SceneType current_scene = SCENE_1;
    int choix;
    int game_running = 1;
    int choix_invalide = 0;
    char lang[5];
    char final_path[200];
    const GameTexts *txt;
    char input_buffer[100];
    int max_scenes = 27; 

    // Chargement du logo et sélection de la langue
    load_scene("menu", "logo.md");
    printf("\nAppuie sur Entrée pour commencer...");
    fgets(input_buffer, sizeof(input_buffer), stdin);
    select_language(lang);
    if (strcmp(lang, "fr") == 0)
    {
        txt = &text_fr;
    }
    else
    {
        txt = &text_en;
    }
    printf("%s\n", txt->welcome_title);
    printf("%s", txt->press_enter);
    fgets(input_buffer, sizeof(input_buffer), stdin);

    // Sélection du personnage et chargement de ses transitions
    Personnage personnage = select_personnage(lang, txt);

    Transition *transitions;
    int nb_transitions;
    Transition transitions_melec[] = {
        {SCENE_1, 1, SCENE_2, 0},
        {SCENE_1, 2, GAME_OVER1, 1},

        {SCENE_2, 1, SCENE_3, 0},
        {SCENE_2, 2, GAME_OVER2, 1},

        {SCENE_3, 1, SCENE_4, 0},
        {SCENE_3, 2, SCENE_5, 0},

        {SCENE_4, 1, SCENE_5, 0},
        {SCENE_4, 2, SCENE_6, 0},

        {SCENE_5, 1, SCENE_14, 0},
        {SCENE_5, 2, SCENE_10, 0},

        {SCENE_6, 1, SCENE_7, 0},
        {SCENE_6, 2, SCENE_4, 0},

        {SCENE_7, 1, SCENE_8_NOTOK, 0},
        {SCENE_7, 2, SCENE_8, 0},
        {SCENE_7, 3, SCENE_8_NOTOK, 0},

        {SCENE_8, 1, SCENE_9_NOTOK, 0},
        {SCENE_8, 2, SCENE_9_NOTOK, 0},
        {SCENE_8, 3, SCENE_9, 0},

        {SCENE_8_NOTOK, 1, GAME_OVER3, 1},
        {SCENE_8_NOTOK, 2, GAME_OVER3, 1},
        {SCENE_8_NOTOK, 3, SCENE_9, 0},

        {SCENE_9, 1, SCENE_4, 0},
        {SCENE_9, 2, SCENE_4, 0},
        {SCENE_9, 3, SCENE_4, 0},

        {SCENE_9_NOTOK, 1, SCENE_4, 0},
        {SCENE_9_NOTOK, 2, GAME_OVER3, 1},
        {SCENE_9_NOTOK, 3, GAME_OVER3, 1},

        {SCENE_10, 1, SCENE_11, 0},
        {SCENE_10, 2, SCENE_14, 0},

        {SCENE_11, 1, SCENE_12_NOTOK, 0},
        {SCENE_11, 2, SCENE_12, 0},
        {SCENE_11, 3, SCENE_12_NOTOK, 0},

        {SCENE_12, 1, SCENE_13, 0},
        {SCENE_12, 2, SCENE_13_NOTOK, 0},
        {SCENE_12, 3, SCENE_13_NOTOK, 0},

        {SCENE_12_NOTOK, 1, SCENE_13, 0},
        {SCENE_12_NOTOK, 2, GAME_OVER4, 1},
        {SCENE_12_NOTOK, 3, GAME_OVER4, 1},

        {SCENE_13, 1, SCENE_14, 0},
        {SCENE_13, 2, SCENE_14, 0},
        {SCENE_13, 3, SCENE_14, 0},

        {SCENE_13_NOTOK, 1, GAME_OVER4, 1},
        {SCENE_13_NOTOK, 2, SCENE_14, 0},
        {SCENE_13_NOTOK, 3, GAME_OVER4, 1},

        {SCENE_14, 1, SCENE_15, 0},
        {SCENE_14, 2, SCENE_18, 0},

        {SCENE_15, 1, SCENE_16_NOTOK, 0},
        {SCENE_15, 2, SCENE_16, 0},
        {SCENE_15, 3, SCENE_16_NOTOK, 0},

        {SCENE_16, 1, SCENE_17_NOTOK, 0},
        {SCENE_16, 2, SCENE_17_NOTOK, 0},
        {SCENE_16, 3, SCENE_17, 0},

        {SCENE_16_NOTOK, 1, GAME_OVER5, 1},
        {SCENE_16_NOTOK, 2, GAME_OVER5, 1},
        {SCENE_16_NOTOK, 3, SCENE_17, 0},

        {SCENE_17, 1, SCENE_18, 0},
        {SCENE_17, 2, SCENE_18, 0},
        {SCENE_17, 3, SCENE_18, 0},

        {SCENE_17_NOTOK, 1, SCENE_18, 0},
        {SCENE_17_NOTOK, 2, GAME_OVER5, 1},
        {SCENE_17_NOTOK, 3, GAME_OVER5, 1},

        {SCENE_18, 1, SCENE_10, 0},
        {SCENE_18, 2, SCENE_19, 0},
        {SCENE_18, 3, SCENE_4, 0},

        {SCENE_19, 1, SCENE_20, 0},
        {SCENE_19, 2, SCENE_23, 0},

        {SCENE_20, 1, SCENE_21, 0},
        {SCENE_20, 2, SCENE_21_NOTOK, 0},
        {SCENE_20, 3, SCENE_21_NOTOK, 0},
        
        {SCENE_21, 1, SCENE_22_NOTOK, 0},
        {SCENE_21, 2, SCENE_22_NOTOK, 0},
        {SCENE_21, 3, SCENE_22, 0},

        {SCENE_21_NOTOK, 1, SCENE_21, 0},
        {SCENE_21_NOTOK, 2, GAME_OVER5, 1},
        {SCENE_21_NOTOK, 3, GAME_OVER5, 1},

        {SCENE_22, 1, SCENE_23, 0},
        {SCENE_22, 2, SCENE_23, 0},
        {SCENE_22, 3, SCENE_23, 0},

        {SCENE_22_NOTOK, 1, GAME_OVER5, 1},
        {SCENE_22_NOTOK, 2, SCENE_23, 0},
        {SCENE_22_NOTOK, 3, GAME_OVER5, 1},

        {SCENE_23, 1, SCENE_14, 0},
        {SCENE_23, 2, SCENE_24, 0},

        {SCENE_24, 1, SCENE_25, 0},
        {SCENE_24, 2, SCENE_19, 0},

        {SCENE_25, 1, SCENE_26, 0},
        {SCENE_25, 2, SCENE_26_NOTOK, 0},
        {SCENE_25, 3, SCENE_26_NOTOK, 0},

        {SCENE_26, 1, SCENE_27_NOTOK, 0},
        {SCENE_26, 2, SCENE_27_NOTOK, 0},
        {SCENE_26, 3, SCENE_27, 0},

        {SCENE_26_NOTOK, 1, GAME_OVER6, 1},
        {SCENE_26_NOTOK, 2, GAME_OVER6, 1},
        {SCENE_26_NOTOK, 3, SCENE_27, 0},

        {SCENE_27, 1, VICTORY, 1},
        {SCENE_27, 2, VICTORY, 1},
        {SCENE_27, 3, VICTORY, 1},
        
        {SCENE_27_NOTOK, 1, GAME_OVER7, 1},
        {SCENE_27_NOTOK, 2, VICTORY, 1},
        {SCENE_27_NOTOK, 3, GAME_OVER7, 1},
    };

    Transition transitions_captain[] = {
        {SCENE_1, 1, SCENE_2, 0},
        {SCENE_1, 2, GAME_OVER1, 1},

        {SCENE_2, 1, SCENE_3, 0},
        {SCENE_2, 2, SCENE_4, 0},

        {SCENE_3, 1, SCENE_5, 0},
        {SCENE_3, 2, SCENE_6, 0},
        {SCENE_3, 3, SCENE_4, 0},

        {SCENE_4, 1, SCENE_7, 0},
        {SCENE_4, 2, GAME_OVER2, 1},

        {SCENE_5, 1, SCENE_8, 0},
        {SCENE_5, 2, SCENE_9, 0},

        {SCENE_6, 1, SCENE_10, 0},
        {SCENE_6, 2, SCENE_2, 0},

        {SCENE_7, 1, SCENE_11, 0},
        {SCENE_7, 2, SCENE_4, 0},

        {SCENE_8, 1, SCENE_12, 0},
        {SCENE_8, 2, SCENE_12_NOTOK, 0},
        {SCENE_8, 3, SCENE_12_NOTOK, 0},

        {SCENE_9, 1, SCENE_13, 0},
        {SCENE_9, 2, SCENE_5, 0},

        {SCENE_10, 1, SCENE_14_NOTOK, 0},
        {SCENE_10, 2, SCENE_14, 0},
        {SCENE_10, 3, SCENE_14_NOTOK, 0},

        {SCENE_11, 1, SCENE_15_NOTOK, 0},
        {SCENE_11, 2, SCENE_15, 0},
        {SCENE_11, 3, SCENE_15_NOTOK, 0},

        {SCENE_12, 1, SCENE_16_NOTOK, 0},
        {SCENE_12, 2, SCENE_16, 0},
        {SCENE_12, 3, SCENE_16_NOTOK, 0},

        {SCENE_12_NOTOK, 1, GAME_OVER3, 1},
        {SCENE_12_NOTOK, 2, SCENE_16, 0},
        {SCENE_12_NOTOK, 3, GAME_OVER3, 1},

        {SCENE_13, 1, SCENE_17, 0},
        {SCENE_13, 2, SCENE_17_NOTOK, 0},
        {SCENE_13, 3, SCENE_17_NOTOK, 0},

        {SCENE_14, 1, SCENE_18, 0},
        {SCENE_14, 2, SCENE_18_NOTOK, 0},
        {SCENE_14, 3, SCENE_18_NOTOK, 0},

        {SCENE_14_NOTOK, 1, SCENE_18, 0},
        {SCENE_14_NOTOK, 2, GAME_OVER4, 1},
        {SCENE_14_NOTOK, 3, GAME_OVER4, 1},

        {SCENE_15, 1, SCENE_19_NOTOK, 0},
        {SCENE_15, 2, SCENE_19_NOTOK, 0},
        {SCENE_15, 3, SCENE_19, 0},

        {SCENE_15_NOTOK, 1, GAME_OVER5, 1},
        {SCENE_15_NOTOK, 2, GAME_OVER5, 1},
        {SCENE_15_NOTOK, 3, SCENE_19, 0},

        {SCENE_16, 1, VICTORY, 1},
        {SCENE_16, 2, VICTORY, 1},
        {SCENE_16, 3, VICTORY, 1}, 

        {SCENE_16_NOTOK, 1, GAME_OVER3, 1},
        {SCENE_16_NOTOK, 2, VICTORY, 1},
        {SCENE_16_NOTOK, 3, GAME_OVER3, 1},

        {SCENE_17, 1, SCENE_20_NOTOK, 0},
        {SCENE_17, 2, SCENE_20_NOTOK, 0},
        {SCENE_17, 3, SCENE_20, 0},

        {SCENE_17_NOTOK, 1, GAME_OVER6, 1},
        {SCENE_17_NOTOK, 2, GAME_OVER6, 1},
        {SCENE_17_NOTOK, 3, SCENE_20, 0},

        {SCENE_18, 1, GAME_OVER4, 1},
        {SCENE_18, 2, SCENE_6, 0},
        {SCENE_18, 3, GAME_OVER4, 1},

        {SCENE_18_NOTOK, 1, GAME_OVER5, 1},
        {SCENE_18_NOTOK, 2, SCENE_6, 0},
        {SCENE_18_NOTOK, 3, GAME_OVER5, 1},

        {SCENE_19, 1, SCENE_7, 0},
        {SCENE_19, 2, GAME_OVER3, 1},
        {SCENE_19, 3, GAME_OVER3, 1},

        {SCENE_19_NOTOK, 1, GAME_OVER3, 1},
        {SCENE_19_NOTOK, 2, GAME_OVER3, 1},
        {SCENE_19_NOTOK, 3, SCENE_19, 0},
    
        {SCENE_20, 1, GAME_OVER6, 1},
        {SCENE_20, 2, SCENE_9, 0},
        {SCENE_20, 3, GAME_OVER6, 1},

        {SCENE_20_NOTOK, 1, GAME_OVER6, 1},
        {SCENE_20_NOTOK, 2, SCENE_9, 0},
        {SCENE_20_NOTOK, 3, GAME_OVER6, 1},
        
    };

    Transition transitions_athe[] = {
        {SCENE_1, 1, SCENE_2, 0},
        {SCENE_1, 2, GAME_OVER1, 1},

        {SCENE_2, 1, SCENE_3, 0},
        {SCENE_2, 2, GAME_OVER2, 1},

        {SCENE_3, 1, SCENE_4, 0},
        {SCENE_3, 2, SCENE_4_NOTOK, 0},
        {SCENE_3, 3, SCENE_4_NOTOK, 0},

        {SCENE_4, 1, SCENE_5_NOTOK, 0},
        {SCENE_4, 2, SCENE_5, 0},
        {SCENE_4, 3, SCENE_5_NOTOK, 0},

        {SCENE_4_NOTOK, 1, GAME_OVER3, 1},
        {SCENE_4_NOTOK, 2, SCENE_5, 0},
        {SCENE_4_NOTOK, 3, GAME_OVER3, 1},

        {SCENE_5, 1, GAME_OVER3, 1},
        {SCENE_5, 2, GAME_OVER3, 1},
        {SCENE_5, 3, SCENE_6, 0},

        {SCENE_6, 1, SCENE_7, 0},
        {SCENE_6, 2, SCENE_12, 0},

        {SCENE_7, 1, SCENE_8, 0},
        {SCENE_7, 2, SCENE_12},

        {SCENE_8, 1, SCENE_9, 0},
        {SCENE_8, 2, SCENE_7, 0},

        {SCENE_9, 1, SCENE_10_NOTOK, 0},
        {SCENE_9, 2, SCENE_10, 0},
        {SCENE_9, 3, SCENE_10_NOTOK, 0},

        {SCENE_10, 1, SCENE_11_NOTOK, 0},
        {SCENE_10, 2, SCENE_11_NOTOK, 0},
        {SCENE_10, 3, SCENE_11, 0},

        {SCENE_10_NOTOK, 1, GAME_OVER4, 1},
        {SCENE_10_NOTOK, 2, GAME_OVER4, 1},
        {SCENE_10, 3, SCENE_11},

        {SCENE_11, 1, VICTORY, 1},
        {SCENE_11, 2, VICTORY, 1},
        {SCENE_11, 3, VICTORY, 1},

        {SCENE_11_NOTOK, 1, VICTORY, 1},
        {SCENE_11_NOTOK, 2, GAME_OVER4, 1},
        {SCENE_11_NOTOK, 3, GAME_OVER4, 1},

        {SCENE_12, 1, SCENE_13, 0},
        {SCENE_12, 2, SCENE_18, 0},

        {SCENE_13, 1, SCENE_14, 0},
        {SCENE_13, 2, SCENE_12, 0},

        {SCENE_14, 1, SCENE_15_NOTOK, 0},
        {SCENE_14, 2, SCENE_15, 0},
        {SCENE_14, 3, SCENE_15_NOTOK, 0},

        {SCENE_15, 1, SCENE_16, 0},
        {SCENE_15, 2, SCENE_16_NOTOK, 0},
        {SCENE_15, 3, SCENE_16_NOTOK, 0},

        {SCENE_15_NOTOK, 1, SCENE_16, 0},
        {SCENE_15_NOTOK, 2, GAME_OVER5, 1},
        {SCENE_15_NOTOK, 3, GAME_OVER6, 1},

        {SCENE_16, 1, SCENE_17, 0},
        {SCENE_16, 2, SCENE_17, 0},
        {SCENE_16, 3, SCENE_17, 0},

        {SCENE_16_NOTOK, 1, SCENE_17, 0},
        {SCENE_16_NOTOK, 2, GAME_OVER6, 0},
        {SCENE_16_NOTOK, 3, GAME_OVER6},

        {SCENE_17, 1, SCENE_18, 0},
        {SCENE_17, 2, SCENE_13, 0},
        {SCENE_17, 3, SCENE_7, 0},

        {SCENE_18, 1, SCENE_19, 0},
        {SCENE_18, 2, SCENE_17, 0},

        {SCENE_19, 1, SCENE_20_NOTOK, 0},
        {SCENE_19, 2, SCENE_20, 0},
        {SCENE_19, 3, SCENE_20_NOTOK, 0},

        {SCENE_20, 1, SCENE_21_NOTOK, 0},
        {SCENE_20, 2, SCENE_21_NOTOK, 0},
        {SCENE_20, 3, SCENE_21, 0},

        {SCENE_20_NOTOK, 1, GAME_OVER7, 1},
        {SCENE_20_NOTOK, 2, GAME_OVER7, 1},
        {SCENE_20_NOTOK, 3, SCENE_21, 0},

        {SCENE_21, 1, SCENE_17, 0},
        {SCENE_21, 2, SCENE_17, 0},
        {SCENE_21, 3, SCENE_17, 0},

        {SCENE_21_NOTOK, 1, SCENE_18, 0},
        {SCENE_21_NOTOK, 2, GAME_OVER7, 1},
        {SCENE_21_NOTOK, 3, GAME_OVER7, 1},

        {SCENE_22, 1, SCENE_23, 0},
        {SCENE_22, 2, SCENE_26, 0},

        {SCENE_23, 1, SCENE_24, 0},
        {SCENE_23, 2, SCENE_24_NOTOK, 0},
        {SCENE_23, 3, SCENE_24_NOTOK, 0},

        {SCENE_24, 1, SCENE_25_NOTOK, 0},
        {SCENE_24, 2, SCENE_25_NOTOK, 0},
        {SCENE_24, 3, SCENE_25, 0},

        {SCENE_24_NOTOK, 1, GAME_OVER8, 1},
        {SCENE_24_NOTOK, 2, GAME_OVER8, 1},
        {SCENE_24_NOTOK, 3, SCENE_25, 0},

        {SCENE_25, 1, SCENE_22, 0},
        {SCENE_25, 2, SCENE_22, 0},
        {SCENE_25, 3, SCENE_22, 0},

        {SCENE_25_NOTOK, 1, GAME_OVER8, 1},
        {SCENE_25_NOTOK, 2, SCENE_22, 0},
        {SCENE_25_NOTOK, 3, GAME_OVER8, 1},

        {SCENE_26, 1, SCENE_27, 0},
        {SCENE_26, 2, SCENE_22, 0},

        {SCENE_27, 1, SCENE_28, 0},
        {SCENE_27, 2, SCENE_28_NOTOK, 0},
        {SCENE_27, 3, SCENE_28_NOTOK, 0},

        {SCENE_28, 1, SCENE_29_NOTOK, 0},
        {SCENE_28, 2, SCENE_29_NOTOK, 0},
        {SCENE_28, 3, SCENE_29, 0},

        {SCENE_28_NOTOK, 1, GAME_OVER8, 1},
        {SCENE_28_NOTOK, 2, GAME_OVER8, 1},
        {SCENE_28_NOTOK, 3, SCENE_29, 0},

        {SCENE_29, 1, SCENE_26, 0},
        {SCENE_29, 2, SCENE_26, 0},
        {SCENE_29, 3, SCENE_26, 0},

        {SCENE_29_NOTOK, 1, GAME_OVER8, 1},
        {SCENE_29_NOTOK, 2, SCENE_26, 0},
        {SCENE_29_NOTOK, 3, GAME_OVER8, 1}, 
    
    };

    // Attribution des transitions selon le personnage choisi
    switch (personnage.id)
    {
    case 1:
        transitions = transitions_melec;
        nb_transitions = sizeof(transitions_melec) / sizeof(Transition);
        max_scenes = 37;
        break;
    case 3:
        transitions = transitions_captain;
        nb_transitions = sizeof(transitions_captain) / sizeof(Transition);
        max_scenes = 28;
        break;
    case 5:
        transitions = transitions_athe;
        nb_transitions = sizeof(transitions_athe) / sizeof(Transition);
        max_scenes = 41;
        break;
    }

    // Boucle principale du jeu
    while (game_running)
    {
        const char *filename = get_scene_filename(current_scene);
        if (filename != NULL)
        {
#ifdef _WIN32
            sprintf(final_path, "%s\\%s", personnage.folder, lang);
#else
            sprintf(final_path, "%s/%s", personnage.folder, lang);
#endif
            load_scene(final_path, filename);
        }

        if (choix_invalide)
        {
            printf("\n%s\n", txt->unknown_choice);
            choix_invalide = 0;
        }

        printf("\n========================================\n");
        printf("[Personnage: %s]\n", personnage.nom);

        if (current_scene < GAME_OVER1)
        {
            display_progress((int)current_scene + 1, max_scenes, lang);
        }

        if (strcmp(lang, "fr") == 0)
            printf("Ton choix (ou 0 pour quitter): ");
        else
            printf("Your choice (or 0 to quit): ");

        if (fgets(input_buffer, sizeof(input_buffer), stdin) != NULL)
        {
            // Carte
            if (input_buffer[0] == 'm' || input_buffer[0] == 'M')
            {
                load_scene("menu", "map.txt");
                
                printf("\n(Appuie sur Entrée pour replier le plan/Press enter to fold the map...)");
                fgets(input_buffer, sizeof(input_buffer), stdin);
                continue; 
            }

            if (sscanf(input_buffer, "%d", &choix) != 1)
            {
                choix = -1;
            }
        }
        else
        {
            choix = -1;
        }
        if (choix == 0)
        {
            printf("\n%s\n", txt->goodbye);
            game_running = 0;
            continue;
        }

        int fin = 0;
        SceneType next_scene = get_next_scene(current_scene, choix, transitions, nb_transitions, &fin);
        if (next_scene == current_scene && fin == 0)
        {
            choix_invalide = 1;
        }
        else
        {
            current_scene = next_scene;

            if (fin == 0)
            {
                current_scene = trigger_random_event(lang, current_scene);
            }

            if (fin == 1)
            {
                const char *final_file = get_scene_filename(current_scene);
                if (final_file != NULL)
                {
#ifdef _WIN32
                    sprintf(final_path, "%s\\%s", personnage.folder, lang);
#else
                    sprintf(final_path, "%s/%s", personnage.folder, lang);
#endif
                    load_scene(final_path, final_file);
                }

                printf("\nAppuie sur Entree pour quitter...");
                fgets(input_buffer, sizeof(input_buffer), stdin);
                game_running = 0;
            }
        }
    }

    printf("%s\n", txt->thanks);
    printf("  Personnage/Character: %s\n", personnage.nom);

    return 0;
}