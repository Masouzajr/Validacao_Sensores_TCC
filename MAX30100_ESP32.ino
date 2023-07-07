double vetor_bpm[amostra]; //Declaração do vetor para armazenar a frequência cardíaca obtida
double vetor_spo2[amostra]; //Declaração do vetor para armazenar a saturação obtida
int flag; //Flag para sinalizar que entrou em alguma função
int fim_inicializacao; //Flag para indicar que o vetor foi preenchido por completo por dados obtidos pelo menos uma vez

//Função para sinalizar que uma pulsação foi detectada
void onBeatDetected() 
{
    Serial.println("Pulsação detectada!");
}

void setup()
{
    pinMode(2, OUTPUT); //led verde ligado no pino 2 de saída do ESP32
    pinMode(4, OUTPUT); //led amarelo ligado no pino 4 de saída do ESP32
    pinMode(5, OUTPUT); //led vermelho ligado no pino 5 de saída do ESP32
    Serial.begin(115200);

    
    Serial.print("Inicializando MAX30100 ...");
    
    //Função para verificar a comunicação com o sensor MAX30100
    if (!pox.begin()) { 
        Serial.println("FALHA NA COMUNICAÇÃO COM O SENSOR"); //Resposta do programa caso a comunicação não esteja correta
        for(;;);
    } else {
        Serial.println("SENSOR COMUNICADO COM SUCESSO"); //Resposta do programa caso a comunicação esteja correta
    }

    pox.setIRLedCurrent(MAX30100_LED_CURR_50MA); //Alterando a corrente do infre-vermelho para 7.6mA

    pox.setOnBeatDetectedCallback(onBeatDetected);

    //Inicializando o vetor da lista com valores iguais a -999
    for (int i=0 ; i<amostra; i++){
          vetor_bpm[i] = -999;
          vetor_spo2[i] = -999;
        }
}

void loop()
{
    pox.update(); //Leitura do sensor

    if (millis() - tsLastReport > REPORTING_PERIOD_MS) { //Atualiza os valores no terminal em um intervalo de 1,0s
        
        Serial.print("Frequência cardíaca:");
        Serial.print(pox.getHeartRate()); //Faz a leitura da frequência cardíaca
        Serial.print("bpm / Saturação do sangue:");
        Serial.print(pox.getSpO2()); //Faz a leitura da saturação do sangue
        Serial.println("%");
        
        flag = 0;

        //Função para verificar se o vetor da lista já foi preenchido por completo pelo menos uma vez
        if (fim_inicializacao != 5){ 
          for (int i=0; i<amostra; i++){ //Função para preencher o vetor da lista pela primeira vez
            if (vetor_bpm[i] == -999){ //Se o valor no index analisado for igual a -999, significa que aquele espaço ainda não recebeu nenhum dado
              vetor_bpm[i] = pox.getHeartRate(); //Espaço vazio igual ao valor de batimento cardíaco obtido
              vetor_spo2[i] = pox.getSpO2(); // Espaço vazio igual ao valor de saturação obtido
              flag = 1; //Bandeira sinaiza que entrou naquela função e impede que o mesmo dado seja adicionado mais de uma vez no vetor da lista
              fim_inicializacao = i; //Atualiza o valor da variável "fim_inicialização"
              break; //Sai da função "for" pois o dado já foi alocado a um espaço vazio
            }
          }
        }
        
        //Função para adicionar os valores medidos na lista utilizando a teoria FIFO
        if (flag == 0){ 
          for (int j=0; j<(amostra-1); j++){
            //Movendo os valores dentro da lista, removendo o primeiro valor contido na lista (FIFO)
            vetor_bpm[j] = vetor_bpm[j+1]; 
            vetor_spo2[j] = vetor_spo2[j+1];
          }
          
          //Atualizando o último valor obtido na lista
          vetor_bpm[amostra-1] = pox.getHeartRate();
          vetor_spo2[amostra-1] = pox.getSpO2(); 
        }

        //Variáveis para guardar valores de soma e média da frequência cardíaca
        float soma_bpm = 0.00;
        float media_bpm = 0.00;

        //Variáveis para guardar valores de soma e média da saturação
        float soma_spo2 = 0.00;
        float media_spo2 = 0.00;

        //Função para somar todos os dados de frequência cardíaca e saturação que possuem na lista
        for (int i=0; i<amostra; i++){ 
          soma_bpm += vetor_bpm[i];
          soma_spo2 += vetor_spo2[i];
        }

        //Média dos dados obtidos na listas
        media_bpm = soma_bpm/amostra;
        media_spo2 = soma_spo2/amostra;
        
        Serial.println("");
        Serial.print("A MÉDIA DOS BATIMENTOS É: ");
        Serial.print(media_bpm);
        Serial.print("bpm / A MÉDIA DA SATURAÇÃO É:");
        Serial.print(media_spo2);
        Serial.println("%");
        Serial.println("");

        //Função para verificar se a média obtida está dentro do esperado, se sim, ascende o LED Verde
        if ((media_bpm>=50 and media_bpm<=80) and media_spo2>=95){
          Serial.println("LED VERDE LIGADO");
          digitalWrite(2, HIGH); //Manipulando o estado do LED Verde
          digitalWrite(4, LOW); //Manipulando o estado do LED Amarelo
          digitalWrite(5, LOW); //Manipulando o estado do LED Vermelho
        }

        //Função para verificar se a média obtida está fora do esperado, entretando nada crítico, se sim, ascende o LED Amarelo
        if ((media_bpm>80 and media_bpm<=100) and (media_spo2<95 and media_spo2>=90)){
          Serial.println("LED AMARELO LIGADO");
          digitalWrite(2, LOW); //Manipulando o estado do LED Verde
          digitalWrite(4, HIGH); //Manipulando o estado do LED Amarelo
          digitalWrite(5, LOW); //Manipulando o estado do LED Vermelho
        }
        
        //Função para verificar se a média obtida está crítica, se sim, ascendo o LED Vermelho
        if ((media_bpm<50 or media_bpm>100) or media_spo2<90){
          Serial.println("LED VERMELHO LIGADO");
          digitalWrite(2, LOW); //Manipulando o estado do LED Verde
          digitalWrite(4, LOW); //Manipulando o estado do LED Amarelo
          digitalWrite(5, HIGH); //Manipulando o estado do LED Vermelho
        }
        tsLastReport = millis();
    }
}
