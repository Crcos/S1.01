#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#pragma warning(disable: 4996)

// D�finition des constantes MAX_ETUDIANTS et MAX_NOM_LENGTH � l'aide d'une �num�ration
enum {
	MAX_ETUDIANTS = 100, // Nombre maximum d'�tudiants
	MAX_NOM_LENGTH = 30, // Longueur maximale du nom d'un �tudiant
	MAX_ABSENCES = 100, // Nombre max d'absence autoris�s
	JOURS_MIN = 1,  // Nombre de jours minimales dans un semestre autoriser
	JOURS_MAX = 40, // Nombre de jours maximales dans un semestre autoriser
	MAX_JUSTIFICATIF_LENGTH = 50 + 1, // Longueur maximale pour un justificatif
	MAX_DEMI_JOURNE_LENGTH = 3, // Longueur maximale d'une demi-jorn�e
	MAX_VALIDATION_LENGTH = 3, // Longueur maximale d'un etat pour une validation
	DELAIS_JUSTIFICATIF = 3, // Nombre de jours max pour d�poser un justificatif
	VALIDATION = 50,
};
// Structure pour stocker l'�tat d'une absence
typedef enum {
	JUSTIFICATIF_RECU,
	JUSTIFICATIF_NON_RECU,
	NON_VALIDE,
	VALIDE,
}Etat_abs;

// Structure repr�sentant un �tudiant
typedef struct {
	unsigned int id;  // Identifiant unique de l'�tudiant
	char nom[MAX_NOM_LENGTH + 1]; // Nom de l'�tudiant
	unsigned int groupe; // Groupe de l'�tudiant
} Etudiant;

// Structure repr�sentant une absence
typedef struct {
	unsigned int id; // Identifiant unique de l'absence
	unsigned int demi_journee; // Demi-journ�e concern�e
	unsigned int num_jour; // Num�ro de jour concern�
	char justificatif[MAX_JUSTIFICATIF_LENGTH]; // Justificatif (si enregistr�)
	unsigned int deja_valide; // Stock les absence_id deja valid�s
	Etat_abs etat; // Stock l'etat d'une absence
	unsigned int id_Etudiant; // Id�e de l'�tudiant qui a �t� absent
} Absence;


// V�rifie si une cha�ne est un entier
unsigned int est_entier(const char* str) {
	for (unsigned int i = 0; str[i] != '\0'; i++) {
		if (!isdigit(str[i])) return 0; // Non entier
	}
	return 1; // Entier
}
//**C1**//
//Inscrit un etudiant
void inscription_etudiant(Etudiant etudiants[], char* nom, unsigned int groupe, unsigned int nb_etudiants) {
	// Assigne un nouvel identifiant � l'�tudiant
	etudiants[nb_etudiants].id = nb_etudiants + 1;
	// Copie le nom de l'�tudiant
	strncpy(etudiants[nb_etudiants].nom, nom, MAX_NOM_LENGTH);
	// Assigne le groupe de l'�tudiant
	etudiants[nb_etudiants].groupe = groupe;
	// Affiche un message de confirmation d'inscription
	printf("Inscription enregistree (%d) \n", nb_etudiants + 1);
}

// V�rifie si l'�tudiant est d�j� inscrit avec le m�me nom et groupe
int verif_inscrit(Etudiant etudiants[], unsigned int nb_etudiants, char nom[], unsigned int groupe) {
	// V�rifie si l'�tudiant est d�j� inscrit avec le m�me nom et groupe
	for (unsigned int i = 0; i < nb_etudiants; i++) {
		if (strcmp(etudiants[i].nom, nom) == 0 && etudiants[i].groupe == groupe) {
			return i;
		}
	}
	return -1;
}
//**C2**//
// Enregistre une absence
void enregistrement_absence(Etudiant etudiants[], unsigned int nb_etudiants, Absence absences[], unsigned int id_Etu, unsigned int num_jour, const char* demi_journee, unsigned int* nb_absences) {
	// V�rifie si l'identifiant de l'�tudiant existe
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

	// V�rifie si la demi-journ�e est valide
	if (strcmp(demi_journee, "am") != 0 && strcmp(demi_journee, "pm") != 0) {
		printf("Demi-journee incorrecte\n");
		return;
	}

	// V�rifie si le jour est dans la plage autoris�e
	if (num_jour < JOURS_MIN || num_jour > JOURS_MAX) {
		printf("Date incorrecte\n");
		return;
	}

	// V�rifie si l'absence est d�j� connue
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
	absences[*nb_absences].id = *nb_absences + 1;  // Assignation de l'ID unique � l'absence
	absences[*nb_absences].num_jour = num_jour;
	absences[*nb_absences].demi_journee = (strcmp(demi_journee, "am") == 0) ? 0 : 1;
	absences[*nb_absences].etat = JUSTIFICATIF_NON_RECU;
	printf("Absence enregistree [%d]\n", *nb_absences + 1);
	(*nb_absences)++;
}
//** C3 **//
// Fonction pour comparer deux cellules du tableau d'�tudiants
unsigned int compare_etudiants(const void* a, const void* b) {
	const Etudiant* p1 = (const Etudiant*)a;
	const Etudiant* p2 = (const Etudiant*)b;
	if (p1->groupe < p2->groupe)
		return -1;
	if (p1->groupe > p2->groupe)
		return 1;
	return strcmp(p1->nom, p2->nom);
}
// Fonction qui permet de compter le nombre d'absences d'un �tudiant
unsigned int compter_absence(Absence absences[], unsigned int jour_courant, unsigned int etudiant_id) {
	unsigned int total_absence = 0;
	for (unsigned int i = 0; i < MAX_ABSENCES; i++) {
		if (absences[i].id_Etudiant == etudiant_id && absences[i].num_jour <= jour_courant) {
			total_absence++;
		}
	}
	return total_absence;
}
// Affiche le tableau d'�tudiants complet et tri�
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
// D�p�t d'un justificatif pour une absence
void depot_justificatif(Absence absences[], unsigned int nb_absence, unsigned int absence_id, unsigned int num_jour, const char justificatif[MAX_JUSTIFICATIF_LENGTH]) {

	// V�rifie si l'absence existe
	if (absence_id <= 0 || absence_id > nb_absence) {
		printf("Identifiant incorrect\n");
		return;
	}
	// V�rifie si la date du justificatif est correcte (apr�s la date d'absence)
	if (absences[absence_id - 1].num_jour > num_jour) {
		printf("Date incorrect \n ");
		return;
	}
	// V�rifie si un justificatif a d�j� �t� d�pos�
	if (absences[absence_id - 1].etat == JUSTIFICATIF_RECU && strlen(absences[absence_id - 1].justificatif) > 0) {
		printf("Justificatif deja connu\n");
		return;
	}

	// V�rifie si le d�p�t du justificatif est dans le d�lai autoris�
	if (num_jour > absences[absence_id - 1].num_jour + DELAIS_JUSTIFICATIF) {
		absences[absence_id - 1].etat = NON_VALIDE; // Justificatif d�pos� trop tard => absence non justifi�e
		printf("Justificatif enregistre\n");
	}
	else {
		absences[absence_id - 1].etat = JUSTIFICATIF_RECU; // Justificatif re�u dans les temps
		printf("Justificatif enregistre\n");
	}

	// Enregistre le justificatif (m�me si tardif, pour conserver une trace)
	strncpy(absences[absence_id - 1].justificatif, justificatif, MAX_JUSTIFICATIF_LENGTH);
}

