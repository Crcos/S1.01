#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#pragma warning(disable: 4996)

// Définition des constantes MAX_ETUDIANTS et MAX_NOM_LENGTH à l'aide d'une énumération
enum {
    MAX_ETUDIANTS = 100, // Nombre maximum d'étudiants
    MAX_NOM_LENGTH = 30, // Longueur maximale du nom d'un étudiant
    MAX_ABSENCES = 100, // Nombre max d'absence autorisés
    JOURS_MIN = 1,  // Nombre de jours minimales dans un semestre autoriser
    JOURS_MAX = 40, // Nombre de jours maximales dans un semestre autoriser
    MAX_JUSTIFICATIF_LENGTH = 50 + 1, // Longueur maximale pour un justificatif
    MAX_DEMI_JOURNE_LENGTH = 3, // Longueur maximale d'une demi-jornée
    MAX_VALIDATION_LENGTH = 3, // Longueur maximale d'un etat pour une validation
    DELAIS_JUSTIFICATIF = 3, // Nombre de jours max pour déposer un justificatif
};
// Structure pour stocker l'état d'une absence
typedef enum {
    JUSTIFICATIF_RECU,
    JUSTIFICATIF_NON_RECU,
    NON_VALIDE,
    VALIDE,
}Etat_abs;

// Structure représentant un étudiant
typedef struct {
    int id;  // Identifiant unique de l'étudiant
    char nom[MAX_NOM_LENGTH + 1]; // Nom de l'étudiant
    int groupe; // Groupe de l'étudiant
} Etudiant;

// Structure représentant une absence
typedef struct {
    int id; // Identifiant unique de l'absence
    int demi_journee; // Demi-journée concernée
    int num_jour; // Numéro de jour concerné
    char justificatif[MAX_JUSTIFICATIF_LENGTH]; // Justificatif (si enregistré)
    int deja_valide; // Stock les absence_id deja validés
    Etat_abs etat; // Stock l'etat d'une absence
} Absence;

