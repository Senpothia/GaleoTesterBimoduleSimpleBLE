/*
 * File:   tester.c
 * Author: Michel
 *
 * Created on 31 août 2023, 15:28
 */


#include <xc.h>
#include "mcc_generated_files/mcc.h"
#include "I2C_LCD.h"
#include "tester.h"
#include "display.h"
#include <stdio.h>
#include <string.h>
#include "I2C-tester.h"

void alimenter(bool active) {

    if (active) {

        REL1_SetHigh();
    } else {
        REL1_SetLow();
    }
}

void pressBP1(bool active) {

    if (active) {

        REL2_SetHigh();
    } else {
        REL2_SetLow();
    }
}

void pressBP2(bool active) {

    if (active) {

        REL3_SetHigh();
    } else {
        REL3_SetLow();
    }
}

void setHorloge(bool active) {

    if (active) {

        REL4_SetHigh();
    } else {
        REL4_SetLow();
    }

}

bool testR1(bool active) {

    bool result = false;
    if (active) {

        if (IN1_GetValue() == 1 && IN2_GetValue() == 0) {

            result = true;
        }
    }

    if (!active) {

        if (IN1_GetValue() == 0 && IN2_GetValue() == 1) {


            result = true;
        }
    }

    return result;
}

bool testR2(bool active) {

    bool result = false;
    if (active) {

        if (IN4_GetValue() == 1 && IN5_GetValue() == 0) {

            result = true;
        }
    }

    if (!active) {

        if (IN4_GetValue() == 0 && IN5_GetValue() == 1) {

            result = true;
        }
    }

    return result;
}

bool testR3(bool active) {


    bool result = false;
    if (active) {

        if (IN6_GetValue() == 1 && IN7_GetValue() == 0) {

            result = true;
        }
    }

    if (!active) {

        if (IN6_GetValue() == 0 && IN7_GetValue() == 1) {

            result = true;
        }
    }

    return result;
}

bool testOK(bool active) {

    bool result = false;
    if (active) {

        if (IN3_GetValue() == 0) {

            result = true;
        }
    }

    if (!active) {

        if (IN3_GetValue() == 1) {

            result = true;
        }
    }

    return result;

}

bool testNOK(bool active) {

    bool result = false;
    if (active) {

        if (IN8_GetValue() == 0) {

            result = true;
        }
    }

    if (!active) {

        if (IN8_GetValue() == 1) {

            result = true;
        }
    }

    return result;
}

bool controlVisuel() {

    bool result = false;


    if (IN3_GetValue() == 0) {

        result = true;
    }

    if (IN3_GetValue() == 1) {

        result = false;

    }

}

void ledNonConforme(bool active) {

    if (active) {

        C3_SetHigh();
    } else {
        C3_SetLow();
    }
}

void ledConforme(bool active) {

    if (active) {

        C4_SetHigh();
    } else {
        C4_SetLow();
    }
}

void ledProgession(bool active) {

    if (active) {

        C2_SetHigh();

    } else {

        C2_SetLow();
    }
}

void alerteDefaut(char etape[], bool *testAct, bool *testVoy) {

    char error[20] = "-> ERREUR: ";
    char eol[10] = "\r\n";
    ledNonConforme(true);
    ledProgession(true);
    ledConforme(false);
    alimenter(false);
    displayManager(etape, NON_CONFORME, ACQ, LIGNE_VIDE);
    printf(strcat(strcat(error, etape), eol));
    errorAlert();

    while (IN3_GetValue() == 0) {
        ;
    }

    // ledNonConforme(false);
    *testAct = false;
    *testVoy = false;

}

bool reponseOperateur(bool automatique) {

    bool reponse = false;
    bool repOperateur = false;
    unsigned char reception;

    if (automatique) {

        while (!repOperateur) {

            if (eusartRxCount != 0) {

                reception = EUSART_Read(); // read a byte for RX

                switch (reception) // check command  
                {


                    case '0':
                    {
                        RESET();

                    }

                    case '2':
                    {

                        __delay_ms(50);
                        reponse = true;
                        repOperateur = true;
                        break;
                    }

                    case '3':
                    {

                        __delay_ms(50);
                        reponse = false;
                        repOperateur = true;
                        break;
                    }

                    case '9': // fin de programmation
                    {

                        __delay_ms(50);
                        reponse = true;
                        repOperateur = true;
                        REL8_SetLow();
                        break;
                    }

                }

            }

        }

    }

    if (!automatique) {

        while (!repOperateur) {

            if (testNOK(true)) {
                reponse = false;
                repOperateur = true;
            }
            if (testOK(true)) {
                reponse = true;
                repOperateur = true;
            }
        }

    }

    return reponse;

}

