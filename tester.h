#define LIM_H 800  // 480 pour Vcc=3.3V
#define LIM_L 700  // 480 pour Vcc=3.3V   // 700 pour D937
#define NBRE_DIGIT_ACQ 20  // longueur de la trâme de transmission code BLE
#define HALF_PERIOD_TRANSMISSION_MS 20  // demi-periode de l'horloge maitre (ms)

void initialConditions(bool *, bool *, bool *, bool *);
void pressBP1(bool active);
void pressBP2(bool active);
void alimenter(bool active);
bool testR1(bool active);
bool testR2(bool active);
bool testR3(bool active);
bool testOK(bool active);
bool testNOK(bool active);
void ledNonConforme(bool active);
void ledConforme(bool active);
void ledProgession(bool active);
void attenteDemarrage(bool *, bool *);
void alerteDefaut(char etape[], bool *, bool *);
void alerteDefautEtape16(char etape[], bool *, bool *, bool *, bool *);
bool reponseOperateur(bool automatique);
bool controlVisuel();
void setHorloge(bool active);
void setP1(bool active);
void setP2(bool active);
void activerBuzzer();
void activerTouche(void);
void startAlert(void);
void errorAlert(void);
void okAlert(void);
void attenteDemarrage2(bool *, bool *, bool *);
void attenteDemarrage3(bool *, bool *, bool *, bool *, bool *);
void attenteAquittement(bool *, bool *);
void sortieErreur(bool *, bool *, bool *, bool *);
void marchePAP();
void processSlaveResponse(char repSlave, bool *);
void processActionForSlave(bool *autom, bool *testAct, bool *prog, bool *testSlaveActive, unsigned char orderFromWin);
unsigned char getRS232();
void startPhaseBLE(int);
void activeCLK();
void releaseCLK();
char getCharacterFormRx();
void getBLEindentifier(char *);
char setCharacterBit(char N, int reading, int K);
void waitForBleAcq();
bool analyseCodeBLE(char *);





