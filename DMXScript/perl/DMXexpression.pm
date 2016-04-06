#this module implements various built-in expressions for dmxscript.
package DMXexpression;
use Term::ReadKey;
use Data::Dumper;
use Switch;
use constant PI    => 4 * atan2(1, 1);

#for reference
my @expressions=qw/set sin cos tan exponential keytrigger gpitrigger/;
my @operators=qw#+ - * /#;

sub evaluate_func {
#$func is hash ref to broken-down function data
my($ticks,$tickspersecond,$func,$triggerstatus)=@_;

my $rtn;

my $time=$ticks/$tickspersecond;
#print "evaluate_func: ticks/sec=$tickspersecond time=$time.\n";

my $args=$func->{'args'};

switch($func->{'name'}){
case "set" { $rtn=$func->{'args'}->{'value'} }
case "sin" {
	$rtn=($args->{'max'}-$args->{'min'})*sin(2*PI*($time/$args->{'period'}));
	$rtn=-$rtn if($rtn<0);
	$rtn+=$args->{'min'};
	#print "debug: time=$time,period=".$args->{'period'}.".\n";
	#print "debug:";
	#print Dumper($args);
}
case "cos" {
	$rtn=($args->{'max'}-$args->{'min'})*cos(2*PI*($time/$args->{'period'}));
	$rtn=-$rtn if($rtn<0);
	$rtn+=$args->{'min'};
	#print "debug: time=$time,period=".$args->{'period'}.".\n";
	#print "debug:";
	#print Dumper($args);
}
case "exponential" {
	my $sign;
	#stores total number of ticks since last event.
	++$func->{'accumulator'};

	if($func->{'accumulator'}/$tickspersecond>$args->{'reset'}){
		$func->{'accumulator'}=0;
	}
	$time=$func->{'accumulator'}/$tickspersecond;

	if($args->{'direction'} eq "down"){
		$sign=-1;
	} else {
		$sign=+1;
	}
	$rtn=($args->{'max'}-$args->{'min'})*exp($sign*$time/$args->{'period'});
	$rtn+=$args->{'min'};
}
case "keytrigger" {
	#print STDERR Dumper(riggerstatus);
	$rtn=$args->{'on'} if($triggerstatus->{$args->{'key'}});
	$rtn=$args->{'off'} if(not $triggerstatus->{$args->{'key'}});
}
}

if($rtn<0){
	return int(-$rtn);
} else {
	return int($rtn);
}
}

#return current value for the given expression
sub evaluate {
	#$ticks is the time running.  $functions is an array ref to the broken-down function data.  $triggerstatus is an hash ref to tell us the state of a trigger - $triggerstatus{'trigger-id'}={0|1}, trigger-id is what was passed to dmxevents::add_trigger.
	my($ticks,$tickspersecond,$functions,$triggerstatus)=@_;
	
	my $rtn;
	foreach(@$functions){
		my $val=evaluate_func($ticks,$tickspersecond,$_,$triggerstatus);
		switch($_->{'oper'}){
			case '' { $rtn=$val }
			case '+' { $rtn+=$val }
			case '-' { $rtn-=$val }
			case '*' { $rtn=($rtn/255)*$val }
			case '/' { $rtn/=$val }
			else { print "syntax error on function ".$_->{'name'}." - operator '".$_->{'oper'}."' not recognised.\n" }
		}
	}
	return int($rtn);
}

1;
