// ======================================================================
//  Commentaires et code écrits par L'intelligence artificielle
//  Module : led.c
//  Description : Gestion des LEDs et génération de signaux lumineux en Morse
//  Fonctionnement :
//     - Initialise trois LEDs (EP1, EP2, ERR)
//     - Fournit des fonctions pour allumer, éteindre et alterner leur état
//     - Encode un message texte en signaux lumineux selon le code Morse
// ======================================================================

#include "led.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <ctype.h>                 // Pour toupper()
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Tag de log pour le terminal
static const char *TAG = "led.c";

// ----------------------------------------------------------------------
//  Définition des GPIO associés à chaque LED
// ----------------------------------------------------------------------
#define LED_GPIO_EP1 5   // LED bleue (succès épisode 1)
#define LED_GPIO_EP2 4   // LED verte (indicateur Morse)
#define LED_GPIO_ERR 2   // LED rouge (erreur ou échec)

// ----------------------------------------------------------------------
//  Durées en millisecondes pour les signaux Morse
// ----------------------------------------------------------------------
#define DOT 200            // Point : 200 ms
#define DASH (3 * DOT)     // Tiret : 600 ms
#define SYMBOL_SPACE DOT   // Pause entre symboles d'une même lettre
#define LETTER_SPACE (5 * DOT) // Pause entre lettres
#define WORD_SPACE (7 * DOT)   // Pause entre mots

// ----------------------------------------------------------------------
//  Définition des objets LED : structure Led définie dans led.h
// ----------------------------------------------------------------------
static Led led_ep1 = {LED_GPIO_EP1, 0};
static Led led_ep2 = {LED_GPIO_EP2, 0};
static Led led_err = {LED_GPIO_ERR, 0};

// ----------------------------------------------------------------------
//  Table du code Morse (lettre → motif . et -)
// ----------------------------------------------------------------------
typedef struct {
    char letter;
    const char *pattern;
} morse_t;

// Cette constante a été générée par IA pour représenter l’alphabet Morse complet.
static const morse_t morse_table[] = {
    {'A', ".-"}, {'B', "-..."}, {'C', "-.-."}, {'D', "-.."}, {'E', "."},
    {'F', "..-."}, {'G', "--."}, {'H', "...."}, {'I', ".."}, {'J', ".---"},
    {'K', "-.-"}, {'L', ".-.."}, {'M', "--"}, {'N', "-."}, {'O', "---"},
    {'P', ".--."}, {'Q', "--.-"}, {'R', ".-."}, {'S', "..."}, {'T', "-"},
    {'U', "..-"}, {'V', "...-"}, {'W', ".--"}, {'X', "-..-"}, {'Y', "-.--"},
    {'Z', "--.."},
    {'1', ".----"}, {'2', "..---"}, {'3', "...--"}, {'4', "....-"}, {'5', "....."},
    {'6', "-...."}, {'7', "--..."}, {'8', "---.."}, {'9', "----."}, {'0', "-----"}
};

// ----------------------------------------------------------------------
//  Initialisation de toutes les LEDs (sorties GPIO)
// ----------------------------------------------------------------------
void leds_init(void) {
    ESP_LOGI(TAG, "Initialisation de toutes les LEDs...");

    gpio_reset_pin(led_ep1.gpio);
    gpio_set_direction(led_ep1.gpio, GPIO_MODE_OUTPUT);

    gpio_reset_pin(led_ep2.gpio);
    gpio_set_direction(led_ep2.gpio, GPIO_MODE_OUTPUT);

    gpio_reset_pin(led_err.gpio);
    gpio_set_direction(led_err.gpio, GPIO_MODE_OUTPUT);
}

// ----------------------------------------------------------------------
//  Allume une LED donnée
// ----------------------------------------------------------------------
void led_on(Led *led) {
    led->state = 1;
    gpio_set_level(led->gpio, 1);
    ESP_LOGI(TAG, "LED GPIO %d ON", led->gpio);
}

// ----------------------------------------------------------------------
//  Éteint une LED donnée
// ----------------------------------------------------------------------
void led_off(Led *led) {
    led->state = 0;
    gpio_set_level(led->gpio, 0);
    ESP_LOGI(TAG, "LED GPIO %d OFF", led->gpio);
}

// ----------------------------------------------------------------------
//  (Réservé) — fonction vide pour extensions futures
// ----------------------------------------------------------------------
void led_morse_sequence(Led *led) {
    // Cette fonction pourrait être utilisée pour déclencher une séquence prédéfinie
}

// ----------------------------------------------------------------------
//  Bascule l’état d’une LED (si ON → OFF, si OFF → ON)
// ----------------------------------------------------------------------
void led_toggle(Led *led) {
    led->state = !led->state;
    gpio_set_level(led->gpio, led->state);
    ESP_LOGI(TAG, "LED GPIO %d %s", led->gpio, led->state ? "ON" : "OFF");
}

// ----------------------------------------------------------------------
//  Fonction écrite par l'intelligence artificielle
//  Fonction interne : clignotement d’un message en code Morse
// ----------------------------------------------------------------------
//  Pour chaque caractère du texte :
//   - conversion en majuscule
//   - recherche du motif (. ou -)
//   - allumage/temporisation/extinction selon la durée correspondante
// ----------------------------------------------------------------------
static void morse_flash(Led *led, const char *text) {
    for (int i = 0; text[i] != '\0'; i++) {
        char c = toupper(text[i]); // Normalisation en majuscule

        // Espace entre les mots
        if (c == ' ') {
            vTaskDelay(pdMS_TO_TICKS(WORD_SPACE));
            continue;
        }

        // Recherche du caractère dans la table Morse
        for (int j = 0; j < sizeof(morse_table) / sizeof(morse_t); j++) {
            if (morse_table[j].letter == c) {
                const char *pattern = morse_table[j].pattern;

                // Parcourt du motif (. ou -)
                for (int k = 0; pattern[k] != '\0'; k++) {
                    led_on(led);
                    vTaskDelay(pdMS_TO_TICKS(pattern[k] == '.' ? DOT : DASH));
                    led_off(led);
                    vTaskDelay(pdMS_TO_TICKS(SYMBOL_SPACE)); // Pause entre symboles
                }

                // Pause entre lettres
                vTaskDelay(pdMS_TO_TICKS(LETTER_SPACE));
                break;
            }
        }
    }
}

// ----------------------------------------------------------------------
//  Fonction écrite par l'intelligence artificielle
//  Fonction publique : démarre une séquence Morse complète
// ----------------------------------------------------------------------
void leds_morse_sequence(const char *message) {
    ESP_LOGI(TAG, "Début de la séquence Morse : %s", message);
    Led *led = get_led_ep2();           // Utilise la LED verte pour le signal
    morse_flash(led, message);          // Exécution de la séquence
    ESP_LOGI(TAG, "Séquence Morse terminée.");
}

// ----------------------------------------------------------------------
//  Accès aux objets LED (interface publique du module)
// ----------------------------------------------------------------------
Led* get_led_ep1(void) { return &led_ep1; }
Led* get_led_ep2(void) { return &led_ep2; }
Led* get_led_err(void) { return &led_err; }
