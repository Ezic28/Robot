/*
Projet: Le nom du script
Equipe: Votre numero d'equipe
Auteurs: Les membres auteurs du script
Description: Breve description du script
Date: Derniere date de modification
*/

/******************************************************************************
Inclure les librairies de functions que vous voulez utiliser
******************************************************************************/

#include <LibRobus.h> // Essentielle pour utiliser RobUS
#include <math.h>


/******************************************************************************
Variables globales et defines
******************************************************************************/
// -> defines...
// L'ensemble des fonctions y ont accès
const float INCREMENTATION_ACCEL = 0.03; // Constante d'incrémentation pour l'accélération
const float DIAMETRE_ROUE = 7.7; // Constante pour le diamètre de la roue
const float DIAMETRE_ROBOT = 39.15; // Constante diamètre du robot
const int ENCOCHES_TOTAL = 3200; // Constante encoches total des encodeurs
const float DISTANCE_PAR_ENCOCHE = (DIAMETRE_ROUE * M_PI) / ENCOCHES_TOTAL; // Constante distance par encoche
const float KP = 0.002; // Constante pour le quotient proportionnel
const int DISTANCE_DECELERATION = 26; // Constante pour la valeur de décélération
bool isForward = false; // bool qui permet de vérifier si le robot avance
float speedTourner = 0.3; // Vitesse pour faire tourner le robot
//uih


/******************************************************************************
Vos propres fonctions sont créées ici
******************************************************************************/

/*
 * - Methode qui sert a calculer la distance en coche, call le PID et a reset les moteur à 0
 * 
 * - distanceEnCm : Distance à parcourir en completer
 * - desiredSpeed : Vitesse de base défini pour le robot (Sans compter l'accélération et la décélération)
 */
void Forward(float distanceEnCm, float desiredSpeed)
{
  float distanceReelle = distanceEnCm/DISTANCE_PAR_ENCOCHE;
  PidLigneDroite(desiredSpeed, distanceReelle);
  MOTOR_SetSpeed(0, 0);
  MOTOR_SetSpeed(1, 0);
}

/*
 * 
 * - Methode qui fait le PID pour avancer en ligne droite, fait accélérer, 
 *   et décélérer le robot.
 * 
 * - desiredSpeed : Vitesse de base défini pour le robot (Sans compter l'accélération et la décélération)
 * - distanceEnCoche : Distance en nombre d'encoche à parcourir
 * - noAccel : Bool qui permet de définir une accélération au robot ou non
 */
void PidLigneDroite(float desiredSpeed, int distanceEnCoche)
{
  float currentSpeed = 0; // Vitesse actuel du robot
  float smallDist = 40 / DISTANCE_PAR_ENCOCHE; // Valeur pour la distance minimal, 
  bool noAccel = false; // bool qui met la variable noAccel à false par défaut

  // Si la distance est trop petite on élimine l'accélération
  if(distanceEnCoche < smallDist){
    noAccel = true;
    currentSpeed = 0.25;
  }

  // L'incrémentation de la constante INCREMENTATION_ACCEL est rapide donc mettre le nombre 10 est plus précis que de mettre 0
  while(distanceEnCoche > 10)
  {
    // Accélération jusqu'à la vitesse voulu
    if (currentSpeed < desiredSpeed && !noAccel)
    {
      currentSpeed = currentSpeed + INCREMENTATION_ACCEL;
      // Si l'incrémention dépasse la vitesse désiré ajuste la vitesse à celle défini
      if (currentSpeed > desiredSpeed)
        currentSpeed = desiredSpeed;
    }

    // Code pour décélérer
    if(distanceEnCoche < DISTANCE_DECELERATION / DISTANCE_PAR_ENCOCHE && !noAccel)
    {
      desiredSpeed = 0;
      if (currentSpeed > desiredSpeed)
      {
      currentSpeed = currentSpeed - INCREMENTATION_ACCEL;
      if (currentSpeed < desiredSpeed)
        currentSpeed = desiredSpeed;
      }
    }


    if(noAccel && distanceEnCoche < 10){
      MOTOR_SetSpeed(0, 0);
      MOTOR_SetSpeed(1, 0);
    }
    // PID
    ENCODER_Reset(0);
    ENCODER_Reset(1);
    // Meilleur valeur de delay
    delay(100);
    int master = ENCODER_Read(0);
    distanceEnCoche = distanceEnCoche - master;
    int slave = ENCODER_Read(1);
    int error = master - slave;
    float incrementation = error * KP;
    MOTOR_SetSpeed(0, currentSpeed);
    MOTOR_SetSpeed(1, (currentSpeed + incrementation));
  }

}

