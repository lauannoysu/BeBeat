int setVermelhos[] = { 12, 13 };
int setAmarelos[] = { 10, 11 };
int setAzuis[] = { 5, 6 };
int setVerdes[] = { 8, 7 };

unsigned long tempoInicio;
bool ledsAcesos = false;
bool rodadaIniciada = false;
int *setAtivo = nullptr;
int buzzer = 9;  // pino do buzzer

int escolhido = -1;
// Joystick
int pinX = A0;
int pinY = A1;
int pinSW = 2;

const int NUM_LEDS = 2;

String direcaoAtual = " ";
// Vetor de sets
int *sets[] = { setVermelhos, setAmarelos, setAzuis, setVerdes };
const int NUM_SETS = 4;

// Nomes dos sets
const char *nomesSets[] = { "Vermelho", "Amarelo", "Azul", "Verde" };

// Tempo aceso
unsigned long tempoAceso = 4000;
int ultimaVelocidade = 4000;

// Pontuacao
int Score = 0;
int valorPorAcerto = 1;
int erros = 0;

bool pausado = false;

void setup() {
  Serial.begin(9600);
  pinMode(pinSW, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);

  while (digitalRead(pinSW) == HIGH) {
  }

  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(setVermelhos[i], OUTPUT);
    pinMode(setAmarelos[i], OUTPUT);
    pinMode(setAzuis[i], OUTPUT);
    pinMode(setVerdes[i], OUTPUT);
  }

  randomSeed(analogRead(A0));
}

void loop() {

 if (digitalRead(pinSW) == LOW) {  // botão pressionado
  pausado = !pausado;             // alterna estado
  delay(250);                     // evita múltiplos toggles enquanto segura
}

  if(!pausado){

  // rodada começando
  if (!rodadaIniciada) {
    flashTransicao();

    // escolher cor APENAS aqui
    escolhido = random(0, NUM_SETS);  // escolhe o numero do set a ser ligado
    setAtivo = sets[escolhido];       //atribui o set ativo a o numero sorteado

    // acende todos os leds do set sorteado
    for (int j = 0; j < NUM_LEDS; j++) {
      digitalWrite(setAtivo[j], HIGH);
    }

    tempoInicio = millis();  //marca o tempo atual
    rodadaIniciada = true;   //diz que ja tem uma rodada em andamento
    ledsAcesos = true;       // comunica que os leds estão acessos
  }

  // DURANTE A RODADA → ler joystick sempre
  lerJoystick();      //ler comando do jystick
  verificarAcerto();  //checa se o comando foi certo

  // se os leds estão acessos, e o tempo tempo atual, menos o tempo que a rodada inicio forem maiores ou iguais ao tempo maximo que a led fica acessa
  if (ledsAcesos && millis() - tempoInicio >= tempoAceso) {

    for (int j = 0; j < NUM_LEDS; j++) {  //apaga o set ativo
      digitalWrite(setAtivo[j], LOW);
    }

    ledsAcesos = false;
    rodadaIniciada = false;  // agora pode iniciar outra rodada
  }

  modos();
  }
}

void lerJoystick() {
  int eixoX = analogRead(pinX);  // valores de 0 a 1023
  int eixoY = analogRead(pinY);
  int botao = digitalRead(pinSW);  // LOW = pressionado


  if (eixoX < 300) {
    direcaoAtual = "Esquerda";
  } else if (eixoX > 700) {
    direcaoAtual = "Direita";
  } else if (eixoY < 300) {
    direcaoAtual = "Baixo";
  } else if (eixoY > 700) {
    direcaoAtual = "Cima";
  } else {
    direcaoAtual = "Centro";
  }

  // Serial.println(direcaoAtual);
}

// Cima - Vermelho [0]
// Esquerda - Amarelo [1]
// Baixo - Azul [2]
// Direita - Verde [3]

