/**
 TEST CARTE D925ED4 - BOX GALEO BLTH
 * 
 * Septembre 2023
 * 
 * Michel LOPEZ
 * 
 */

/*
 Equipement: automate v1.0
 * PIC16LF1939
 * 
 * Relais:
 * 
 * R1: alimentation
 * R2: BP1
 * R3: BP2
 * R4: Horloge
 * R5: Cavalier P1
 * R6: Cavalier P2
 * R7: Touche clavier - "Doigt"
 * R8: Commutation alimentation programmateur STM32
 * 
 * Entrées:
 * 
 * J13/(IN1 dans le code): NC1
 * J15/IN2: NO1
 * J16/IN4: NC2
 * J17/IN5: NO2
 * J19/IN6: NC3
 * J18/IN7: NO30
 * J14/IN3: Bouton de validation OK 
 * J20/IN8: Bouton de confirmation NOK
 * J21/AN1: test éclairage clavier par mesure analogique
 * 
 * J26(serigraphie OV)/GPIO1: inhibition test des leds - starp au OV = tests des leds inhibé
 * J26(serigraphie OV)/GPIO4: sélection maître/esclave - starp au OV = esclave
 * 
 * Sorties:
 * J25.1/C2: led rouge (indication non conforme)
 * J25.2/C4: led verte (indication conforme)
 * J25.3/C2: led jaune (indication test en cours)
 
 * 
 * Mode d'emploi:
 * 1- Sélectionner mode de test des leds (strap = inhibition)
 * 2- Appuyer sur OK, pour lancer la séquence
 * 3- Appuyer sur OK pour valider / sur NOK pour invalider
 * 4- En fin de séquence le résultat est donné par la led verte ou rouge
 * 5- Acquiter le résultat en appuyant sur OK
 * 
 * 
 */

#include "mcc_generated_files/mcc.h"
#include "I2C_LCD.h"
#include "tester.h"
#include "display.h"
#include "I2C_tester.h"
#include <stdlib.h>
#include "mcc_generated_files/i2c_master.h"
#include "I2C-tester.h"

/*
                         Main application
 */
