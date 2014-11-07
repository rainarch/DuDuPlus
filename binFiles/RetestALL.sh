#EN
./DuDuPlus task:TagParser test model-name:_Models/DPModel.EN.O2LBFULL posmodel-name:_Models/POSModel.EN test-file:toydata/en.test.gold output-file:tmp/test.out.en format:CONLL lang:en order:2 runtype:LBFULL
#CN
./DuDuPlus task:TagParser test model-name:_Models/DPModel.EN.O2LBST posmodel-name:_Models/POSModel.CN segmodel-name:_Models/SEGModel.CN test-file:toydata/cn.test.gold output-file:tmp/test.out.cn format:CONLL lang:cn order:2 runtype:LBST





echo "EN"
perl binFiles/EvalPOSDP.pl toydata/en.test.gold tmp/test.out.en 
echo "CN"
perl binFiles/EvalSEGPOSDP.pl toydata/cn.test.gold tmp/test.out.cn tmp/test.out.cn
