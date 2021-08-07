#include <iostream>
#include <fstream>

#include <iri2016.h>

int main(void){

  AndoLab::iri2016 a;
  a.set_coord(35.0f, 142.0f);
  a.set_datetime(2015, 6, 14, 4, 0);
//  a.set_datetime(2015, 6, 14, 12, 0);
  a.set_height(60.f, 800.f, 1.0f);

  float *Ne = new float [800 - 60];
  float *Tn = new float [800 - 60];
  float *Ti = new float [800 - 60];
  float *Te = new float [800 - 60];
  float *D_region = new float [77];
  a.get_Ne(Ne);
  a.get_Tn(Tn);
  a.get_Ti(Ti);
  a.get_Te(Te);
  a.get_Ne_Dregion(D_region);
  std::ofstream ofs("output_cpp.dat");
  for(int i = 0; i < 800-60; i++){
    ofs << i+60 << " " << Ne[i] << " " << Tn[i] << " "
    << Ti[i] << " " << Te[i] << std::endl;
  }
  ofs.close();

  ofs.open("d_region.dat", std::ios::out);
  for(int m = 0; m < 7; m++){
    for(int i = 0; i < 11; i++){
      ofs << i*5 + 60 << " " << D_region[i + m*11] << std::endl;
    }
    ofs << std::endl;
  }
  ofs.close();


  return 0;
}
