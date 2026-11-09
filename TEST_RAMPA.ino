// ===== PINOS =====
const int led1Pin = 22;
const int led2Pin = 24;

const int ldr1Pin = A1; // LDR1 (início)
const int ldr2Pin = A0; // LDR2 (fim)

// ===== LIMIARES COM HISTERESE (adaptados às suas leituras) =====
const int LDR1_ESCURA = 50;
const int LDR1_CLARA  = 100;

const int LDR2_ESCURA = 147;
const int LDR2_CLARA  = 94;

// ===== ESTADOS =====
enum Estado {
  AGUARDANDO_LDR1,
  CRONOMETRANDO,
  AGUARDANDO_REARME
};

Estado estado = AGUARDANDO_LDR1;

// ===== VARIÁVEIS DE CONTROLE =====
bool ldr1Claro = true;
bool ldr2Claro = true;

unsigned long tempoInicio = 0;

void setup() {
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  digitalWrite(led1Pin, HIGH);
  digitalWrite(led2Pin, HIGH);

  Serial.begin(9600);
  delay(50);

  // inicializa os estados claros com base nas leituras iniciais
  int v1 = analogRead(ldr1Pin);
  int v2 = analogRead(ldr2Pin);
  ldr1Claro = (v1 > LDR1_CLARA);
  ldr2Claro = (v2 > LDR2_CLARA);

  Serial.println("Sistema pronto. Aguardando passagem (LDR1).");
}

void loop() {
  int v1 = analogRead(ldr1Pin);
  int v2 = analogRead(ldr2Pin);

  // aplica histerese LDR1
  if (ldr1Claro) {
    if (v1 < LDR1_ESCURA) ldr1Claro = false;
  } else {
    if (v1 > LDR1_CLARA) ldr1Claro = true;
  }

  // aplica histerese LDR2
  if (ldr2Claro) {
    if (v2 < LDR2_ESCURA) ldr2Claro = false;
  } else {
    if (v2 > LDR2_CLARA) ldr2Claro = true;
  }

  // máquina de estados
  if (estado == AGUARDANDO_LDR1) {
    // detecta sombra no LDR1 (claro -> escuro)
    if (!ldr1Claro) {
      tempoInicio = micros();
      estado = CRONOMETRANDO;
      Serial.println("Cronometro iniciado.");
    }
  }
  else if (estado == CRONOMETRANDO) {
    // detecta sombra no LDR2 (claro -> escuro)
    if (!ldr2Claro) {
      unsigned long tempoFim = micros();
      unsigned long delta = tempoFim - tempoInicio;
      float tempoSegundos = delta / 1000000.0;
      Serial.print("Tempo medido: ");
      Serial.print(tempoSegundos, 3); // mostra milésimos
      Serial.println(" s");
      estado = AGUARDANDO_REARME;
      Serial.println("Aguardando os dois sensores voltarem a claro...");
    }
  }
  else if (estado == AGUARDANDO_REARME) {
    // rearmar só quando ambos estiverem claros
    if (ldr1Claro && ldr2Claro) {
      estado = AGUARDANDO_LDR1;
      Serial.println("Pronto para nova leitura.");
    }
  }

  delay(10);
}