void setP1(bool active) {

    if (active) {

        REL5_SetHigh();
    } else {
        REL5_SetLow();
    }

}

void setP2(bool active) {

    if (active) {

        REL6_SetHigh();
    } else {
        REL6_SetLow();
    }

}

void initialConditions(bool *testAct, bool *testVoy, bool *autom, bool *prog) {

    if (!*autom) {

        printf("-> FIN TEST MANUEL\r\n");
    }
    *testAct = false;
    *testVoy = false;
    *autom = false;
    *prog = true;
    ledConforme(false);
    ledNonConforme(false);
    ledProgession(false);
    alimenter(false);
    pressBP1(false);
    pressBP2(false);
    setP1(false);
    setP2(false);
    REL8_SetLow();
    C1_SetLow();
    AN3_SetLow();

}

void activerBuzzer() {


    for (int i = 0; i < 50; i++) {

        BUZ_SetHigh();
        // __delay_ms(1);
        __delay_us(1000);

        BUZ_SetLow();
        // __delay_ms(1);
        __delay_us(1000);

    }

}

void activerTouche(void) {

    REL7_SetHigh();
    __delay_ms(250);
    REL7_SetLow();
    __delay_ms(250);

}

void startAlert(void) {

    for (int i = 0; i < 2; i++) {

        activerBuzzer();
        __delay_ms(500);

    }

}

void errorAlert(void) {

    for (int j = 0; j < 4; j++) {

        for (int i = 0; i < 4; i++) {

            activerBuzzer();
            __delay_ms(500);

        }
        __delay_ms(500);
    }


}

void okAlert(void) {


    printf("-> TEST CONFORME - ATTENTE ACQUITTEMENT\r\n");
    for (int i = 0; i < 2; i++) {

        startAlert();

    }


}

