/*
 * IRI2016 wrapper for C++, ver.0.1
 *
 * TODO:
 * - enable variable parameters in IRIWEB
 * - enable options
 *
 *
 *
 */
#ifndef __IRI_2016__
#define __IRI_2016__

extern "C" {
  void iri_web_(int *jmag, int *jf, float *lati, float *longi,
                int *iyyyy, int *mmdd, int *iut, float *dhour,
                float *height, float* h_tec_max, int *ivar,
                float *vbeg, float *vend, float *vstp,
                float *outf, float *oarr);

  void read_ig_rz_(void);
  void readapf107_(void);
}

namespace AndoLab {

enum class iri_coord { geog, geomag };
enum class iri_time { LT, UT };
enum class parameter {
  Electron_density,
  Neutral_Temperature,
  Ion_Temperature,
  Electron_Temperature
};

class iri2016{
private:
  int jmag;
  int year, mmdd, iut;
  int ivar;
  float hour;
  float lati, longi;
  float *outf, *oarr;
  float vbeg, vend, vstp;
  int *jf;
  float height, h_tec_max;
  bool calculated;

  int nstep;

  void calculate(void);
public:
  iri2016(void);
  ~iri2016(void);

  /* Accessors */
  void set_coord /* Set the coordinate to get IRI2016 output */
  (const float Latitude,  /* in degree, -90 -> 90 */
   const float Longitude, /* in degree, -180 -> 180 */
   iri_coord=iri_coord::geog /* Specify the above coordinate is geomagnetic or geographic */
   );

  void set_datetime
  (const int v_year, const int v_month, const int v_day,
   const int v_hour, const int v_minute,
   iri_time=iri_time::UT
   );

  void set_height
  (const float height_begin_km,
   const float height_end_km,
   const float height_step_km);

  /* Obtain calculated results */
  void get_parameter(parameter, float *array);
  void get_Ne(float *Ne_array); /* Electron density */
  void get_Tn(float *Tn_array); /* Neutral temperature */
  void get_Ti(float *Ti_array); /* Ion temperature */
  void get_Te(float *Te_array); /* Electron temperature */
  void get_Ne_Dregion(float *NeD_array); /* Electron density in D-region 60, 65, ..., 100km */
};

}

#endif