// Vérifie si l'étudiant est déjà inscrit avec le même nom et groupe
int verif_inscrit(Etudiant etudiants[], int nb_etudiants, char nom[], int groupe) {
    for (int i = 0; i < nb_etudiants; i++) {
        if (strcmp(etudiants[i].nom, nom) == 0 && etudiants[i].groupe == groupe) {
            return i;
        }
    }
    return -1;
}
// Vérifie si une chaîne est un entier
bool est_entier(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) return false;// Non entier
    }
    return true; // Entier
}
//**C1**//
//Inscrit un etudiant
void effectuer_inscription(Etudiant etudiants[], int* nombre_etudiants) {
    char nom[MAX_NOM_LENGTH + 1], groupe_str[20];
    int groupe;
    scanf("%s %s", nom, groupe_str);
    if (!est_entier(groupe_str)) {
        printf("Le groupe doit etre un nombre entier\n");
        return;
    }
    groupe = atoi(groupe_str);
    if (verif_inscrit(etudiants, *nombre_etudiants, nom, groupe) >= 0) {
        printf("Nom incorrect\n");
        return;
    }
    // Assigne un nouvel identifiant à l'étudiant
    etudiants[*nombre_etudiants].id = *nombre_etudiants + 1;
    // Copie le nom de l'étudiant
    strncpy(etudiants[*nombre_etudiants].nom, nom, MAX_NOM_LENGTH);
    // Assigne le groupe de l'étudiant
    etudiants[*nombre_etudiants].groupe = groupe;
    // Affiche un message de confirmation d'inscription
    printf("Inscription enregistree (%d)\n", *nombre_etudiants + 1);
    (*nombre_etudiants)++;
}
//**C2**//
// Enregistre une absence
void enregistrement_absence(Etudiant etudiants[], int nb_etudiants, Absence absences[], int id_Etu, int num_jour, const char* demi_journee, int* nb_absences) {
    // Vérifie si l'identifiant de l'étudiant existe
    bool etudiant_existe = false;
    for (int i = 0; i < nb_etudiants; i++) {
        if (etudiants[i].id == id_Etu) {
            etudiant_existe = true;
            break;
        }
    }
    if (!etudiant_existe) {
        printf("Identifiant incorrect\n");
        return;
    }
    // Vérifie si la demi-journée est valide
    if (strcmp(demi_journee, "am") != 0 && strcmp(demi_journee, "pm") != 0) {
        printf("Demi-journee incorrecte\n");
        return;
    }
    // Vérifie si le jour est dans la plage autorisée
    if (num_jour < JOURS_MIN || num_jour > JOURS_MAX) {
        printf("Date incorrecte\n");
        return;
    }
    // Vérifie si l'absence est déjà connue
    for (int i = 0; i < *nb_absences; i++) {
        if (absences[i].id == id_Etu && absences[i].num_jour == num_jour &&
            ((strcmp(demi_journee, "am") == 0 && absences[i].demi_journee == 0) ||
                (strcmp(demi_journee, "pm") == 0 && absences[i].demi_journee == 1))) {
            printf("Absence deja connue\n");
            return;
        }
    }
    // Enregistre la nouvelle absence si elle n'est pas connue
    absences[*nb_absences].id = id_Etu;
    absences[*nb_absences].num_jour = num_jour;
    absences[*nb_absences].demi_journee = (strcmp(demi_journee, "am") == 0) ? 0 : 1;
    printf("Absence enregistree [%d]\n", *nb_absences);
    (*nb_absences)++;
}
//** C3 **//
// Fonction pour comparer deux cellules du tableau d'étudiants
int compare_etudiants(const void* a, const void* b) {
    const Etudiant* p1 = (const Etudiant*)a;
    const Etudiant* p2 = (const Etudiant*)b;
    if (p1->groupe < p2->groupe)
        return -1;
    if (p1->groupe > p2->groupe)
        return 1;
    return strcmp(p1->nom, p2->nom);
}
// Fonction qui permet de compter le nombre d'absences d'un étudiant
int compter_absence(Absence absences[], int jour_courant, int etudiant_id) {
    int total_absence = 0;
    for (int i = 0; i < MAX_ABSENCES; i++) {
        if (absences[i].id == etudiant_id && absences[i].num_jour <= jour_courant) {
            total_absence++;
        }
    }
    return total_absence;
}
// Affiche le tableau d'étudiants complet et trié
void liste_etudiants(Etudiant etudiants[], Absence absences[], int num_jour, int num_etudiants) {
    if (num_jour >= 1) {
        qsort(etudiants, num_etudiants, sizeof(Etudiant), compare_etudiants);
        for (int i = 0; i < num_etudiants; i++) {
            int absences_etudiant = compter_absence(absences, num_jour, etudiants[i].id);
            printf("(%d) %-13s %2d %d\n", etudiants[i].id, etudiants[i].nom, etudiants[i].groupe, absences_etudiant);
        }
    }
    else {
        printf("Date incorrecte\n");
    }
}
//**C4**//
void depot_justificatif(Absence absences[], int absence_id, int num_jour, const char justificatif[MAX_JUSTIFICATIF_LENGTH], int nb_absence) {
    // Vérifie si l'absence existe
    if (absence_id <= 0 || absence_id > nb_absence || absences[absence_id - 1].id == 0) {
        printf("Identifiant incorrect\n");
        return;
    }

    // Vérifie si le justificatif est déjà connu pour cette absence
    if (absences[absence_id - 1].etat > JUSTIFICATIF_NON_RECU) {
        printf("Justificatif deja connu\n");
        return;
    }

    // Vérifie si la date du justificatif est correcte (après la date d'absence)
    if (num_jour < absences[absence_id - 1].num_jour) {
        printf("Date incorrecte\n");
        return;
    }

    // Vérifie si le justificatif est déposé dans le délai autorisé
    if (num_jour > absences[absence_id - 1].num_jour + DELAIS_JUSTIFICATIF) {
        printf("Justificatif enregistre\n");
        absences[absence_id - 1].etat = NON_VALIDE; // Hors délai
    }
    else {
        // Enregistre le justificatif et le marque comme "en traitement"
        strncpy(absences[absence_id - 1].justificatif, justificatif, MAX_JUSTIFICATIF_LENGTH - 1);
        absences[absence_id - 1].justificatif[MAX_JUSTIFICATIF_LENGTH - 1] = '\0'; // Ensure null-termination
        printf("Justificatif enregistre\n");
        absences[absence_id - 1].etat = JUSTIFICATIF_RECU; // Justificatif déposé dans les temps
    }
}