// Fonction pour afficher les absences en attente de validation
void afficher_absences_attente_validation(Absence absences[], unsigned int nb_absence) {
	printf("Liste des absences en attente de validation :\n");
	for (unsigned int i = 0; i < nb_absence; i++) {
		// On n'affiche que les absences en attente de validation (ni valid�es ni non justifi�es)
		if (absences[i].etat == JUSTIFICATIF_NON_RECU) {
			printf("Absence ID: %d, Date: Jour %d, Etat: En attente de validation\n", absences[i].id, absences[i].num_jour);
		}
	}
}

//**C5**//
void faire_validations(Absence absences[], Etudiant etudiants[], unsigned int nb_absence) {
	unsigned int cpt = 0;
	for (unsigned int i = 0; i < nb_absence; i++) {
		if (absences[i].etat != NON_VALIDE && absences[i].etat != JUSTIFICATIF_NON_RECU && absences[i].etat != VALIDE) {
			unsigned int indiceEtu = absences[i].id_Etudiant - 1;

			// Supprimer les sauts de ligne �ventuels � la fin du justificatif
			char* justificatif = absences[i].justificatif;
			size_t len = strlen(justificatif);
			if (len > 0 && justificatif[len - 1] == '\n') {
				justificatif[len - 1] = '\0';  // Remplacer le '\n' par un caract�re de fin de cha�ne
			}
			// Afficher les d�tails correctement 
			printf("[%d] (%d) %s %d %d/%s (%s) \n", absences[i].id, absences[i].id_Etudiant, etudiants[indiceEtu].nom, etudiants[indiceEtu].groupe, absences[i].num_jour, absences[i].demi_journee == 0 ? "am" : "pm", absences[i].justificatif);
			cpt++;
		}
	}
	if (cpt == 0) {
		printf("Aucune validation en attente\n");
	}
}



/** C6 **/
void validation(Absence absences[], unsigned int id_absence, char validation[MAX_VALIDATION_LENGTH], unsigned int nb_absence) {
	// V�rifie si l'identifiant de l'absence est correcte
	if (id_absence == 0) {
		printf("Identifiant incorrect\n");
		return;
	}
	if (id_absence > nb_absence || absences[id_absence - 1].etat < JUSTIFICATIF_RECU) {
		printf("Identifiant incorrect\n");
		return;
	}
	// V�rifie si la validation est d�j� connue
	if (absences[id_absence - 1].etat == VALIDE || absences[id_absence - 1].etat == NON_VALIDE) {
		printf("Validation deja connue\n");
		return;
	}
	// V�rifie si la saisie de l'identifiant est correcte
	if (strcmp(validation, "ok") != 0 && strcmp(validation, "ko") != 0) {
		printf("Code incorrect\n");
		return;
	}
	else {
		printf("Validation enregistree\n");
		absences[id_absence - 1].etat = VALIDE;

	}
}




// C8 Commande pour afficher les �tudiants d�faillants

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
			defaillant_trouve = 1; // Indication qu'un d�faillant a �t� trouv�
		}
	}
	if (defaillant_trouve == 0) {
		printf("Aucun defaillant\n");
	}
}


/*C1 Commande */
//Inscrit un �tudunsigned int
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
	enregistrement_absence(etudiants, nombre_etudiants, absences, etudiant_id, num_jour, demi_journee, nb_absence);
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
	getchar(); // Pour consommer le caract�re de nouvelle ligne
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