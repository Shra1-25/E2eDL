//#include "ProdTutorial/ProducerTest/plugins/predict_tf.h"
#include "E2eDL/E2eDLrec/plugins/predict_tf.h"
#include "tensorflow/core/graph/default_device.h"

std::vector<float> predict_tf(std::vector<std::vector<float>>& vinputFrame, string model_filename, string input_layer_name, string output_layer_name){
 tensorflow::Session* session;
 tensorflow::GraphDef graph_def;
 tensorflow::SessionOptions opts;
 std::vector<tensorflow::Tensor> outputs; // Store outputs
 // create a new session
 TF_CHECK_OK(NewSession(opts, &session));
 
 std::string graph_definition="E2eDL/"+model_filename;
 std::cout<<" >> Running Inference."<<endl;
 int frame_height = vinputFrame.size();
 int frame_width = vinputFrame[0].size();
 //TF_CHECK_OK(ReadBinaryProto(Env::Default(), graph_definition, &graph_def));
 // load the graph definition, i.e. an object that contains the computational graph
 tensorflow::GraphDef* graphDef = tensorflow::loadGraphDef(graph_definition);
 tensorflow::Tensor tmp(tensorflow::DT_FLOAT, tensorflow::TensorShape({frame_height, frame_width}));
 auto _XTensor = tmp.matrix<float>();
 //std::copy_n(vEB_frame.begin(), vEB_frame.size(), tmp.flat<float>().data());
 for (int frame_row=0;frame_row<int(vinputFrame.size());frame_row++){
  for (int frame_col=0;frame_col<int(vinputFrame[0].size());frame_col++){
   _XTensor(frame_row,frame_col)=vinputFrame[frame_row][frame_col];
  }
 }
 std::cout<<" >> Reading input data file done."<<endl;

  
  tensorflow::Tensor x(tensorflow::DT_FLOAT, tensorflow::TensorShape({1, frame_height, frame_width, 1}));
  if(!x.CopyFrom(tmp, tensorflow::TensorShape({1, frame_height, frame_width, 1}))){
    std::cout<<" >> Reshape not successfull."<<endl;
  }
 // Set GPU options
 //graph::SetDefaultDevice("/gpu:0", &graph_def);
 //opts.config.mutable_gpu_options()->set_per_process_gpu_memory_fraction(0.5);
 //opts.config.mutable_gpu_options()->set_allow_growth(true);
 
 //int GPUID = std::stoi(params->getGpuDeviceStr());
 //setenv("CUDA_VISIBLE_DEVICES", "", GPUID);

 //std::cout << "Initial  visible_device_list : "<<opts.config.gpu_options().visible_device_list() << std::endl;
 opts.config.mutable_gpu_options()->set_allow_growth(true);
 opts.config.mutable_gpu_options()->set_per_process_gpu_memory_fraction(0.5);//params->getGpuMemoryRatio());
 
 
 // Load graph into session
 //TF_CHECK_OK(session->Create(graph_def));
 
 // create a session
 session = tensorflow::createSession(graphDef);
 
 // Initialize our variables
 //TF_CHECK_OK(session->Run({}, {}, {"init_all_vars_op"}, nullptr));
 
 //tensorflow::run(session, {}, {"init_all_vars_op"}, nullptr);
 

        
 //TF_CHECK_OK(session->Run({{"x", x}, {"y", y}}, {"cost"}, {}, &outputs)); // Get cost
 TF_CHECK_OK(session->Run({{input_layer_name/*"inputs"*/, x}/*, {"y", y}*/}, {output_layer_name/*"softmax_1/Sigmoid"*/}, {}, &outputs)); // Get output
 //tensorflow::run(session, { { "x", x }, {"y", y} }, { "cost" }, &outputs);
 
 //float cost = outputs[0].scalar<float>()(0);
 //std::cout << "Cost: " <<  cost << std::endl;
 //TF_CHECK_OK(session->Run({{"x", x}, {"y", y}}, {}, {"train"}, nullptr)); // Train
 //tensorflow::run(session, { { "x", x }, {"y", y} }, {}, {"train"}, &outputs);
 std::cout<<" >> Number of samples is: "<<outputs.size()<<std::endl;
 //float classifier_out = outputs[0].matrix<float>()(0,0);
 //std::cout << "Output 0: " <<  max_out << std::endl;
 std::vector<int> out_size;
 std::vector<float> predictions;
 std::vector<std::vector<float>> full_predictions; 
 int pred_len=1;
 int dim_sel=1;
 predictions.clear();
 full_predictions.clear();
 out_size.clear();
 for (int dims_idx=0;dims_idx<outputs[0].shape().dims();dims_idx++){
  out_size.push_back(outputs[0].shape().dim_size(dims_idx));
 }
 
 // Ensuring that the predictions are not 2D and are flat instead.
 if (out_size.size()>2){std::cout<<" * Output dimensions greater than 2. Please flatten the output size and reduce dimensions to atmost 2."<<std::endl;}
 else { 
 if (out_size[0]>1) { pred_len=out_size[0]; dim_sel=0;}
 else if (out_size[1]>1) { pred_len=out_size[1]; dim_sel=1;}
 else if (out_size[0]>1 && out_size[1]>1) {pred_len=1; std::cout<<" * Expected flat vector of predictions of size [1,n] or [n,1]. Storing just the first value (0,0) as prediction."<<std::endl;}
 
 // Storing outputs in predictions vector according to the number of output dimensions (1 or 2). i.e. size=[n] or size=[1,n] or [n,1]
 for (int out_idx=0;out_idx<int(outputs.size());out_idx++){
  for (int pred_idx=0;pred_idx<pred_len;pred_idx++){ 
   if(dim_sel==0) {predictions.push_back(outputs[out_idx].matrix<float>()(pred_idx,0));}
   else if(dim_sel==1) {predictions.push_back(outputs[out_idx].matrix<float>()(0,pred_idx));}
   else {predictions.push_back(outputs[out_idx].matrix<float>()(0,0));}
   std::cout<<" >> Prediction at ("<<out_idx<<", "<<pred_idx<<") is: "<<predictions[pred_idx]<<std::endl; 
  }
  full_predictions.push_back(predictions);
  predictions.clear();
 }
 //std::cout<<" >> Size of prediction vector is= "<<full_predictions.size()<<std::endl;
 //std::cout<<"Size of each element of full pred= "<<full_predictions[0].size()<<std::endl;
 }
 //std::cout<<" >> Class: "<<classifier_out<<endl;
 
 for (int sample_idx=0; sample_idx<int(full_predictions.size());sample_idx++){
  std::cout<<" >> Sample "<<sample_idx+1<<"/"<<full_predictions.size()<<" predictions: [";
  for (int pred_idx=0; pred_idx<int(full_predictions.size());pred_idx++){
   std::cout<<full_predictions[sample_idx][pred_idx]<<", ";
  }
  std::cout<<"]"<<std::endl;
 }
 outputs.clear();
  
 session->Close();
 delete session;
 std::cout<<" >> Classification done"<<endl;
 // cleanup
 //tensorflow::closeSession(session);
 //delete graphDef;
 /*if (classifier_out>0.5){return 1;}
 else {return 0;}*/
 return full_predictions[0];
}

