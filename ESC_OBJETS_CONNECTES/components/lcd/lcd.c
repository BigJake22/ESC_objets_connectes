// ======================================================================
//  Commentaires et code écrits par L'intelligence artificielle
//  Module : lcd.c
//  Description : Contrôle d’un écran LCD 16x2 via un module I2C (PCF8574)
//  Fonctionnement : 
//    - Initialise l’interface I2C sur l’ESP32.
//    - Traduit les commandes HD44780 en signaux I2C.
//    - Permet d’afficher du texte, effacer l’écran et positionner le curseur.
// ======================================================================

// ----- Dépendances principales -----
#include "lcd.h"                  // En-tête du module LCD (fonctions publiques)
#include "driver/i2c.h"           // Pilote I2C de l’ESP-IDF
#include "freertos/FreeRTOS.h"    // Système d’exploitation temps réel
#include "freertos/task.h"
#include "esp_rom_sys.h"          // Délai en microsecondes
#include "esp_log.h"              // Logs pour débogage

// ----- Paramètres matériels I2C -----
#define I2C_MASTER_NUM I2C_NUM_0  // Utilisation du bus I2C n°0
#define SDA_PIN 21                // Broche SDA (données)
#define SCL_PIN 22                // Broche SCL (horloge)
#define LCD_ADDR 0x27             // Adresse I2C du module PCF8574
#define I2C_FREQ_HZ 100000        // Fréquence I2C (100 kHz standard)

// ----- Bits de contrôle du PCF8574 -----
#define PIN_RS 0x01  // Register Select : 0 = commande, 1 = données
#define PIN_RW 0x02  // Read/Write : 0 = écriture, 1 = lecture (jamais utilisé ici)
#define PIN_EN 0x04  // Enable : déclenchement de la lecture par le LCD
#define PIN_BL 0x08  // Backlight : allume le rétroéclairage

// Tag de log pour affichage console
static const char *TAG = "lcd";

// ----------------------------------------------------------------------
// Initialisation de l’interface I2C
// Configure l’ESP32 en maître I2C pour communiquer avec le PCF8574
// ----------------------------------------------------------------------
void lcd_i2c_init(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA_PIN,
        .scl_io_num = SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_FREQ_HZ,
    };

    // Application de la configuration et activation du pilote
    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));
}

// ----------------------------------------------------------------------
// Envoie un octet brut au module LCD via I2C
// ----------------------------------------------------------------------
static void lcd_write(uint8_t data) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();                  // Création d'une commande I2C
    i2c_master_start(cmd);                                         // Démarrage de la communication
    i2c_master_write_byte(cmd, (LCD_ADDR << 1) | I2C_MASTER_WRITE, true); // Adresse + bit écriture
    i2c_master_write_byte(cmd, data, true);                        // Envoi de la donnée
    i2c_master_stop(cmd);                                          // Fin de transmission
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(100)); // Exécution de la commande
    i2c_cmd_link_delete(cmd);                                      // Libération
}

// ----------------------------------------------------------------------
// Envoie une impulsion sur la broche EN pour valider une commande LCD
// (le LCD lit la donnée lors de la transition HIGH→LOW sur EN)
// ----------------------------------------------------------------------
static void lcd_pulse(uint8_t data) {
    lcd_write(data | PIN_EN);      // Met EN à 1
    esp_rom_delay_us(600);         // Attente (~0.6 ms)
    lcd_write(data & ~PIN_EN);     // Met EN à 0
    esp_rom_delay_us(600);
}

// ----------------------------------------------------------------------
// Envoi d’une commande ou donnée sur 8 bits au LCD (en 4 bits à la fois)
// Le module LCD fonctionne en mode "4-bit" : chaque octet est divisé
// ----------------------------------------------------------------------
static void lcd_send(uint8_t value, uint8_t mode) {
    uint8_t high = (value & 0xF0);        // Quatre bits de poids fort
    uint8_t low  = (value << 4) & 0xF0;   // Quatre bits de poids faible

    lcd_pulse(high | mode | PIN_BL);      // Envoi des 4 bits hauts
    lcd_pulse(low  | mode | PIN_BL);      // Puis des 4 bits bas
}

// ----------------------------------------------------------------------
// Envoie une commande de contrôle (ex: effacer, déplacer curseur)
// ----------------------------------------------------------------------
static void lcd_cmd(uint8_t cmd) {
    lcd_send(cmd, 0x00);                  // mode=0 → commande
    vTaskDelay(pdMS_TO_TICKS(2));         // Petit délai de traitement
}

// ----------------------------------------------------------------------
// Envoie une donnée affichable (caractère ASCII)
// ----------------------------------------------------------------------
static void lcd_data(uint8_t data) {
    lcd_send(data, PIN_RS);               // mode=RS → écriture de texte
    esp_rom_delay_us(600);
}

// ----------------------------------------------------------------------
// Efface l’écran LCD
// ----------------------------------------------------------------------
void lcd_clear(void) {
    lcd_cmd(0x01);                        // Commande "Clear display"
    vTaskDelay(pdMS_TO_TICKS(5));         // Attente complète du cycle
}

// ----------------------------------------------------------------------
// Initialise le LCD en mode 4 bits selon la séquence HD44780
// ----------------------------------------------------------------------
void lcd_init(void) {
    vTaskDelay(pdMS_TO_TICKS(50));        // Attente après mise sous tension

    // Séquence d’initialisation 8 bits → 4 bits
    lcd_write(0x30);
    vTaskDelay(pdMS_TO_TICKS(5));
    lcd_write(0x30);
    esp_rom_delay_us(600);
    lcd_write(0x20);                      // Passage en mode 4 bits
    vTaskDelay(pdMS_TO_TICKS(5));

    // Configuration du mode d’affichage
    lcd_cmd(0x28);                        // 4 bits, 2 lignes, police 5x8
    lcd_cmd(0x0C);                        // Écran ON, curseur OFF
    lcd_cmd(0x06);                        // Incrément automatique du curseur
    lcd_clear();                          // Efface tout

    ESP_LOGI(TAG, "LCD initialisé");
}

// ----------------------------------------------------------------------
// Positionne le curseur à une ligne et colonne donnée
// row = 0 ou 1, col = 0..15
// ----------------------------------------------------------------------
void lcd_set_cursor(int row, int col) {
    static const uint8_t offsets[] = {0x00, 0x40}; // Adresse de début ligne
    lcd_cmd(0x80 | (col + offsets[row]));          // Commande Set DDRAM Address
}

// ----------------------------------------------------------------------
// Affiche une chaîne de caractères sur le LCD
// ----------------------------------------------------------------------
void lcd_print(const char *str) {
    while (*str) lcd_data(*str++);        // Envoie chaque caractère
}
