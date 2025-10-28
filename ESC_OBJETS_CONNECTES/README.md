//Readme rédigé en partie par l'intelligence artificielle

Boîte noire ESP32 — Prototype TP1
🎯 Objectif du projet

Ce projet implémente un prototype interactif simulant la boîte noire d’un avion expérimental.
L’utilisateur doit entrer un code secret sur un clavier matriciel pour “déverrouiller” le système, avec un retour visuel sur un écran LCD et des LEDs d’état.

🧩 Matériel utilisé
Composant	Rôle	GPIO utilisés
ESP32 DevKit v1	Microcontrôleur principal	—
Écran LCD I2C (0x27)	Affichage texte	SDA : 21, SCL : 22
Clavier matriciel 4x4	Entrée utilisateur	Lignes : 13, 19, 14, 27 / Colonnes : 26, 25, 33, 32
Bouton poussoir	Déclencheur Morse	GPIO 23
LED bleue (EP2)	Transmission Morse	GPIO 4
LED verte (EP1)	Indicateur de réussite	GPIO 5
LED rouge (ERR)	Indicateur d’erreur	GPIO 2
🧠 Fonctionnement général

Au démarrage, l’écran LCD affiche “Entrez le code:”.

L’utilisateur saisit un code à 5 caractères via le clavier matriciel.

Si le code est correct (B947D) :

La LED verte s’allume.

Le LCD affiche “Réussite! Wait for part 2!”

Si le code est erroné :

La LED rouge s’allume brièvement.

L’écran affiche “Nope!” avant de réinitialiser.

Le bouton GPIO 23 déclenche une séquence lumineuse Morse correspondant au mot b947d, jouée sur la LED bleue.

🧱 Organisation du code

lcd.c/h : communication I2C et contrôle de l’écran LCD.

keypad.c/h : lecture des touches du clavier matriciel (anti-rebond inclus).

push_button.c/h : lecture du bouton via gpio_get_level().

led.c/h : gestion des LEDs et génération du code Morse.

game_logic.c/h : boucle principale du jeu, intégration des modules.

main.c : point d’entrée, lance launch_game().

🧩 Compilation et flash
Étapes sous ESP-IDF :

Cloner le projet :

git clone https://github.com/BigJake22/ESC_objets_connectes.git
cd projet_boite_noire_esp32


Sélectionner le port série (COM7) :

idf.py set-target esp32
idf.py -p PORT build


Flasher le programme :

idf.py -p PORT flash

Protocole UART utilisé pour le flash


Lancer le moniteur série pour observer les logs :

idf.py monitor

🧪 Tests

Cas 1 : Entrer le bon code B947D → affichage “Réussite”.

Cas 2 : Code erroné → message “Nope!”.

Cas 3 : Appuyer sur le bouton → séquence Morse visible sur LED bleue.

👩‍💻 Auteur

Projet réalisé par Jacob Bergeron, dans le cadre du cours de systèmes embarqués (ESP32 / ESP-IDF).
Les commentaires et certaines parties du code ont été réalisés avec l’assistance d’une intelligence artificielle pour assurer la clarté pédagogique.

📄 Licence

Usage pédagogique uniquement. Reproduction ou réutilisation à des fins commerciales interdite sans autorisation.

✅ Conseils finaux avant livraison

Vérifiez que chaque .h contient bien des gardes d’inclusion :

#ifndef LCD_H
#define LCD_H
// ...
#endif


Supprimez les fichiers inutiles (.vscode, .git, etc.) avant de compresser.

Testez une compilation propre (idf.py fullclean && idf.py build) avant de livrer.

Assurez-vous que votre projet fonctionne sur une carte ESP32 DevKit v1 standard.
