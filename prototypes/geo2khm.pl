#!/usr/bin/perl -w
# Ported from http://blog.grimpoteuthis.org/2005/04/more-maps.html
# $Id$
# Blick auf den Heise Verlag :
#    http://khm2.google.de/kh?v=29&hl=de&t=trtqtqsrqrrrssttrtqrq
#    ./geo2khm.pl 9.806262 52.380508 1


use strict;
use warnings;


my ($lon, $lat, $zoomFactor) = @ARGV;



sub usage
{
    print "Aufruf: geo2khm.pl Längengrad Breitengrad Zoom\n\n";
}


if (!defined($lon) || !defined($lat) || !defined($zoomFactor)) { usage();  }


my @gZoomLevels = map{ 2**(25 - $_) / 360 }(0..16);

# print join "\n", @gZoomLevels;

sub getLngLat($$$)
{
    my ($pixelX, $pixelY, $zoom) = @_;
    return
        $pixelX / $gZoomLevels[$zoom] - 180,
        180 - $pixelY / $gZoomLevels[$zoom];
}


sub getPixelPos($$$)
{
    my ($longitude, $latitude, $zoom) = @_;
    return
        ($longitude + 180) * $gZoomLevels[$zoom],
        (180 - $latitude) * $gZoomLevels[$zoom];
}


sub tileUrl($$$)
{
    my ($x, $y, $zoom) = @_;

    $x /= 256;
    $y /= 256;

    my $range = 2**(17 - $zoom);

    # Wrap-around x coordinate.
    if (($x < 0) || ($x > $range - 1))
    {
        $x = $x % $range;
        # The mod operator isn't quite the same on a computer as it is
        # in your math class (negative isn't handled correctly).
        if ($x < 0) { $x += $range; }
    }

    # Build the quadtree path, working our way down from 2^16
    # to the current zoom level.
    my $Ac = "t";
    for (my $pow = 16; $pow >= $zoom; --$pow)
    {
        # Drop to the next zoom level.
        $range /= 2;
        if ($y < $range)
        {
            if ($x < $range)
            {
                # Upper-left quadrant.
                $Ac .= "q";
            }
            else
            {
                # Upper-right quadrant.
                $Ac .= "r";
                $x -= $range;
            }
        }
        else
        {
            if ($x < $range)
            {
                # Lower-left quadrant.
                $Ac .= "t";
                $y -= $range;
            }
            else
            {
                # Lower-right quadrant.
                $Ac .= "s";
                $x -= $range;
                $y -= $range;
            }
        }
    }
    return "http://khm.google.com/kh?v=29&hl=de&t=" . $Ac;
}


my ($px, $py) = getPixelPos($lon, $lat, $zoomFactor);

print "getPixelPos($lon, $lat, $zoomFactor) = $px / $py\n";

print "tileUrl = ", tileUrl($px, $py, $zoomFactor), "\n";

exit 0;