void attenteDemarrage3(bool *autom, bool *testAct, bool *prog, bool *testSlaveActive, bool *slaveIsWaiting) {

    unsigned char reception;
    bool repOperateur = false;

    while (!repOperateur) {


        if (IN3_GetValue() == 0) {

            printf("-> TEST MANUEL EN COURS\r\n");
            repOperateur = true;
            *autom = false;
            *prog = false;
            *testAct = true;
        }

        if (eusartRxCount != 0) {

            reception = EUSART_Read(); // read a byte for RX

            switch (reception) // check command  
            {

                case '0':
                {
                    RESET();

                }

                case '1':
                {
                    printf("-> TEST ON\r\n");
                    *autom = true;
                    *testAct = true;
                    *prog = false;
                    __delay_ms(50);
                    repOperateur = true;
                    break;
                }

                case '6':
                {
                    printf("-> ERREUR PROGRAMMATION\r\n");
                    displayManager(TITRE, LIGNE_VIDE, ERREUR_PROGRAMMATION, LIGNE_VIDE);
                    *autom = true;
                    __delay_ms(50);
                    repOperateur = true;
                    *testAct = false;
                    REL8_SetLow();
                    break;
                }

                case '7':
                {
                    printf("-> PROGRAMMATION EN COURS\r\n");
                    displayManager(TITRE, LIGNE_VIDE, EN_PROG, LIGNE_VIDE);
                    *autom = true;
                    *testAct = false;
                    *prog = false;
                    __delay_ms(50);
                    repOperateur = true;
                    REL8_SetHigh();
                    break;
                }

                case '9':
                {
                    printf("-> PROGRAMMATION MODULE 1 TERMINEE\r\n");
                    displayManager(TITRE, LIGNE_VIDE, FIN_PROG, LIGNE_VIDE);
                    *autom = true;
                    __delay_ms(50);
                    repOperateur = true;
                    *testAct = false;
                    REL8_SetLow();
                    break;
                }


                case '8':
                {
                    printf("-> EFFACEMENT EN COURS\r\n");
                    displayManager(TITRE, LIGNE_VIDE, EN_EFFACEMENT, LIGNE_VIDE);
                    *autom = true;
                    *testAct = false;
                    *prog = true;
                    __delay_ms(50);
                    repOperateur = true;
                    REL8_SetHigh();
                    break;
                }

                case '5':
                {
                    printf("-> EFFACEMENT TERMINE\r\n");
                    displayManager(TITRE, LIGNE_VIDE, FIN_EFFACEMENT, LIGNE_VIDE);
                    *autom = true;
                    *testAct = false;
                    *prog = true;
                    __delay_ms(50);
                    repOperateur = true;
                    REL8_SetLow();
                    break;
                }

                case 'a':
                {
                    char responseSlave = startTestSlave();
                    if (responseSlave == 'a') {

                        printf("-> SLAVE_TEST ON\r\n");
                        *autom = true;
                        *testAct = false;
                        *testSlaveActive = true;
                        *prog = false;
                        __delay_ms(50);
                        repOperateur = true;
                        displayManagerSlave("ETAPE 1", "TEST 3 RELAIS ON", LIGNE_VIDE, LIGNE_VIDE);

                    } else {

                        printf("-> SLAVE RESPONSE NULL\r\n");
                        repOperateur = true;

                    }

                    break;
                }


                case 'u':
                {
                    char responseSlave = sendOKToSlave();
                    if (responseSlave == 'u') {

                        printf("-> SLAVE_TEST GET OK\r\n");
                        *autom = true;
                        *testAct = false;
                        *testSlaveActive = true;
                        *prog = false;
                        __delay_ms(50);
                        repOperateur = true;

                    } else {

                        printf("-> SLAVE RESPONSE NULL\r\n");
                        repOperateur = true;

                    }

                    break;
                }

                case 'v':
                {
                    char responseSlave = sendNOKToSlave();
                    if (responseSlave == 'v') {

                        printf("-> SLAVE_TEST GET KO\r\n");
                        *autom = true;
                        *testAct = false;
                        *testSlaveActive = true;
                        *prog = false;
                        __delay_ms(50);
                        repOperateur = true;

                    } else {

                        printf("-> SLAVE RESPONSE NULL\r\n");
                        repOperateur = true;

                    }

                    break;
                }

                case 'w':
                {
                    char responseSlave = sendACQToSlave();
                    if (responseSlave == 'w') {

                        printf("-> SLAVE_TEST GET ACQ\r\n");
                        *autom = true;
                        *testAct = false;
                        *testSlaveActive = false;
                        *prog = false;
                        __delay_ms(50);
                        repOperateur = true;

                    } else {

                        printf("-> SLAVE RESPONSE NULL\r\n");
                        repOperateur = true;

                    }

                    break;
                }


                case '=':
                {
                    char responseSlave = startProgSlave();
                    if (responseSlave == '=') {

                        printf("-> SLAVE_PROG START\r\n");
                        *autom = true;
                        *testAct = false;
                        *testSlaveActive = false;
                        *prog = false;

                        __delay_ms(50);
                        repOperateur = true;
                        displayManagerSlave(TITRE, EN_PROG, LIGNE_VIDE, LIGNE_VIDE);

                    } else {

                        printf("-> SLAVE RESPONSE NULL\r\n");
                        repOperateur = true;

                    }

                    break;
                }

                case '*':
                {
                    char responseSlave = endProgSlave();
                    if (responseSlave == '*') {

                        printf("-> PROGRAMMATION MODULE 2 TERMINEE\r\n");
                        *autom = true;
                        *testAct = false;
                        *testSlaveActive = false;
                        *prog = false;

                        __delay_ms(50);
                        repOperateur = true;
                        displayManagerSlave(TITRE, FIN_PROG, LIGNE_VIDE, LIGNE_VIDE);

                    } else {

                        printf("-> SLAVE RESPONSE NULL\r\n");
                        repOperateur = true;

                    }

                    break;
                }




            }
        }

        // Interrogation module esclave

        if (*testSlaveActive) {

            char repSlave = getSlaveStatus(INTERROG_SLAVE);
            processSlaveResponse(repSlave, slaveIsWaiting);
            __delay_ms(200);

        }


    }

}

