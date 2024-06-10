#include <mbed.h>
#include <threadLvgl.h>
#include "demos/lv_demos.h"
#include <cstdio>

ThreadLvgl threadLvgl(30);

// Configuration des pins pour les servomoteurs et les capteurs
PwmOut servoPin1(PA_8);
PwmOut servoPin2(PB_15);
DigitalIn sensor1(D2);
DigitalIn sensor2(D3);
DigitalIn sensor3(D4);
DigitalIn sensor4(D5);


// Constantes pour la période du servo et les positions
const int SERVO_PERIOD_MS = 20; // Période du servo en ms

// Constantes pour le servomoteur FT90M
const float FT90M_pulseMin = 0.001;  // Impulsion pour pivoter le servo à 0º (1000 μs)
const float FT90M_pulseMax = 0.002;  // Impulsion pour pivoter le servo à 280º (2000 μs)

// Constantes pour les servomoteurs standards
const float pulseMin = 0.00065;  // Impulsion pour pivoter le servo à 0º (650 μs)
const float pulseMax = 0.00255;  // Impulsion pour pivoter le servo à 180º (2550 μs)

// Variables pour stocker les valeurs de mesure en degrés
int angle1 = 0;  // Initialisé à la position correspondante
int angle2 = 0;  // Initialisé à la position correspondante

lv_obj_t *servo1_label;
lv_obj_t *servo2_label;

