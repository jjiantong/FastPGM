#include "mex.hpp"
#include "mexAdapter.hpp"
#include "smile_license.h"
#include "smile.h"

using namespace std;

class MexFunction : public matlab::mex::Function {
public:
    void operator()(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs) {
        auto f = findFunction(outputs, inputs);
        (this->*(f.ptr))(outputs,inputs);
    }

private:    
    shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();
    matlab::data::ArrayFactory factory;
    union uint64netptr {uint64_t integer; DSL_network *pointer;};
    typedef void (MexFunction::*PTR)(matlab::mex::ArgumentList &, matlab::mex::ArgumentList &);
    struct MatSmileFunction {
        const char * name;
        int inputSize;
        PTR ptr;
    };
 
    void error(const string &msg) {
        matlabPtr->feval(u"error", 
                0, vector<matlab::data::Array>({ factory.createScalar(msg.c_str()) }));
    }
    
    matlab::data::TypedArray<matlab::data::MATLABString> createStringArray(const vector<string>& vec) {
        return factory.createArray( {vec.size()}, vec.begin(), vec.end());
    }
    
    const MatSmileFunction & findFunction(matlab::mex::ArgumentList & outputs, matlab::mex::ArgumentList & inputs) {
        static const MatSmileFunction wrapFx[] = {
            "newNetwork", 1, &MexFunction::newNetwork,
            "deleteNetwork", 2, &MexFunction::deleteNetwork,
            "getNodeCount", 2, &MexFunction::getNodeCount,
            "readFile", 3, &MexFunction::readFile,
            "updateBeliefs", 2, &MexFunction::updateBeliefs,
            "setEvidence", 4, &MexFunction::setEvidence,
            "getValue", 3, &MexFunction::getValue,
            "getAllNodeIds", 2, &MexFunction::getAllNodeIds,
            "getOutcomeIds", 3, &MexFunction::getOutcomeIds,
            "isEvidence", 3, &MexFunction::isEvidence,
            "getEvidence", 3, &MexFunction::getEvidence,
            "getOutcomeCount", 3, &MexFunction::getOutcomeCount
        };
        matlab::data::CharArray charArray = inputs[0];
        auto functionName = charArray.toAscii().c_str();
        const auto iterator = find_if(begin(wrapFx), end(wrapFx), [&functionName](auto f) {
            return !strcmp(f.name, functionName);
        });
        if (iterator == end(wrapFx)) {
            string msg = "Cannot find function with name ";
        	msg += functionName;
            error(msg);
        }
        int inputSize = iterator->inputSize;
        if (inputs.size() != inputSize) {
            string msg = "Invalid input size for function: ";
        	msg += functionName;
            msg += ". Expected: ";
            DSL_appendInt(msg, inputSize);
            msg += ", given: ";
            DSL_appendInt(msg, inputs.size());
            error(msg);
        }
        if (outputs.size() > 1) {
            error("Outputs size cannot be greater than 1.");
        }
        return *iterator; 
    }
    
    int validateNodeId(const DSL_network & net, const char * nodeId) {
        int handle = net.FindNode(nodeId);
        if (handle < 0) {
            string msg = "Cannot find node with ID '";
            msg += nodeId;
            msg += '\'';
            error(msg);
        }
        return handle;
    }
    
    int validateOutcomeId(const DSL_network & net, int nodeHandle, const char * outcomeId) {
        DSL_node * node = net.GetNode(nodeHandle);

        DSL_idArray *outcomeNames = node->Definition()->GetOutcomesNames();
        int outcomeIndex = outcomeNames->FindPosition(outcomeId);
        if (outcomeIndex < 0) {
            string msg = "Invalid outcome identifier '";
            msg += outcomeId;
            msg += "' for node '";
            msg += node->GetId();
            msg += '\'';
            error(msg);
        }
        return outcomeIndex;
    }
    
    uint64_t encodePtr(DSL_network* pointer) {
        uint64netptr ivp;
        ivp.pointer = pointer;
        return ivp.integer;
    }
    
    DSL_network * decodePtr(uint64_t integer) {
        uint64netptr ivp;
        ivp.integer = integer;
        return ivp.pointer;
    }
  
    void newNetwork(matlab::mex::ArgumentList & outputs, matlab::mex::ArgumentList & inputs) {
        auto net = new DSL_network();
        outputs[0] = factory.createScalar<uint64_t>(encodePtr(net));
    }
    
    void deleteNetwork(matlab::mex::ArgumentList & outputs, matlab::mex::ArgumentList & inputs) {
        matlab::data::TypedArray<uint64_t> arr = inputs[1];
        auto net = decodePtr(arr[0]);
        delete net;
    }
    
    void getNodeCount(matlab::mex::ArgumentList & outputs, matlab::mex::ArgumentList & inputs) {
        matlab::data::TypedArray<uint64_t> arr = inputs[1];
        auto net = decodePtr(arr[0]);
        outputs[0] = factory.createScalar(net->GetNumberOfNodes());
    }
    
    void readFile(matlab::mex::ArgumentList & outputs, matlab::mex::ArgumentList & inputs) {
        matlab::data::TypedArray<uint64_t> arr = inputs[1];
        auto net = decodePtr(arr[0]);
        matlab::data::CharArray charArray = inputs[2];
        auto filePath = charArray.toAscii();
        net->ReadFile(filePath.c_str());
    }
    