void main(void) {
    // initialize the device
    SYSTEM_Initialize();

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();


    bool testActif = false;
    bool testVoyants = false;
    int lectureAN1;
    char slectureAN1[5];
    bool testLeds = true;
    bool automatique = false;
    bool pap = false;
    bool programmation = true;
    bool master = true;
    bool slaveInTest = false;
    char slaveSummary;
    bool slaveWaiting = false;
    char orderFormWin;
    char bleCode[NBRE_DIGIT_ACQ + 1];

    // Détermination mode de fonctionnement: master/slave
    // Affichage message d'accueil

    __delay_ms(1000);

   


    if (GPIO1_GetValue() == 1) {

        testLeds = true;


    } else {

        testLeds = false;



    }



    if (GPIO2_GetValue() == 0) {

        pap = true;

    } else {

        pap = false;
    }




    while (1) {

        // LCD_Init(0x4E);
        // displayManager(TITRE, MODE_MASTER, BOARD_REQUEST, OK_REQUEST);
        displayManagerMaster(TITRE, MODE_MASTER, BOARD_REQUEST, OK_REQUEST);
        __delay_ms(100);
        
        
     

        // sélection test individuel des leds
        // le test est inhibé si l'entrée GPIO1 est à zéro


        REL8_SetLow();
        C1_SetLow();

        // Attente de démarrage


        while (!testActif) {

            attenteDemarrage2(&automatique, &testActif, &programmation);
        }


        startPhaseBLE(1);
        __delay_ms(100);
        programmation = false;
        startAlert();
        testActif = true;
        ledConforme(false);
        ledNonConforme(false);
        ledProgession(true);


        // test I2C vers esclave

        __delay_ms(100);

      
        //LCD_Init(0x4E);
        displayManagerMaster("ETAPE 1", "TEST 3 RELAIS ON", LIGNE_VIDE, LIGNE_VIDE);

        // DEMARRAGE SEQUENCE DE TEST


        pressBP1(true);
        pressBP2(true);
        __delay_ms(1000);
        alimenter(true);
        __delay_ms(2000); // 2000 pour D925ED4; 10000 pour D850

        if (testR1(true) && testR2(true) && testR3(true)) {

            printf("-> TEST:1:1");

        } else {

            testActif = false;
            pressBP1(false);
            pressBP2(false);
            alerteDefaut("ETAPE 1", &testActif, &testVoyants);
            sortieErreur(&automatique, &testActif, &testVoyants, &programmation);

        }

      
        __delay_ms(1000);

        pressBP1(false);
        pressBP2(false);

        // ETAPE 2

        if (testActif) {

            displayManagerMaster("ETAPE 2", "TEST 3 RELAIS OFF", LIGNE_VIDE, LIGNE_VIDE);
            pressBP1(false);
            pressBP2(false);
            __delay_ms(500);
            if (!testR1(true) && !testR2(true) && !testR3(true)) {

                printf("-> TEST:2:1");

            } else {

                testActif = false;
                alerteDefaut("ETAPE 2", &testActif, &testVoyants);
                sortieErreur(&automatique, &testActif, &testVoyants, &programmation);

            }
        }

     

        // ETAPE 3



        if (testActif) {

            displayManagerMaster("ETAPE 3", "TEST LED ROUGE", LIGNE_VIDE, LIGNE_VIDE);
            pressBP1(true);
            __delay_ms(250);
            pressBP1(false);
            if (testLeds) {

                printf("Attente validation led rouge\r\n");
                testVoyants = reponseOperateur(automatique);
                if (!testVoyants) {

                    testActif = false;
                    alerteDefaut("ETAPE 3", &testActif, &testVoyants);
                    sortieErreur(&automatique, &testActif, &testVoyants, &programmation);

                } else {

                    printf("-> TEST:3:1");
                }
            }


        }

        // ETAPE 4



        if (testActif) {

            displayManagerMaster("ETAPE 4", "TEST LED BLEUE", LIGNE_VIDE, LIGNE_VIDE);
            pressBP1(true);
            __delay_ms(250);
            pressBP1(false);
            if (testLeds) {

                testVoyants = reponseOperateur(automatique);
                if (!testVoyants) {

                    testActif = false;
                    alerteDefaut("ETAPE 4", &testActif, &testVoyants);
                    sortieErreur(&automatique, &testActif, &testVoyants, &programmation);

                } else {

                    printf("-> TEST:4:1");
                }
            }


        }

   

        // ETAPE 5



        if (testActif) {

            displayManagerMaster("ETAPE 5", "TEST LED VERTE", LIGNE_VIDE, LIGNE_VIDE);
            pressBP1(true);
            __delay_ms(250);
            pressBP1(false);
            if (testLeds) {

                testVoyants = reponseOperateur(automatique);
                if (!testVoyants) {

                    testActif = false;
                    alerteDefaut("ETAPE 5", &testActif, &testVoyants);
                    sortieErreur(&automatique, &testActif, &testVoyants, &programmation);

                } else {

                    printf("-> TEST:5:1");
                }
            }

        }

        // ETAPE 6



        if (testActif) {

            displayManagerMaster("ETAPE 6", "TEST R1 ON", LIGNE_VIDE, LIGNE_VIDE);
            pressBP1(true);
            __delay_ms(1000);
            pressBP1(false);

            __delay_ms(1000);

            if (testR1(true)) {

                printf("-> TEST:6:1");

            } else {

                testActif = false;
                alerteDefaut("ETAPE 6", &testActif, &testVoyants);
                sortieErreur(&automatique, &testActif, &testVoyants, &programmation);

            }

        }
     

        // ETAPE 7



        if (testActif) {

            displayManagerMaster("ETAPE 7", "TEST R1 OFF - R2 ON", LIGNE_VIDE, LIGNE_VIDE);
            pressBP1(true);
            __delay_ms(1000);
            pressBP1(false);

            __delay_ms(1000);

            if (testR1(false) && testR2(true)) {

                printf("-> TEST:7:1");

            } else {

                testActif = false;
                alerteDefaut("ETAPE 7", &testActif, &testVoyants);
                sortieErreur(&automatique, &testActif, &testVoyants, &programmation);
            }

        }
     
        // ETAPE 8


        if (testActif) {

            displayManagerMaster("ETAPE 8", "TEST R2 OFF - R3 ON", LIGNE_VIDE, LIGNE_VIDE);
            pressBP1(true);
            __delay_ms(1000);
            pressBP1(false);

            __delay_ms(1000);

            if (testR2(false) && testR3(true)) {

                printf("-> TEST:8:1");

            } else {

                testActif = false;
                alerteDefaut("ETAPE 8", &testActif, &testVoyants);
                sortieErreur(&automatique, &testActif, &testVoyants, &programmation);
            }

        }

     

        // ETAPE 9


        if (testActif) {

            displayManagerMaster("ETAPE 9", "TEST LED CLAVIER", "CLAVIER ECLAIRE?", LIGNE_VIDE);
            pressBP1(true);
            __delay_ms(250);
            pressBP1(false);
            __delay_ms(500);
            // Lecture analogique
            // Led on: Van = 433
            // Led off: Van = 587

            lectureAN1 = ADC_GetConversion(VIN1);
            int buffer = sprintf(slectureAN1, "%d", lectureAN1);
            if (lectureAN1 < LIM_H) {


                REL8_SetHigh();
                printf("-> TEST:9:1");

            } else {

                alerteDefaut("ETAPE 9", &testActif, &testVoyants);
                displayManager("ETAPE 9", "TEST LED CLAVIER", slectureAN1, LIGNE_VIDE); // Ligne de test: affichage valeur de mesure analogique
                REL8_SetLow();
                sortieErreur(&automatique, &testActif, &testVoyants, &programmation);

            }

            __delay_ms(2000);

        }
    

        // ETAPE 10



        if (testActif) {

            displayManagerMaster("ETAPE 10", "TEST LED CLAVIER", "CLAVIER ETEINT?", LIGNE_VIDE);
            pressBP1(true);
            __delay_ms(250);
            pressBP1(false);

            // Lecture analogique
            // Led on: Van = 433
            // Led off: Van = 587
            __delay_ms(500);
            lectureAN1 = ADC_GetConversion(VIN1);
            int buffer = sprintf(slectureAN1, "%d", lectureAN1);

            if (lectureAN1 < LIM_L) {


                REL8_SetLow();
                printf("-> TEST:10:1");

            } else {

                alerteDefaut("ETAPE 10", &testActif, &testVoyants);
                displayManagerMaster("ETAPE 10", "TEST LED CLAVIER", slectureAN1, LIGNE_VIDE); // Ligne de test: affichage valeur de mesure analogique
                REL8_SetHigh();
                sortieErreur(&automatique, &testActif, &testVoyants, &programmation);

            }
            __delay_ms(2000);

        }

     

        // ETAPE 12



        if (testActif) {

            displayManagerMaster("ETAPE 12", "TEST SFLASH", LIGNE_VIDE, LIGNE_VIDE);
            __delay_ms(500);
            pressBP1(true);
            __delay_ms(250);
            pressBP1(false);

            __delay_ms(3000);

            pressBP1(true);
            __delay_ms(250);
            pressBP1(false);
            __delay_ms(750);

            if (testR1(true) && testR2(true) && testR3(false)) {

                printf("-> TEST:12:1");

            } else {

                testActif = false;
                pressBP1(false);
                pressBP2(false);
                alerteDefaut("ETAPE 12", &testActif, &testVoyants);
                sortieErreur(&automatique, &testActif, &testVoyants, &programmation);

            }

            __delay_ms(1000);

            pressBP1(false);
            pressBP2(false);

        }
      

        // ETAPE 13:TEST DES 3 LEDS ALLUMEES SIMULTANEMENT



        if (testActif) {

            displayManagerMaster("ETAPE 13", "TEST LEDS CARTE", "LEDS ALLUMEES", "PRESSER OK / NOK");
            pressBP1(true);
            __delay_ms(250);
            pressBP1(false);

            printf("ATTENTE VALIDATION LEDS\r\n");
            testVoyants = reponseOperateur(automatique);
            if (!testVoyants) {

                testActif = false;
                alerteDefaut("ETAPE 13", &testActif, &testVoyants);
                sortieErreur(&automatique, &testActif, &testVoyants, &programmation);

            } else {

                printf("-> TEST:13:1");
            }
        }
 
        // ETAPE 14


        if (testActif) {

            displayManagerMaster("ETAPE 14", "TEST BP2", LIGNE_VIDE, LIGNE_VIDE);
            pressBP2(true);
            __delay_ms(250);
            pressBP2(false);
            __delay_ms(500);

            if (testR1(true) && testR2(true) && testR3(true)) {

                printf("-> TEST:14:1");

            } else {

                testActif = false;
                alerteDefaut("ETAPE 14", &testActif, &testVoyants);
                sortieErreur(&automatique, &testActif, &testVoyants, &programmation);

            }

        }
     
        // ETAPE 15



        if (testActif) {

            displayManagerMaster("ETAPE 15", "TEST HORLOGE", LIGNE_VIDE, LIGNE_VIDE);
            setHorloge(true);
            __delay_ms(250);
            setHorloge(false);
            __delay_ms(500);

            if (testR1(false) && testR2(false) && testR3(false)) {

                printf("-> TEST:15:1");

            } else {

                testActif = false;
                alerteDefaut("ETAPE 15", &testActif, &testVoyants);
                sortieErreur(&automatique, &testActif, &testVoyants, &programmation);

            }

        }
      

        // ETAPE 16


        if (testActif) {

            displayManagerMaster("ETAPE 16", "TEST P1", LIGNE_VIDE, LIGNE_VIDE);
            setP1(true);
            __delay_ms(1200); // 1200 pour D925ED2

            setP1(false);
            __delay_ms(500);
            if (testR1(true) && testR2(true) && testR3(true)) {

                printf("-> TEST:16:1");

            } else {

                alerteDefautEtape16("ETAPE 16", &testActif, &testVoyants, &automatique, &programmation);

            }

        }
       

        // ETAPE 17



        if (testActif) {

            displayManagerMaster("ETAPE 17", "TEST P2", LIGNE_VIDE, LIGNE_VIDE);
            setP2(true);
            __delay_ms(1200);
            setP2(false);
            __delay_ms(500);

            if (testR1(false) && testR2(false) && testR3(false)) {

                printf("-> TEST:17:1");

            } else {

                testActif = false;
                alerteDefaut("ETAPE 17", &testActif, &testVoyants);
                sortieErreur(&automatique, &testActif, &testVoyants, &programmation);

            }

        }

     

        // ETAPE 18



        if (testActif) {

            displayManagerMaster("ETAPE 18", "TEST BLUETOOTH", "ACQUISITION EN COURS", LIGNE_VIDE);
            activerTouche();
            //printf("ATTENTE VALIDATION BLUETOOTH\r\n");
            __delay_ms(100);
            startPhaseBLE(2);
            waitForBleAcq();
            startPhaseBLE(3);
            // Transmission code BLE

            waitForBleAcq();
            getBLEindentifier(bleCode);

            __delay_ms(100);
            displayManagerMaster("ETAPE 18", "TEST BLUETOOTH", bleCode, LIGNE_VIDE);

            // Fin transmission code BLE


            testVoyants = analyseCodeBLE(bleCode);


            if (!testVoyants) {

                testActif = false;
                alerteDefaut("ETAPE 18", &testActif, &testVoyants);
                sortieErreur(&automatique, &testActif, &testVoyants, &programmation);
                __delay_ms(2000);
            } else {

                printf("-> TEST:18:1");
            }
        }

      
        // ETAPE: SORTIE

        if (testActif) {

            displayManagerMaster("TEST CONFORME", bleCode, "RETIRER CARTE", ACQ);
            ledConforme(true);
            alimenter(false);
            okAlert();
            attenteAquittement(&automatique, &testActif);
            initialConditions(&testActif, &testVoyants, &automatique, &programmation);

            __delay_ms(2000);

        }

    } // fin boucle infinie



} // fin fonction main

/**
 End of File
 */