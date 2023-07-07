/*====================================================================================================== 
   MLX90614 ------------- Arduino
   VDD ------------------ 3.3V
   VSS ------------------ GND
   SDA ------------------ SDA (A4 on older boards)
   SCL ------------------ SCL (A5 on older boards) 
======================================================================================================*/


// Inclusão das bibliotecas necessárias
#include <Wire.h>                
#include <SparkFunMLX90614.h>    

#define amostra 6 //Tamanho da amostra que ficará armazenada na lista encadeada simples

//Criação da variável e do vetor que receberá os valores lidos pelo sensor
IRTherm temp;
double vetor_temp[amostra]; //Declaração do vetor para armazenar a temperatura obtida 
int flag; //Flag para indicar que o vetor foi preenchido por completo por dados obtidos pelo menos uma vez
int fim_inicializacao; //Flag para indicar que o vetor foi preenchido por completo por dados obtidos pelo menos uma vez

//Inicialização do sensor e determinação da unidade de medição das temperaturas
void setup() 
{
  pinMode(3, OUTPUT); //led verde ligado no pino 2 de saída do ESP32
  pinMode(4, OUTPUT); //led amarelo ligado no pino 4 de saída do ESP32
  pinMode(5, OUTPUT); //led vermelho ligado no pino 5 de saída do ESP32

  Serial.begin(9600);             
  temp.begin();                  
  temp.setUnit(TEMP_C); 

  Serial.print("Inicializando MLX30614 ...");   

  for (int i=0 ; i<amostra; i++){
          vetor_temp[i] = -999;
        

        }          
} 


//Loop de leitura de temperatura para o objeto a ser medido
void loop() 
{  
  if (temp.read()) 
  {    
    Serial.print("Object: " + String(temp.object(), 2));    
    Serial.println("°C");    
    Serial.println();
    flag = 0;

        //Função para verificar se o vetor da lista já foi preenchido por completo pelo menos uma vez
        if (fim_inicializacao != amostra-1){ 
          for (int i=0; i<amostra; i++){ //Função para preencher o vetor da lista pela primeira vez
            if (vetor_temp[i] == -999){ //Se o valor no index analisado for igual a -999, significa que aquele espaço ainda não recebeu nenhum dado
              vetor_temp[i] = temp.object(); //Espaço vazio igual ao valor de temperatura obtido
              flag = 1; //Bandeira sinaiza que entrou naquela função e impede que o mesmo dado seja adicionado mais de uma vez no vetor da lista
              fim_inicializacao = i; //Atualiza o valor da variável "fim_inicialização"
              break; //Sai da função "for" pois o dado já foi alocado a um espaço vazio
            }
          }
        }

        if (flag == 0){ 
          for (int j=0; j<(amostra-1); j++){
            //Movendo os valores dentro da lista, removendo o primeiro valor contido na lista (FIFO)
            vetor_temp[j] = vetor_temp[j+1];
            
          }          
          //Atualizando o último valor obtido na lista
          vetor_temp[amostra-1] = temp.object();         
        }
         //Variáveis para guardar valores de soma e média de temperatura
        float soma_temp = 0.00;
        float media_temp = 0.00;
        
        //Função para somar todos os dados de temperatura que possuem na lista
        for (int i=0; i<amostra; i++){ 
          soma_temp += vetor_temp[i];
        }

        //Média dos dados obtidos na listas
        media_temp = soma_temp/amostra;
        Serial.println("");
        Serial.print("A MÉDIA DAS TEMPERATURAS É: ");
        Serial.print(media_temp);
        Serial.println("°C");
        if (media_temp>=36 and media_temp<=37.5)  {
          digitalWrite(2, HIGH); //Manipulando o estado do LED Verde
          digitalWrite(4, LOW); //Manipulando o estado do LED Amarelo
          digitalWrite(5, LOW); //Manipulando o estado do LED Vermelho
        }
        //Função para verificar se a média obtida está fora do esperado, entretando nada crítico, se sim, ascende o LED Amarelo
        if ((media_temp<36 and media_temp>35.5) or (media_temp>37.5 and media_temp<=38.5)){
          digitalWrite(2, LOW); //Manipulando o estado do LED Verde
          digitalWrite(4, HIGH); //Manipulando o estado do LED Amarelo
          digitalWrite(5, LOW); //Manipulando o estado do LED Vermelho
        }        
        //Função para verificar se a média obtida está crítica, se sim, ascendo o LED Vermelho
        if (media_temp<35.5 or media_temp>38.5){
          digitalWrite(2, LOW); //Manipulando o estado do LED Verde
          digitalWrite(4, LOW); //Manipulando o estado do LED Amarelo
          digitalWrite(5, HIGH); //Manipulando o estado do LED Vermelho
        } 
  }
  delay(1000);  
} 





