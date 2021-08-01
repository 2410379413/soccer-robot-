#pragma once
#include "BPNeuralNetwork.h"

BPNeuralNetwork::BPNeuralNetwork(double learningRate, double momentum):m_learningRate(learningRate),m_momentum(momentum)
{
}
BPNeuralNetwork::~BPNeuralNetwork()
{
}

void BPNeuralNetwork::CreateInputsLayer(int inputsNum, int nerveCellNum)
{
	HiddenLayer *layer = new HiddenLayer;
	layer->Layer.resize(nerveCellNum);
	for (int i = 0; i < nerveCellNum; i++)
	{
		layer->Layer[i] = new NerveCell(inputsNum);
	}
	m_network.push_back(layer);
}
void BPNeuralNetwork::CreateHiddenLayer(int nerveCellNum)
{
	HiddenLayer *layer = new HiddenLayer;
	layer->Layer.resize(nerveCellNum);

	for (int i = 0; i < nerveCellNum; i++)
	{
		layer->Layer[i] = new NerveCell(m_network.back()->Layer.size());
	}
	m_network.push_back(layer);
}
void BPNeuralNetwork::CreateOutputsLayer(int outputsNum)
{
	HiddenLayer *layer = new HiddenLayer;
	layer->Layer.resize(outputsNum);

	for (int i = 0; i < outputsNum; i++)
	{
		layer->Layer[i] = new NerveCell(m_network.back()->Layer.size());
	}
	m_network.push_back(layer);
}

double BPNeuralNetwork::Train(vector<double>* inputs, vector<double>* outputs, vector<double>* expectation)
{
	vector<double>* pOutputs = outputs;
	vector<double> tmpOutputs;
	if (pOutputs == NULL)
		pOutputs = &tmpOutputs;
	Input(inputs, pOutputs);


	int CurrentLayer = m_network.size() - 1;
	double IncrementWeight = 0;
	double LocalGradient;
	double errorSum;
	double error;

	for (int i = 0; i < m_network[CurrentLayer]->Layer.size(); i++)
	{
		error = ((*expectation)[i] - (*outputs)[i]);
		errorSum = 0.5* error * error;
		LocalGradient = error * differentialSigmoid(m_network[CurrentLayer]->Layer[i]->out);//(*outputs)[i]
		m_network[CurrentLayer]->Layer[i]->gradient = LocalGradient;
		for (int j = 0; j < m_network[CurrentLayer]->Layer[0]->weight.size(); j++)
		{
			IncrementWeight = m_learningRate * LocalGradient * m_network[CurrentLayer - 1]->Layer[j]->out;
			m_network[CurrentLayer]->Layer[i]->weight[j] += IncrementWeight + m_network[CurrentLayer]->Layer[i]->lastIncrement[j] * m_momentum;
			m_network[CurrentLayer]->Layer[i]->lastIncrement[j] = IncrementWeight;
 		}
	}

	m_MseNum++;
	m_Mse += errorSum;

	//“˛≤ÿ≤„
	for (int i = CurrentLayer - 1; i>0; i--)
	{
		for (int j = 0; j < m_network[i]->Layer.size(); j++)
		{

			LocalGradient = 0;
			for (int f = 0; f < m_network[i + 1]->Layer.size(); f++)
			{
				LocalGradient += m_network[i + 1]->Layer[f]->weight[j] * m_network[i + 1]->Layer[f]->gradient;
			}
			LocalGradient *= differentialSigmoid(m_network[i]->Layer[j]->out);
			m_network[i]->Layer[j]->gradient = LocalGradient;
			for (int k = 0; k < m_network[i]->Layer[0]->weight.size(); k++)
			{
				IncrementWeight = m_learningRate * LocalGradient * m_network[i - 1]->Layer[k]->out;
				m_network[i]->Layer[j]->weight[k] += IncrementWeight + m_network[i]->Layer[j]->lastIncrement[k] * m_momentum;
				m_network[i]->Layer[j]->lastIncrement[k] = IncrementWeight;
			}
		}
	}

	// ‰»Î≤„
	for (int j = 0; j < m_network[0]->Layer.size(); j++)
	{

		LocalGradient = 0;
		for (int f = 0; f < m_network[1]->Layer.size(); f++)
		{
			LocalGradient += m_network[1]->Layer[f]->weight[j] * m_network[1]->Layer[f]->gradient;
		}
		LocalGradient *= differentialSigmoid(m_network[0]->Layer[j]->out);

		for (int k = 0; k < m_network[0]->Layer[0]->weight.size(); k++)
		{
			IncrementWeight = m_learningRate * LocalGradient * (*inputs)[k];
			m_network[0]->Layer[j]->weight[k] += IncrementWeight + m_network[0]->Layer[j]->lastIncrement[k] * m_momentum;
			m_network[0]->Layer[j]->lastIncrement[k] = IncrementWeight;
		}
	}

	return m_Mse/m_MseNum;
}
double BPNeuralNetwork::sigmoid(double x)
{
	 
	return 1 / (1 + exp(0 - sigmoid_a*x));
	//a*tanh(b*x);
	//return a*tanh(b*x);
}

double BPNeuralNetwork::differentialSigmoid(double y)
{
	return sigmoid_a*y*(1 - y);
	//return (b / a)*(a - y)*(a + y);
}
void BPNeuralNetwork::Input(vector<double>* inputs, vector<double>* outputs)
{
	vector<double> tmp1;
	vector<double> tmp2;

	vector<double> *pInputs = inputs;
	vector<double> *pOutputs = &tmp1;


	double v = 0;

	for (int i = 0; i < m_network.size(); i++)
	{
		for (int j = 0; j < m_network[i]->Layer.size(); j++)
		{
			for (int k = 0; k <m_network[i]->Layer[0]->weight.size(); k++)
			{
				v += m_network[i]->Layer[j]->weight[k] * (*pInputs)[k];
			}
			v += m_network[i]->Layer[j]->bias;

			m_network[i]->Layer[j]->out = sigmoid(v);
			pOutputs->push_back(m_network[i]->Layer[j]->out);
			v = 0;
		}

		pInputs = pOutputs;
		pOutputs =( pOutputs == &tmp1 ? &tmp2 : &tmp1);
		pOutputs->clear();
 	}
	*outputs = *pInputs;
}
