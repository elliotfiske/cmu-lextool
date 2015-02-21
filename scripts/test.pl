#
use Data::Dumper;

my %h;

$a='abe';
$aa='abba';
@bc=('bud','cotton');
@de=('dog','eyore');
@fg=(('filet','gumbo'),'gophe');
@hi=('ugh',@fg);

print Dumper(\%h);

print "\n1 ----------------------------\n";
@{$h{$a}[0]} = @bc;
print scalar @{$h{$a}[0]},"\n";
print Dumper(\%h);
print @{$h{$a}[0]}[1],"\n";

print "\n2 ----------------------------\n";
@{$h{$a}[1]} = @de;
print Dumper(\%h);

print "\n3 ----------------------------\n";
@{$h{$a}[2]} = @fg;
print Dumper(\%h);
print @{$h{$a}[1]}[1],"\n";
print scalar @{$h{$a}[0]}," -0\n";
print scalar @{$h{$a}[2]}," -2\n";

print "\n4 ----------------------------\n";
print "@hi\n";
@{$h{$a}[3]} = @hi;
print Dumper(\%h);


#