// Fonction pour mapper les valeurs (similaire à la fonction Arduino map)
float map(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Fonction pour définir l'angle du servo standard
void setServoAngle(PwmOut &servoPin, int angle) {
    float pulseWidth = map(angle, 0, 180, pulseMin, pulseMax);
    servoPin.pulsewidth(pulseWidth);
}

// Fonction pour définir l'angle du servomoteur FT90M
void setServoAngleFT90M(PwmOut &servoPin, int angle) {
    float pulseWidth = map(angle, 0, 280, FT90M_pulseMin, FT90M_pulseMax);
    servoPin.pulsewidth(pulseWidth);
}

// Fonction pour récupérer la position du servomoteur
int getServoAngle(PwmOut &servoPin) {
    float pulseWidth = servoPin.read();
    return (int)map(pulseWidth, pulseMin, pulseMax, 0, 180);
}

// Fonction pour mettre à jour le label de position du servomoteur
void updateServoLabel(lv_obj_t *label, int angle) {
    lv_label_set_text_fmt(label, "Angle: %d deg", angle);
}

// Fonction pour faire bouger le servo de 0 à 180 degrés et vice-versa
void moveServo(PwmOut &servoPin, lv_obj_t *label) {
    for (int pos = 0; pos <= 180; pos += 1) {
        setServoAngle(servoPin, pos);
        ThisThread::sleep_for(10ms);
        updateServoLabel(label, pos);
    }
    for (int pos = 180; pos >= 0; pos -= 1) {
        setServoAngle(servoPin, pos);
        ThisThread::sleep_for(10ms);
        updateServoLabel(label, pos);
    }
}

// Fonction de rappel pour le premier bouton LVGL
static void btn_event_cb1(lv_event_t *e) {
    moveServo(servoPin1, servo1_label);
}

// Fonction de rappel pour le deuxième bouton LVGL
static void btn_event_cb2(lv_event_t *e) {
    moveServo(servoPin2, servo2_label);
}

void create_button1() {
    // Créer un premier bouton sur l'écran pour le premier servomoteur
    lv_obj_t *btn1 = lv_btn_create(lv_scr_act());
    lv_obj_set_pos(btn1, 50, 50);
    lv_obj_set_size(btn1, 120, 50);
    lv_obj_add_event_cb(btn1, btn_event_cb1, LV_EVENT_CLICKED, NULL);
    lv_obj_t *label1 = lv_label_create(btn1);
    lv_label_set_text(label1, "Servo 1");
    lv_obj_center(label1);
}

void create_button2() {
    // Créer un deuxième bouton sur l'écran pour le deuxième servomoteur
    lv_obj_t *btn2 = lv_btn_create(lv_scr_act());
    lv_obj_set_pos(btn2, 50, 150);
    lv_obj_set_size(btn2, 120, 50);
    lv_obj_add_event_cb(btn2, btn_event_cb2, LV_EVENT_CLICKED, NULL);
    lv_obj_t *label2 = lv_label_create(btn2);
    lv_label_set_text(label2, "Servo 2");
    lv_obj_center(label2);
}

int main() {
    // Initialiser les signaux PWM pour les servomoteurs
    servoPin1.period_ms(SERVO_PERIOD_MS);
    setServoAngleFT90M(servoPin1, 0); // Position initiale du servomoteur FT90M

    servoPin2.period_ms(SERVO_PERIOD_MS);
    setServoAngleFT90M(servoPin2, 0); // Position initiale du servomoteur FT90M

    // Attendre pour laisser les servos se stabiliser
    ThisThread::sleep_for(100ms); 
    
    // Initialiser LVGL
    threadLvgl.lock();
    
    create_button1();
    create_button2();

    // Créer un label pour afficher la valeur de mesure du premier servomoteur
    servo1_label = lv_label_create(lv_scr_act());
    lv_obj_align(servo1_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_label_set_text_fmt(servo1_label, "Angle: %d deg", angle1);

    // Créer un label pour afficher la valeur de mesure du deuxième servomoteur
    servo2_label = lv_label_create(lv_scr_act());
    lv_obj_align(servo2_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 110);
    lv_label_set_text_fmt(servo2_label, "Angle: %d deg", angle2);

    // Créer des labels pour afficher l'état des capteurs
    lv_obj_t *sensor1_label = lv_label_create(lv_scr_act());
    lv_obj_align(sensor1_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 210);
    lv_label_set_text(sensor1_label, "Sensor 1: 1");

    lv_obj_t *sensor2_label = lv_label_create(lv_scr_act());
    lv_obj_align(sensor2_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 230);
    lv_label_set_text(sensor2_label, "Sensor 2: 1");

    lv_obj_t *sensor3_label = lv_label_create(lv_scr_act());
    lv_obj_align(sensor3_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 250);
    lv_label_set_text(sensor3_label, "Sensor 3: 1");

    lv_obj_t *sensor4_label = lv_label_create(lv_scr_act());
    lv_obj_align(sensor4_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 270);
    lv_label_set_text(sensor4_label, "Sensor 4: 1");

    threadLvgl.unlock();

    while (1) {
        // Vérifier l'état du capteur D6 et faire bouger le servo si le capteur est activé
        if (sensor2.read() && sensor1.read() == 1) {
            // Faire tourner le servomoteur de 0 à 180 degrés et vice-versa
            moveServo(servoPin1, servo1_label);
            // Revenir à la position d'origine
            setServoAngle(servoPin1, 0);
            updateServoLabel(servo1_label, 0);
        }


        if (sensor3.read() && sensor4.read() == 1) {
            // Faire tourner le servomoteur de 0 à 180 degrés et vice-versa
            moveServo(servoPin2, servo2_label);
            // Revenir à la position d'origine
            setServoAngle(servoPin2, 0);
            updateServoLabel(servo2_label, 0);
        }

        // Mettre à jour les labels des positions des servomoteurs
        updateServoLabel(servo1_label, angle1);
        updateServoLabel(servo2_label, angle2);

        // Mettre à jour les labels des capteurs avec valeurs inversées
        lv_label_set_text_fmt(sensor1_label, "Sensor 1: %d", !sensor1.read());
        lv_label_set_text_fmt(sensor2_label, "Sensor 2: %d", !sensor2.read());
        lv_label_set_text_fmt(sensor3_label, "Sensor 3: %d", !sensor3.read());
        lv_label_set_text_fmt(sensor4_label, "Sensor 4: %d", !sensor4.read());

        // Boucle principale
        ThisThread::sleep_for(20ms);
    }
}
