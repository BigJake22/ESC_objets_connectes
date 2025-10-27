// ======================================================================
//  Commentaires écrits par l'intelligence artificielle
//  Module : keypad.c
//  Description : Gestion d’un clavier matriciel 4x4 avec l’ESP32
//  Fonctionnement : Les 4 lignes sont activées successivement (en sortie).
//                   Les 4 colonnes sont lues (en entrée) pour détecter
//                   quelle touche est pressée selon l’intersection.
// ======================================================================

// Bibliothèques nécessaires
#include "driver/gpio.h"        // Contrôle des broches GPIO
#include "esp_log.h"            // Journalisation (logs pour débogage)
#include "freertos/FreeRTOS.h"  // Système d’exploitation temps réel
#include "freertos/task.h"      // Gestion des délais et des tâches

// Tag de log (identifie les messages dans la console série)
static const char *TAG = "keypad";

// ----------------------------------------------------------------------
// Définition de la matrice des touches
// Chaque élément correspond à la touche physique à l’intersection
// de la ligne (row) et de la colonne (col).
// ----------------------------------------------------------------------
char keys[4][4] = {
    {'5', '6', 'B', '7'},
    {'8', '9', 'C', '*'},
    {'0', '#', 'D', '1'},
    {'2', '3', 'A', '4'}
};

// ----------------------------------------------------------------------
// Définition des broches GPIO associées aux lignes et colonnes
// (adapter selon votre câblage sur la carte ESP32)
// ----------------------------------------------------------------------
int rowPins[4] = {13, 19, 14, 27};  // Lignes → sorties
int colPins[4] = {26, 25, 33, 32};  // Colonnes → entrées

// ----------------------------------------------------------------------
// Initialisation du clavier
// Configure les GPIO selon leur rôle (ligne ou colonne)
// ----------------------------------------------------------------------
void keypad_init(void) {
    // Configuration des lignes comme sorties
    for (int i = 0; i < 4; i++) {
        gpio_reset_pin(rowPins[i]);                 // Réinitialise la broche
        gpio_set_direction(rowPins[i], GPIO_MODE_OUTPUT); // Définit en sortie
        gpio_set_level(rowPins[i], 1);              // État haut par défaut
    }

    // Configuration des colonnes comme entrées avec résistance pull-up
    for (int i = 0; i < 4; i++) {
        gpio_reset_pin(colPins[i]);                 // Réinitialise la broche
        gpio_set_direction(colPins[i], GPIO_MODE_INPUT);  // Définit en entrée
        gpio_pullup_en(colPins[i]);                 // Active la résistance interne
    }
}

// ----------------------------------------------------------------------
// Lecture d’une touche sur le clavier
// Parcourt chaque ligne en la mettant à 0 pour voir quelle colonne
// devient également à 0 → cela indique la touche pressée.
// ----------------------------------------------------------------------
char keypad_scan(void) {
    for (int row = 0; row < 4; row++) {
        gpio_set_level(rowPins[row], 0); // Active la ligne courante

        // Vérifie chaque colonne associée
        for (int col = 0; col < 4; col++) {
            // Si une colonne passe à 0, une touche est pressée
            if (gpio_get_level(colPins[col]) == 0) {
                vTaskDelay(pdMS_TO_TICKS(100)); // Délai anti-rebond mécanique

                // Double vérification pour éviter les faux positifs
                if (gpio_get_level(colPins[col]) == 0) {
                    gpio_set_level(rowPins[row], 1); // Désactive la ligne
                    return keys[row][col];           // Retourne le caractère
                }
            }
        }

        gpio_set_level(rowPins[row], 1); // Remet la ligne à l’état haut
    }

    // Si aucune touche n’est pressée
    return '\0';
}
