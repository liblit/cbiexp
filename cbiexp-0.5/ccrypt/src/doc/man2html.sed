s/^\.TH \([^ ]*\) \([^ ]*\) "\([^"]*\)" .*$/<html><body bgcolor=#ffffff><title>Man page for \1(\2)<\/title><h1>Man page for \1(\2)<\/h1>/
s/^\.SH \(.*\)$/<h3>\1<\/h3>/
s/^\.nf/<pre>/
s/^\.fi/<\/pre><p>/
s/^\.TP \([0-9]*\)/<table border=0><tr><td width=30% valign=top><nobr>__sed_gaga__/
s/^\.TP$/<\/td><\/tr><tr><td width=30% valign=top><nobr>__sed_gaga__/
/__sed_gaga__/N
s/^\.PD$/<\/td><\/tr><\/table>/
s/^\.IP$/<table border=0><tr><td width=30><\/td><td>/
s/^\.LP$/<\/td><\/tr><\/table>/
s/^\.B \(.*\)$/<b>\1<\/b>/
/__sed_gaga__/s/, /<\/nobr>, <nobr>/g
s/__sed_gaga__\n\.B \(.*\)$/<b>\1<\/b><\/nobr><\/td><td>/
s/__sed_gaga__/<\/nobr><!oops>/
s/^\.I \(.*\)$/<i>\1<\/i>/
s/^\.\\" \(.*\)/<!--\1-->/
s/^$/<p>/
s/\\-/-/g
s/\\fI\([^\\]*\)\\fP/<i>\1<\/i>/g
s/\\fB\([^\\]*\)\\fP/<b>\1<\/b>/g