    void updateBeliefs(matlab::mex::ArgumentList & outputs, matlab::mex::ArgumentList & inputs) {
        matlab::data::TypedArray<uint64_t> arr = inputs[1];
        auto net = decodePtr(arr[0]);
        net->UpdateBeliefs();
    }
    
    void setEvidence(matlab::mex::ArgumentList & outputs, matlab::mex::ArgumentList & inputs) {
        matlab::data::TypedArray<uint64_t> arr = inputs[1];
        auto net = decodePtr(arr[0]);
        matlab::data::CharArray nodeIdCharArray = inputs[2];
        matlab::data::CharArray outcomeIdCharArray = inputs[3];
        int nodeHandle = validateNodeId(*net, nodeIdCharArray.toAscii().c_str());
        int outcomeIndex = validateOutcomeId(*net, nodeHandle, outcomeIdCharArray.toAscii().c_str());
        net->GetNode(nodeHandle)->Value()->SetEvidence(outcomeIndex);
    }
    
    void getValue(matlab::mex::ArgumentList & outputs, matlab::mex::ArgumentList & inputs) {
        matlab::data::TypedArray<uint64_t> arr = inputs[1];
        auto net = (DSL_network*)decodePtr(arr[0]);
        matlab::data::CharArray nodeIdCharArray = inputs[2];
        auto nodeId = nodeIdCharArray.toAscii();
        int nodeHandle = validateNodeId(*net, nodeId.c_str());
        DSL_nodeValue * nodeValue = net->GetNode(nodeHandle)->Value();
        if(!nodeValue->IsValueValid()) {
            string msg = "Invalid node value for node ";
            msg += nodeId;
            error(msg);
        }
        const DSL_Dmatrix *m = nodeValue->GetMatrix();
        const double *p = m->GetItems().Items();
        size_t arraySize = m->GetSize();
        matlab::data::TypedArray<double> resArr = factory.createArray( {arraySize}, p, p+arraySize);
        outputs[0] = resArr;
    }
    
    void getAllNodeIds(matlab::mex::ArgumentList & outputs, matlab::mex::ArgumentList & inputs) {
        matlab::data::TypedArray<uint64_t> arr = inputs[1];
        auto net = (DSL_network*)decodePtr(arr[0]);
        size_t count = net->GetNumberOfNodes();
        vector<string> ids;
        ids.resize(count);
        DSL_intArray nodes;
        net->GetAllNodes(nodes);
        for (int i = 0; i < count; i++) {
            ids[i] = string(net->GetNode(nodes[i])->GetId());
        }
        auto resArr = createStringArray(ids);
        outputs[0] = resArr;
    }
    
    void getOutcomeCount(matlab::mex::ArgumentList & outputs, matlab::mex::ArgumentList & inputs) {
        matlab::data::TypedArray<uint64_t> arr = inputs[1];
        auto net = decodePtr(arr[0]);
        matlab::data::CharArray nodeIdCharArray = inputs[2];
        int nodeHandle = validateNodeId(*net, nodeIdCharArray.toAscii().c_str()); 
        outputs[0] = factory.createScalar(net->GetNode(nodeHandle)->Definition()->GetNumberOfOutcomes());
    }
    
    void getOutcomeIds(matlab::mex::ArgumentList & outputs, matlab::mex::ArgumentList & inputs) {
        matlab::data::TypedArray<uint64_t> arr = inputs[1];
        auto net = decodePtr(arr[0]);
        matlab::data::CharArray nodeIdCharArray = inputs[2];
        int nodeHandle = validateNodeId(*net, nodeIdCharArray.toAscii().c_str());
        auto def = net->GetNode(nodeHandle)->Definition();
        auto names = def->GetOutcomesNames()->Items();
        size_t count = def->GetNumberOfOutcomes();
        vector<string> namesVec(names, names+count);
        auto resArr = createStringArray(namesVec);
        outputs[0] = resArr;
    }
    
    void isEvidence(matlab::mex::ArgumentList & outputs, matlab::mex::ArgumentList & inputs) {
        matlab::data::TypedArray<uint64_t> arr = inputs[1];
        auto net = decodePtr(arr[0]);
        matlab::data::CharArray nodeIdCharArray = inputs[2];
        int nodeHandle = validateNodeId(*net, nodeIdCharArray.toAscii().c_str());
        outputs[0] = factory.createScalar(0 != net->GetNode(nodeHandle)->Value()->IsEvidence());
    }
    
    void getEvidence(matlab::mex::ArgumentList & outputs, matlab::mex::ArgumentList & inputs) {
        matlab::data::TypedArray<uint64_t> arr = inputs[1];
        auto net = decodePtr(arr[0]);
        matlab::data::CharArray nodeIdCharArray = inputs[2];
        int nodeHandle = validateNodeId(*net, nodeIdCharArray.toAscii().c_str());
        DSL_node * node = net->GetNode(nodeHandle);
        int evidence = node->Value()->GetEvidence();
        if (evidence < 0) {
            string msg = "Evidence for node ";
            msg += node->GetId();
            msg += " does not exist";
            error(msg);
        }
        std::string outcomeId = node->Definition()->GetOutcomesNames()->Subscript(evidence);
        outputs[0] = factory.createCharArray(outcomeId);
    }
    
};