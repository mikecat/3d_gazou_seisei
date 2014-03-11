#!/usr/bin/perl

use strict;

my $status=0;
my $category="";
my $title="";
my $alias="";
my $content="";
my @params=();

while(my $line=<STDIN>) {
	chomp($line);
	if($line =~ /^!CATEGORY/) {
		my ($l,$cate)=split(/ +/,$line,2);
		$category=$cate;
		print "\n";
	} elsif($line =~ /^!END/) {
		last;
	} elsif($line eq "") {
		$status++;
		if($status>=3) {
			$title =~ /^[^ ]+ +([^ \(]+)\(/;
			chomp($content);
			chomp($alias);
			my $funcName=$1;
			unless(-e $category){mkdir($category);}
			open(OUT,"> $category/$funcName.html") or die("file open error\n");
			print OUT "<!doctype html>\n";
			print OUT "<html lang=\"ja\">\n";
			print OUT "<head>\n";
			print OUT "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=Shift_JIS\">\n";
			print OUT "<title>$funcName</title>\n";
			print OUT "</head>\n";
			print OUT "<body>\n";
			print OUT "<h2>$funcName</h2>\n";
			print OUT "<h3>プロトタイプ</h3>\n";
			printf OUT "<p><code>%s</code></p>\n",&HTMLescape($title);
			print OUT "<h3>概要</h3>\n";
			printf OUT "<p>%s</p>\n",&HTMLescape($content);
			print OUT "<h3>引数</h3>\n";
			if(@params>0) {
				print OUT "<dl>\n";
				for(my $i=0;$i<@params;$i+=2) {
					printf OUT "<dt>%s</dt><dd>%s</dd>\n",
						&HTMLescape($params[$i]),&HTMLescape($params[$i+1]);
				}
				print OUT "</dl>\n";
			} else {
				print OUT "<p>なし</p>\n";
			}
			if($alias ne "") {
				print OUT "<h3>別名</h3>\n";
				printf OUT "<p>%s</p>\n",&HTMLescape($alias);
			}
			print OUT "</body>\n";
			print OUT "</html>\n";
			close(OUT);
			print "<LI> <OBJECT type=\"text/sitemap\">\n";
			print "\t<param name=\"Name\" value=\"$funcName\">\n";
			print "\t<param name=\"Local\" value=\"$category/$funcName.html\">\n";
			print "\t</OBJECT>\n";
			$status=0;
			$title="";
			$alias="";
			$content="";
			@params=();
		}
	} else {
		if($status==0) {
			if($title eq ""){$title=$line;} else {$alias.=$line."\n";}
		} elsif($status==1) {
			$content.=$line."\n";
		} elsif($status==2) {
			$line =~ /^([^ ]+) *: *([^ ].*)$/;
			push(@params,$1);
			push(@params,$2);
		}
	}
}

sub HTMLescape {
	my ($a) =@_;
	$a =~ s/&/&amp;/g;
	$a =~ s/ /&nbsp;/g;
	$a =~ s/"/&quot;/g;
	$a =~ s/'/&#39;/g;
	$a =~ s/</&lt;/g;
	$a =~ s/>/&gt;/g;
	$a =~ s/\r\n/\n/g;
	$a =~ s/\r/\n/g;
	$a =~ s/\n/<br>\n/g;
	return $a;
}