void attenteAquittement(bool *autom, bool *testAct) {

    unsigned char reception;
    bool repOperateur = false;

    while (!repOperateur) {


        if (IN3_GetValue() == 0) {

            printf("-> FIN TEST MANUEL\r\n");
            repOperateur = true;
            *autom = false;
            *testAct = false;
        }

        if (eusartRxCount != 0) {

            reception = EUSART_Read(); // read a byte for RX

            switch (reception) // check command  
            {

                case '0':
                {
                    RESET();
                    break;

                }

                case '4':
                {
                    printf("-> TEST ACQUITTE\r\n");
                    *autom = false;
                    *testAct = false;
                    __delay_ms(50);
                    repOperateur = true;
                    break;
                }
            }
        }
    }

}

void sortieErreur(bool *autom, bool *testAct, bool *testVoy, bool *prog) {

    attenteAquittement(*autom, *testAct);
    initialConditions(*testAct, *testVoy, *autom, *prog);
    __delay_ms(2000);

}

void alerteDefautEtape16(char etape[], bool *testAct, bool *testVoy, bool *autom, bool *prog) {

    char error[20] = "-> ERREUR: ";
    char eol[10] = "\r\n";
    ledNonConforme(true);
    ledProgession(true);
    ledConforme(false);
    //alimenter(false);
    displayManager(etape, NON_CONFORME, "VERIFIER P1 ET P2", "PRESSER OK OU ERREUR");
    printf(strcat(strcat(error, etape), eol));
    errorAlert();

    bool reponse = reponseOperateur(*autom);
    __delay_ms(500);
    if (reponse) {

        // ledNonConforme(false);
        *testAct = false;
        *testVoy = false;
        //alerteDefaut("ETAPE 16 CONFIRMEE", &testAct, &testVoy);
        displayManager("ETAPE 16", "NON CONFORME", "RESULTAT CONFIRME", ACQ);
        sortieErreur(&autom, &testAct, &testVoy, &prog);

    } else {

        displayManager("ETAPE 16", "TEST P1", LIGNE_VIDE, LIGNE_VIDE);
        ledNonConforme(false);
        ledProgession(true);
        ledConforme(false);
        setP1(true);
        __delay_ms(1200);
        setP1(false);
        __delay_ms(1000);
        if (testR1(true) && testR2(true) && testR3(true)) {

            displayManager("ETAPE 16", "TEST P1", "ERREUR VALIDEE", "TEST OK");

        } else {

            *testAct = false;
            alerteDefaut("ETAPE 16", &testAct, &testVoy);
            sortieErreur(&autom, &testAct, &testVoy, &prog);
        }

    }


}

void marchePAP() {

    bool repOperateur = false;
    printf("-> Appuyer sur OK\r\n");
    while (!repOperateur) {


        if (IN3_GetValue() == 0) {

            repOperateur = true;
        }
    }
}

