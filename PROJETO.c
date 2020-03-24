//18.06 - PRECISA VER A QUESTAO DAS MASCARAS PARA ENVIAR NA FUNCAO SELECIONA!

///p2: p2.0 - din ; 2.1 - cs; 2.2 - clock

#include "io430.h"

void enviatabuleiro(char a[][8]);

char endereco, dado;
char bits[8] = {BIT0, BIT1, BIT2, BIT3, BIT4, BIT5, BIT6, BIT7};
char posicao_i, posicao_j, posicao_i2, posicao_j2;
char fimdejogo;
char flagselec;
char flagintp4;
char colunas[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
char linhas[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
char tabuleiro[8][8];
char primeiro, segundo;
int i, j;
char cod, line;
char irel, jrel;

void enviar (char endereco, char dado)
{
  int i = 7;
  P2OUT |= BIT1;
  // manda o endereço - qual coluna?
  while(i>=0)
  {
    P2OUT &= ~BIT2;
    if(endereco & bits[i])
    {
      P2OUT |= BIT0;
    }
    else
    {
      P2OUT &= ~BIT0;
    }
    P2OUT |= BIT2;
    i = i - 1;
  }
  i = 7;
  // manda o dado - qual linha?
  while(i>=0)
  {
    P2OUT &= ~BIT2;
    if(dado & bits[i])
    {
      P2OUT |= BIT0;
    }
    else
    {
      P2OUT &= ~BIT0;
    }
    P2OUT |= BIT2;
    P2OUT &= ~BIT0;
    i = i - 1;
  }
  P2OUT &= ~BIT1;
  P2OUT |= BIT1;
}

void limpar( void )
{
  int i;
  for(i=0; i <= 7; i++)
  {
    enviar(i, 0x00);
  }
}
  
void brilho (char intensidade)
{
  intensidade &= 0x0F;                                 // mask off extra bits
  enviar(0x0A, intensidade);           // set brightness
}

void enviatabuleiro(char a[][8])
{
  int u, v;
  cod = 0x00; // cod é o numero em hexa q sera enviado, dos elementos da linha
  line = 0x00;
  for(u = 0; u <= 7; u++)
  {
    for(v = 0; v <= 7; v++)
    {
      if(a[u][v] == 1)
      {
        cod |= bits[v];
      }
    }
    line = line + 1;
    enviar(line, cod);
    cod = 0x00;
  } 
}

void preenchetabuleiro(void)
{
  int x,y;
  for(x = 0; x <= 7; x++)
  {
    for(y = 0; y <= 7; y++)
    {
      tabuleiro[x][y] = 1;
    }
  }
  int d;
  for(d = 0; d<=7; d++)
  {
    tabuleiro[0][d] = 4;
  }
  tabuleiro[1][0] = 4;
  tabuleiro[1][1] = 4;
  tabuleiro[1][2] = 4;
  tabuleiro[1][6] = 4;
  tabuleiro[1][7] = 4;
  tabuleiro[2][0] = 4;
  tabuleiro[2][1] = 4;
  tabuleiro[2][2] = 4;  
  tabuleiro[2][6] = 4;
  tabuleiro[2][7] = 4;
  tabuleiro[3][0] = 4;
  tabuleiro[4][0] = 4;
  tabuleiro[4][4] = 0;
  tabuleiro[5][0] = 4;  
  tabuleiro[6][0] = 4;
  tabuleiro[6][1] = 4;
  tabuleiro[6][2] = 4;
  tabuleiro[6][6] = 4;
  tabuleiro[6][7] = 4;  
  tabuleiro[7][0] = 4;
  tabuleiro[7][1] = 4;
  tabuleiro[7][2] = 4;
  tabuleiro[7][6] = 4;
  tabuleiro[7][7] = 4;  
  enviatabuleiro(tabuleiro);
}

void inicializar (void)
{
  enviar(0x0B, 7);
  enviar(0x09, 0x00);
  enviar(0x0C, 1);
  enviar(0x0F, 0);
  limpar();
  brilho(0x0C);
}  

void config_micro (void)
{
  WDTCTL = WDTPW + WDTHOLD;
  DCOCTL = CALDCO_1MHZ;   //seta o clock em 1MHz
  BCSCTL1= CALBC1_1MHZ;  //Seta o clock em 1MHz
  
  posicao_i = 4;
  posicao_j = 4; // pósicoes do cursor, inicialmente em 4,4
  
  posicao_i2 = 2;
  posicao_j2 = 4;
  
  P1DIR = 0x00; // p1.7, 1.6, 1.5 e 1.4 são para os botôes, p1.3 confirma
  P2DIR = 0x07;
  P1REN = 0xF8;
  P1OUT = 0xF8;
  P1SEL = 0x00;
  P1SEL2 = 0x00;
  P1IE = 0xF8;
  P1IES = 0xF8;
  P1IFG = 0x00;
  P2SEL = 0x00;
  P2SEL2 = 0x00;
}

void seleciona (void)
{
  flagselec = 0;
  flagintp4 = 0;
  primeiro = 1;
  segundo = 0;
  int n;
  char mask = 0x00;
  while(flagselec == 0)
  {
    for(n = 1; n <= 7; n++)
    {
      if(tabuleiro[posicao_i][n] != 4)
      {
        if(tabuleiro[posicao_i][n] == 1)
        {
          mask |= bits[n];
        }
        else
        {
          mask |= 0x00 & bits[n];
        }
      }
    }
    n = 1;
    // pisca o led que está selecionado, no momento.
    enviar(colunas[posicao_i], mask ^ bits[posicao_j]);
    __delay_cycles(10000);
    mask = mask ^ bits[posicao_j];
    enviar(colunas[posicao_i], mask ^ bits[posicao_j]);
    mask = 0x00;
  }
}
 
void selecionadestino(void)
{
  flagselec = 0;
  primeiro = 0;
  segundo = 1;
  char mask = 0x00;
  while(flagselec == 0)
  {
    for(j = 1; j <= 7; j++)
    {
      if(tabuleiro[posicao_i2][j]!=4)
      {
        if(tabuleiro[posicao_i2][j] == 1)
        {
          mask |= bits[j];
        }
      }
    }
    j = 1;
    enviar(colunas[posicao_i2], mask ^ bits[posicao_j2]);
    __delay_cycles(10000);
    mask = mask ^ bits[posicao_j2];
    enviar(colunas[posicao_i2], mask ^ bits[posicao_j2]);
    mask = 0x00;
  }
  if(irel == 0)
  {
    if(jrel >0)
    {
      tabuleiro[posicao_i2][posicao_j2-1] = 0;
      tabuleiro[posicao_i2][posicao_j2] = 1;
    }
    else if((jrel < 0) && (tabuleiro[posicao_i][posicao_j-1] = 1))
    {
      tabuleiro[posicao_i2][posicao_j2+1] = 0;
      tabuleiro[posicao_i2][posicao_j2] = 1;
    }
  }
  else if(jrel == 0)
  {
    if((irel >0) & (tabuleiro[posicao_i+1][posicao_j] = 1))
    {
      tabuleiro[posicao_i2-1][posicao_j2] = 0;
      tabuleiro[posicao_i2][posicao_j2] = 1;
    }
    else if((irel <0) & (tabuleiro[posicao_i-1][posicao_j] = 1))
    {
      tabuleiro[posicao_i2+1][posicao_j2] = 0;
      tabuleiro[posicao_i2][posicao_j2] = 1;
    }
  }
}

void checarfim(void)
{
  int k, l;
  for(k = 0; k < 7 ; k++)
  {
    for(l = 0; l < 7; l++)
    {
      if((tabuleiro[k][l] == 1) & (tabuleiro[k+1][l] == 0) & (tabuleiro[k-1][l] == 0) & (tabuleiro[k][l+1] ==0) & (tabuleiro[k][l-1] == 0))
      {
        fimdejogo = 1;
      }
      else
      {
        fimdejogo = 0;
      }
    }
  }
}
                   
void main( void )
{
  config_micro();
  inicializar();
  preenchetabuleiro();
  fimdejogo = 0;
  __enable_interrupt();
  while(fimdejogo == 0) // cada ciclo do while é uma jogada.
  {
    seleciona();
    selecionadestino();
    enviatabuleiro(tabuleiro);
    checarfim();
  }
}

#pragma vector = PORT1_VECTOR 
__interrupt void interr_p1(void)
{
  if(P1IFG && BIT7) // botao cima
  {
    if((posicao_i>1) && (primeiro == 1))
    {
      posicao_i = posicao_i - 1;
    }
    else if((posicao_i2>1) && (segundo == 1))
    {
      posicao_i2 = posicao_i2 - 1;
    }  
    P1IFG = 0x00;
    enviatabuleiro(tabuleiro);
  }
  else if(P1IFG && BIT6) // botao abaixo
  {
    if((posicao_i<7) && (primeiro == 1))
    {
    posicao_i = posicao_i + 1;
    }
    else if((posicao_i2<7) && (segundo == 1))
    {
    posicao_i2 = posicao_i2 + 1;
    }
    P1IFG = 0x00;
    enviatabuleiro(tabuleiro);
  }
  else if (P1IFG && BIT5) // botao esquerda
  {
    if((posicao_j>1) && (primeiro == 1))
    {
      posicao_j = posicao_j - 1;
    }
    else if((posicao_j2>1) && (segundo == 1))
    {
    posicao_j2 = posicao_j2 - 1;
    }
    P1IFG = 0x00;
    enviatabuleiro(tabuleiro);
  }
  else if(P1IFG & BIT4) // botao direita
  {
    if((posicao_j<7) && (primeiro == 1))
    {
      posicao_j = posicao_j + 1;
    }
    else if((posicao_j2<7) && (segundo == 1))
    {
      posicao_j2 = posicao_j2 + 1;
    }
    P1IFG = 0x00;
    enviatabuleiro(tabuleiro);
  }
//botao confirma
  else if(P1IFG && BIT3)
  {
    if(flagintp4 == 0)
    {
      if(tabuleiro[posicao_i][posicao_j] == 1)
      {
        flagselec = 1;
      }
    }
    else if(flagintp4 == 1)
    {
      irel = posicao_i2 - posicao_i;
      jrel = posicao_j2 - posicao_j;
      if((!((posicao_i2 = posicao_i) & (posicao_j2 = posicao_j))) && ((posicao_i2 == posicao_i) || (posicao_j2 == posicao_j)) && ((jrel==2) ||(jrel==-2) || (irel==2) || (irel==-2)))
      {
        if(irel == 0)
        {
          if((jrel >0) && (tabuleiro[posicao_i][posicao_j+1] == 1) && (tabuleiro[posicao_i2][posicao_j2] == 0))
          {
            flagselec = 1;
          }
          else if((jrel <0) && (tabuleiro[posicao_i][posicao_j-1] == 1) && (tabuleiro[posicao_i2][posicao_j2] == 0))
          {
            flagselec = 1;
          }
        }
        else if(jrel == 0)
        {
          if((irel >0) && (tabuleiro[posicao_i+1][posicao_j] == 1) && (tabuleiro[posicao_i2][posicao_j2] == 0))
          {
            flagselec = 1;
          }
          else if((irel <0) && (tabuleiro[posicao_i-1][posicao_j] == 1) && (tabuleiro[posicao_i2][posicao_j2] == 0))
          {
            flagselec = 1;
          }
        }
      }
    }
    P1IFG = 0x00;
    enviatabuleiro(tabuleiro);
  }
}