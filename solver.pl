#!/usr/bin/perl


use strict;
use warnings;

my $exp = $ARGV[0];

open FILE, $exp or die "geht nich";

my $filecontent = "";

while (<FILE>){
 $filecontent .= $_;
}

close FILE;

our $size = 3;
our @sudoku;
our $no_len = 1;

sub read_sudoku
{
    my @lines = split(/\n/, $filecontent);
    my $puzzlesize = $lines[3];
    $puzzlesize =~ /(\d+)x(\d+)/;
    $size = $1;

$no_len = 3 if ($size < 99);
$no_len = 2 if ($size < 9);
$no_len = 1 if ($size < 4);

shift @lines; shift @lines; shift @lines; shift @lines;
foreach (@lines) {
    if (substr($_, 0, 1) eq "+")  {
    } else {
	my @row;
	my $sepfree = $_;
	$sepfree =~ s/\|//g;
	my @cells = split(/\s+/, $sepfree);
	foreach my $cell (@cells) {
	    if ($cell ne "") {
		if (substr($cell, 0, 1) eq "_") {
		    push @row, "*";
		} else {
		    push @row, $cell;
		}
	    }
	}
	push @sudoku, \@row;
    }
}    

}


my $i = 0;
my $j = 0;


=item C<void print_clingo_input>

Writes clingo input file into 'temp.asp'

=cut

sub print_clingo_input 
{
    my $beginning = <<END;
#show cell/3.

block(X, Y) :-
% Grounding
	range(X),
	range(Y),
% Parameter
	size(S),
% Conditions
	X \\ S == 1,
	Y \\ S == 1.

in_block(BX, BY, X, Y) :-
% Grounding
	srange(W),
	srange(H),
	range(X),
	range(Y),
	range(BX),
	range(BY),
% Conditions
	X=BX+W,
	Y=BY+H.


% Each cell must have digit
1 {cell(X, Y, D) : range(D)} 1 :- range(X), range(Y).

% ROW
1 {cell(X, Y, D) : range(X)} 1 :- range(Y), range(D). 

% COLUMN
1 {cell(X, Y, D) : range(Y)} 1 :- range(X), range(D). 

% BLOCK
1 {cell(X2, Y2, D) : in_block(X,Y,X2,Y2)} 1  :- block(X,Y), range(D).
END

open (MYFILE, '>temp.asp');



print MYFILE "$beginning\n";
print MYFILE "size($size). srange(0..", $size-1, "). range(1..", $size*$size, ").";

    for ($i = 1; $i < $size*$size+1; $i++) {
    for ($j = 1; $j < $size*$size+1; $j++) {
	my @val = @{$sudoku[$i-1]};
	my $val2 = $val[$j-1];
	print MYFILE "cell($i,$j,$val2).\n" if ($val2 =~ m/^\d+$/);
    }
}

 close (MYFILE);
}





sub fancy_line
{
    my $i = 0;
    my $j = 0;
    for ($i = 0; $i < $size; $i++) {
	print "+";
	for ($j = 0; $j < $no_len*$size+$size+1; $j++) {
	    print "-";
	}
    }
    print "+\n";
}

sub print_sudoku
{
for ($i = 0; $i < $size*$size; $i++) {
    fancy_line if ($i % $size eq 0);
    for ($j = 0; $j < $size*$size; $j++) {
	print "| " if ($j % $size eq 0);
	my @val = @{$sudoku[$i]};
	my $val2 = $val[$j];
        print "$val2 " if        ($no_len eq 1);
	printf '%2s ',  $val2 if ($no_len eq 2);
	printf '%3s ',  $val2 if ($no_len eq 3);
    }
	print "|\n";
}
fancy_line
}



our $clingo_answer = "";

sub gen_sudoku
{
    for (my $i = 0; $i < $size*$size; $i++) {
	push @sudoku, [1..($size*$size)];
    }
}



sub parse_clingo_answer
{
    my @lines = split(/\n/, $clingo_answer);
    my $model = $lines[4];
    my @atoms = split(/ /, $model);

    gen_sudoku;

    foreach (@atoms) {
	/cell\((\d+),(\d+),(\d+)\)/;
	${$sudoku[$1-1]}[$2-1] = $3;
    }
}

read_sudoku;
print "INITIAL:\n";
print_sudoku;

print_clingo_input;

###
system("clingo temp.asp > clingo_out.txt");



####

open FILE, "clingo_out.txt" or die "geht nich";
while (<FILE>){
  $clingo_answer .= $_;
}
close FILE;

parse_clingo_answer;

print "RESULT:\n";
    print_sudoku;
