#include <iostream>
#include <fstream>
#include <cmath>

#include "iri2016.h"

namespace AndoLab
{

  float func(float *Dan, float h){
    constexpr float Lowest_Danilov { 60.0f };
    constexpr float Step_Danilov { 5.0f };

    /* D regaion で 60 + 5*base ≦ h ≦ 60 + 5*(base+1) */
    int base = int( (h - Lowest_Danilov) / Step_Danilov );

    /* D regaion で 60 + 5*base を 0、 60 + 5*(base+1) を 1 とした h の 値 */
    float x = ( h - Lowest_Danilov - base*Step_Danilov ) / Step_Danilov;

    float l1 = std::log( Dan[base+1] );
    float l0 = std::log( Dan[base] );

    return std::exp( (l1 - l0) * x + l0 );
  }


  void original_model(const int N, const float Alt_min, const float Alt_stp, AndoLab::iri2016 &iri, float *mixed_Ne){

    /*
     * このルーチン内では 高度60km - Alt_max0 [km] で Alt_stp毎に求める
     * その後に Alt_min - Alt_max に直す
     */

    constexpr float Alt_min0 { 60.0f };
    const float Alt_max0 { Alt_min + N*Alt_stp };

    const int N_alt { fround((Alt_max0 - Alt_min0) / Alt_stp) + 1 };
    float *Ne = new float [N_alt];      /* IRIの電子密度 */
    float *D_region = new float [77];   /* IRIのD層 */
    float *protoNe = new float [N_alt]; /* 60-110km を Alt_stp毎 に D層部分を加えたもの */

    iri.set_height(Alt_min0, Alt_max0, Alt_stp);

    iri.get_Ne(Ne); /* IRIを取得 */
    iri.get_Ne_Dregion(D_region); /* D層オプション */

    /*** Danilovモデルの電子密度 ***/
    /* Danilov[0] : 60km
       Danilov[1] : 65km
       :
       Danilov[10] : 110km
     */
    float *Danilov = D_region + 2*11;

    /**** Danilov が 高高度側で 0 となる問題 ****/
    /* Danilovが 0 とならない最も高い高度のインデックス */
    int I_Alt_ZeroDanilov = 0;
    while( (I_Alt_ZeroDanilov < 11) && (Danilov[I_Alt_ZeroDanilov] != 0.0f) ){
      I_Alt_ZeroDanilov++;
    }
    I_Alt_ZeroDanilov--;
    float AltMax_Dan = 60.0f + 5.0f*I_Alt_ZeroDanilov; /* Danilovデータの最高高度 */

    /* Danilov が 0 となる高度があるなら、0となったところに IRI の値を入れる */
    if (I_Alt_ZeroDanilov < 10){
      Danilov[I_Alt_ZeroDanilov + 1] = Ne[ fround(5*(I_Alt_ZeroDanilov+1)/Alt_stp) ];
    }

    /* Danilov, IRI が 0 でないところで、交点 or 最接近を探す。交点 or 最接近高度が Min_alt */
    float Min_alt = 60.0f; /* 探せた高度 */
    int iH = 0;            /* 探した高度のインデックス */
    float Min_dif = 1e38;  /* その高度での両データの差。交点なら 0 */

    const int Nd = fround( (AltMax_Dan - Alt_min0) / Alt_stp ); /* 60km から Danilovデータの最高高度までのIRIデータ数 */
    for(int i = 0; i < Nd; i++){
      if ( Ne[i] == 0.0f ) continue; /* IRI のデータがない */
      float h = Alt_min0 + i * Alt_stp;

      /* 交点があった場合 */
      if ( (std::log10(func(Danilov, h)) - std::log10(Ne[i])) *
           (std::log10(func(Danilov, h+Alt_stp)) - std::log10(Ne[i+1])) < 0.0f ){

        /* 高度のサンプルで、値が近い方を交点の形にする */
        if ( std::abs(std::log10(func(Danilov, h)) - std::log10(Ne[i]))
             < (std::log10(func(Danilov, h+Alt_stp)) - std::log10(Ne[i+1])) ){
          Min_dif = 0.0f;
          Min_alt = h;
          iH = i;
        } else {
          Min_dif = 0.0f;
          Min_alt = h + Alt_stp;
          iH = i+1;
        }
        break;
        
      }

      /* 交点がない場合は、最近接点 */
      float dif = std::abs( std::log10(func(Danilov, h)) - std::log10(Ne[i]) );
      if (dif < Min_dif){
        Min_dif = dif;
        Min_alt = h;
        iH = i;
      }
    }

    /*** 2. データ作成 ***/

    const float Delta { 5.0f }; /* 最接近の点から上下この高度が遷移 */
    const int iH_low  = int( (Min_alt - Delta - 60.0f)/Alt_stp ); /* 遷移領域下部未満 */
    const int iH_high = int( (Min_alt + Delta - 60.0f)/Alt_stp ); /* 遷移領域下部未満 */

    /* 遷移領域下部未満は Danilovモデルをコピー */
    for(int ih = 0; ih < iH_low; ih++){
      float h = 60.0f + ih * Alt_stp;
      protoNe[ ih ] = func(Danilov, h);
    }

    if ( Min_dif == 0.0f ){ /* 交点が存在 */
      /* iH までは Danilov をコピー */
      for(int ih = iH_low; ih < iH; ih++){
        float h = 60.0f + ih * Alt_stp;
        protoNe[ ih ] = func(Danilov, h);
      }

      /* iH からは IRI をコピー */
      for(int ih = iH; ih < iH_high; ih++){
        protoNe[ ih ] = Ne[ ih ];
      }

    } else { /* 交点が存在しない */

    /*
     * 遷移領域下部( 最接近 Min_alt より Delta[km] 下 )で Danilov、
     * 遷移領域中央(Min_alt) で、Danilov と IRI の 指数部が平均
     * 
     */

    /* 遷移領域の計算 */
      const float fdm = std::log( func(Danilov, Min_alt) ); /* 最接近Danilov */
      const float fim = std::log( Ne[iH] ); /* 最接近IRI */
      const float fm = 0.5 * ( fdm + fim ); /* 平均、最接近の値とする */
      const float sd = fm - fdm; /* 下部のDanilovシフト量 */

      for(int ih = iH_low; ih < iH; ih++){
        float h = Alt_min0 + ih * Alt_stp;
        protoNe[ih] = func(Danilov, h) * std::exp( sd * (h - (Min_alt - Delta))/Delta );
      }

      const float si = fm - fim; /* 上部のIRIシフト量 */
      for(int ih = iH; ih < iH_high; ih++){
        float h = Alt_min0 + ih * Alt_stp;
        protoNe[ih] = Ne[ih] * std::exp( si * (Min_alt + Delta - h)/Delta );
      }
    }

    /* 遷移領域から上 (単にコピー) */
    for(int ih = iH_high; ih < N_alt; ih++){
      protoNe[ih] = Ne[ih];
    }

    /*** 3. 必要部分をコピー ***/
    for(int i = 0; i < N; i++){
      float h = Alt_min + i*Alt_stp;
      int ih = fround( (h - 60.0f)/Alt_stp );
      mixed_Ne[i] = protoNe[ih];
    }

    delete [] Ne;
    delete [] D_region;
    delete [] protoNe;

  }

}
