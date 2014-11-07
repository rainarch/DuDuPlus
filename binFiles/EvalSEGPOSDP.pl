sub eval{
	#dep, head
	my @dd = @{$lines[0]};
	my @dd1 = @{$lines1[0]};
	my $w = $dd[1];
	my $w1 = $dd1[1];
	my $s = $w;
	my $s1 = $w1;
	my $i = 0;
	my $i1 = 0;
	$nW += @lines;
	$nW1 += @lines1;
	#One sent could have same words
	my %words = ();
	my %words1 = ();
	for(my $k = 0; $k < @lines; $k++){
		@dd = @{$lines[$k]};
		my $tmpW = $dd[1];
		$words{$tmpW}++;
		$words{$tmpW."-".($k+1)} = $words{$tmpW};
		$nWNoPU++ if($dd[3] ne "PU");
	}
	for(my $k = 0; $k < @lines1; $k++){
		@dd1 = @{$lines1[$k]};
        my $tmpW = $dd1[1];
        $words1{$tmpW}++;
        $words1{$tmpW."-".($k+1)} = $words1{$tmpW};
		$nWNoPU1++ if($dd1[3] ne "PU");
    }

	while(1){
		@dd = @{$lines[$i]};
		@dd1 = @{$lines1[$i1]};
		#print stderr "$i:$w:$dd[3] vs $i1:$w1:$dd1[3]\n";
		#print stderr "$s\n$s1\n";
		if($w eq $w1){
			#print "$lineN-$lineN1:$i:$w vs $i1:$w1\n";
			$rW++;
			#check pos
			if($dd[3] eq $dd1[3]){
				$rPos++;
			}
			#check head
			$hd = $dd[6];
			@hh = @{$lines[$hd-1]};
			if($hd eq "0"){$hw = "ROOT";}
			else{$hw = $hh[1];}

			$hd1 = $dd1[6];
			@hh1 = @{$lines1[$hd1-1]};
			if($hd1 eq "0"){$hw1 = "ROOT";}
			else{$hw1 = $hh1[1];}
			if($hw eq $hw1 and $words{$hw."-".$hd}  eq $words1{$hw1."-".$hd1}){
				$rDep++;
				$rDepNoPU++ if($dd[3] ne "PU");
			}
			#print "$rW\t$rDep\n";
			
			#print "---$w\t$hd:$hw v-hd-s $hd1:$hw1\n";
		}
		#let two subs equ
		$i++;
		$i1++;
		while(length($s) != length($s1)){
			@dd = @{$lines[$i]};
        	@dd1 = @{$lines1[$i1]};
			if(length($s) > length($s1)){
				$s1 .= $dd1[1];
				$i1++;
			}
			else{
				$s .= $dd[1];
				$i++;
			}
			if($i > @lines or $i1 > @lines1){die "==Err==\n$s\n$s1\n";}
		}
		#print "$i -- $i1\n";
		#print @lines." vs ".@lines1."\n";
		if($i >= @lines or $i1 >= @lines1){last;}
		@dd = @{$lines[$i]};
        @dd1 = @{$lines1[$i1]};
		$w = $dd[1];
		$w1 = $dd1[1];
		$s .= $w;
		$s1 .= $w1;
	}
}
##############
print stderr "Usage: perl a.pl std tag taginput\n";
open FP, $ARGV[0] or die;#std
open FP1, $ARGV[1] or die;#tag
open FP2, $ARGV[2] or die;#taginput for some normalization, likes <num>
$lineN = 0;
$lineN1 = 0;
while(<FP>){
	chomp;
	
	if(/^$/){
		while(<FP1>){
			chomp;
			$line2 = <FP2>;
			if(/^$/){last;}
			@dd = split "\t";
			@dd2 = split "\t",$line2;
			#if($dd[1] ne $dd2[1]){$nnn++;print stderr "--$nnn: $dd[1] vs $dd2[1]\n"}
			$dd[1] = $dd2[1];
			push @lines1, [@dd];
		}
		if(@lines == 0){last;}
		#print @lines." v-init-s ".@lines1."\n";
		&eval();
		$lineN += @lines +1;
		$lineN1 += @lines1+1;
		#print stderr "$lineN -- $lineN1 $rDepNoPU\t$nWNoPU1\t$nWNoPU \n";
		@lines = ();
		@lines1 = ();
		#if($lineN > 20 ){last;}
		$sentNo++;
		#if($sentNo > 100){last;}
		next;
	}
	@dd = split "\t";
	push @lines, [@dd];
}
if(@lines > 1){
	while(<FP1>){
            chomp;
            $line2 = <FP2>;
            if(/^$/){last;}
            @dd = split "\t";
            @dd2 = split "\t",$line2;
            $dd[1] = $dd2[1];
            push @lines1, [@dd];
        }
    &eval();
}

#output
$prW = $rW/$nW1;
$reW = $rW/$nW;
$f1W = 2*$prW*$reW/($prW+$reW);
$prPos = $rPos/$nW1;
$rePos = $rPos/$nW;
if($prPos+$rePos >  0){
$f1Pos = 2*$prPos*$rePos/($prPos+$rePos);
}
$prDep = $rDep/$nW1;
$reDep = $rDep/$nW;
if($prDep+$reDep > 0){
$f1Dep = 2*$prDep*$reDep/($prDep+$reDep);
}
$prDepNP = $rDepNoPU/$nWNoPU1;
$reDepNP = $rDepNoPU/$nWNoPU;
if($prDepNP+$reDepNP){
$f1DepNP = 2*$prDepNP*$reDepNP/($prDepNP+$reDepNP);
}
if(1){
print stderr "Right\tTag\tSTD\n";
print stderr "$rW\t$nW1\t$nW\n";
print stderr "$rPos\t$nW1\t$nW\n";
print stderr "$rDep\t$nW1\t$nW\n";
print stderr "$rDepNoPU\t$nWNoPU1\t$nWNoPU\n";
}
print stderr "Item\tP\tR\tF1\n";
printf stderr "Seg\t%.4f\t%.4f\t%.4f\n",$prW,$reW,$f1W;
printf stderr "POS\t%.4f\t%.4f\t%.4f\n",$prPos,$rePos,$f1Pos;
#printf stderr "Dep\t%.4f\t%.4f\t%.4f\n",$prDep,$reDep,$f1Dep;
printf stderr "DepNP\t%.4f\t%.4f\t%.4f\n",$prDepNP,$reDepNP,$f1DepNP;

