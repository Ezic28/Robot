/*
Projet: Le nom du script
Equipe: Votre numero d'equipe
Auteurs: Les membres auteurs du script
Description: Breve description du script
Date: Derniere date de modification
*/

/* ****************************************************************************
Inclure les librairies de functions que vous voulez utiliser
**************************************************************************** */

#include <LibRobus.h> // Essentielle pour utiliser RobUS
#include <math.h>


/* ****************************************************************************
Variables globales et defines
**************************************************************************** */
// -> defines...
// L'ensemble des fonctions y ont acces
const float INCREMENTATION_ACCEL = 0.03;
const float DIAMETRE_ROUE = 7.7;
const float DIAMETRE_ROBOT = 39.15;
const int ENCOCHES_TOTAL = 3200;
const float DISTANCE_PAR_ENCOCHE = (DIAMETRE_ROUE * M_PI) / ENCOCHES_TOTAL;
const float KP = 0.01 ;
bool isForward = false;
// FUCK !!!!!!!!!!!!


/* ****************************************************************************
Vos propres fonctions sont creees ici
**************************************************************************** */
void TestEncodeurRoue()
{
  int pulse0;
  int pulse1;

  ENCODER_Reset(0);
  ENCODER_Reset(1);

  MOTOR_SetSpeed(0, 0.5);
  MOTOR_SetSpeed(1, 0.5);
  delay(2000);

  MOTOR_SetSpeed(0, 0);
  MOTOR_SetSpeed(1, 0);

  pulse0 = ENCODER_ReadReset(0);
  delay(200);
  pulse1 = ENCODER_ReadReset(1);

  printf("Encodeur gauche %d\n", pulse0);
  printf("Encodeur droit %d\n", pulse1);
}

void TestTourner(bool turn)
{
  if (turn == 0)
  {
    MOTOR_SetSpeed(0, 0.2);
    MOTOR_SetSpeed(1, 0.5);
  }
  else if(turn == 1)
  {
    MOTOR_SetSpeed(0, 0.5);
    MOTOR_SetSpeed(1, 0.2);
  }

}

//void PidLigneDroite

/**
 * Methode qui accelere le robot
 * ACCEPTE SEULEMENT UN DESIRED SPEED DE 0 OU 0.5
 * 
 * 
void Accel(float desiredSpeed, float currentSpeed)
{
    if (currentSpeed < desiredSpeed)
    {
      currentSpeed = currentSpeed + INCREMENTATION_ACCEL;
      if (currentSpeed > desiredSpeed)
        currentSpeed = desiredSpeed;
    }
    if (currentSpeed > desiredSpeed)
    {
      currentSpeed = currentSpeed - INCREMENTATION_ACCEL;
      if (currentSpeed < desiredSpeed)
        currentSpeed = desiredSpeed;
    }
    MOTOR_SetSpeed(0, currentSpeed);
    MOTOR_SetSpeed(1, currentSpeed);

  if(currentSpeed > 0 || currentSpeed < 0.5 )
  {
    delay(100);
    Accel(desiredSpeed, currentSpeed);
  }
}
*/



void Forward(float distanceEnCm, float desiredSpeed)
{
  float distanceReelle = distanceEnCm/DISTANCE_PAR_ENCOCHE;
  PidLigneDroite(desiredSpeed, distanceReelle);
  MOTOR_SetSpeed(0, 0);
  MOTOR_SetSpeed(1, 0);
}

void PidLigneDroite(float desiredSpeed, int distanceEnCoche)
{
  float currentSpeed = 0;
  float smallDist = 40/DISTANCE_PAR_ENCOCHE;
  bool noAccel = false;

  if(distanceEnCoche<smallDist){
    noAccel = true;
    currentSpeed = 0.3;
  }

  while(distanceEnCoche > 10)
  {
    if (currentSpeed < desiredSpeed && !noAccel)
    {
      currentSpeed = currentSpeed + INCREMENTATION_ACCEL;
      if (currentSpeed > desiredSpeed)
        currentSpeed = desiredSpeed;
    }

    if(distanceEnCoche < 26/DISTANCE_PAR_ENCOCHE)
    {
      desiredSpeed = 0;
      if (currentSpeed > desiredSpeed)
      {
      currentSpeed = currentSpeed - INCREMENTATION_ACCEL;
      if (currentSpeed < desiredSpeed)
        currentSpeed = desiredSpeed;
      }
    }
    MOTOR_SetSpeed(0, currentSpeed);
    MOTOR_SetSpeed(1, currentSpeed);
    ENCODER_Reset(0);
    ENCODER_Reset(1);
    delay(100);
    int master = ENCODER_Read(0);
    distanceEnCoche = distanceEnCoche + master;
    int slave = ENCODER_Read(1);
    int error = master - slave;
    int incrementation = error*KP;
    MOTOR_SetSpeed(1, currentSpeed + incrementation);
  }

}


void Turn (float teta, bool aGauche)
{
  float distToDo = M_PI * 39.15 * teta/360;
  float speedTourner = 0.1;
  int moteur;

  if(aGauche){
    moteur = 1;
  }
  else
  {
    moteur = 0;
  }

  while(distToDo > 10)
  {
    ENCODER_Reset(moteur);
    MOTOR_SetSpeed(moteur, speedTourner);
    delay(25);
    float distTemp = ENCODER_Read(moteur);
    Serial.println(distTemp);
    distToDo = distToDo - distTemp;
  }
  MOTOR_SetSpeed(moteur, 0);

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
  //TestEncodeurRoue();
  //TestTourner(0);
  delay(10000);
  Forward(50, 0.5);
  Turn(90, true);

}
