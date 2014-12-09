#!/bin/bash
./tracelib /bin/bash -f > test_result/bash.txt
./tracelib /bin/fgconsole -f > test_result/fgconsole.txt
./tracelib /bin/nc.openbsd -f > test_result/nc.openbsd.txt
./tracelib /bin/setfont -f > test_result/setfont.txt
./tracelib /bin/bunzip2 -f > test_result/bunzip2.txt
./tracelib /bin/fgrep -f > test_result/fgrep.txt
./tracelib /bin/netcat -f > test_result/netcat.txt
./tracelib /bin/sh -f > test_result/sh.txt
./tracelib /bin/bzcat -f > test_result/bzcat.txt
./tracelib /bin/fuser -f > test_result/fuser.txt
./tracelib /bin/nisdomainname -f > test_result/nisdomainname.txt
./tracelib /bin/sh.distrib -f > test_result/sh.distrib.txt
./tracelib /bin/fusermount -f > test_result/fusermount.txt
./tracelib /bin/ntfs-3g -f > test_result/ntfs-3g.txt
./tracelib /bin/sleep -f > test_result/sleep.txt
./tracelib /bin/getfacl -f > test_result/getfacl.txt
./tracelib /bin/ntfs-3g.probe -f > test_result/ntfs-3g.probe.txt
./tracelib /bin/ss -f > test_result/ss.txt
./tracelib /bin/grep -f > test_result/grep.txt
./tracelib /bin/ntfs-3g.secaudit -f > test_result/ntfs-3g.secaudit.txt
./tracelib /bin/busybox -f > test_result/busybox.txt
./tracelib /bin/ntfs-3g.usermap -f > test_result/ntfs-3g.usermap.txt
exit	
