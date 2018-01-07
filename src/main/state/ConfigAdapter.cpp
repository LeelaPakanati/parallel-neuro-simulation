#include "ConfigAdapter.h"
#include "../global/GlobalDefinitions.h"

using namespace state;
using namespace global_definitions::offsets;
using namespace std;

ConfigAdapter::ConfigAdapter(protobuf_config::Config &protoConfig) {
  #pragma omp parallel sections
  {
    #pragma omp section
    {
      // Initialize solver info
      absoluteError = protoConfig.solver().abserror();
      relativeError = protoConfig.solver().relerror();
    }

    #pragma omp section
    {
      numOfNeurons = protoConfig.neurons_size();
      numOfSynapses = protoConfig.synapses_size();
      numOfNeuronVariables = numOfNeurons * NUM_OF_NEURON_VARIABLES;
      numOfSynapseVariables = numOfSynapses * NUM_OF_SYNAPSE_VARIABLES;
    }
  };

  #pragma omp parallel sections
  {
    #pragma omp section
    {
      initializeNeuronOffsets();
    }

    #pragma omp section
    {
      initializeSynapseOffsets();
    }
  };

  #pragma omp parallel sections
  {
    #pragma omp section
    {
      neurons = static_cast<Neuron *>(malloc(numOfNeurons * sizeof(Neuron)));
      if (!initialStateValues) {
        cerr << "Failed to allocate memory for the Neuron array" << "\n";
        exit(1);
      }
      initializeNeuronConstantProperties(protoConfig);
    }

    #pragma omp section
    {
      synapses = static_cast<Synapse *>(malloc(numOfSynapses * sizeof(Synapse)));
      if (!initialStateValues) {
        cerr << "Failed to allocate memory for the Synapse array" << "\n";
        exit(1);
      }
      initializeSynapseConstantProperties(protoConfig);
    }

    #pragma omp section
    {
      initialStateValues = static_cast<double *>(malloc((numOfNeuronVariables + numOfSynapseVariables) * sizeof(double)));
      if (!initialStateValues) {
        cerr << "Failed to allocate memory for the state value array" << "\n";
        exit(1);
      }

      #pragma omp parallel sections
      {
        #pragma omp section
        {
          initializeNeuronVariables(protoConfig);
        }

        #pragma omp section
        {
          initializeSynapseVariables(protoConfig);
        }
      };
    }
  };
}

ConfigAdapter::~ConfigAdapter() {
  free(initialStateValues);
}

double * ConfigAdapter::getInitialStateValues() {
  return initialStateValues;
}

void ConfigAdapter::initializeNeuronOffsets() {
  offset_V = OFF_V * numOfNeurons;
  offset_mk2 = OFF_mk2 * numOfNeurons;
  offset_mp = OFF_mp * numOfNeurons;
  offset_mna = OFF_mna * numOfNeurons;
  offset_hna = OFF_hna * numOfNeurons;
  offset_mcaf = OFF_mcaf * numOfNeurons;
  offset_hcaf = OFF_hcaf * numOfNeurons;
  offset_mcas = OFF_mcas * numOfNeurons;
  offset_hcas = OFF_hcas * numOfNeurons;
  offset_mk1 = OFF_mk1 * numOfNeurons;
  offset_hk1 = OFF_hk1 * numOfNeurons;
  offset_mka = OFF_mka * numOfNeurons;
  offset_hka = OFF_hka * numOfNeurons;
  offset_mkf = OFF_mkf * numOfNeurons;
  offset_mh = OFF_mh * numOfNeurons;
}

void ConfigAdapter::initializeSynapseOffsets() {
  offset_A = numOfNeuronVariables + OFF_A * numOfSynapses;
  offset_P = numOfNeuronVariables + OFF_P * numOfSynapses;
  offset_M = numOfNeuronVariables + OFF_M * numOfSynapses;
  offset_g = numOfNeuronVariables + OFF_g * numOfSynapses;
  offset_h = numOfNeuronVariables + OFF_h * numOfSynapses;
}