void verificarAcerto() {
  if (!ledsAcesos) return;  // Impede verificar depois do fim da rodada

  bool acertou = false;

  if (escolhido == 0 && direcaoAtual == "Cima") acertou = true;
  else if (escolhido == 1 && direcaoAtual == "Esquerda") acertou = true;
  else if (escolhido == 2 && direcaoAtual == "Baixo") acertou = true;
  else if (escolhido == 3 && direcaoAtual == "Direita") acertou = true;

  if (acertou) {

    Score += valorPorAcerto;
    Serial.println("Acertou!");
    somAcerto();

    // APAGAR LEDs imediatamente
    for (int j = 0; j < NUM_LEDS; j++) {
      digitalWrite(setAtivo[j], LOW);
    }

    // FINALIZA rodada
    ledsAcesos = false;
    rodadaIniciada = false;

    return;
  }

  // Se não acertou:
  if (direcaoAtual != "Centro") {
    Serial.println("Direcao Errada");
    // APAGAR LEDs imediatamente
    for (int j = 0; j < NUM_LEDS; j++) {
      digitalWrite(setAtivo[j], LOW);
    }

    // FINALIZA rodada
    ledsAcesos = false;
    rodadaIniciada = false;
    erros += valorPorAcerto;
    somErro();
  }
}

void somAcerto() {
  // Nota 1: 3000 Hz (a mais alta inicial)
  tone(buzzer, 3000, 150);
  delay(150);

  // Nota 2: 2000 Hz (tom médio)
  tone(buzzer, 2000, 150);
  delay(150);

  // Nota 3: 1500 Hz (a mais baixa de erro)
  tone(buzzer, 1500, 200);
  delay(200);

  noTone(buzzer);
  delay(50);
}

void somErro() {

  tone(buzzer, 2000, 300);  // nota aguda

  delay(200);

  tone(buzzer, 2000, 150);  // mais agudo ainda

  delay(200);
}

void efeitoTrocaDeFase() {
  somPowerUp();
  for (int k = 0; k < 5; k++) {
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(setVermelhos[i], HIGH);
      digitalWrite(setAmarelos[i], HIGH);
      digitalWrite(setAzuis[i], HIGH);
      digitalWrite(setVerdes[i], HIGH);
    }
    delay(200);

    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(setVermelhos[i], LOW);
      digitalWrite(setAmarelos[i], LOW);
      digitalWrite(setAzuis[i], LOW);
      digitalWrite(setVerdes[i], LOW);
    }
    delay(200);
  }
}



// Mudança de velocidade
void modos() {
  int novaVelocidade = tempoAceso;

  if (Score > 30)
    novaVelocidade = 1000;
  else if (Score > 20)
    novaVelocidade = 2000;
  else if (Score > 10)
    novaVelocidade = 3000;
  else
    novaVelocidade = 4000;


  if (novaVelocidade != ultimaVelocidade) {
    Serial.println("TROCANDO DE FASE!");
    efeitoTrocaDeFase();
    ultimaVelocidade = novaVelocidade;
  }

  tempoAceso = novaVelocidade;
}

void somPowerUp() {
  int notas[] = { 300, 400, 550, 700, 900, 1200 };
  int duracoes[] = { 120, 120, 100, 100, 100, 200 };

  for (int i = 0; i < 6; i++) {
    tone(buzzer, notas[i], duracoes[i]);
    delay(duracoes[i] + 20);
  }

  noTone(buzzer);
}

void piscarLuz() {
  for (int j = 0; j < NUM_LEDS; j++) {
    digitalWrite(setAtivo[j], LOW);
  }
}

void flashTransicao() {
  for (int k = 0; k < 2; k++) {
    // LIGA TODOS
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(setVermelhos[i], HIGH);
      digitalWrite(setAmarelos[i], HIGH);
      digitalWrite(setAzuis[i], HIGH);
      digitalWrite(setVerdes[i], HIGH);
    }
    delay(80);

    // DESLIGA TODOS
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(setVermelhos[i], LOW);
      digitalWrite(setAmarelos[i], LOW);
      digitalWrite(setAzuis[i], LOW);
      digitalWrite(setVerdes[i], LOW);
    }
    delay(80);
  }
}

