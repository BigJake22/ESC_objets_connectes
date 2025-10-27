// ======================================================================
//  Commentaires écrits par l'intelligence artificielle
//  Fichier : main.c
//  Description : Point d’entrée principal du programme ESP32
//  Fonctionnement :
//      - Initialise le système FreeRTOS.
//      - Lance la logique du jeu via la fonction launch_game().
// ======================================================================

#include "game_logic.h"        // Déclaration de launch_game()
#include "freertos/FreeRTOS.h" // OS temps réel de l’ESP32
#include "freertos/task.h"
#include "esp_log.h"           // Journalisation dans la console série

// Tag de log pour identifier les messages dans le terminal
static const char *TAG = "MAIN";

// ----------------------------------------------------------------------
//  Fonction principale (équivalent de main() dans un programme classique)
//  app_main() est le point d’entrée des projets ESP-IDF.
//  Elle est exécutée une seule fois au démarrage et tourne dans une tâche FreeRTOS.
// ----------------------------------------------------------------------
void app_main(void) {
    ESP_LOGI(TAG, "Démarrage du jeu...");
    
    // Lance la logique principale du jeu (boucle keypad/LCD/LED)
    launch_game();

    // Si launch_game() se termine, le programme atteindra ce point.
    ESP_LOGI(TAG, "Le jeu est terminé, app_main() quitte.");
}
