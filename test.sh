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
./tracelib /bin/sh.distrib -f > test_result/sh.distrib.txt
./tracelib /bin/fusermount -f > test_result/fusermount.txt
./tracelib /bin/ntfs-3g -f > test_result/ntfs-3g.txt
./tracelib /bin/sleep -f > test_result/sleep.txt
./tracelib /bin/getfacl -f > test_result/getfacl.txt
./tracelib /bin/ss -f > test_result/ss.txt
./tracelib /bin/grep -f > test_result/grep.txt
./tracelib /bin/busybox -f > test_result/busybox.txt
./tracelib /bin/stty -f > test_result/stty.txt
./tracelib /bin/ntfscat -f > test_result/ntfscat.txt
./tracelib /bin/gzip -f > test_result/gzip.txt
./tracelib /bin/ntfsck -f > test_result/ntfsck.txt
./tracelib /bin/sync -f > test_result/sync.txt
./tracelib /bin/bzip2 -f > test_result/bzip2.txt
./tracelib /bin/hostname -f > test_result/hostname.txt
./tracelib /bin/tailf -f > test_result/tailf.txt
./tracelib /bin/ip -f > test_result/ip.txt
./tracelib /bin/ntfscmp -f > test_result/ntfscmp.txt
./tracelib /bin/tar -f > test_result/tar.txt
./tracelib /bin/tempfile -f > test_result/tempfile.txt
./tracelib /bin/kill -f > test_result/kill.txt
./tracelib /bin/ntfsfix -f > test_result/ntfsfix.txt
./tracelib /bin/touch -f > test_result/touch.txt
./tracelib /bin/cat -f > test_result/cat.txt
./tracelib /bin/kmod -f > test_result/kmod.txt

./tracelib /bin/bash -c > test_result/ratio.txt
./tracelib /bin/fgconsole -c >> test_result/ratio.txt
./tracelib /bin/nc.openbsd -c >> test_result/ratio.txt
./tracelib /bin/setfont -c >> test_result/ratio.txt
./tracelib /bin/bunzip2 -c >> test_result/ratio.txt
./tracelib /bin/fgrep -c >> test_result/ratio.txt
./tracelib /bin/netcat -c >> test_result/ratio.txt
./tracelib /bin/sh -c >> test_result/ratio.txt
./tracelib /bin/bzcat -c >> test_result/ratio.txt
./tracelib /bin/fuser -c >> test_result/ratio.txt
./tracelib /bin/sh.distrib -c >> test_result/ratio.txt
./tracelib /bin/fusermount -c >> test_result/ratio.txt
./tracelib /bin/ntfs-3g -c >> test_result/ratio.txt
./tracelib /bin/sleep -c >> test_result/ratio.txt
./tracelib /bin/getfacl -c >> test_result/ratio.txt
./tracelib /bin/ss -c >> test_result/ratio.txt
./tracelib /bin/grep -c >> test_result/ratio.txt
./tracelib /bin/busybox -c >> test_result/ratio.txt
./tracelib /bin/stty -c >> test_result/ratio.txt
./tracelib /bin/ntfscat -c >> test_result/ratio.txt
./tracelib /bin/gzip -c >> test_result/ratio.txt
./tracelib /bin/ntfsck -c >> test_result/ratio.txt
./tracelib /bin/sync -c >> test_result/ratio.txt
./tracelib /bin/bzip2 -c >> test_result/ratio.txt
./tracelib /bin/hostname -c >> test_result/ratio.txt
./tracelib /bin/tailf -c >> test_result/ratio.txt
./tracelib /bin/ip -c >> test_result/ratio.txt
./tracelib /bin/ntfscmp -c >> test_result/ratio.txt
./tracelib /bin/tar -c >> test_result/ratio.txt
./tracelib /bin/tempfile -c >> test_result/ratio.txt
./tracelib /bin/kill -c >> test_result/ratio.txt
./tracelib /bin/ntfsfix -c >> test_result/ratio.txt
./tracelib /bin/touch -c >> test_result/ratio.txt
./tracelib /bin/cat -c >> test_result/ratio.txt
./tracelib /bin/kmod -c >> test_result/ratio.txt


#./tracelib /bin/nisdomainname -f > test_result/nisdomainname.txt
#./tracelib /bin/kbd_mode -f > test_result/kdb_mode.txt
#./tracelib /bin/ntfsdump_logfile -f > test_result/ntfsdump_logfile.txt
#./tracelib /bin/ntfscluster -f > test_result/ntfscluster.txt
#./tracelib /bin/bzip2recover -f > test_result/bzip2recover.txt
#./tracelib /bin/ntfs-3g.probe -f > test_result/ntfs-3g.probe.txt
#./tracelib /bin/ntfs-3g.secaudit -f > test_result/ntfs-3g.secaudit.txt
#./tracelib /bin/ntfs-3g.usermap -f > test_result/ntfs-3g.usermap.txt

exit	