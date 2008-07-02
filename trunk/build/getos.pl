#!/usr/bin/perl
##################################################
#
# Retrieve the current operating system
# that the user is building under
#
#    linux
#    solaris
#
##################################################

my $osname = `uname`;
$osname = lc($osname);
my $platform = "unknown";

if($osname =~ m/linux/)
{
   $platform = "linux";
}
elsif($osname =~ m/solaris/ || $osname =~ m/sunos/)
{
   $platform = "solaris";
}
elsif($osname =~ m/darwin/)
{
   $platform = "mac";
}

print "$platform\n"
