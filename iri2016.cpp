#include <iostream>
#include "iri2016.h"

AndoLab::iri2016::iri2016(void){ /* */
  outf = new float [20*1000];
  oarr = new float [100];

  ivar = 1;

  jf = new int [50];
  /* set default options */
  for(int i = 0; i < 50; i++){
    jf[i] = 1;
  }
  jf[3] = 0;
  jf[4] = 0;
  jf[5] = 0;
  jf[20] = 0;
  jf[22] = 0;
  jf[23] = 0; /* not default */
  //jf[25] = 0;
  jf[27] = 0;
  jf[28] = 0;
  jf[29] = 0;
  jf[32] = 0;
  jf[34] = 0;
  jf[38] = 0;
  jf[46] = 0;

  for(int i = 0; i < 100; i++){
    oarr[i] = -1.0f;
  }

  /* need to call prior to computation */
  read_ig_rz_();
  readapf107_();
}

AndoLab::iri2016::~iri2016(void){ /* memory deallocation */

  delete [] outf;
  delete [] oarr;
  delete [] jf;
}

void AndoLab::iri2016::set_coord(const float latitude, const float longitude,
                        iri_coord coordinate){
  lati = latitude;
  longi = longitude;
  jmag = int(coordinate);

  calculated = false;
}

void AndoLab::iri2016::set_datetime
(const int v_year, const int v_month, const int v_day,
 const int v_hour, const int v_minute, iri_time UT_or_LT){

  year = v_year;
  mmdd = v_month * 100  +  v_day;
  hour = v_hour + v_minute / 60.0;
  iut = int(UT_or_LT);

  calculated = false;
}

void AndoLab::iri2016::set_height
(const float height_begin_km,
 const float height_end_km,
 const float height_step_km){

  vbeg = height_begin_km;
  vend = height_end_km;
  vstp = height_step_km;

  nstep = int( (vend - vbeg) / vstp );

  height = 100.0;
  h_tec_max = 0.0;

  calculated = false;
}

constexpr int Number_of_column { 20 };

void AndoLab::iri2016::get_parameter(parameter param, float *array){
  if ( !calculated ){
    calculate();
    calculated = true;
  }

  for(int i = 0; i < nstep; i++){
    float v = outf[int(param) + i*Number_of_column];
    if (v == -1.0){
      v = 0.0;
    }
    array[i] = v;
  }
}

void AndoLab::iri2016::get_Ne(float *Ne_array){
  get_parameter(parameter::Electron_density, Ne_array);
}

void AndoLab::iri2016::get_Tn(float *Tn_array){
  get_parameter(parameter::Neutral_Temperature, Tn_array);
}

void AndoLab::iri2016::get_Ti(float *Ti_array){
  get_parameter(parameter::Ion_Temperature, Ti_array);
}

void AndoLab::iri2016::get_Te(float *Te_array){
  get_parameter(parameter::Electron_Temperature, Te_array);
}


constexpr int Dregion_Electron_Density { 14-1 };
void AndoLab::iri2016::get_Ne_Dregion(float *NeD_array){
  if ( !calculated ){
    calculate();
    calculated = true;
  }

  for(int i = 0; i < 77; i++){
    float v = outf[Dregion_Electron_Density + i*Number_of_column];
    if (v == -1.0){
      v = 0.0;
    }
    NeD_array[i] = v;
  }
}

void AndoLab::iri2016::calculate(void){
  iri_web_(&jmag, jf, &lati, &longi,
           &year, &mmdd, &iut, &hour,
           &height, &h_tec_max, &ivar,
           &vbeg, &vend, &vstp,
           outf, oarr);
}
