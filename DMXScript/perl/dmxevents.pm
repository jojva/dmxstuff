#this module creates a thread that listens for various events and passes them back to the main program.
package dmxevents;

use Term::ReadKey;
use threads;
use threads::shared;
use Thread::Queue;
use Switch;
use Data::Dumper;

my $terminateFlag : shared =0;
my $dataQueue : shared;
my $threadId;
my %triggers : shared;

#constructor/destructors which are called from main thread.
sub run {
	$dataQueue=Thread::Queue->new();
	$threadId=threads->create(\&dmx_event_thread) if defined $dataQueue;
	$triggers{'key'}=shared_clone {};
	return $dataQueue;
}

sub terminate {
	$terminateFlag=1;
	sleep 1;	#wait for the thread to finish
	$threadId->join;
}

#this should be called to set up all triggers, BEFORE setup is run.
#triggerType can currently be 'key' or 'gpi'
sub add_trigger {
my ($triggerType,$triggerValue,$triggerId)=@_;

print "debug: adding $triggerType trigger with value $triggerValue and id $triggerId\n";

print Dumper(\%triggers);

#if(defined $trigger{$triggerType}){
	my $temp=$triggers{$triggerType};
	print Dumper($temp);
	$temp->{$triggerValue}=$triggerId;
	$triggers{$triggerType}=shared_clone $temp;
#} else {
#	$triggers{$triggerType}=shared_clone { $triggerValue=>$triggerId };
#}

print Dumper(\%triggers);
}

#this is the thread which responds to events
sub dmx_event_thread {

my %controlkeys=GetControlChars;

while(not $terminateFlag){
	#ReadMode 4;		#change mode to 'raw'
	my $ch=ReadKey 0;	#do a blocking read
	#ReadMode 0;
	terminate if($ch eq $controlkeys{INTERRUPT});
	print STDERR "trigger: picked up character $ch.\n";
	foreach(keys %triggers){
		if($_ eq "key"){
			print STDERR "keytrigger: got $ch corresponding to ".$triggers{'key'}->{$ch}.".\n";
			$dataQueue->enqueue($triggers{'key'}->{$ch});
		}
	}
}
}

1;
