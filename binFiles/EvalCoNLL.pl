sub printOut{

print "UAS =$unlabeledRightPU/$wordNumPU = ".($unlabeledRightPU/$wordNumPU)."\n";
print "LAS =$labeledRightPU/$wordNumPU = ".($labeledRightPU/$wordNumPU)."\n";
print "ROOT=$rootRight/$rootNum = ".($rootRight/$rootNum)."\n";
print "COMP=$sentCorrectPUNo/$sentNo=".( $sentCorrectPUNo/$sentNo)."\n";
}

open FP1, $ARGV[0] or die;#std
open FP2, $ARGV[1] or die "err: tagFile\n";#$tag
$flag = $ARGV[2];
$sentNo = 0;
$sentCorrectNo = 0;
while(<FP1>){
	chomp;
	$line2 = <FP2>;
	chomp $line2;
	if(/^$/){
		if($line2 ne ""){die;}
		$sentNo++;
		$sentCorrectNo++ if($errNo == 0);
		$sentCorrectPUNo++ if($errPUNo == 0);
		$errNo = 0;
		$errPUNo = 0;
		next;
	}
	@dd1 = split "\t";
	@dd2 = split "\t",$line2;
	#if($dd1[3] ne $dd2[3]){print $_."\n".$line2."\n"; die;}
	$wordNum++;
	$rootNum++ if($dd1[6] eq "0" or $dd1[6] eq "-1");
	$rootNum1++ if($dd2[6] eq "0" or $dd2[6] eq "-1");
	$rootRight++ if($dd1[6] eq $dd2[6] and ($dd1[6] eq "0" or $dd1[6] eq "-1"));
	$unlabeledRight++ if($dd1[6] eq $dd2[6]);
	$errNo++ if($dd1[6] ne $dd2[6]);
	$labeledRight++ if($dd1[6] eq $dd2[6] and $dd1[7] eq $dd2[7]);
	next if($dd1[3] eq "PU");
	next if($dd1[3] !~ /^[a-zA-Z]*$/);
	next if($dd1[1] =~ /^[,\\.]$/);
	$errPUNo++ if($dd1[6] ne $dd2[6]);
	$wordNumPU++;
	$unlabeledRightPU++ if($dd1[6] eq $dd2[6]);
	$labeledRightPU++ if($dd1[6] eq $dd2[6] and $dd1[7] eq $dd2[7]);
	&printOut() if($flag eq "show" and $wordNumPU % 1000 == 0);
}
close FP1;
close FP2;
&printOut();