/*
 * Méthode qui permet de faire tourner le robot
 * 
 * - Théta est l'angle a laquelle on veut tourner, et aGauche est true (True pour tourner à gauche, false pour tourner à droite)
 */
void Turn (float theta, bool aGauche)
{
  // Distance de l'arc en encoche 
  float encochesToDo = (M_PI * 39.15 * theta / 360) / DISTANCE_PAR_ENCOCHE;
  int moteur;

  // Permet d'assigner le bon moteur pour tourner dans le bon sens selon le bool aGauche
  if(aGauche){
    moteur = 1;
  }
  else
  {
    moteur = 0;
  }

  // À chaque 25mili, trouver le nombre d'encoches parcourue et le diminuer à la distance à parcourir
  while(encochesToDo > 5)
  {
    ENCODER_Reset(moteur);
    MOTOR_SetSpeed(moteur, speedTourner);
    delay(100);
    float encochesTemp = ENCODER_Read(moteur);
    Serial.println(encochesTemp);
    encochesToDo = encochesToDo - encochesTemp;
  }
  // Arrêter les moteurs pour ne pas qu'il continue à tourner
  MOTOR_SetSpeed(moteur, 0);
}

/* 
 * Méthode pour faire le virage à 180 pour refaire le parcours inverse
 */
void UTurn()
{
  int encochesToDo = 28 / DISTANCE_PAR_ENCOCHE;
  while(encochesToDo > 0)
  {
    ENCODER_Reset(0);
    ENCODER_Reset(1);
    MOTOR_SetSpeed(0, speedTourner);
    MOTOR_SetSpeed(1, -speedTourner);
    delay(100);
    float distTemp = ENCODER_Read(0);
    encochesToDo = encochesToDo - distTemp;
  }
  // Arrêter les moteurs pour ne pas qu'il continue à tourner
  MOTOR_SetSpeed(0, 0);
  MOTOR_SetSpeed(1, 0);
}

/* 
 * Méthode pour le parcours
 */
void Parcours()
{
  delay(2000);
  Forward(215, 0.5);
  delay(100);
  Turn(85, true);
  delay(250);
  Forward(30, 0.5);
  delay(100);
  Turn(85, false);
  delay(100);
  Forward(25, 0.5);
  delay(100);
  Turn(85, false);
  delay(100);
  Forward(20, 0.5);
  delay(100);
  Turn(50, true);
  delay(100);
  Forward(58, 0.5);
  delay(100);
  Turn(80, true);
  delay(100);
  Forward(70, 0.5);
  delay(100);
  Turn(50, false);
  delay(100);
  Forward(120, 0.5);
}

/* 
 * Méthode pour le parcours inverse
 */
void ParcoursInverse()
{
  Forward(120, 0.5);
  delay(100);
  Turn(50, true);
  delay(250);
  Forward(70, 0.5);
  delay(100);
  Turn(80, false);
  delay(100);
  Forward(58, 0.5);
  delay(100); 
  Turn(50, false);
  delay(100);
  Forward(20, 0.5);
  delay(100);
  Turn(85, true);
  delay(100);
  Forward(25, 0.5);
  delay(100);
  Turn(85, true);
  delay(100);
  Forward(30, 0.5);
  delay(100);
  Turn(85, false);
  delay(100);
  Forward(215, 0.5);
}

/* ****************************************************************************
Fonctions d'initialisation (setup)
**************************************************************************** */
// -> Se fait appeler au debut du programme
// -> Se fait appeler seulement un fois
// -> Generalement on y initilise les varibbles globales

void setup(){
  BoardInit();
}


/* ****************************************************************************
Fonctions de boucle infini (loop())
**************************************************************************** */
// -> Se fait appeler perpetuellement suite au "setup"

void loop() {
  // SOFT_TIMER_Update(); // A decommenter pour utiliser des compteurs logiciels
  //delay(10);// Delais pour décharger le CPU

  Parcours(); // Fait le trajet de base du parcours
  UTurn(); // Effectue un virage à 180 degrée
  ParcoursInverse(); // Fait le trajet inverse du parcours
} 
