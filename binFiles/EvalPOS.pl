#test_file train_file:for unknown
print stderr "test_file train_file\n";
open FP1, $ARGV[1] or die;
open FP, $ARGV[0] or die;
while(<FP1>){
    chomp;
	s/[\r\n]//g;
    if(/^$/){next;}
    @dd = split "\t";
    $wordlist{$dd[1]}++;
}

while(<FP>){
    chomp;
	s/[\r\n]//g;
    if(/^$/){next;}
    @dd = split "\t";
    if($dd[@dd-2] eq $dd[@dd-1]){$rN++;}
    $aN++;
    if( $wordlist{$dd[1]} > 0){next}
    if($dd[@dd-2] eq $dd[@dd-1]){$rUN++;}
    $aUN++;
}
print "ALL:$rN/$aN=".($rN/$aN)."\n";
if($aUN > 0){
print "UNK:$rUN/$aUN=".($rUN/$aUN)."\n";
}
