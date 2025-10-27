// ======================================================================
//  Commentaires écrits par l'intelligence artificielle
// Module : push_button.c
//  Description : Gestion d’un bouton-poussoir sur ESP32
//  Fonctionnement :
//    - Configure un GPIO comme entrée pour détecter un appui.
//    - Fournit une fonction d’interrogation simple (polling) du niveau logique.
//    - Retourne 1 si le bouton est appuyé, 0 sinon.
// ======================================================================

#include "push_button.h"
#include "driver/gpio.h"
#include "esp_log.h"

// Tag pour l’affichage des messages de log dans la console série
static const char *TAG = "push_button.c";

// Variable interne stockant le numéro de broche utilisée
static gpio_num_t s_button_gpio = -1;

// Broche utilisée par le bouton (GPIO 23 = entrée classique avec pull-down interne)
#define PUSH_BUTTON_GPIO 23

// ----------------------------------------------------------------------
//  Initialisation du bouton
//  - Définit la broche comme entrée
//  - Configure le mode de tirage à 0V (pull-down)
//  - Cette fonction doit être appelée avant toute lecture avec button_poll()
// ----------------------------------------------------------------------
void button_init()
{
    ESP_LOGI(TAG, "Configuration du GPIO %d en entrée", PUSH_BUTTON_GPIO);

    s_button_gpio = PUSH_BUTTON_GPIO;          // Mémorise le GPIO utilisé
    gpio_reset_pin(s_button_gpio);             // Réinitialise la broche
    gpio_set_direction(s_button_gpio, GPIO_MODE_INPUT);  // Définit en entrée

    // Active une résistance interne de pull-down (maintient à 0 lorsqu’inactif)
    gpio_set_pull_mode(s_button_gpio, GPIO_PULLDOWN_ONLY);
}

// ----------------------------------------------------------------------
//  Lecture de l’état du bouton
//  - Renvoie 1 si le bouton est pressé, 0 sinon.
//  - Utilise une lecture directe (polling) sans interruption.
//  - Log le résultat pour le débogage (console série).
// ----------------------------------------------------------------------
int button_poll(void)
{
    int level = gpio_get_level(s_button_gpio);  // Lecture du niveau logique
    ESP_LOGI(TAG, "Bouton %s", level ? "appuyé" : "relâché");
    return level;
}