void ConfigAdapter::initializeNeuronConstantProperties(protobuf_config::Config &protoConfig) {
  #pragma omp parallel for
  for (int i = 0; i < numOfNeurons; i++) {
    const auto &protoNeuron = protoConfig.neurons(i);
    // TODO array access check; we don't want element copy
    Neuron *neuronPtr = neurons + i;
    neuronPtr->gbarna = protoNeuron.gbarna();
    neuronPtr->gbarp = protoNeuron.gbarp();
    neuronPtr->gbarcaf = protoNeuron.gbarcaf();
    neuronPtr->gbarcas = protoNeuron.gbarcas();
    neuronPtr->gbark1 = protoNeuron.gbark1();
    neuronPtr->gbark2 = protoNeuron.gbark2();
    neuronPtr->gbarka = protoNeuron.gbarka();
    neuronPtr->gbarkf = protoNeuron.gbarkf();
    neuronPtr->gbarh = protoNeuron.gbarh();
    neuronPtr->gbarl = protoNeuron.gbarl();
    neuronPtr->ena = protoNeuron.ena();
    neuronPtr->eca = protoNeuron.eca();
    neuronPtr->ek = protoNeuron.ek();
    neuronPtr->eh = protoNeuron.eh();
    neuronPtr->el = protoNeuron.el();
    neuronPtr->capacitance = protoNeuron.capacitance();
    neuronPtr->incoming = protoNeuron.incoming();
  }
}

void ConfigAdapter::initializeSynapseConstantProperties(protobuf_config::Config &protoConfig) {
  #pragma omp parallel for
  for (int i = 0; i < numOfSynapses; i++) {
    const auto &protoSynapse = protoConfig.synapses(i);
    // TODO array access check; we don't want element copy
    Synapse *synapsePtr = synapses + i;
    synapsePtr->source = protoSynapse.source();
    synapsePtr->gbarsyng = protoSynapse.gbarsyng();
    synapsePtr->esyn = protoSynapse.esyn();
    synapsePtr->buffering = protoSynapse.buffering();
    synapsePtr->h0 = protoSynapse.h0();
    synapsePtr->thresholdV = protoSynapse.thresholdv();
    synapsePtr->tauDecay = protoSynapse.taudecay();
    synapsePtr->tauRise = protoSynapse.taurise();
    synapsePtr->cGraded = protoSynapse.cgraded();
  }
}

void ConfigAdapter::initializeNeuronVariables(protobuf_config::Config &protoConfig) {
  #pragma omp parallel for
  for (int i = 0; i < numOfNeurons; i++) {
    const auto &protoNeuron = protoConfig.neurons(i);
    initialStateValues[offset_V + i] = protoNeuron.ivoltage();
    initialStateValues[offset_mk2 + i] = protoNeuron.imk2();
    initialStateValues[offset_mp + i] = protoNeuron.imp();
    initialStateValues[offset_mna + i] = protoNeuron.imna();
    initialStateValues[offset_hna + i] = protoNeuron.ihna();
    initialStateValues[offset_mcaf + i] = protoNeuron.imcaf();
    initialStateValues[offset_hcaf + i] = protoNeuron.ihcaf();
    initialStateValues[offset_mcas + i] = protoNeuron.imcas();
    initialStateValues[offset_hcas + i] = protoNeuron.ihcas();
    initialStateValues[offset_mk1 + i] = protoNeuron.imk1();
    initialStateValues[offset_hk1 + i] = protoNeuron.ihk1();
    initialStateValues[offset_mka + i] = protoNeuron.imka();
    initialStateValues[offset_hka + i] = protoNeuron.ihka();
    initialStateValues[offset_mkf + i] = protoNeuron.imkf();
    initialStateValues[offset_mh + i] = protoNeuron.imh();
  }
}

void ConfigAdapter::initializeSynapseVariables(protobuf_config::Config &protoConfig) {
  #pragma omp parallel for
  for (int i = 0; i < numOfSynapses; i++) {
    const auto &protoSynapse = protoConfig.synapses(i);
    initialStateValues[offset_A + i] = protoSynapse.ia();
    initialStateValues[offset_P + i] = protoSynapse.ip();
    initialStateValues[offset_M + i] = protoSynapse.im();
    initialStateValues[offset_g + i] = protoSynapse.ig();
    initialStateValues[offset_h + i] = protoSynapse.ih();
  }
}