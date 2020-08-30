#ifndef predict_tf_h
#define predict_tf_h

#include <vector>
#include "PhysicsTools/TensorFlow/interface/TensorFlow.h"
#include <iostream>
#include <fstream>
using namespace std;

std::vector<float> predict_tf(std::vector<std::vector<float>>&, string, string, string);

#endif
