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
	VALIDATION = 50,
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
	unsigned int id;  // Identifiant unique de l'étudiant
	char nom[MAX_NOM_LENGTH + 1]; // Nom de l'étudiant
	unsigned int groupe; // Groupe de l'étudiant
} Etudiant;

// Structure représentant une absence
typedef struct {
	unsigned int id; // Identifiant unique de l'absence
	unsigned unsigned int demi_journee; // Demi-journée concernée
	unsigned int num_jour; // Numéro de jour concerné
	char justificatif[MAX_JUSTIFICATIF_LENGTH]; // Justificatif (si enregistré)
	unsigned int deja_valide; // Stock les absence_id deja validés
	Etat_abs etat; // Stock l'etat d'une absence
	unsigned int id_Etudiant; // Idée de l'étudiant qui a été absent
} Absence;


// Vérifie si une chaîne est un entier
unsigned int est_entier(const char* str) {
	for (unsigned int i = 0; str[i] != '\0'; i++) {
		if (!isdigit(str[i])) return 0; // Non entier
	}
	return 1; // Entier
}
//**C1**//
//Inscrit un etudiant
void inscription_etudiant(Etudiant etudiants[], char* nom, unsigned int groupe, unsigned int nb_etudiants) {
	// Assigne un nouvel identifiant à l'étudiant
	etudiants[nb_etudiants].id = nb_etudiants + 1;
	// Copie le nom de l'étudiant
	strncpy(etudiants[nb_etudiants].nom, nom, MAX_NOM_LENGTH);
	// Assigne le groupe de l'étudiant
	etudiants[nb_etudiants].groupe = groupe;
	// Affiche un message de confirmation d'inscription
	printf("Inscription enregistree (%d)je \n", nb_etudiants + 1);
}