void processSlaveResponse(char repSlave, bool *slaveIsWaiting) {

    unsigned char reception;

    switch (repSlave) // check command  
    {

        case 'z':
        {
            printf("-> SLAVE EN ATTENTE:");
            break;
        }

        case 'A':
        {
            printf("-> SLAVETEST:1:1");
            displayManagerSlave("ETAPE 2", "TEST 3 RELAIS OFF", LIGNE_VIDE, LIGNE_VIDE);
            break;

        }

        case 'a':
        {
            printf("-> SLAVE ERREUR:");
            break;
        }



        case 'B':
        {
            printf("-> SLAVETEST:2:1");
            displayManagerSlave("ETAPE 3", "TEST LED ROUGE", LIGNE_VIDE, LIGNE_VIDE);
            break;

        }

        case 'b':
        {
            printf("-> SLAVE ERREUR:");
            break;
        }

        case 'C':
        {
            printf("-> SLAVETEST:3:1");
            displayManagerSlave("ETAPE 4", "TEST LED BLEUE", LIGNE_VIDE, LIGNE_VIDE);
            break;

        }

        case 'c':
        {
            printf("-> SLAVE ERREUR:");
            break;
        }

        case 'D':
        {
            printf("-> SLAVETEST:4:1");
            displayManagerSlave("ETAPE 5", "TEST LED VERTE", LIGNE_VIDE, LIGNE_VIDE);
            break;

        }

        case 'd':
        {
            printf("-> SLAVE ERREUR:");
            break;
        }

        case 'E':
        {
            printf("-> SLAVETEST:5:1");
            displayManagerSlave("ETAPE 6", "TEST R1 ON", LIGNE_VIDE, LIGNE_VIDE);
            break;

        }

        case 'e':
        {
            printf("-> SLAVE ERREUR:");
            break;
        }

        case 'F':
        {
            printf("-> SLAVETEST:6:1");
            displayManagerSlave("ETAPE 7", "TEST R1 OFF - R2 ON", LIGNE_VIDE, LIGNE_VIDE);
            break;

        }

        case 'f':
        {
            printf("-> SLAVE ERREUR:");
            break;
        }

        case 'G':
        {
            printf("-> SLAVETEST:7:1");
            displayManagerSlave("ETAPE 8", "TEST R2 OFF - R3 ON", LIGNE_VIDE, LIGNE_VIDE);
            break;

        }

        case 'g':
        {
            printf("-> SLAVE ERREUR:");
            break;
        }

        case 'H':
        {
            printf("-> SLAVETEST:8:1");
            displayManagerSlave("ETAPE 9", "TEST LED CLAVIER", "CLAVIER ECLAIRE?", LIGNE_VIDE);
            break;

        }

        case 'h':
        {
            printf("-> SLAVE ERREUR:");
            break;
        }

        case 'I':
        {
            printf("-> SLAVETEST:9:1");
            displayManagerSlave("ETAPE 10", "TEST LED CLAVIER", "CLAVIER ETEINT?", LIGNE_VIDE);
            break;

        }

        case 'i':
        {
            printf("-> SLAVE ERREUR:");
            break;
        }

        case 'J':
        {
            printf("-> SLAVETEST:10:1");
            displayManagerSlave("ETAPE 12", "TEST SFLASH", LIGNE_VIDE, LIGNE_VIDE);
            break;

        }

        case 'j':
        {
            printf("-> SLAVE ERREUR:");
            break;
        }

        case 'K':
        {
            printf("-> SLAVETEST:11:1");
            displayManagerSlave("ETAPE 12", LIGNE_VIDE, LIGNE_VIDE, LIGNE_VIDE);
            break;

        }

        case 'k':
        {
            printf("-> SLAVE ERREUR:");
            break;
        }

        case 'L':
        {
            printf("-> SLAVETEST:12:1");
            displayManagerSlave("ETAPE 13", "TEST LEDS CARTE", "LEDS ALLUMEES", "PRESSER OK / NOK");
            *slaveIsWaiting = true;
            break;

        }

        case 'l':
        {
            printf("-> SLAVE ERREUR:");
            break;
        }

        case 'M':
        {
            printf("-> SLAVETEST:13:1");
            *slaveIsWaiting = false;
            displayManagerSlave("ETAPE 14", "TEST BP2", LIGNE_VIDE, LIGNE_VIDE);
            break;

        }

        case 'm':
        {
            printf("-> SLAVE ERREUR:");
            break;
        }

        case 'N':
        {
            printf("-> SLAVETEST:14:1");
            displayManagerSlave("ETAPE 15", "TEST HORLOGE", LIGNE_VIDE, LIGNE_VIDE);
            break;

        }

        case 'n':
        {
            printf("-> SLAVE ERREUR:");
            break;
        }

        case 'O':
        {
            printf("-> SLAVETEST:15:1");
            displayManagerSlave("ETAPE 16", "TEST P1", LIGNE_VIDE, LIGNE_VIDE);
            break;

        }

        case 'o':
        {
            printf("-> SLAVE ERREUR:");
            break;
        }

        case 'P':
        {
            printf("-> SLAVETEST:16:1");
            displayManagerSlave("ETAPE 17", "TEST P2", LIGNE_VIDE, LIGNE_VIDE);
            break;

        }

        case 'p':
        {
            printf("-> SLAVE ERREUR:");
            break;
        }

        case 'Q':
        {
            printf("-> SLAVETEST:17:1");
            displayManagerSlave("ETAPE 18", "TEST BLUETOOTH", "VOIR APPLI", "PRESSER OK / NOK");
            *slaveIsWaiting = true;
            break;

        }


        case 'q':
        {
            printf("-> SLAVE ERREUR:");
            break;
        }

        case 'R':
        {
            printf("-> SLAVETEST:18:1");
            *slaveIsWaiting = false;
            displayManagerSlave("FIN DE TEST", "CONFORME", "RETIRER CARTE", ACQ);
            break;

        }

        case 'r':
        {
            printf("-> SLAVE ERREUR:");
            break;
        }

        case 'S':
        {
            printf("-> SLAVE TEST CONFORME");
            displayManagerSlave(TITRE, MODE_SLAVE, BOARD_REQUEST, OK_REQUEST);
            break;
        }

        case 'u':
        {
            printf("-> SLAVE TEST OK");
            break;
        }


        case 'v':
        {
            printf("-> SLAVE TEST KO");
            break;
        }


        case 'w':
        {
            printf("-> SLAVE TEST ACQUITTE");
            displayManagerSlave(TITRE, MODE_SLAVE, BOARD_REQUEST, OK_REQUEST);
            break;
        }


        case '=':
        {
            printf("-> SLAVE PROG K8 ON");
            displayManagerSlave(TITRE, MODE_SLAVE, EN_PROG, OK_REQUEST);
            break;
        }

        case '*':
        {
            printf("-> SLAVE END PROG K8 OFF");
            displayManagerSlave(TITRE, MODE_SLAVE, FIN_PROG, OK_REQUEST);
            break;
        }

        default:
            break;
    }


}

