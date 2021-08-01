#pragma once
#include <vector>
#include <math.h>
#include <stdlib.h>
using namespace std;

inline double RandFloat() 
{
	return (rand()) / (RAND_MAX + 1.0);
}
//returns a random float in the range -1 < n < 1
inline double RandomClamped() 
{
	return  (RandFloat() - RandFloat());
}

#define sigmoid_a (0.6) 
 

struct NerveCell
{
	vector<double> weight;
	vector<double> lastIncrement;
	double bias;
	double out;
	double gradient;
	NerveCell(int inputsNum)
	{
		bias = 1;
		out = 0;
		gradient = 0;
		weight.resize(inputsNum);
		for (int i = 0; i < inputsNum; i++)
		{
			weight[i] = RandomClamped();
 		}
		lastIncrement.resize(inputsNum);
	}
};
struct HiddenLayer
{
	vector<NerveCell*> Layer;
};
 

class BPNeuralNetwork
{

public:
	BPNeuralNetwork(double  learningRate, double  momentum);
	~BPNeuralNetwork();
	void CreateInputsLayer(int inputsNum,int nerveCellNum);
	void CreateHiddenLayer(int nerveCellNum);
	void CreateOutputsLayer(int outputsNum);
	double Train(vector<double> *inputs, vector<double> *outputs, vector<double> *expectation);
	void Input(vector<double> *inputs, vector<double> *outputs);
	double  m_learningRate;
	double  m_momentum;
	double  m_Mse = 0;
	int		m_MseNum = 0;
private:
	double sigmoid(double x);
	double differentialSigmoid(double y);
	vector<HiddenLayer*> m_network;	
};

