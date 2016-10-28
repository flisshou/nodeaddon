#include "cplexcpp.h"

using namespace std;

CplexCpp calculateValues(int value1, int value2) {

  CplexCpp cplexcpp1;//建立物件
  IloEnv env;

  cplexcpp1.cppValue = (value1 + value2) * 11;

  env.out() << "ths cppValue is: " << cplexcpp1.cppValue << endl;

  return cplexcpp1;//回傳物件

}
