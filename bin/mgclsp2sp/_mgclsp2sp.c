/* ----------------------------------------------------------------- */
/*             The Speech Signal Processing Toolkit (SPTK)           */
/*             developed by SPTK Working Group                       */
/*             http://sp-tk.sourceforge.net/                         */
/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 1984-2007  Tokyo Institute of Technology           */
/*                           Interdisciplinary Graduate School of    */
/*                           Science and Engineering                 */
/*                                                                   */
/*                1996-2014  Nagoya Institute of Technology          */
/*                           Department of Computer Science          */
/*                                                                   */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/* - Redistributions of source code must retain the above copyright  */
/*   notice, this list of conditions and the following disclaimer.   */
/* - Redistributions in binary form must reproduce the above         */
/*   copyright notice, this list of conditions and the following     */
/*   disclaimer in the documentation and/or other materials provided */
/*   with the distribution.                                          */
/* - Neither the name of the SPTK working group nor the names of its */
/*   contributors may be used to endorse or promote products derived */
/*   from this software without specific prior written permission.   */
/*                                                                   */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND            */
/* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,       */
/* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF          */
/* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS */
/* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          */
/* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     */
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   */
/* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    */
/* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           */
/* POSSIBILITY OF SUCH DAMAGE.                                       */
/* ----------------------------------------------------------------- */

/***************************************************************
    $ $

    Transform mgc-lsp to spectrum

        void   mgclsp2sp(a, g, lsp, m, x, l)

        double   a     : alpha
        double   g     : gamma
        double   *lsp  : mgc-lsp
        int      m     : order of mgc-lsp
        double   *x    : real part of spectrum
        double   l     : FFT length

***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

#define LOG2 (0.693147180559945)

double log_conv_in_mgclsp2sp(double x)
{
   double temp;

   temp = log(fabs(x));
   if (temp < LSMALL)
      return LZERO;
   else
      return temp;
}

double log_add_in_mgclsp2sp(double x, double y)
{
   double lmin, lmax;

   if (x == y)
      return x + LOG2;

   lmin = (x < y) ? x : y;
   lmax = (x < y) ? y : x;
   if (lmax > lmin + 50)
      return lmax;
   else
      return lmax + log_conv_in_mgclsp2sp(exp(lmin - lmax) + 1.0);
}

double mel_conv(double a, double w)
{
   return w + 2.0 * atan(a * sin(w) / (1.0 - a * cos(w)));
}

void mgclsp2sp(double a, double g, double *lsp, const int m, double *x,
               const int l, const int gain)
{
   int i, p;
   double w, eq1, eq2, ap = 0.0;

   for (p = 0; p < l; p++) {
      eq1 = 0.0;
      eq2 = 0.0;
      w = mel_conv(a, p * (M_PI / (l - 1)));

      if (m % 2 == 0) {
         for (i = 0; i < m / 2; i++) {
            eq1 += 2.0 * log_conv_in_mgclsp2sp(cos(w) - cos(lsp[2 * i + gain]));
            eq2 += 2.0 * log_conv_in_mgclsp2sp(cos(w) - cos(lsp[2 * i + 1 + gain]));
         }
         eq1 += 2.0 * log_conv_in_mgclsp2sp(cos(w / 2.0));
         eq2 += 2.0 * log_conv_in_mgclsp2sp(sin(w / 2.0));

         ap = m * log(2.0) + log_add_in_mgclsp2sp(eq1, eq2);
      } else {
         for (i = 0; i < (m + 1) / 2; i++)
            eq1 += 2.0 * log_conv_in_mgclsp2sp(cos(w) - cos(lsp[2 * i + gain]));
         for (i = 0; i < (m - 1) / 2; i++)
            eq2 += 2.0 * log_conv_in_mgclsp2sp(cos(w) - cos(lsp[2 * i + 1 + gain]));
         eq2 += 2.0 * log_conv_in_mgclsp2sp(sin(w));

         ap = (m - 1.0) * log(2.0) + log_add_in_mgclsp2sp(eq1, eq2);
      }

      x[p] = -0.5 * ap;
      x[p] *= -(1.0 / g);
      if (gain == 1)
         x[p] += lsp[0];
   }
}
