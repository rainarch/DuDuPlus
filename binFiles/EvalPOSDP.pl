open FP, $ARGV[0] or die;
open FP1, $ARGV[1] or die;
while(<FP>){
	chomp;
	$line1 = <FP1>;
	chomp $line1;
	if(/^$/){
		if($line1 ne ""){die}
	#	print "\n";
		$sN++;
		if($errN == 0){
			$sNr++;
		}
		$errN = 0;
		next;
	}
	@dd = split "\t";
	if($line1 eq ""){last;}
	@dd1 = split "\t", $line1;
	#print "$_\t$dd1[@dd1-1]\n";
	if($dd[3] eq $dd1[3]){$rN++;}
	$flagPUN = &isPUN($dd[1], $dd[3]);
	if($dd[6] eq $dd1[6]){
		$rDep++;
		if($flagPUN == 0 ){$rDepNPU++;}
		if($dd[7] eq $dd1[7]){
			$rLab++;
			if($flagPUN == 0) {$rLabNPU++;}
		}
	}
	else{
		if($flagPUN == 0){$errN++;}
	}
	if($flagPUN == 0){$nNPU++;}
	$n++;
}
print stderr "POS:$rN/$n=".($rN/$n)."\n";
print stderr "UAS:$rDepNPU/$nNPU=".($rDepNPU/$nNPU)."\n";
print stderr "LAS:$rLabNPU/$nNPU=".($rLabNPU/$nNPU)."\n";
print stderr "SEN:$sNr/$sN=".($sNr/$sN)."\n";
#printf stderr "%.4f\t%.4f\t%.4f\n",($rDepNPU/$nNPU),($rLabNPU/$nNPU),($sNr/$sN);

sub isPUN{
	my($w, $p) = @_;
	return 1 if($p !~ /^[a-zA-Z]*$/);
    return 1 if($w =~ /^[,\\.]$/); 
	return 1 if($p eq "PU");#Chinese
	return 0;
}