// Vérifie si l'étudiant est déjà inscrit avec le même nom et groupe
unsigned int verif_inscrit(Etudiant etudiants[], unsigned int nb_etudiants, char nom[], unsigned int groupe) {
	// Vérifie si l'étudiant est déjà inscrit avec le même nom et groupe
	for (unsigned int i = 0; i < nb_etudiants; i++) {
		if (strcmp(etudiants[i].nom, nom) == 0 && etudiants[i].groupe == groupe) {
			return i;
		}
	}
	return -1;
}
//**C2**//
// Enregistre une absence
void enregistrement_absence(Etudiant etudiants[], unsigned int nb_etudiants, Absence absences[], unsigned int id_Etu, unsigned int num_jour, const char* demi_journee, unsigned int* nb_absences, unsigned int id_Etudiant) {
	// Vérifie si l'identifiant de l'étudiant existe
	unsigned int etudiant_existe = 0; // 0 pour false, 1 pour true
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
	for (unsigned int i = 0; i < *nb_absences; i++) {
		if (absences[i].id == id_Etu && absences[i].num_jour == num_jour &&
			((strcmp(demi_journee, "am") == 0 && absences[i].demi_journee == 0) ||
				(strcmp(demi_journee, "pm") == 0 && absences[i].demi_journee == 1))) {
			printf("Absence deja connue\n");
			return;
		}
	}
	// Enregistre la nouvelle absence si elle n'est pas connue
	absences[*nb_absences].id_Etudiant = id_Etu;
	absences[*nb_absences].id = *nb_absences + 1;
	absences[*nb_absences].num_jour = num_jour;
	absences[*nb_absences].demi_journee = (strcmp(demi_journee, "am") == 0) ? 0 : 1;
	absences[*nb_absences].etat = JUSTIFICATIF_NON_RECU;
	printf("Absence enregistree [%d]\n", *nb_absences + 1);
	(*nb_absences)++;
}
//** C3 **//
// Fonction pour comparer deux cellules du tableau d'étudiants
unsigned int compare_etudiants(const void* a, const void* b) {
	const Etudiant* p1 = (const Etudiant*)a;
	const Etudiant* p2 = (const Etudiant*)b;
	if (p1->groupe < p2->groupe)
		return -1;
	if (p1->groupe > p2->groupe)
		return 1;
	return strcmp(p1->nom, p2->nom);
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

//** C4 **//
// Dépôt d'un justificatif pour une absence
void depot_justificatif(Absence absences[], unsigned int nb_absence, unsigned int absence_id, unsigned int num_jour, const char justificatif[MAX_JUSTIFICATIF_LENGTH]) {

	// Vérifie si l'absence existe
	if (absence_id <= 0 || absence_id > nb_absence) {
		printf("Identifiant incorrect\n");
		return;
	}
	// Vérifie si un justificatif a déjà été déposé
	if (absences[absence_id - 1].etat == JUSTIFICATIF_RECU && strlen(absences[absence_id - 1].justificatif) > 0) {
		printf("Justificatif deja connu\n");
		return;
	}
	// Vérifie si la date du justificatif est correcte (après la date d'absence)
	for (unsigned int i = 0; i < nb_absence; i++) {
		if (num_jour < absences[i].num_jour) {
			printf("Date incorrecte\n");
			return;
		}
	}

	// Vérifie si le dépôt du justificatif est dans le délai autorisé
	if (num_jour > absences[absence_id - 1].num_jour + DELAIS_JUSTIFICATIF) {
		absences[absence_id - 1].etat = NON_VALIDE; // Justificatif déposé trop tard => absence non justifiée
		printf("Justificatif enregistre\n");
	}
	else {
		absences[absence_id - 1].etat = JUSTIFICATIF_RECU; // Justificatif reçu dans les temps
		printf("Justificatif enregistre\n");
	}

	// Enregistre le justificatif (même si tardif, pour conserver une trace)
	strncpy(absences[absence_id - 1].justificatif, justificatif, MAX_JUSTIFICATIF_LENGTH);
}

// Fonction pour afficher les absences en attente de validation
void afficher_absences_attente_validation(Absence absences[], unsigned int nb_absence) {
	printf("Liste des absences en attente de validation :\n");
	for (unsigned int i = 0; i < nb_absence; i++) {
		// On n'affiche que les absences en attente de validation (ni validées ni non justifiées)
		if (absences[i].etat == JUSTIFICATIF_NON_RECU) {
			printf("Absence ID: %d, Date: Jour %d, Etat: En attente de validation\n", absences[i].id, absences[i].num_jour);
		}
	}
}

//**C5**//
void faire_validations(Absence absences[], Etudiant etudiants[], unsigned int nb_absence) {
	unsigned unsigned int cpt = 0;
	for (unsigned unsigned int i = 0; i < nb_absence; i++) {
		if (absences[i].etat != NON_VALIDE && absences[i].etat != JUSTIFICATIF_NON_RECU) {
			unsigned int indiceEtu = absences[i].id_Etudiant - 1;
			printf("[%d] (%d) %s %d %d/%s %s\n", absences[i].id, absences[i].id_Etudiant, etudiants[indiceEtu].nom, etudiants[indiceEtu].groupe, absences[i].num_jour, absences[i].demi_journee == 0 ? "am" : "pm", absences[i].justificatif);
			cpt++;
		}
	}
	if (cpt == 0) {
		printf("Aucune validation en attente\n");
	}
}




/*void faire_validations(Absence absences[], Etudiant etudiants[], unsigned int nb_absence, unsigned int nb_etudiants) {
	unsigned int en_attente = 0; // Utilisation d'un entier à la place du booléen
	for (unsigned int i = 0; i < nb_absence; i++) {
		if (absences[i].etat == JUSTIFICATIF_RECU) {
			for (unsigned int j = 0; j < nb_etudiants; j++) {
				if (etudiants[j].id == absences[i].id) {
					printf("[%d] (%d) %-13s %2d %d/%s (%s)\n",
						i + 1, etudiants[j].id, etudiants[j].nom,
						etudiants[j].groupe, absences[i].num_jour,
						absences[i].demi_journee == 0 ? "am" : "pm",
						absences[i].justificatif);
					en_attente = 1; // Indication qu'une validation est en attente
					break;
				}
			}
		}
	}
	if (en_attente == 0) {
		printf("Aucune validation en attente\n");
	}
}*/


/** C6 **/
void validation(Absence absences[], unsigned int id_absence, char validation[MAX_VALIDATION_LENGTH], unsigned int nb_absence) {
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




// C8 Commande pour afficher les étudiants défaillants

void liste_defaillants(Etudiant etudiants[], Absence absences[], unsigned int num_jour, unsigned int nb_etudiants) {
	unsigned int defaillant_trouve = 0; 
	for (unsigned int i = 0; i < nb_etudiants; i++) {
		unsigned int total_absences_non_justifiees = 0;
		for (unsigned int j = 0; j < MAX_ABSENCES; j++) {
			if (absences[j].id_Etudiant == etudiants[i].id && absences[j].num_jour <= num_jour && (absences[j].etat == NON_VALIDE || absences[j].etat == JUSTIFICATIF_NON_RECU)) {
				total_absences_non_justifiees++;
			}
		}
		if (total_absences_non_justifiees >= 5) {
			printf("(%d) %-13s %2d %d\n", etudiants[i].id, etudiants[i].nom, etudiants[i].groupe, total_absences_non_justifiees);
			defaillant_trouve = 1; // Indication qu'un défaillant a été trouvé
		}
	}
	if (defaillant_trouve == 0) {
		printf("Aucun defaillant\n");
	}
}


/*C1 */
//Inscrit un étudunsigned int
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
	enregistrement_absence(etudiants, nombre_etudiants, absences, etudiant_id, num_jour, demi_journee, nb_absence, etudiant_id);
}
/*C3 commande*/
void commande_etudiants(Etudiant etudiants[], Absence absences[], unsigned int nombre_etudiants) {
	unsigned int num_jour;
	scanf("%d", &num_jour);
	liste_etudiants(etudiants, absences, num_jour, nombre_etudiants);
}
/*C4 commande*/
void commande_justificatif(Absence absences[], unsigned int nb_absence) {
	unsigned int num_jour, absence_id;
	char justificatif[MAX_JUSTIFICATIF_LENGTH];
	scanf("%d %d", &absence_id, &num_jour);
	getchar(); // Pour consommer le caractère de nouvelle ligne
	fgets(justificatif, MAX_JUSTIFICATIF_LENGTH, stdin);
	depot_justificatif(absences, nb_absence, absence_id, num_jour, justificatif);
}
/*C5 commande*/
void commande_validations(Absence absences[], Etudiant etudiants[], unsigned int nb_absence, unsigned int nb_etudiants) {
	faire_validations(absences, etudiants, nb_absence);
}

/*C6 commande*/
void commande_validation(Absence absences[], unsigned int nb_absence) {
	unsigned int absence_id;
	char validations[MAX_VALIDATION_LENGTH];
	scanf("%d %s", &absence_id, validations);
	validation(absences, absence_id, validations, nb_absence);
}
/*C7 commande*/
void commande_etudiant() {
	// TODO: Implement this command
}
/*C8 commande*/

void commande_liste_defaillants(Etudiant etudiants[], Absence absences[], unsigned int nb_etudiants) {
	unsigned int num_jour;

	scanf("%d", &num_jour);

	liste_defaillants(etudiants, absences, num_jour, nb_etudiants);
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
			commande_validations(absences, etudiants, nb_absence, nombre_etudiants);
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
			commande_liste_defaillants(etudiants, absences, nombre_etudiants);
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