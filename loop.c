/* loop.c - Measure the time spend in some computations.

   Copyright (c) 2016, Monaco F. J. - <moanco@icmc.usp.br>

   This file is part of Real-Time Sand Box - RTSB

   RTSB is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* 

      Can you optimize this code?

*/


#include <stdio.h>
#include <math.h>
#include "utils.h"

#define MAX_OUTER_LOOP 100
#define MAX_INNER_LOOP 1000
#define PI 3.1416
#define LENGTH 100

/* This function returns length*cos(angle). */

float foo (int length, int angle, int* cos)
{
  return length * cos[angle];
}

/* Main program. */

int main ()
{
  int angle;
  float val;
  long int i, j;
  struct timeval now, before, elapsed;
  double average_loop_time, min_loop_time, max_loop_time;

  average_loop_time = 0;
  min_loop_time = average_loop_time;
  max_loop_time = average_loop_time;

  /* angle = (j * 30) % 360 
   * Valor de angle esta entre 0 e 330, de 30 em 30, sendo desnecessario recalcular os valores novamente.
   * Assim, indexar esses valores seria uma otima opcao para reduzir o tempo de calculo 
   * 
   * b = length * cos (angle * PI / 180);
   * Os valores no calculo da funcao foo original
   * Dependem apenas de angle, pois length eh uma constante
   * Como angle esta entre 0 a 330, ele eh o unico valor que varia no cosseno e o 
   * Calculo de cosseno depende diretamente dele, fazer uma indexacao
   * Para registrar os resultados de cos(angle*PI/180) eh uma otima opcao, pois gastaria menos
   * Tempo de calculo e obteria o valor em tempo constante*/

 
  int size = 360/30;/*numero de index que teria do valor 0 indo a 330, de 30 em 30*/
  int angle[size];
  int cos[331];/*Os unicos valores que variam para o calculo do cosseno eh o angulo, 
		 assim podemos usar o angulo como index para identificar o valor do calculo do cosseno*/
 
  /* Outer loop, to compute averages. */

  for (i = 0; i < size; i++){
  	angle[i] = i*30; 
	/* angle[0]=0 ,
	 * angle[1]=30,
	 * angle[2]=60,
	 * ...,
	 * angle[11]=330*/
  }

  for (i = 0; i < 360; i+=30){
  	cos[i] = cos(angle[i%30]*PI/180) 
	/* (angle[0 % 30]=0) => cos[0] = cos(0*PI/180)
	 * (angle[30 % 30]=30) => cos[30] = cos(30*PI/180)
	 * (angle[60 % 30]=60) => cos[60] = cos(60*PI/180)
	 * ...
	 * (angle[330 % 30] = 330) => cos[330] = cos(330*PI/180)*/
  }

  for (i = 0; i < MAX_OUTER_LOOP; i++)
    {

      /* Get current time before computation work. */
      
      gettimeofday (&before, NULL);

      /* Exec computation several times. */
      
      for (j=0; j<MAX_INNER_LOOP; j++)
	{
	  angle = angle[j % size];	/* 0, 30, 60, 90 ... 330, 0, 30 ... */
	  val = foo (LENGTH, angle, cos);
	  
	  #if DEBUG
	  fprintf (stdout, "(%lu) %3d %8.3f %u\n",
		   i, angle, val, (unsigned int) elapsed.tv_usec);
	  #endif
	  
	}

      /* Get current time after computation work. */
      
      gettimeofday (&now, NULL);
      timeval_subtract (&elapsed, &now, &before);


      /* Do some statistics. */
      
      average_loop_time += elapsed.tv_usec;

      if (i==0)
	min_loop_time = elapsed.tv_usec;
      else
	if (elapsed.tv_usec < min_loop_time)
	  min_loop_time = elapsed.tv_usec;
      if (elapsed.tv_usec > max_loop_time)
	max_loop_time = elapsed.tv_usec;
   
    }

  /* Show statistics. */
  
  average_loop_time = average_loop_time / MAX_OUTER_LOOP;

  printf ("Average loop time: %u\n", (int) average_loop_time);
  printf ("Minimum loop time: %u\n", (int) min_loop_time);
  printf ("Maximum loop time: %u\n", (int) max_loop_time);
   
  return 0;
}

