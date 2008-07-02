#!/usr/bin/perl
##################################################
#
# Retrieve the architecture
# that the user is building under
#
#    64
#
##################################################

my $arch = `uname -m`;
$arch = lc($arch);
my $platform = "unknown";

if($arch =~ m/x86_64/)
{
   $platform = "64";
}
else
{
   $platform = "32";
}

print "$platform\n"

