// ======================================================================
//  Commentaires écrits par l'intelligence artificielle
//  Programme : launch_game()
//  Description : Logique principale du mini-jeu sur ESP32
//  Fonctionnement : le joueur doit entrer un code secret sur le clavier
//  Si le code est correct → LED de réussite s’allume et message de succès
//  Si le code est incorrect → LED d’erreur clignote et message d’échec
// ======================================================================

#include "led.h"          // Gestion des LED (initialisation, on/off, séquences)
#include "lcd.h"          // Gestion de l’écran LCD (affichage de texte)
#include "keypad.h"       // Gestion du clavier matriciel
#include "push_button.h"  // Gestion du bouton physique
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"      // Journalisation pour le débogage (console série)

// Tag de log, utilisé pour les messages ESP_LOGI
static const char *TAG = "GAME_LOGIC";

// ----------------------------------------------------------------------
// Fonction principale du jeu
// ----------------------------------------------------------------------
void launch_game() {
    // Initialisation de tous les périphériques
    leds_init();       // Prépare les LED (EP1, erreur, etc.)
    button_init();     // Configure le bouton poussoir
    lcd_i2c_init();    // Initialise la communication I2C pour l’écran LCD
    lcd_init();        // Initialise l’écran LCD
    keypad_init();     // Prépare le clavier matriciel

    // Message de confirmation dans le terminal série
    ESP_LOGI(TAG, "Keypad prêt !");

    // Récupération des pointeurs vers les LED spécifiques
    Led *ep1 = get_led_ep1();  // LED de réussite (épisode 1)
    Led *err = get_led_err();  // LED d’erreur

    // Variables pour le mot de passe et le contrôle du jeu
    char password[6] = {0};  // Buffer du code entré (5 caractères + '\0')
    int index = 0;           // Position d’écriture dans le mot de passe
    int sentinelle = 0;      // Sert de condition de sortie de la boucle principale

    // ------------------------------------------------------------------
    // Boucle principale : tourne jusqu’à ce que le bon code soit entré
    // ------------------------------------------------------------------
    while (sentinelle == 0) {

        // Vérifie si le bouton est pressé
        int button_pressed = button_poll() == 1;

        // Affiche le message d’invite sur la première ligne du LCD
        lcd_set_cursor(0, 0);
        lcd_print("Entrez le code:");

        // Lecture d’une touche du clavier matriciel
        char key = keypad_scan();

        // Si le bouton est pressé, joue une séquence en Morse via les LED
        if (button_pressed) {
            leds_morse_sequence("b947d");
        }

        // Si une touche a été détectée
        if (key != '\0') {
            password[index++] = key;  // Ajoute la touche au mot de passe
            password[index] = '\0';   // Termine la chaîne proprement

            lcd_clear();              // Efface l’écran
            lcd_set_cursor(1, 0);     // Se place sur la 2ᵉ ligne
            lcd_print(password);      // Affiche le code tapé

            // Quand 5 caractères sont saisis
            if (index >= 5) {
                // Vérifie si le code est correct
                if (strcmp(password, "B947D") == 0) {
                    led_on(ep1);                          // Allume LED de réussite
                    vTaskDelay(pdMS_TO_TICKS(500));
                    lcd_set_cursor(0, 0);
                    lcd_print("Reussite!");               // Message de succès
                    lcd_set_cursor(1, 0);
                    lcd_print("Wait for part 2!");        // Indique la suite
                    vTaskDelay(pdMS_TO_TICKS(500));

                    sentinelle = 1;                       // Sortie de la boucle
                }
                else {
                    // Code incorrect → LED rouge + message d’erreur
                    led_on(err);
                    vTaskDelay(pdMS_TO_TICKS(500));
                    lcd_set_cursor(0, 0);
                    lcd_print("Nope!");
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    led_off(err);                         // Éteint la LED erreur
                    lcd_clear();                          // Réinitialise l’affichage
                }
                
                // Réinitialise les variables pour une nouvelle tentative
                vTaskDelay(pdMS_TO_TICKS(1000));
                index = 0;
            }
        }

        // Petite pause pour ne pas surcharger le CPU
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