unsigned char getRS232() {

    unsigned char reception;


    if (eusartRxCount != 0) {

        reception = EUSART_Read(); // read a byte for RX

    } else {

    }
    return reception;

}

void processActionForSlave(bool *autom, bool *testAct, bool *prog, bool *testSlaveActive, unsigned char ordreFromWin) {

    switch (ordreFromWin) // check command  
    {

            // Ordre de lancement du test
        case 'a':
        {
            char responseSlave = startTestSlave();
            if (responseSlave == 'a') {

                printf("-> SLAVE_TEST ON\r\n");
                *autom = true;
                *testAct = true;
                *testSlaveActive = true;
                *prog = false;
                __delay_ms(50);


            } else {

                printf("-> SLAVE RESPONSE NULL\r\n");

            }

            break;
        }

            // Reception OK_SLAVE
        case 'u':
        {
            char echo = getSlaveStatus('u');
            break;

        }
            // Reception KO_SLAVE
        case 'v':
        {
            char echo = getSlaveStatus('v');
            break;
        }

            // Reception AQC_SLAVE
        case 'w':
        {
            char echo = getSlaveStatus('w');
            if (echo == 'w') {

                printf("-> SLAVE_TEST GET OK");
            }
            break;

        }


        case '=':
        {
            char responseSlave = startProgSlave();
            if (responseSlave == '=') {

                printf("-> SLAVE_PROG START\r\n");


                __delay_ms(50);

                displayManagerSlave(TITRE, EN_PROG, LIGNE_VIDE, LIGNE_VIDE);

            } else {

                printf("-> SLAVE RESPONSE NULL\r\n");


            }

            break;
        }

        case '*':
        {
            char responseSlave = endProgSlave();
            if (responseSlave == '*') {

                printf("-> PROGRAMMATION MODULE 2 TERMINEE\r\n");


                __delay_ms(50);

                displayManagerSlave(TITRE, FIN_PROG, LIGNE_VIDE, LIGNE_VIDE);

            } else {

                printf("-> SLAVE RESPONSE NULL\r\n");

            }

            break;
        }

        default:
            break;
    }
}

void startPhaseBLE(int phase) {

    for (int i = 0; i < phase; i++) {

        C1_SetHigh();
        __delay_ms(HALF_PERIOD_TRANSMISSION_MS);
        C1_SetLow();
        __delay_ms(HALF_PERIOD_TRANSMISSION_MS);
    }
    C1_SetLow();

}

void activeCLK() {

    C1_SetLow();
    __delay_ms(HALF_PERIOD_TRANSMISSION_MS);
    C1_SetHigh();

}

void releaseCLK() {

    C1_SetLow();
    __delay_ms(HALF_PERIOD_TRANSMISSION_MS); // 20->10ms

}

char getCharacterFormRx() {

    char N = 0;
    int reading;

    for (int i = 7; i > -1; i--) {

        activeCLK();
        __delay_ms(HALF_PERIOD_TRANSMISSION_MS / 2);
        reading = AN4_GetValue();
        N = setCharacterBit(N, reading, i);
        //__delay_ms(HALF_PERIOD_TRANSMISSION_MS);

    }

    return N;
}

void getBLEindentifier(char * bleCode) {

    for (int i = 0; i < NBRE_DIGIT_ACQ; i++) {

        bleCode[i] = getCharacterFormRx();
    }
    if (bleCode[0] == '#') {

        bleCode[0] = 32;
    }
    bleCode[NBRE_DIGIT_ACQ - 1] = '\0';
    releaseCLK();

}

