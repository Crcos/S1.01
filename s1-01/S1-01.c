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
    JOURS_MIN = 1,  // Nombre de jours minimales dans un semestre autorisé
    JOURS_MAX = 40, // Nombre de jours maximales dans un semestre autorisé
    MAX_JUSTIFICATIF_LENGTH = 50 + 1, // Longueur maximale pour un justificatif
    MAX_DEMI_JOURNE_LENGTH = 3, // Longueur maximale d'une demi-journée
    DELAIS_JUSTIFICATIF = 3
};

// Structure représentant un étudiant
typedef struct {
    unsigned int id;  // Identifiant unique de l'étudiant
    char nom[MAX_NOM_LENGTH + 1]; // Nom de l'étudiant
    unsigned int groupe; // Groupe de l'étudiant
} Etudiant;

// Structure représentant une absence avec ajout d'un justificatif
typedef struct {
    unsigned int id; // Identifiant unique de l'absence
    unsigned int demi_journee; // Demi-journée concernée (0 = AM, 1 = PM)
    unsigned int num_jour; // Jour de l'absence
    unsigned int jour_depot; // Jour de dépôt du justificatif
    unsigned int id_Etudiant; // Identifiant de l'étudiant qui a été absent
    char justificatif[MAX_JUSTIFICATIF_LENGTH]; // Justificatif pour l'absence
    bool en_traitement; // Indicateur si le justificatif est en cours de traitement
    bool valide; // Indique si l'absence est justifiée (validée)
} Absence;


// Vérifie si une chaîne est un entier
unsigned int est_entier(const char* str) {
    for (unsigned int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) return 0; // Non entier
    }
    return 1; // Entier
}

// Fonction pour comparer deux étudiants par groupe et nom
int compare_etudiants(const void* a, const void* b) {
    const Etudiant* p1 = (const Etudiant*)a;
    const Etudiant* p2 = (const Etudiant*)b;

    // Comparaison par groupe
    if (p1->groupe < p2->groupe)
        return -1;
    if (p1->groupe > p2->groupe)
        return 1;

    // Comparaison lexicographique par nom
    return strcmp(p1->nom, p2->nom);
}

// Fonction pour comparer les absences par identifiant d'étudiant et par date
int compare_absences(const void* a, const void* b) {
    const Absence* p1 = (const Absence*)a;
    const Absence* p2 = (const Absence*)b;

    // Comparaison par identifiant d'étudiant
    if (p1->id_Etudiant < p2->id_Etudiant)
        return -1;
    if (p1->id_Etudiant > p2->id_Etudiant)
        return 1;

    // Comparaison par date (numéro de jour et demi-journée)
    if (p1->num_jour < p2->num_jour)
        return -1;
    if (p1->num_jour > p2->num_jour)
        return 1;
    if (p1->demi_journee < p2->demi_journee)
        return -1;
    if (p1->demi_journee > p2->demi_journee)
        return 1;

    return 0;
}

// Fonction qui permet de compter le nombre d'absences d'un étudiant
unsigned int compter_absence(Absence absences[], unsigned int jour_courant, unsigned int etudiant_id) {
    unsigned int total_absence = 0;
    for (unsigned int i = 0; i < MAX_ABSENCES; i++) {
        if (absences[i].id_Etudiant == etudiant_id && absences[i].num_jour <= jour_courant) {
            total_absence++;
        }
    }
    return total_absence;
}

//**C1**//
//Inscrit un étudiant
void inscription_etudiant(Etudiant etudiants[], char* nom, unsigned int groupe, unsigned int nb_etudiants) {
    // Assigne un nouvel identifiant à l'étudiant
    etudiants[nb_etudiants].id = nb_etudiants + 1;
    // Copie le nom de l'étudiant
    strncpy(etudiants[nb_etudiants].nom, nom, MAX_NOM_LENGTH);
    // Assigne le groupe de l'étudiant
    etudiants[nb_etudiants].groupe = groupe;
    // Affiche un message de confirmation d'inscription
    printf("Inscription enregistree (%d) \n", nb_etudiants + 1);
}

