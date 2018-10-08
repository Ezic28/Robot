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


/**
 * - Methode qui sert a calculer la distance en coche, call le PID et a reset les moteur a 0
 * 
 * - distanceEnCm s'explique de soi-meme, et desiredSpeed est la vitesse de 
 *   croisiere puisque le robot accellere et descelere peu importe cette valeur
 * */
void Forward(float distanceEnCm, float desiredSpeed)
{
  float distanceReelle = distanceEnCm/DISTANCE_PAR_ENCOCHE;
  PidLigneDroite(desiredSpeed, distanceReelle);
  MOTOR_SetSpeed(0, 0);
  MOTOR_SetSpeed(1, 0);
}


/**
 * 
 * - Methode qui fait le PID pour avancer en ligne droite, et qu'a la fois 
 *   fait accellerer et descellerer le robot. De plus, le robot s'arrete apres 
 *   d'avoir completer la distance desirer
 * 
 * - desiredSpeed est la vitesse de croisiere, et distanceEnCoche est la 
 *   distance en nombres d'encoches a parcourir. 
 * */
void PidLigneDroite(float desiredSpeed, int distanceEnCoche)
{
  float currentSpeed = 0;
  float smallDist = 40/DISTANCE_PAR_ENCOCHE;
  bool noAccel = false;

  // Si petite distance, aucun point a accellerer jusqua une certaine vitesse
  if(distanceEnCoche<smallDist){
    noAccel = true;
    currentSpeed = 0.3;
  }

  //L'incrementation descendant vite, avoir une valeur de 10 permet d'etre plus precis qu'une valeur de 0
  while(distanceEnCoche > 10)
  {
    //code qui fait accellerer jusqua la vitesse de croisiere
    if (currentSpeed < desiredSpeed && !noAccel)
    {
      currentSpeed = currentSpeed + INCREMENTATION_ACCEL;
      //du coups que lincrementation depasse la desiredSpeed
      if (currentSpeed > desiredSpeed)
        currentSpeed = desiredSpeed;
    }

    //code qui fait descellerer
    //jai hard code le 26, cest trash mais cest ce qui fittait le mieux pour un speed de 0.5
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
    //code pour le PID
    ENCODER_Reset(0);
    ENCODER_Reset(1);
    //meilleur valeur pour le PID selon hard test, MEN PARLER AVANT DE CHANGER
    delay(100);
    int master = ENCODER_Read(0);
    distanceEnCoche = distanceEnCoche + master;
    int slave = ENCODER_Read(1);
    int error = master - slave;
    int incrementation = error*KP;
    MOTOR_SetSpeed(1, currentSpeed + incrementation);
  }

}

/**
 * - Methode qui permet de faire tourner le robot
 * 
 * - teta est langle a laquelle on veut tourner, et aGauche est true
 *   si on veut tourner a gauche, sinon est false
 * */
void Turn (float teta, bool aGauche)
{
  //je trouve la distance de l'arc en encoches 
  float distToDo = (M_PI * 39.15 * teta/360)/DISTANCE_PAR_ENCOCHE;
  float speedTourner = 0.1;
  int moteur;

  //Permet d'asigner le bon moteur pour tourner dans le bon sens selon le bool aGauche
  if(aGauche){
    moteur = 1;
  }
  else
  {
    moteur = 0;
  }

  //a chaque 25mili, je trouve le nombre d'encoches parcourue pi je le diminue a la distance a parcourir
  while(distToDo > 10)
  {
    ENCODER_Reset(moteur);
    MOTOR_SetSpeed(moteur, speedTourner);
    delay(25);
    float distTemp = ENCODER_Read(moteur);
    distToDo = distToDo - distTemp;
  }
  //jarrete le moteur pour pas qui spin2win
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
  delay(5000);
  //Forward(223, 0.5);
  Turn(90, true);
  /*
  Forward(50, 0.5);
  Turn(90, false);
  Forward(50, 0.5);
  Turn(90, true);
  Forward(25, 0.5);
  Turn(45, false);
  Forward(40, 0.5);
  Turn(90, true);
  Forward(70, 0.5);
  Turn(90, false);
  Forward(100, 0.5);
*/
}