void validation(Absence absences[], int id_absence, char validation[MAX_VALIDATION_LENGTH], int nb_absence) {
    // Vérifie si l'identifiant de l'absence est correcte
    if (id_absence == 0) {
        printf("Identifiant incorrect\n");
        return;
    }
    if (id_absence > nb_absence || absences[id_absence - 1].etat < JUSTIFICATIF_RECU) {
        printf("Identifiant incorrect\n");
        return;
    }
    // Vérifie si la saisie de l'identifiant est correcte
    if (strcmp(validation, "ok") != 0 && strcmp(validation, "ko") != 0) {
        printf("Code incorrect\n");
        return;
    }
    // Vérifie si la validation est déjà connue
    if (absences[id_absence - 1].etat == VALIDE) {
        printf("Validation deja connue\n");
        return;
    }
    else {
        printf("Validation enregistree\n");
        absences[id_absence - 1].etat = VALIDE;

    }
}
void commande_absence(Etudiant etudiants[], int nombre_etudiants, Absence absences[], int* nb_absence) {
    int etudiant_id, num_jour;
    char demi_journee[MAX_DEMI_JOURNE_LENGTH];
    scanf("%d %d %s", &etudiant_id, &num_jour, demi_journee);
    enregistrement_absence(etudiants, nombre_etudiants, absences, etudiant_id, num_jour, demi_journee, nb_absence);
}

void commande_etudiants(Etudiant etudiants[], Absence absences[], int nombre_etudiants) {
    int num_jour;
    scanf("%d", &num_jour);
    liste_etudiants(etudiants, absences, num_jour, nombre_etudiants);
}

void commande_justificatif(Absence absences[], int nb_absence) {
    int num_jour, absence_id;
    char justificatif[MAX_JUSTIFICATIF_LENGTH];
    scanf("%d %d", &absence_id, &num_jour);
    getchar();
    fgets(justificatif, MAX_JUSTIFICATIF_LENGTH, stdin);
    depot_justificatif(absences, absence_id, num_jour, justificatif, nb_absence);
}

void commande_validations() {
    // TODO: Implement this command
}

void commande_validation(Absence absences[], int nb_absence) {
    int absence_id;
    char validations[MAX_VALIDATION_LENGTH];
    scanf("%d %s", &absence_id, validations);
    validation(absences, absence_id, validations, nb_absence);
    // TODO: Implement this command
}

void commande_etudiant() {
    // TODO: Implement this command
}

void commande_defaillants() {
    // TODO: Implement this command
}

int main() {
    char commande[20] = "";
    int nombre_etudiants = 0;
    Etudiant etudiants[MAX_ETUDIANTS];
    Absence absences[MAX_ABSENCES];
    int nb_absence = 1;

    do {
        scanf("%s", commande);
        if (strcmp(commande, "inscription") == 0) // C1
        {
            effectuer_inscription(etudiants, &nombre_etudiants);
        }
        else if (strcmp(commande, "absence") == 0) // C2
        {
            commande_absence(etudiants, nombre_etudiants, absences, &nb_absence);
        }
        else if (strcmp(commande, "etudiants") == 0) // C3
        {
            commande_etudiants(etudiants, absences, nombre_etudiants);
        }
        else if (strcmp(commande, "justificatif") == 0) // C4
        {
            commande_justificatif(absences, nb_absence);
        }
        else if (strcmp(commande, "validations") == 0) // C5
        {
            commande_validations();
        }
        else if (strcmp(commande, "validation") == 0) // C6
        {
            commande_validation(absences, nb_absence);
        }
        else if (strcmp(commande, "etudiant") == 0) // C7
        {
            commande_etudiant();
        }
        else if (strcmp(commande, "defaillants") == 0) // C8
        {
            commande_defaillants();
        }
        else if (strcmp(commande, "exit") == 0) // C0
        {
            break;
        }
        else {
            printf("Commande inconnue\n");
        }
    } while (true);

    return 0;
}