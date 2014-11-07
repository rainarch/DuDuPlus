./DuDuPlus task:Parser train train-file:toydata/en.toy.train model-name:_Models/ToyModel.en feat-file:tmp/feat.dp.en order:1 lang:en runtype:LBBASIC 

./DuDuPlus task:Parser model-name:_Models/ToyModel.en test test-file:toydata/en.test.gold output-file:tmp/test.out order:1 lang:en runtype:LBBASIC 

 perl binFiles/EvalCoNLL.pl toydata/en.test.gold tmp/test.out 

./DuDuPlus task:Tagger train train-file:toydata/en.toy.train posmodel-name:_Models/ToyModel.POS.en feat-file:tmp/feat.pos.en lang:en

./DuDuPlus task:Tagger test posmodel-name:_Models/ToyModel.POS.en test-file:toydata/en.test.rawline output-file:tmp/test.out format:SENT lang:en

./DuDuPlus task:Tagger test posmodel-name:_Models/ToyModel.POS.en test-file:toydata/en.test.gold output-file:tmp/test.out format:CONLL lang:en

perl binFiles/EvalPOS.pl tmp/test.out toydata/en.toy.train 

./DuDuPlus task:TagParser test model-name:_Models/ToyModel.en posmodel-name:_Models/ToyModel.POS.en test-file:toydata/en.test.gold output-file:tmp/test.out format:CONLL lang:en order:1 runtype:LBBASIC 

perl binFiles/EvalPOSDP.pl toydata/en.test.gold tmp/test.out 