// Vérifie si l'étudiant est déjà inscrit avec le même nom et groupe
int verif_inscrit(Etudiant etudiants[], unsigned int nb_etudiants, char nom[], unsigned int groupe) {
    for (unsigned int i = 0; i < nb_etudiants; i++) {
        if (strcmp(etudiants[i].nom, nom) == 0 && etudiants[i].groupe == groupe) {
            return i;
        }
    }
    return -1;
}
//**C2**//
// Enregistre une absence 
void enregistrement_absence(Etudiant etudiants[], unsigned int nb_etudiants, Absence absences[], unsigned int id_Etu, unsigned int num_jour, unsigned int jour_courant, const char* demi_journee, unsigned int* nb_absences) {
    // Vérifie si l'identifiant de l'étudiant existe
    unsigned int etudiant_existe = 0;
    for (unsigned int i = 0; i < nb_etudiants; i++) {
        if (etudiants[i].id == id_Etu) {
            etudiant_existe = 1;
            break;
        }
    }
    if (!etudiant_existe) {
        printf("Identifiant incorrect\n");
        return;
    }

    // Vérifie si l'absence est déjà enregistrée pour cet étudiant, ce jour, et cette demi-journée
    for (unsigned int i = 0; i < *nb_absences; i++) {
        if (absences[i].id_Etudiant == id_Etu && absences[i].num_jour == num_jour &&
            ((strcmp(demi_journee, "am") == 0 && absences[i].demi_journee == 0) ||
                (strcmp(demi_journee, "pm") == 0 && absences[i].demi_journee == 1))) {
            printf("Absence deja connue\n");
            return;
        }
    }

    // Enregistre la nouvelle absence si elle n'est pas connue
    absences[*nb_absences].id_Etudiant = id_Etu;
    absences[*nb_absences].id = *nb_absences + 1;  // Assignation de l'ID unique à l'absence
    absences[*nb_absences].num_jour = num_jour;
    absences[*nb_absences].jour_depot = jour_courant; // Stockage du jour courant au dépôt
    absences[*nb_absences].demi_journee = (strcmp(demi_journee, "am") == 0) ? 0 : 1;
    absences[*nb_absences].justificatif[0] = '\0';  // Pas de justificatif au départ
    absences[*nb_absences].en_traitement = false;  // Pas en traitement
    absences[*nb_absences].valide = false; // Par défaut, l'absence n'est pas encore validée
    printf("Absence enregistree [%d]\n", *nb_absences + 1);
    (*nb_absences)++;
}



//**C3**//
// Affiche le tableau d'étudiants complet et trié
void liste_etudiants(Etudiant etudiants[], Absence absences[], unsigned int num_jour, unsigned int num_etudiants) {
    if (num_etudiants == 0) {
        printf("Aucun inscrit\n");
        return;
    }
    Etudiant etudiants_copy[MAX_ETUDIANTS];
    for (unsigned int i = 0; i < num_etudiants; i++) {
        etudiants_copy[i] = etudiants[i];
    }
    if (num_jour >= 1) {
        qsort(etudiants_copy, num_etudiants, sizeof(Etudiant), compare_etudiants);
        for (unsigned int i = 0; i < num_etudiants; i++) {
            unsigned int absences_etudiant = compter_absence(absences, num_jour, etudiants_copy[i].id);
            printf("(%d) %-13s %2d %d\n", etudiants_copy[i].id, etudiants_copy[i].nom, etudiants_copy[i].groupe, absences_etudiant);
        }
    }
    else {
        printf("Date incorrecte\n");
    }
}
//**C4**//
// Enregistre un justificatif pour une absence
void depot_justificatif(Absence absences[], unsigned int nb_absences, unsigned int id_absence, unsigned int num_jour, const char* texte_justificatif) {
    // Recherche de l'absence par son identifiant
    unsigned int absence_trouvee = 0;
    for (unsigned int i = 0; i < nb_absences; i++) {
        if (absences[i].id == id_absence) {
            absence_trouvee = 1;

            // Vérification si un justificatif existe déjà
            if (strlen(absences[i].justificatif) > 0) {
                printf("Justificatif deja connu\n");
                return;
            }

            // Enregistrement du justificatif et du jour de dépôt
            strncpy(absences[i].justificatif, texte_justificatif, MAX_JUSTIFICATIF_LENGTH - 1);
            absences[i].justificatif[MAX_JUSTIFICATIF_LENGTH - 1] = '\0'; // Sécurité pour la fin de chaîne
            absences[i].en_traitement = true; // Indique que le justificatif est en cours de traitement
            absences[i].jour_depot = num_jour; // Stocke le jour de dépôt dans `jour_depot`
            printf("Justificatif enregistre\n");
            return;
        }
    }

    if (!absence_trouvee) {
        printf("Identifiant incorrect\n");
    }
}

//**C5**//
// Affiche la liste des absences en attente de validation avec justificatif dans le délai de 3 jours
void liste_validations(Etudiant etudiants[], Absence absences[], unsigned int nb_absences, unsigned int nb_etudiants) {
    Absence absences_en_attente[MAX_ABSENCES];
    unsigned int nb_validations = 0;

    // Filtrer les absences avec justificatif en attente et dans le délai de 3 jours
    for (unsigned int i = 0; i < nb_absences; i++) {
        if (absences[i].en_traitement && strlen(absences[i].justificatif) > 0 &&
            (absences[i].jour_depot - absences[i].num_jour) <= DELAIS_JUSTIFICATIF) {
            absences_en_attente[nb_validations++] = absences[i];
        }
    }

    // Si aucune absence en attente de validation
    if (nb_validations == 0) {
        printf("Aucune validation en attente\n");
        return;
    }

    // Tri des absences en attente par identifiant d'étudiant puis par ordre chronologique
    qsort(absences_en_attente, nb_validations, sizeof(Absence), compare_absences);

    // Affichage des absences en attente
    for (unsigned int i = 0; i < nb_validations; i++) {
        Absence a = absences_en_attente[i];
        Etudiant etu = etudiants[a.id_Etudiant - 1]; // Recherche de l'étudiant correspondant
        printf("[%d] (%d) %s %d %d/%s (%s)\n", a.id, etu.id, etu.nom, etu.groupe, a.num_jour, (a.demi_journee == 0) ? "am" : "pm", a.justificatif);
    }
}

