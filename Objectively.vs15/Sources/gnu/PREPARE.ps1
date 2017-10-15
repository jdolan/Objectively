function EnsureHere($filename, $url)
{
	If (!(Test-Path $filename)) {
	    Write-Output "Downloading $filename..."
	    (New-Object System.Net.WebClient).DownloadFile($url, $filename)
	}
}

EnsureHere "intprops.h" "https://raw.githubusercontent.com/coreutils/gnulib/master/lib/intprops.h"
EnsureHere "regcomp.c" "https://raw.githubusercontent.com/coreutils/gnulib/master/lib/regcomp.c"
EnsureHere "regex.c" "https://raw.githubusercontent.com/coreutils/gnulib/master/lib/regex.c"
EnsureHere "regex.h" "https://raw.githubusercontent.com/coreutils/gnulib/master/lib/regex.h"
EnsureHere "regex_internal.h" "https://raw.githubusercontent.com/coreutils/gnulib/master/lib/regex_internal.h"
EnsureHere "regex_internal.c" "https://raw.githubusercontent.com/coreutils/gnulib/master/lib/regex_internal.c"
EnsureHere "regexec.c" "https://raw.githubusercontent.com/coreutils/gnulib/master/lib/regexec.c"