char setCharacterBit(char N, int reading, int K) {


    if (reading == 1) {

        return (N = N | 1 << K);

    }


    if (reading == 0) {

        return (N = N & ~(1 << K));
    }


}

void waitForBleAcq() {

    while (AN4_GetValue() == 1) {
    }

    while (AN4_GetValue() == 0) {
    }

    __delay_ms(40);
}

bool waitForBleAcq2() {

    long time = 0;
    while (AN4_GetValue() == 1) {

        time++;
        if (time > 4000000) {

            return false;
        }
    }
    time = 0;
    while (AN4_GetValue() == 0) {

        time++;
        if (time > 4000000) {

            return false;
        }
    }

    __delay_ms(40);
    return true;
}

bool analyseCodeBLE(char * bleCode) {

    if (bleCode[1] == 'B' && bleCode[2] == 'X') {

        return true;
    }

    if (bleCode[1] == '0' && bleCode[2] == '0') {

        return false;
    }

    return false;
}

void attenteDemarrage2(bool *autom, bool *testAct, bool *prog) {

    unsigned char reception;
    bool repOperateur = false;

    while (!repOperateur) {


        if (IN3_GetValue() == 0) {

            printf("-> TEST MANUEL EN COURS\r\n");
            repOperateur = true;
            *autom = false;
            *prog = false;
            *testAct = true;
        }

        if (eusartRxCount != 0) {

            reception = EUSART_Read(); // read a byte for RX

            switch (reception) // check command  
            {

                case '0':
                {
                    RESET();

                }

                case '1':
                {
                    printf("-> TEST ON\r\n");
                    *autom = true;
                    *testAct = true;
                    *prog = false;
                    __delay_ms(50);
                    repOperateur = true;
                    break;
                }

                case '6':
                {
                    printf("-> ERREUR PROGRAMMATION\r\n");
                    displayManager(TITRE, LIGNE_VIDE, ERREUR_PROGRAMMATION, LIGNE_VIDE);
                    *autom = true;
                    __delay_ms(50);
                    repOperateur = true;
                    *testAct = false;
                    REL8_SetLow();
                    break;
                }

                case '7':
                {
                    printf("-> PROGRAMMATION EN COURS\r\n");
                    displayManager(TITRE, LIGNE_VIDE, EN_PROG, LIGNE_VIDE);
                    *autom = true;
                    *testAct = false;
                    *prog = false;
                    __delay_ms(50);
                    repOperateur = true;
                    REL8_SetHigh();
                    break;
                }

                case '9':
                {
                    printf("-> PROGRAMMATION TERMINEE\r\n");
                    displayManager(TITRE, LIGNE_VIDE, FIN_PROG, LIGNE_VIDE);
                    *autom = true;
                    __delay_ms(50);
                    repOperateur = true;
                    *testAct = false;
                    REL8_SetLow();
                    break;
                }

                case '8':
                {
                    printf("-> EFFACEMENT EN COURS\r\n");
                    displayManager(TITRE, LIGNE_VIDE, EN_EFFACEMENT, LIGNE_VIDE);
                    *autom = true;
                    *testAct = false;
                    *prog = true;
                    __delay_ms(50);
                    repOperateur = true;
                    REL8_SetHigh();
                    break;
                }

                case '5':
                {
                    printf("-> EFFACEMENT TERMINE\r\n");
                    displayManager(TITRE, LIGNE_VIDE, FIN_EFFACEMENT, LIGNE_VIDE);
                    *autom = true;
                    *testAct = false;
                    *prog = true;
                    __delay_ms(50);
                    repOperateur = true;
                    REL8_SetLow();
                    break;
                }

            }
        }
    }

}

void resetModuleBle() {

    activerResetModuleBle();
    __delay_ms(3000);
    startAlert();
    RESET();
}

void activerResetModuleBle() {

    ledConforme(true);
    ledNonConforme(true);
    ledProgession(true);
    AN3_SetHigh();
    __delay_ms(5000);
    AN3_SetLow();

}

bool checkModuleBle(){

    long time = 0;
    while (AN4_GetValue() == 0) {

        time++;
        if (time > 4000000) {

            return false;
        }
    }

    return true;

}
