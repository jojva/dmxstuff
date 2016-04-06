#!/usr/bin/perl

#this script reads a .dmx script and runs the k8062 accordingly....
#config
my $tickspersecond=25;
my $maxchannels=512;
#end config

my @operators=qw#+ - * /#;

use Data::Dumper;
use Time::HiRes qw/usleep/;
use lib ".";
use dmxinterface;
use dmxevents;
use DMXexpression;
use Thread::Queue;

sub find_operator {
	my($string)=@_;

	my $char=substr($string,0,1);
	foreach(@operators){
		if($char eq $_){
			print STDERR "find_operator: got $char\n";
			return $char;
		}
	}
	return undef;
}

sub parse_arguments {
	my($args)=@_;
	my %rtn;
	#args should be of the format arg=val,arg=val,arg=val
	my @list=split(/,/,$args);
	foreach(@list){
		#my %arg;
		if(/([^=]*)=(.*)/){
			#$arg{'arg'}=$1;
			#$arg{'val'}=$2;
			$rtn{$1}=$2;
		} elsif(/^(\d+)$/){	#static value
			$rtn{'value'}=$1;
		}
		#push @rtn,\%arg;
	}
	return \%rtn;
}

sub parse_expression {
	my($expr,$functionsRef)=@_;
	#expression should be of the format func(args) oper func(args) oper etc.
	while($expr=~/^\s*([^\(]+)\(([^\)]+)\)(.*)$/){
		my %func;
		#function name could include an operator before it.
		$expr=$3;
		$func{'name'}=$1;
		$func{'oper'}=find_operator($func{'name'});
		$func{'name'}=substr($func{'name'},1) if(defined $func{'oper'});
		$func{'args'}=parse_arguments($2);
		if($func{'name'} eq "keytrigger"){
			dmxevents::add_trigger('key',$func{'args'}{'key'},$func{'args'}{'key'});
		}
		push @$functionsRef,\%func;
	}
}

#START MAIN
dmxinterface::initDMX;

#this will start up threads to handle incoming events
$evq=dmxevents::run;

while(not dmxinterface::dmxConnect()){
	print STDERR "unable to connect to k8062, retrying (press CTRL-C to cancel)\n";
	sleep 2;
}

if(scalar @ARGV<1){
	print STDERR "Usage: dmxrun.pl <dmxscript>\n";
	exit 1;
}

my @dmxchannels;	#array of hashes, has info on channel functions.
#$dmxchannels[n]->{'functions'}[n] - first function, second function, etc.
#		->{'operations}[n] - operation to combine 1st->2nd, then 2nd->3rd, etc.

open FH,"<".$ARGV[0] or die "Could not open '".$ARGV[0]."'.\n";

while(<FH>){
	#each line is read into $_
	chomp;
	print "Got line $_\n";
	if(length $_>1 and not /^#/){
		/^([^=]+)=(.*)$/;
		my %dmxchannel;

		my $descriptor=$1;
		my $expression=$2;
		my $n_dmx=-1;
		if($descriptor=~/dmx_(\d+)/){
			$n_dmx=$1;
		}
		
		print "\tGot channel num $n_dmx, descriptor $descriptor, expression $expression.\n";
		parse_expression($expression,\@{$dmxchannel{'functions'}});
		$dmxchannel{'number'}=$n_dmx;
		$dmxchannels[$n_dmx]=\%dmxchannel;
	}
}
#print Dumper(\@dmxchannels);

my $t=0;
my $delay=1/$tickspersecond;

print "Delay time set to $delay.\n";

my %triggerstatus;
do {
	while((my $trig=$evq->dequeue_nb)){
		print STDERR '.';
		print STDERR "debug: received trigger $trig.\n";
		if($triggerstatus{$trig}){
			$triggerstatus{$trig}=0;
		} else {
			$triggerstatus{$trig}=1;
		}
		$triggerstatus{$trig}=1 if(not defined $triggerstatus{$trig});
	}
	for(my $ch=0;$ch<$maxchannels;++$ch){
		if(defined $dmxchannels[$ch]){
			$channelvalue=DMXexpression::evaluate($t,$tickspersecond,$dmxchannels[$ch]->{'functions'},\%triggerstatus);
			#print STDERR "$ch=$channelvalue\n";# if($ch==2);
			dmxinterface::setChannel($ch,$channelvalue) if($channelvalue<256 and $channelvalue>=0);
		}
	}
	++$t;
	usleep $delay*1000000;
} while(1);