//**C6**//
// Valide ou invalide un justificatif pour une absence
void validation_justificatif(Absence absences[], unsigned int nb_absences, unsigned int id_absence, const char* verdict) {
    // Recherche de l'absence par son identifiant
    unsigned int absence_trouvee = 0;
    for (unsigned int i = 0; i < nb_absences; i++) {
        if (absences[i].id == id_absence) {
            absence_trouvee = 1;

            // Vérification si l'absence est en attente de validation
            if (!absences[i].en_traitement) {
                printf("Validation deja connue\n");
                return;
            }

            // Vérification du verdict ("ok" ou "ko")
            if (strcmp(verdict, "ok") == 0) {
                absences[i].valide = true; // Justificatif accepté
                printf("Validation enregistree\n");
            }
            else if (strcmp(verdict, "ko") == 0) {
                absences[i].valide = false; // Justificatif refusé
                printf("Validation enregistree\n");
            }
            else {
                printf("Code incorrect\n");
                return;
            }

            // Désactivation de l'indicateur "en_traitement"
            absences[i].en_traitement = false;
            return;
        }
    }

    if (!absence_trouvee) {
        printf("Identifiant incorrect\n");
    }
}

/*C1 Commande */
//Inscrit un étudiant
void commande_inscription(Etudiant etudiants[], unsigned int* nombre_etudiants) {
    char nom[MAX_NOM_LENGTH + 1], groupe_str[20];
    unsigned int groupe;
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
    inscription_etudiant(etudiants, nom, groupe, *nombre_etudiants);
    (*nombre_etudiants)++;
}

/*C2 commande*/
void commande_absence(Etudiant etudiants[], unsigned int nombre_etudiants, Absence absences[], unsigned int* nb_absence) {
    unsigned int etudiant_id, num_jour;
    char demi_journee[MAX_DEMI_JOURNE_LENGTH];
    scanf("%d %d %s", &etudiant_id, &num_jour, demi_journee);
    enregistrement_absence(etudiants, nombre_etudiants, absences, etudiant_id, num_jour, num_jour, demi_journee, nb_absence);
}

/*C3 commande*/
void commande_etudiants(Etudiant etudiants[], Absence absences[], unsigned int nombre_etudiants) {
    unsigned int num_jour;
    scanf("%d", &num_jour);
    liste_etudiants(etudiants, absences, num_jour, nombre_etudiants);
}

/*C4 commande*/
void commande_justificatif(Absence absences[], unsigned int nb_absences) {
    unsigned int id_absence, num_jour;
    char texte_justificatif[MAX_JUSTIFICATIF_LENGTH];

    // Lecture des entrées
    scanf("%d %d ", &id_absence, &num_jour);  // `num_jour` est le jour de dépôt ici
    fgets(texte_justificatif, MAX_JUSTIFICATIF_LENGTH, stdin);

    // Suppression du retour à la ligne à la fin du texte si présent
    texte_justificatif[strcspn(texte_justificatif, "\n")] = '\0';

    // Enregistrement du justificatif avec le jour de dépôt
    depot_justificatif(absences, nb_absences, id_absence, num_jour, texte_justificatif);
}


/*C5 commande*/
void commande_validations(Etudiant etudiants[], Absence absences[], unsigned int nombre_etudiants, unsigned int nb_absences) {
    liste_validations(etudiants, absences, nb_absences, nombre_etudiants);
}
/*C6 commande*/
void commande_validation(Absence absences[], unsigned int nb_absences) {
    unsigned int id_absence;
    char verdict[3];

    // Lecture des entrées
    scanf("%d %s", &id_absence, verdict);

    // Validation du justificatif
    validation_justificatif(absences, nb_absences, id_absence, verdict);
}
unsigned int main() {
    char commande[20] = "";
    unsigned int nombre_etudiants = 0;
    Etudiant etudiants[MAX_ETUDIANTS];
    Absence absences[MAX_ABSENCES];
    unsigned int nb_absence = 0;

    do {
        scanf("%s", commande);
        if (strcmp(commande, "inscription") == 0) // C1
        {
            commande_inscription(etudiants, &nombre_etudiants);
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
            commande_validations(etudiants, absences, nombre_etudiants, nb_absence);
        }
        else if (strcmp(commande, "validation") == 0) // C6
        {
            commande_validation(absences, nb_absence);
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